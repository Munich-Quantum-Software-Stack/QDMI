#include <string.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <unistd.h>

#include "qdmi_backend_planqc.h"

#include "include/apiClient.h"
#include "api/InfoAPI.h"
#include "api/OpenqasmAPI.h"

#define RING_BUFFER_SIZE 100 // Adjust this value to your needs

typedef struct
{
    char *buffer[RING_BUFFER_SIZE];
    int start;
    int end;
} RingBuffer;

// Initialize the ring buffer
void ring_buffer_init(RingBuffer *rb)
{
    rb->start = 0;
    rb->end = 0;
}

// Add a UUID to the ring buffer
int ring_buffer_push(RingBuffer *rb, char *uuid)
{
    int pos = rb->end;
    rb->buffer[pos] = strdup(uuid);
    rb->end = (pos + 1) % RING_BUFFER_SIZE;
    if (rb->end == rb->start)
    {
        // The buffer is full, so we remove the oldest UUID
        free(rb->buffer[rb->start]);
        rb->start = (rb->start + 1) % RING_BUFFER_SIZE;
    }
    return pos;
}

// Get a UUID from the ring buffer by index
char *ring_buffer_get(RingBuffer *rb, int index)
{
    if (index < 0 || index >= RING_BUFFER_SIZE)
    {
        // The index is out of bounds
        return NULL;
    }
    return rb->buffer[(rb->start + index) % RING_BUFFER_SIZE];
}

RingBuffer g_ring_buffer;
apiClient_t *g_api_client;

int QDMI_backend_init(QInfo info)
{
    printf("   [Backend].............Initializing planqc via QDMI\n");

    char *uri = NULL;
    void *regpointer = NULL;
    int err;

    err = QDMI_core_register_belib(uri, regpointer);
    // CHECK_ERR(err, "QDMI_core_register_belib");

    ring_buffer_init(&g_ring_buffer);
    g_api_client = apiClient_create(); // Create an API client
    g_api_client->basePath = strdup("http://localhost:5000");

    return QDMI_SUCCESS;
}

// get status of device.
int QDMI_device_status(QDMI_Device dev, QInfo qinfo, int *status)
{
    printf("   [Backend].............planqc query device status\n");

    *status = DEVICE_STATUS_OFFLINE;

    // Get info
    info_t *info = InfoAPI_apiInfo(g_api_client);
    if (!g_api_client->response_code || info == NULL)
    {
        printf("   [Backend].............planqc query request failed\n");
        // Cleanup
        apiClient_free(g_api_client); // Free the API client when you're done with it
        return QDMI_ERROR_FATAL;      // Or some other appropriate error code
    }

    if (strcmp(info->status, "up") == 0)
    {
        *status = DEVICE_STATUS_ONLINE;
    }

    // Cleanup
    free(info);

    return QDMI_SUCCESS;
}

int QDMI_control_pack_qasm2(QDMI_Device dev, char *qasmstr, QDMI_Fragment *frag)
{
    if (!frag)
        return QDMI_ERROR_FATAL;
    if (!qasmstr)
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

char *base64(const char *input)
{
    BIO *bio, *b64;
    BUF_MEM *bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_write(bio, input, strlen(input));
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);
    BIO_set_close(bio, BIO_NOCLOSE);
    BIO_free_all(bio);

    char *b64text = (char *)malloc((bufferPtr->length + 1) * sizeof(char));
    memcpy(b64text, bufferPtr->data, bufferPtr->length);
    b64text[bufferPtr->length] = '\0';

    return b64text;
}

int QDMI_control_submit(QDMI_Device dev, QDMI_Fragment *frag, int numshots, QInfo info, QDMI_Job *job)
{
    printf("   [Backend].............planqc submit job\n");
    printf("   [Backend].............planqc qasm: %s\n", (*frag)->qasmstr);

    char *origin = "LRZ";
    char *token = "6288f98721a7246c4b25638edddbd50e44ddbee732a9fb6f7461f3cbbc9ef990NzU4MjEyMzQtYmZkNi00YmZkLWEyYzQtZTY2ODdjNWNlODU3O2RhbmllbC5oYWFnOzE3MTcxOTI3NDAuMA==";

    char *b64_encoded = base64((*frag)->qasmstr);

    open_qasm_request_t *request = open_qasm_request_create(b64_encoded, numshots, origin, token);
    job_accepted_t *response = OpenqasmAPI_apiSubmit(g_api_client, request);

    if (g_api_client->response_code != 202)
    {
        printf("Error code {%li}: Server Error\n", g_api_client->response_code);
        return QDMI_ERROR_FATAL;
    }

    if (!response)
    {
        printf("Resquest not successful\n");
        return QDMI_ERROR_FATAL;
    }

    (*job)->task_id = ring_buffer_push(&g_ring_buffer, response->job_id);

    printf("   [Backend].............planqc response job_id: %s -> QDMI job.task_id: %i\n", response->job_id, (*job)->task_id);

    free(b64_encoded);
    return QDMI_SUCCESS;
}

int QDMI_control_test(QDMI_Device dev, QDMI_Job *job, int *flag, QDMI_Status *status)
{
    char *job_id = ring_buffer_get(&g_ring_buffer, (*job)->task_id);
    job_status_t *response = OpenqasmAPI_apiStatus(g_api_client, job_id);

    if (g_api_client->response_code != 200)
    {
        printf("Error code {%li}: Server Error\n", g_api_client->response_code);
        return QDMI_ERROR_FATAL;
    }

    if (response->status == planqcapi_job_status_STATUS_NULL) {
        printf("Missing Status\n");
        return QDMI_ERROR_FATAL;
    }

    switch (response->status) {
        case planqcapi_job_status_STATUS_queued:
            *flag = QDMI_WAITING;
            break;
        case planqcapi_job_status_STATUS_running:
            *flag = QDMI_EXECUTING;
            break;
        case planqcapi_job_status_STATUS_completed:
            *flag = QDMI_COMPLETE;
            break;
        case planqcapi_job_status_STATUS_failed:
            *flag = QDMI_HALTED;
            break;
        default:
            printf("Unexpected status: %d\n", response->status);
            return QDMI_ERROR_FATAL;
    }

    return QDMI_SUCCESS;
}

const char* get_flag_status_str(int flag) {
    switch (flag) {
        case QDMI_WAITING:
            return "QDMI_WAITING";
        case QDMI_EXECUTING:
            return "QDMI_EXECUTING";
        case QDMI_COMPLETE:
            return "QDMI_COMPLETE";
        case QDMI_HALTED:
            return "QDMI_HALTED";
        default:
            return "UNKNOWN";
    }
}

int QDMI_control_wait(QDMI_Device dev, QDMI_Job *job, QDMI_Status *status)
{
    printf("   [Backend].............planqc wait job\n");
    int flag = QDMI_EXECUTING;

    while (flag == QDMI_EXECUTING || flag == QDMI_WAITING)
    {
        QDMI_control_test(dev, job, &flag, status);
        printf("   [Backend].............planqc task %i status: %s\n", (*job)->task_id, get_flag_status_str(flag));

        sleep(0.1);
    }

    return QDMI_SUCCESS;
}

void process_list_entry(listEntry_t* entry, void* additionalData) {
    char* binary_str = (char*)entry->data;
    int* num = (int*)additionalData;

    //printf("[Debug] RESULT: %s\n", binary_str);

    for (int i = 0; binary_str[i] != '\0'; i++) {
        if (binary_str[i] == '1') {
            num[i]++;
        }
    }
}

int QDMI_control_readout_raw_num(QDMI_Device dev, QDMI_Status *status, int task_id, int *num)
{
    printf("   [Backend].............Returning results\n");

    char *job_id = ring_buffer_get(&g_ring_buffer, task_id);
    open_qasm_response_t *response = OpenqasmAPI_apiResult(g_api_client, job_id);

    list_iterateThroughListForward(response->results, process_list_entry, num);

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
