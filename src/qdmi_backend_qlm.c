#include <assert.h>
#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#include <llvm-c/Core.h>
#include <llvm-c/BitReader.h>

#include "jansson.h"

#include "qdmi_backend_qlm.h"

#define CHECK_ERR(a,b) { if (a!=QDMI_SUCCESS) { printf("\n[Error]: %i at %s",a,b); return 1; }}

char *createTheRequest(unsigned int shoots, QDMI_Fragment *frag, int task_id)
{
    time_t timer;
    char submit_time[26];
    struct tm *tm_info;
    timer = time(NULL);
    tm_info = localtime(&timer);
    
    //strftime(submit_time, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    //char task_id[10];
    //snprintf(task_id, 10, "%d%d", getpid(), tm_info->tm_sec);
    json_t *json_obj = json_integer(task_id);
    char *task_id_str = json_dumps(json_obj, JSON_ENCODE_ANY);
    
    json_t *root = json_object();

    json_object_set_new(root, "task_id", json_string(task_id_str));
    json_object_set_new(root, "n_qbits", json_integer(5));
    json_object_set_new(root, "n_shots", json_integer(shoots));
    json_object_set_new(root, "circuit_file", json_string((*frag)->qasmstr));
    json_object_set_new(root, "circuit_file_type", json_string("qasm"));
    json_object_set_new(root, "result_destination", json_string("qlm_test_dest"));
    json_object_set_new(root, "prefered_qpu", json_string("iqm5"));
    json_object_set_new(root, "prioirity", json_integer(0));
    json_object_set_new(root, "optimisation_level", json_integer(0));
    json_object_set_new(root, "transpiler_flag", json_boolean(1));
    json_object_set_new(root, "result_type", json_integer(0));
    json_object_set_new(root, "submit_time", json_string(submit_time));

    //taskId = atoi(task_id);
    return json_dumps(root, JSON_INDENT(4));
}

int getTheQubitIndex(LLVMValueRef theQubit){
    // Might need to updated if QIR adapts opaque pointer
    if(LLVMIsNull(theQubit))
        return 0;
    
    return LLVMConstIntGetZExtValue(LLVMGetOperand(theQubit, 0));
}

float getTheAngleValue(LLVMValueRef theQubit){
    // Gets the angle of a rotation 
    return LLVMConstRealGetDouble(theQubit, 0);
}

bool isRotation(const char* calledFunction){
    // Returns true if it is a rotation function
    for(int gate_index = 0; gate_index < N_R_GATE; gate_index++)
        if(!strcmp(calledFunction, rotation_gate_set[gate_index]))
            return true;
    return false;
}

int gateFormatter(char **instruction, const char* gate, const LLVMValueRef theInstruction, bool isRotationGate){
    // Creates QASM Instruction from QIR Instruiction.
    int numberOfArgs = LLVMGetNumOperands(theInstruction) -1;
    LLVMValueRef theOperand = LLVMGetOperand (theInstruction, 0);
    if (numberOfArgs == 1)
        return asprintf(instruction, gate, getTheQubitIndex(theOperand));

    LLVMValueRef theSecondOperand = LLVMGetOperand (theInstruction, 1);
    if (numberOfArgs == 2){
        if(isRotationGate)
            return asprintf(instruction, gate, getTheAngleValue(theOperand), getTheQubitIndex(theSecondOperand));
        return asprintf(instruction, gate, getTheQubitIndex(theOperand), getTheQubitIndex(theSecondOperand));
    }

    LLVMValueRef theThirdOperand = LLVMGetOperand (theInstruction, 2);
    if (numberOfArgs == 3){
        if(isRotationGate)
            return asprintf(instruction, gate, getTheAngleValue(theOperand), getTheQubitIndex(theSecondOperand), getTheQubitIndex(theThirdOperand));
        return asprintf(instruction, gate, getTheQubitIndex(theOperand), getTheQubitIndex(theSecondOperand), getTheQubitIndex(theThirdOperand));
    }

    LLVMValueRef theFourthOperand = LLVMGetOperand (theInstruction, 3);
    if (numberOfArgs == 4 && isRotationGate)
            return asprintf(instruction, gate, getTheAngleValue(theOperand), getTheAngleValue(theSecondOperand), getTheAngleValue(theThirdOperand), getTheQubitIndex(theFourthOperand));
    
    return -1;
}

void getRequiredNum(LLVMValueRef Function, int *requiredQubits, int *requiredResults) {
    // Gets the requiredQubits and requiredResults
    int AttrCount =  LLVMGetAttributeCountAtIndex(Function, LLVMAttributeFunctionIndex);
    LLVMAttributeRef* Attrs = (LLVMAttributeRef *)malloc(AttrCount * sizeof(LLVMAttributeRef));
    LLVMGetAttributesAtIndex(Function, LLVMAttributeFunctionIndex, Attrs); 
   
    while(AttrCount){     
        if(LLVMIsStringAttribute(*Attrs)){
            unsigned int size;
            const char* key = LLVMGetStringAttributeKind(*Attrs, &size);
            if(!strcmp(key, "num_required_qubits")){
                const char* value = LLVMGetStringAttributeValue(*Attrs, &size);
                *requiredQubits = atoi(value);
            }
            else if(!strcmp(key, "num_required_results")){
                const char* value = LLVMGetStringAttributeValue(*Attrs, &size);
                *requiredResults = atoi(value);
            }
        }
        AttrCount--;
        Attrs++;
    }

    *requiredResults = 3;
    *requiredQubits  = 3;
}

char* createQASMfromQIR(LLVMModuleRef qirModule)
{
    // Goes though all the function and creates qasm code.
    char* qasmCode; 
    char* instruction;
    int requiredQubits = 0, requiredResults = 0;
    getRequiredNum(LLVMGetFirstFunction(qirModule), &requiredQubits, &requiredResults);
    asprintf(&qasmCode, "OPENQASM 2.0;\ninclude \"qelib1.inc\";\nqreg q[%d];\ncreg c[%d];\n", requiredQubits, requiredResults);    
    int numberOfInstruction = 2;        
    for (LLVMValueRef theFunction = LLVMGetFirstFunction(qirModule);
            theFunction != NULL; theFunction = LLVMGetNextFunction(theFunction)){
        for (LLVMBasicBlockRef theBB = LLVMGetFirstBasicBlock(theFunction);
                theBB != NULL; theBB = LLVMGetNextBasicBlock(theBB)){
            for (LLVMValueRef theInstruction = LLVMGetFirstInstruction(theBB);
                    theInstruction != NULL; theInstruction = LLVMGetNextInstruction(theInstruction)){
                    LLVMOpcode theOpcode = LLVMGetInstructionOpcode(theInstruction);
                    if(theOpcode == LLVMCall){
                        LLVMValueRef calledFunction = LLVMGetCalledValue(theInstruction);
                        const char* functionName = LLVMGetValueName(calledFunction);
                        for(int gate_index = 0; gate_index <= N_GATE; gate_index++){
                            if(!strcmp(functionName, gate_set[gate_index])){
                                numberOfInstruction++;
                                gateFormatter(&instruction, qasm_gate_set[gate_index], theInstruction, isRotation(functionName));
                                qasmCode = realloc(qasmCode, numberOfInstruction*INSTRUCTIONSIZE);
                                strcat(qasmCode, instruction);
                            }
                        }
                }
            }
        }
    }
    //printf("\n\t%s", qasmCode);
    //fflush(stdout);
    return qasmCode;
}

void connectToTheRabbitMQ(amqp_connection_state_t *Connection,
                          amqp_socket_t **Socket)
{
    *Connection = (amqp_new_connection());
    assert(Connection != NULL);

    *Socket = amqp_tcp_socket_new(*Connection);
    assert(Socket != NULL);
    
    int status = amqp_socket_open(*Socket, HOST_NAME, PORT);
    assert(status == AMQP_STATUS_OK);
    
    amqp_rpc_reply_t reply = amqp_login(*Connection, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, "guest", "Qrmguest");
    assert(reply.reply_type == AMQP_RESPONSE_NORMAL);
    
    amqp_channel_open_ok_t *res = amqp_channel_open(*Connection, 1);
    assert(res != NULL);
    reply = amqp_get_rpc_reply(*Connection);
    assert(reply.reply_type == AMQP_RESPONSE_NORMAL);
}


void publishTheString(amqp_connection_state_t *Connection, char *Message,
                      char *Queue)
{
    amqp_connection_state_t LocalConnection = *Connection;
    amqp_basic_properties_t props;
    props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
    props.content_type = amqp_cstring_bytes("text/plain");
    props.delivery_mode = 2;
    amqp_basic_publish(*Connection, 1, amqp_empty_bytes,
                       amqp_cstring_bytes(Queue), 0, 0, &props,
                       amqp_cstring_bytes(Message));
}

void closeConnections(amqp_connection_state_t *Connection)
{
    amqp_channel_close(*Connection, 1, AMQP_REPLY_SUCCESS);
    amqp_connection_close(*Connection, AMQP_REPLY_SUCCESS);
    amqp_destroy_connection(*Connection);
}

char *startConsume(amqp_connection_state_t *Connection, char *Queue)
{
    amqp_bytes_t queuename;
    amqp_queue_declare_ok_t *r = amqp_queue_declare(
        *Connection, 
        1, 
        amqp_cstring_bytes(Queue), 
        0, 
        0, 
        0,
        0, 
        amqp_empty_table
    );

    amqp_get_rpc_reply(*Connection);
    queuename = amqp_bytes_malloc_dup(r->queue);
    if (queuename.bytes == NULL)
    {
        fprintf(stderr, "Out of memory while copying queue name");
        return "1";
    }
    char *TheResponse = (char *)malloc(sizeof(char) * 50);
    amqp_basic_consume(
        *Connection, 
        1, 
        amqp_cstring_bytes(Queue),
        amqp_empty_bytes, 
        0, 
        1, 
        0, 
        amqp_empty_table
    );
    
    amqp_rpc_reply_t rpc_reply = amqp_get_rpc_reply(*Connection);

    if (rpc_reply.reply_type != AMQP_RESPONSE_NORMAL)
        fprintf(stderr, "Failed to start consuming messages.\n");

    // TODO What do we need this loop for?
    for (;;)
    {
        amqp_rpc_reply_t res;
        amqp_envelope_t envelope;

        struct timeval* timeout = malloc(sizeof(struct timeval));
        timeout->tv_sec = 100;

        amqp_maybe_release_buffers(*Connection);

        res = amqp_consume_message(
            *Connection, 
            &envelope, 
            NULL, 
            0
        );

        if (AMQP_RESPONSE_NORMAL != res.reply_type)
            break;

        json_error_t theError;

        TheResponse = (char *)envelope.message.body.bytes;
        json_t *root = json_loads(TheResponse, 0, &theError);

        //printf("   [Backend].............The Response: %s\n", TheResponse);

        if (!root)
        {
            TheResponse[theError.column - 1] = ' ';
            root = json_loads(TheResponse, 0, NULL);
        }
        break;
    }

    return TheResponse;
}

int QDMI_backend_init(QInfo info)
{
    printf("   [Backend].............Initializing QLM (simulator) via QDMI\n");

    char *uri = NULL;
    void *regpointer = NULL;
    int err;

    err = QDMI_core_register_belib(uri, regpointer);
    /* It is problem of future*/
    
    connectToTheRabbitMQ(&SendConnection, &SendSocket);
    return QDMI_SUCCESS;
}

int QDMI_control_readout_size(QDMI_Device dev, QDMI_Status *status, int *numbits)
{
    *numbits = 2;
    return QDMI_SUCCESS;
}

int QDMI_device_status(QDMI_Device dev, QInfo info, int *status)
{
    printf("   [Backend].............QLM (simulator) query device status OK\n");

    *status = 1;
    return QDMI_SUCCESS;
}

int QDMI_control_pack_qir(QDMI_Device dev, void *qirmod, QDMI_Fragment *frag)
{
    if(!dev || !qirmod || !frag)
        return QDMI_ERROR_FATAL;

    (*frag)->qirmod = qirmod;

    return QDMI_SUCCESS;
}

int QDMI_control_pack_qasm2(QDMI_Device dev, char *qasmstr, QDMI_Fragment *frag)
{
    
    if(!frag)
        return 1;
    if(!qasmstr)
        return 2;
    (*frag)->qasmstr = malloc(sizeof(qasmstr));
    strcpy((*frag)->qasmstr, qasmstr);

    return QDMI_SUCCESS;
}

int QDMI_query_gateset_num(QDMI_Device dev, int *num_gates)
{
    if (sizeof(gate_set) > 0)
        *num_gates = sizeof(gate_set) / sizeof(gate_set[0]);
    else
        *num_gates = 0;

    return QDMI_SUCCESS;
}

int QDMI_control_submit(QDMI_Device dev, QDMI_Fragment *frag, int numshots, QInfo info, QDMI_Job *job)
{
    if(!dev || !frag || !info || !job || !numshots || 
      (!(*frag)->qasmstr && !((*frag)->qirmod)))
        return QDMI_ERROR_FATAL;

	printf("   [Backend].............Circuit received\n");

    LLVMModuleRef module = NULL;
    LLVMMemoryBufferRef mem_buffer = LLVMCreateMemoryBufferWithMemoryRange(
        (const char *)(*frag)->qirmod,
        (*frag)->sizebuffer,
        "QIR_module",
        0
    );

    char *error = NULL;
    if (LLVMParseBitcode2(mem_buffer, &module) != 0)
	{
        fprintf(stderr, 
		        "   [Backend]............Error - Failed to parse bitcode: %s\n", 
	            error
		);

        LLVMDisposeMemoryBuffer(mem_buffer);
        return QDMI_ERROR_FATAL;
    }

    LLVMDisposeMemoryBuffer(mem_buffer);

	char *qasmCode = createQASMfromQIR(module);
	QDMI_control_pack_qasm2(dev, qasmCode , &*frag);
    LLVMDisposeModule(module);
	free(qasmCode);
    
    char *TheRequest = createTheRequest(numshots, frag, (*job)->task_id);
    publishTheString(
		&SendConnection, 
		TheRequest, 
		//"qd_qrequest_reception_queue_274973451958055"
        "qs_qlm_274973448280338"
	);

    return QDMI_SUCCESS;
}

void QDMI_get_gate_info(QDMI_Device dev, int gate_index, QDMI_Gate gate)
{
    gate->name     = gate_set[gate_index];
    gate->unitary  = "Unitary_Matrix";
    gate->fidelity = 1.0;
}

int QDMI_set_coupling_mapping(QDMI_Device dev, int qubit_index, QDMI_Qubit qubit)
{
    int err, num_qubits;

    err = QDMI_query_qubits_num(dev, &num_qubits);
    qubit->index = qubit_index;

    assert(qubit_index >= 0 && qubit_index < num_qubits);

    if (qubit_index == 0)
    {
        qubit->coupling_mapping = (QDMI_qubit_index*)malloc(1 * sizeof(QDMI_qubit_index));
        qubit->coupling_mapping[0] = 1;
        qubit->size_coupling_mapping = 1;
    }
    else if (qubit_index == num_qubits - 1)
    {
        qubit->coupling_mapping = (QDMI_qubit_index*)malloc(1 * sizeof(QDMI_qubit_index));
        qubit->coupling_mapping[0] = num_qubits - 2;
        qubit->size_coupling_mapping = 1;
    }
    else
    {
        qubit->coupling_mapping = (QDMI_qubit_index*)malloc(2 * sizeof(QDMI_qubit_index));
        qubit->coupling_mapping[0] = qubit_index - 1;
        qubit->coupling_mapping[1] = qubit_index + 1;
        qubit->size_coupling_mapping = 2;
    }
}

int QDMI_query_all_qubits(QDMI_Device dev, QDMI_Qubit *qubits)
{
    int err, num_qubits;

    err = QDMI_query_qubits_num(dev, &num_qubits);

    if (err != QDMI_SUCCESS)
    {
        printf("   [Backend].............QDMI failed to return number of qubits\n");
        return QDMI_WARN_GENERAL;
    }

    *qubits = (QDMI_Qubit)malloc(num_qubits * sizeof(QDMI_Qubit_impl_t));

    if (*qubits == NULL)
    {
        printf("   [Backend].............Couldn't allocate memory for the qubit array\n");
        return QDMI_WARN_GENERAL;
    }

    int i;
    for (i = 0; i < num_qubits; i++)
        QDMI_set_coupling_mapping(dev, i, (*qubits) + i);

    printf("   [Backend].............Returning available qubits\n");
    return QDMI_SUCCESS;
}

int QDMI_query_all_gates(QDMI_Device dev, QDMI_Gate *gates)
{
    int i, num_gates = 0;
    QDMI_query_gateset_num(dev, &num_gates);

    *gates = (QDMI_Gate)malloc(num_gates * sizeof(QDMI_Gate_impl_t));

    for (i = 0; i < num_gates; i++)
        QDMI_get_gate_info(dev, i, (*gates) + i);

    return QDMI_SUCCESS;
}

int QDMI_query_qubits_num(QDMI_Device dev, int *num_qubits)
{
    *num_qubits = 38;
    return QDMI_SUCCESS;
}

unsigned int reverseInt(unsigned int index)
{
    unsigned int reversed = index & 1;

    index >>= 1;
    while (index)
    {
        reversed <<= 1;
        reversed |= index & 1;
        index >>= 1;
    }
    return reversed;
}

int QDMI_control_readout_raw_num(QDMI_Device dev, QDMI_Status *status, int task_id, int *num)
{
    printf("   [Backend].............Returning results\n");

    connectToTheRabbitMQ(&ListenConnection, &Listensocket);
    char *TheResponse = startConsume(&ListenConnection, "qlm_test_dest");
    closeConnections(&ListenConnection);

    json_error_t error;
    json_t *root = json_loads(TheResponse, 0, &error);

    if(!root)
        return QDMI_ERROR_FATAL;

    //json_t *task_id = json_object_get(root, "task_id");
    json_t *results = json_object_get(root, "results");
    json_t *destination = json_object_get(root, "destination");
    json_t *execution_status = json_object_get(root, "execution_status");
    json_t *additional_information = json_object_get(root, "additional_information");
    json_t *execution_time = json_object_get(root, "execution_time");
    json_t *executed_circuit = json_object_get(root, "executed_circuit");
    json_t *executed_qpu = json_object_get(root, "executed_qpu");

    int value_task_id = task_id /*json_integer_value(task_id)*/;
    int destination_str = json_integer_value(execution_time);
    bool execution = json_boolean_value(execution_status);
    const char *additional_information_str = json_string_value(additional_information);
    int execution_time_d = json_integer_value(execution_time);
    void *iter = json_object_iter(results);
    const char *key;
    int err, numbits, value;

    // make sure results are an array of zeros
    err = QDMI_control_readout_size(dev, status, &numbits);
    CHECK_ERR(err, "QDMI_control_readout_raw_num");
    int state_space = 1 << numbits;
    for (int i = 0; i < state_space; i++)
        num[i] = 0;

    while (iter)
    {
        key = json_object_iter_key(iter);
        value = json_integer_value(json_object_iter_value(iter));
        if (key == NULL)
        {
            printf("   [Backend].............Failed to retrieve key from JSON object iterator\n");
            return QDMI_ERROR_FATAL;
        }
        unsigned int index = reverseInt((unsigned int) strtol(key, NULL, 2));
        num[index] = value;
        iter = json_object_iter_next(results, iter);
    }
    fflush(stdout);
    
    return QDMI_SUCCESS;
}

int QDMI_control_wait(QDMI_Device dev, QDMI_Job *job, QDMI_Status *status)
{
    return QDMI_SUCCESS;
}
