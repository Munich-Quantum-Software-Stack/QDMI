#include <string.h>
#include <unistd.h>

#include "qdmi_backend_planqc.h"

#include "planqc/generated/include/apiClient.h"
#include "planqc/generated/api/InfoAPI.h"

int QDMI_backend_init(QInfo info)
{
    printf("   [Backend].............Initializing planqc via QDMI\n");

    char *uri = NULL;
    void *regpointer = NULL;
    int err;

    err = QDMI_core_register_belib(uri, regpointer);
    //CHECK_ERR(err, "QDMI_core_register_belib");

    return QDMI_SUCCESS;
}

// get status of device.
int QDMI_device_status(QDMI_Device dev, QInfo qinfo, int *status)
{
    printf("   [Backend].............planqc query device status\n");

    *status = DEVICE_STATUS_OFFLINE;


    apiClient_t *api_client = apiClient_create(); // Create an API client
    api_client->basePath = strdup("http://localhost:5000");

    // Get info
    info_t *info = InfoAPI_apiInfo(api_client);
    printf("Status: %s\n", info->status);

    if (strcmp(info->status, "up") == 0) {
        *status = DEVICE_STATUS_ONLINE;
    }

    // Cleanup
    free(info);
    apiClient_free(api_client); // Free the API client when you're done with it

    return QDMI_SUCCESS;
}


int QDMI_control_pack_qasm2(QDMI_Device dev, char *qasmstr, QDMI_Fragment *frag)
{
    if(!frag)
        return QDMI_ERROR_FATAL;
    if(!qasmstr)
        return QDMI_ERROR_FATAL;

    (*frag)->qasmstr = malloc(strlen(qasmstr) + 1);
    if (!(*frag)->qasmstr)
        return QDMI_ERROR_FATAL;

    strncpy((*frag)->qasmstr, qasmstr, strlen(qasmstr) + 1);

    return QDMI_SUCCESS;
}

int QDMI_control_readout_size(QDMI_Device dev, QDMI_Status *status, int *numbits)
{
    *numbits = 2;
    return QDMI_SUCCESS;
}

int QDMI_control_submit(QDMI_Device dev, QDMI_Fragment *frag, int numshots, QInfo info, QDMI_Job *job)
{
    printf("   [Backend].............planqc submit job\n");
    printf("   [Backend].............planqc qasm: %s\n", (*frag)->qasmstr);
    return QDMI_SUCCESS;
}

int QDMI_control_test(QDMI_Device dev, QDMI_Job *job, int *flag, QDMI_Status *status)
{
    printf("   [Backend].............planqc test job\n");
    *flag = QDMI_COMPLETE;
    return QDMI_SUCCESS;

}

int QDMI_control_wait(QDMI_Device dev, QDMI_Job *job, QDMI_Status *status)
{
    printf("   [Backend].............planqc wait job\n");
    int flag = QDMI_EXECUTING;

    while (!flag)
    {
        QDMI_control_test(dev, job, &flag, status);
        sleep(5);
    }
    return QDMI_SUCCESS;
}


int QDMI_control_readout_raw_num(QDMI_Device dev, QDMI_Status *status, int task_id, int *num)
{
    printf("   [Backend].............Returning results\n");

    num[0] = 0;
    num[1] = 1;
    return QDMI_SUCCESS;
}

int QDMI_query_qubits_num(QDMI_Device dev, int *num_qubits)
{
    return QDMI_ERROR_NOTIMPL;
}

int QDMI_query_all_qubits(QDMI_Device dev, QDMI_Qubit *qubits)
{
    return QDMI_ERROR_NOTIMPL;
}

int QDMI_query_all_gates(QDMI_Device dev, QDMI_Gate *gates)
{
    return QDMI_ERROR_NOTIMPL;
}
