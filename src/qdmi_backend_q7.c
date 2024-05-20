#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <dlfcn.h>
#include <string.h>

#include <llvm-c/Core.h>
#include <llvm-c/BitReader.h>

#include "qdmi_backend_q7.h"

#define CHECK_ERR(a,b) { if (a!=QDMI_SUCCESS) { printf("\n[Error]: %i at %s",a,b); return 1; }}

int QDMI_query_gateset_num(QDMI_Device dev, int *num_gates)
{
    if (sizeof(gate_set) > 0)
        *num_gates = sizeof(gate_set) / sizeof(gate_set[0]);
    else
        *num_gates = 0;

    return QDMI_SUCCESS;
}

void QDMI_get_gate_info(QDMI_Device dev, int gate_index, QDMI_Gate gate)
{
    gate->name     = gate_set[gate_index];
    gate->unitary  = "Unitary_Matrix";
    gate->fidelity = 1.0;
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

int QDMI_device_status(QDMI_Device dev, QInfo info, int *status)
{
    printf("   [Backend].............Q7 query device status\n");

    *status = 1;
    return QDMI_SUCCESS;
}

int QDMI_backend_init(QInfo info)
{
    printf("   [Backend].............Initializing Q7 via QDMI\n");

    char *uri = NULL;
    void *regpointer = NULL;
    int err;

    err = QDMI_core_register_belib(uri, regpointer);
    //CHECK_ERR(err, "QDMI_core_register_belib");

    return QDMI_SUCCESS;
}

int QDMI_control_readout_size(QDMI_Device dev, QDMI_Status *status, int *numbits)
{
    *numbits = 7;
    return QDMI_SUCCESS;
}

int QDMI_control_readout_raw_num(QDMI_Device dev, QDMI_Status *status, int task_id, int *num)
{
    //printf("   [Backend]..............Returning raw numbers\n");

    int err = 0, numbits = 0;
    long i;

    err = QDMI_control_readout_size(dev, status, &numbits);
    CHECK_ERR(err, "QDMI_control_readout_raw_num");

    for (i = 0; i < ((long)1 << numbits); i++)
        num[i] = rand();

    return QDMI_SUCCESS;
}

int QDMI_set_coupling_mapping(QDMI_Device dev, int qubit_index, QDMI_Qubit qubit)
{
    qubit->index = qubit_index;

    switch (qubit_index) {
        case 0:
            qubit->coupling_mapping = (QDMI_qubit_index*)malloc(2 * sizeof(QDMI_qubit_index));
            qubit->coupling_mapping[0] = 2;
            qubit->coupling_mapping[1] = 4;
            qubit->size_coupling_mapping = 2;
            break;
        case 1:
            qubit->coupling_mapping = (QDMI_qubit_index*)malloc(2 * sizeof(QDMI_qubit_index));
            qubit->coupling_mapping[0] = 2;
            qubit->coupling_mapping[1] = 5;
            qubit->size_coupling_mapping = 2;
            break;
        case 2:
            qubit->coupling_mapping = (QDMI_qubit_index*)malloc(3 * sizeof(QDMI_qubit_index));
            qubit->coupling_mapping[0] = 0;
            qubit->coupling_mapping[1] = 1;
            qubit->coupling_mapping[2] = 6;
            qubit->size_coupling_mapping = 3;
            break;
        case 3:
            qubit->coupling_mapping = (QDMI_qubit_index*)malloc(2 * sizeof(QDMI_qubit_index));
            qubit->coupling_mapping[0] = 4;
            qubit->coupling_mapping[1] = 5;
            qubit->size_coupling_mapping = 2;
            break;
        case 4:
            qubit->coupling_mapping = (QDMI_qubit_index*)malloc(3 * sizeof(QDMI_qubit_index));
            qubit->coupling_mapping[0] = 0;
            qubit->coupling_mapping[1] = 3;
            qubit->coupling_mapping[2] = 6;
            qubit->size_coupling_mapping = 3;
            break;
        case 5:
            qubit->coupling_mapping = (QDMI_qubit_index*)malloc(3 * sizeof(QDMI_qubit_index));
            qubit->coupling_mapping[0] = 1;
            qubit->coupling_mapping[1] = 3;
            qubit->coupling_mapping[2] = 6;
            qubit->size_coupling_mapping = 3;
            break;
        case 6:
            qubit->coupling_mapping = (QDMI_qubit_index*)malloc(3 * sizeof(QDMI_qubit_index));
            qubit->coupling_mapping[0] = 2;
            qubit->coupling_mapping[1] = 4;
            qubit->coupling_mapping[2] = 5;
            qubit->size_coupling_mapping = 3;
            break;
        default:
            qubit->coupling_mapping = NULL;
            break; 
    }
}

int QDMI_query_all_qubits(QDMI_Device dev, QDMI_Qubit *qubits)
{
    int err, num_qubits;

    err = QDMI_query_qubits_num(dev, &num_qubits);

    if (err != QDMI_SUCCESS)
    {
        printf("   [Backend]..............QDMI failed to return number of qubits\n");
        return QDMI_WARN_GENERAL;
    }

    *qubits = (QDMI_Qubit)malloc(num_qubits * sizeof(QDMI_Qubit_impl_t));

    if (*qubits == NULL)
    {
        printf("   [Backend]..............Couldn't allocate memory for the qubit array\n");
        return QDMI_WARN_GENERAL;
    }

    int i;
    for (i = 0; i < num_qubits; i++)
        QDMI_set_coupling_mapping(dev, i, (*qubits) + i);

    printf("   [Backend].............Returning available qubits\n");
    return QDMI_SUCCESS;
}

int QDMI_query_qubits_num(QDMI_Device dev, int *num_qubits)
{
    *num_qubits = 7;
    return QDMI_SUCCESS;
}

int QDMI_control_submit(QDMI_Device dev, QDMI_Fragment *frag, int numshots, QInfo info, QDMI_Job *job)
{
    printf("   [Backend].............QIR received");
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
        fprintf(stderr, "   [Backend].............Error - Failed to parse bitcode: %s\n", error);
        LLVMDisposeMemoryBuffer(mem_buffer);
        return QDMI_ERROR_FATAL;
    }

    LLVMDisposeMemoryBuffer(mem_buffer);

    // You can print the QIR like this:
    //char *qir_string = LLVMPrintModuleToString(module);
    //printf(":\n%s", qir_string);

    //LLVMDisposeMessage(qir_string);
    LLVMDisposeModule(module);

    printf("\n");

    return QDMI_SUCCESS;
}

int QDMI_control_wait(QDMI_Device dev, QDMI_Job *job, QDMI_Status *status)
{
    return QDMI_SUCCESS;
}

int QDMI_control_pack_qir(QDMI_Device dev, void *qirmod, QDMI_Fragment *frag)
{
    (*frag)->qirmod = qirmod;

    return QDMI_SUCCESS;
}

