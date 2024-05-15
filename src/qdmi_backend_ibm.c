#include "qdmi_backend_ibm.h"

#define CHECK_ERR(a,b) { if (a!=QDMI_SUCCESS) { printf("\n[Error]: %i at %s",a,b); return 1; }}

json_t *root;
char **gate_set;

int QDMI_control_submit(QDMI_Device dev, QDMI_Fragment *frag, int numshots, QInfo info, QDMI_Job *job)
{
    printf("   [Backend].............QDMI_control_submit\n");

    return QDMI_SUCCESS;
}

int QDMI_control_pack_qasm2(QDMI_Device dev, char *qasmstr, QDMI_Fragment *frag)
{
    printf("   [Backend].............QDMI_control_pack_qasm2\n");

    return QDMI_SUCCESS;
}


int QDMI_control_wait(QDMI_Device dev, QDMI_Job *job, QDMI_Status *status)
{
    bool done = false;
    int flag = QDMI_EXECUTING;

    while (!done)
    {
        // Implementation not needed for Qiskit
        QDMI_control_test(dev, job, &flag, status);

        if (flag == QDMI_COMPLETE)
        {
            break;
        }
        else if (flag == QDMI_HALTED)
        {
            return QDMI_ERROR_BACKEND;
        }

        sleep(5);
    }
    return QDMI_SUCCESS;
}

int QDMI_control_pack_qir(QDMI_Device dev, void *qirmod, QDMI_Fragment *frag)
{
    (*frag)->qirmod = qirmod;

    return QDMI_SUCCESS;
}


int fetch_backend_configuration()
{
    char *conf_ibm = getenv("CONF_IBM");
    if (conf_ibm == NULL)
    {
        printf("   [Backend].............Couldn't open IBM's config file\n");
        return QDMI_ERROR_CONFIG;
    }

    FILE *fp = fopen(conf_ibm, "r");
    if (!fp)
    {
        printf("   [Backend].............Failed to open configuration JSON file\n");
        return QDMI_ERROR_CONFIG;
    }

    json_error_t error;
    root = json_loadf(fp, 0, &error);
    fclose(fp);
    if (!root) 
    {
        fprintf(stderr, "Error parsing JSON: %s\n", error.text);
        return QDMI_ERROR_CONFIG;
    }
    if (!json_is_object(root)) 
    {
        fprintf(stderr, "Root element is not an object.\n");
        return QDMI_ERROR_CONFIG;
    }
    
    return QDMI_SUCCESS;
}

int QDMI_device_status(QDMI_Device dev, QInfo info, int *status)
{
    printf("   [Backend].............IBM (mockup) query device status OK\n");
    *status = 1;
    return QDMI_SUCCESS;
}

int QDMI_backend_init(QInfo info)
{
    printf("   [Backend].............Initializing IBM (mockup) via QDMI\n");

    char *uri = NULL;
    void *regpointer = NULL;
    int err;

    err = QDMI_core_register_belib(uri, regpointer);
    //CHECK_ERR(err, "QDMI_core_register_belib");

    return fetch_backend_configuration();
}
/*
* This allows the QDMI to check if a property exists before querying it by just comparing the string

*/
int QDMI_query_device_property_exists(QDMI_Device dev, QDMI_Device_property prop, int* scope)
{
    //json_t *property = json_object_get(root, "backend_name");
    json_t *property = json_object_get(root, backend_properties[prop->name]);
    if (property) {
        *scope = 1;
        printf("%s property exists\n", backend_properties[prop->name]);
        // Determine the type of the property using json_typeof
        switch (json_typeof(property)) {
            case JSON_STRING:
                prop->type = QDMI_CHARS;
                break;
            case JSON_INTEGER:
                prop->type = QDMI_INT;
                break;
            case JSON_REAL:
                prop->type = QDMI_DOUBLE;
                break;
            case JSON_TRUE:
                prop->type = QDMI_BOOLEAN;
            case JSON_FALSE:
                prop->type = QDMI_BOOLEAN;
                break;
            case JSON_ARRAY:
                prop->type = QDMI_ARRAY;
                break;
            case JSON_OBJECT:
                prop->type = QDMI_OBJECT;
                break;
            case JSON_NULL:
                prop->type = QDMI_NULL;
                break;
            default:
                printf("%s property has an unknown type.\n", backend_properties[prop->name]);
                break;
        }
        return QDMI_SUCCESS;
    } else {
        printf("Queried property doesn't exists: Device property #%d\n", prop->name);
        return QDMI_ERROR_FATAL;
    }
}

int QDMI_query_device_property_c(QDMI_Device dev, QDMI_Device_property prop, char *value)
{
    json_t *property = json_object_get(root, backend_properties[prop->name]);
    if (property && json_is_string(property)) {
        const char *tmp_value = json_string_value(property);
        strcpy(value, tmp_value);
        return QDMI_SUCCESS;
    } else {
        printf("Queried property either doesn't exists or it isn't a string: Device property #%d\n", prop->name);
        return QDMI_ERROR_FATAL;
    }
}

int QDMI_query_device_property_i(QDMI_Device dev, QDMI_Device_property prop, int *value)
{
    json_t *property = json_object_get(root, backend_properties[prop->name]);
    if (property && json_is_integer(property)) {
        *value = json_integer_value(property);
        return QDMI_SUCCESS;
    } else {
        printf("Queried property either doesn't exists or it isn't an integer: Device property #%d\n", prop->name);
        return QDMI_ERROR_FATAL;
    }
}

int QDMI_query_device_property_f(QDMI_Device dev, QDMI_Device_property prop, float *value)
{
    json_t *property = json_object_get(root, backend_properties[prop->name]);
    if (property && json_is_real(property)) {
        *value = (float)json_real_value(property);
        return QDMI_SUCCESS;
    } else {
        printf("Queried property either doesn't exists or it isn't a float: Device property #%d\n", prop->name);
        return QDMI_ERROR_FATAL;
    }
}

int QDMI_query_device_property_d(QDMI_Device dev, QDMI_Device_property prop, double *value)
{
    json_t *property = json_object_get(root, backend_properties[prop->name]);
    if (property && json_is_real(property)) {
        *value = json_real_value(property);
        return QDMI_SUCCESS;
    } else {
        printf("Queried property either doesn't exists or it isn't a double: Device property #%d\n", prop->name);
        return QDMI_ERROR_FATAL;
    }
}

int QDMI_populate_gateset(int num_gates, json_t *basis_gates)
{
    // Populate the gate_set array
    size_t index, i=0;
    json_t *value;
    gate_set = (char **)malloc(num_gates * sizeof(char *));
    json_array_foreach(basis_gates, index, value) {
        if (json_is_string(value)) {
            const char *gate_str = json_string_value(value);
            gate_set[i] = strdup(gate_str);
            if (!gate_set[i]) {
                fprintf(stderr, "Memory allocation failed.\n");
                // Free already allocated memory
                while (i > 0) {
                    free(gate_set[--i]);
                }
                free(gate_set);
                json_decref(root);
                return QDMI_ERROR_BACKEND;
            }
            i++;
        } else {
            printf("Value at index %zu is not a string.\n", index);
            // Free already allocated memory
            while (i > 0) {
                free(gate_set[--i]);
            }
            free(gate_set);
            json_decref(root);
            return QDMI_ERROR_CFGFILE;
        }
    }   
    
    return QDMI_SUCCESS;
}
int QDMI_query_gateset_num(QDMI_Device dev, int *num_gates)
{
    //json_t *basis_gates = json_object_get(root, "basis_gates");
    json_t *basis_gates = json_object_get(root, backend_properties[3]);
    if (basis_gates && json_is_array(basis_gates)) {
        size_t index;
        json_t *value;
        *num_gates = (int)json_array_size(basis_gates);
        return QDMI_populate_gateset(*num_gates, basis_gates);
    }
    else {
        fprintf(stderr, "Key 'basis_gates' not found or its value is not an array.\n");
        return QDMI_ERROR_CFGFILE;
    }

}

void QDMI_get_gate_info(QDMI_Device dev, int gate_index, QDMI_Gate gate)
{
    gate->name     = gate_set[gate_index];

    // Add coupling_mapps for the gate
    json_t *gates_array = json_object_get(root, backend_properties[4]);
    if (!json_is_array(gates_array)) {
        fprintf(stderr, "Error: 'gates' is not an array.\n");
    }

    json_t *gate_obj = json_array_get(gates_array, gate_index);
    if (!json_is_object(gate_obj)) {
        fprintf(stderr, "Error: Invalid gate object.\n");
    }
    json_t *coupling_map_array = json_object_get(gate_obj, "coupling_map");
    if (!json_is_array(coupling_map_array)) {
        //fprintf(stderr, "Error: 'coupling_map' is not an array.\n");
    }
    size_t num_coupling_maps = json_array_size(coupling_map_array);
    gate->size_coupling_map = num_coupling_maps;
    gate->gate_size = 0; // Set later
    
    if(num_coupling_maps != 0){
        // Allocate memory for coupling maps
        gate->coupling_mapping = (QDMI_Gate_property **)malloc((num_coupling_maps) * sizeof(QDMI_Gate_property *));
        if (!gate->coupling_mapping) {
            fprintf(stderr, "Error: Memory allocation failed.\n");
        }
        // Loop through each coupling map
        for (size_t j = 0; j < num_coupling_maps; ++j) {
            json_t *coupling_map = json_array_get(coupling_map_array, j);
            if (!json_is_array(coupling_map)) {
                fprintf(stderr, "Error: Invalid coupling map.\n");
                continue;
            }

            size_t num_connections = json_array_size(coupling_map);
            gate->gate_size = num_connections;

            // Allocate memory for current coupling map
            gate->coupling_mapping[j] = (int *)malloc((num_connections)* sizeof(int));
            if (!gate->coupling_mapping[j]) {
                fprintf(stderr, "Error: Memory allocation failed.\n");
                // Free previously allocated memory
                for (size_t k = 0; k < j; ++k) {
                    free(gate->coupling_mapping[k]);
                }
                free(gate->coupling_mapping);
                continue;
            }
            // Loop through each connection in the coupling map
            for (size_t k = 0; k < num_connections; ++k) {
                json_t *connection = json_array_get(coupling_map, k);
                if (!json_is_integer(connection)) {
                    fprintf(stderr, "Error: Invalid connection.\n");
                    continue;
                }
                int qubit = json_integer_value(connection);
                gate->coupling_mapping[j][k] = qubit; 

            }
        }
    }
    // Set coupling_mapping to NULL if the backend doesn't provide it
    else {
        gate->coupling_mapping = NULL;
    }   
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

int QDMI_query_byname(QDMI_Device dev, char *name, QDMI_Gate *gate)
{
    int i, num_gates = 0;
    QDMI_query_gateset_num(dev, &num_gates);

    for (i = 0; i < num_gates; i++) {
        if (strcmp(gate_set[i], name) == 0)
        {
            QDMI_get_gate_info(dev, i, *gate);
            return QDMI_SUCCESS;
        }
    }
}

int QDMI_query_gate_name(QDMI_Device dev, QDMI_Gate gate, char* name, int* len)
{
    *len = strlen(gate->name);
    strcpy(name, gate->name);
    return QDMI_SUCCESS;
}


int QDMI_control_readout_size(QDMI_Device dev, QDMI_Status *status, int *numbits)
{
    //printf("   [Backend]..............Returning size\n");
    *numbits = 5;//QDMI_query_qubits_num(dev, *numbits);
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

/*The coupling map for IBM Athens backend is this:

"coupling_map": [
        [0, 1],
        [1, 0],
        [1, 2],
        [2, 1],
        [2, 3],
        [3, 2],
        [3, 4],
        [4, 3]
    ]

* Following method reads the sets the coupling mapping for the corresponding qubit, 
    assuming the coupling map is in the above format
* The space and time complexity of this method can be further improved and 
    this version is just a prototype.
*/
int QDMI_set_coupling_mapping(QDMI_Device dev, int qubit_index, QDMI_Qubit qubit)
{
    qubit->index = qubit_index;

    json_t *coupling_map = json_object_get(root, backend_properties[5]); // "coupling_map"
    size_t num_coupling_maps = json_array_size(coupling_map);
    int *temp_array = (int *)malloc(num_coupling_maps * sizeof(int));
    int count = 0;

    if (!json_is_array(coupling_map)) {
        fprintf(stderr, "'coupling_map' for qubit #%d is null.\n", qubit_index);
        qubit->coupling_mapping = NULL;
        return QDMI_SUCCESS;
    }
    // Traverse the "coupling_map" array
    for (size_t i = 0; i < num_coupling_maps; ++i) {
        json_t *pair = json_array_get(coupling_map, i);
        if (json_is_array(pair) && json_array_size(pair) == 2) {
            json_t *first_index = json_array_get(pair, 0);
            json_t *second_index = json_array_get(pair, 1);

            // Check if the first index is equal to the qubit_index
            if (json_is_integer(first_index)) {
                if(json_integer_value(first_index) == qubit_index) {
                    if (json_is_integer(second_index)) {
                        temp_array[count] = json_integer_value(second_index);
                        count++;
                    } else {
                        fprintf(stderr, "Second index is not an integer for qubit .\n");
                        return QDMI_ERROR_CONFIG;
                    }
                }
                else if(json_integer_value(first_index) < qubit_index) {
                    continue;
                }
                else {
                    break;
                }
            }
            else {
                fprintf(stderr, "First index is not an integer for qubit #%d. Check the config file!!!\n", qubit_index);
                return QDMI_ERROR_CONFIG;
            }
        } else {
            // TODO
            //fprintf(stderr, "Invalid format for pair at index %zu for qubit #.\n");
            fprintf(stderr, "Invalid format.\n");
            return QDMI_ERROR_CONFIG;
        }
    }

    qubit->coupling_mapping = (QDMI_qubit_index*)malloc(count * sizeof(QDMI_qubit_index));
    for (int i = 0; i < count; i++) {
        qubit->coupling_mapping[i] = temp_array[i];
    }
    qubit->size_coupling_mapping = count;
    free(temp_array);
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

    printf("   [Backend]..............Returning available qubits\n");
    return QDMI_SUCCESS;
}

int QDMI_query_qubits_num(QDMI_Device dev, int *num_qubits)
{
    json_t *property = json_object_get(root, backend_properties[2]);
    if (property && json_is_integer(property)) {
        *num_qubits = json_integer_value(property);
        return QDMI_SUCCESS;
    } else {
        printf("Queried property either doesn't exists or it isn't an integer: Device property - num_qubits\n");
        return QDMI_ERROR_FATAL;
    }
    return QDMI_SUCCESS;
}
