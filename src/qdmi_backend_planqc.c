#include <string.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <unistd.h>
#include <math.h>

#include "qdmi_backend_planqc.h"

#include "include/apiClient.h"
#include "api/InfoAPI.h"
#include "api/JobAPI.h"
#include "api/BackendAPI.h"

#define CHECK_ERR(a, b)                                         \
    {                                                           \
        if (a != QDMI_SUCCESS)                                  \
        {                                                       \
            printf("\n   [Backend] [Error]: %i at %s\n", a, b); \
            return 1;                                           \
        }                                                       \
    }

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
        rb->buffer[rb->start] = NULL;
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
char *g_api_token;
char *g_backend_id;
int g_num_qubits;

int get_config(QInfo info, char *key, char **value)
{
    QInfo_topic topic;
    int err = QInfo_topic_query(info, key, &topic);
    if (err != QINFO_SUCCESS)
    {
        return err;
    }

    QInfo_value val;
    err = QInfo_topic_get(info, topic, &val);
    if (err != QINFO_SUCCESS)
    {
        return err;
    }

    *value = val.value_string;
    return QINFO_SUCCESS;
}

int planqc_device_status(backend_status_response_t **backend_status)
{
    // Get info
    info_t *info = InfoAPI_info(g_api_client);
    if (!g_api_client->response_code || info == NULL)
    {
        printf("   [Backend].............planqc query request failed\n");
        return QDMI_ERROR_FATAL; // Or some other appropriate error code
    }

    if (strcmp(info->status, "up") != 0)
    {
        printf("   [Backend].............planqc platform is offline\n");
        free(info);
        return QDMI_SUCCESS;
    }
    printf("   [Backend].............planqc platform is online\n");

    free(info);

    *backend_status = BackendAPI_backendStatus(g_api_client, g_backend_id);
    return QDMI_SUCCESS;
}

int QDMI_backend_init(QInfo info)
{
    printf("   [Backend].............Initializing planqc via QDMI\n");

    char *uri = NULL;
    void *regpointer = NULL;
    int err;

    err = QDMI_core_register_belib(uri, regpointer);
    // CHECK_ERR(err, "QDMI_core_register_belib");

    ring_buffer_init(&g_ring_buffer);

    char *api_endpoint = NULL;
    err = get_config(info, "api_endpoint", &api_endpoint);
    CHECK_ERR(err, "get_config api_endpoint");

    g_api_client = apiClient_create(); // Create an API client
    g_api_client->basePath = strdup(api_endpoint);

    err = get_config(info, "api_token", &g_api_token);
    CHECK_ERR(err, "get_config api_token");

    if (strlen(g_api_token) == 0)
    {
        printf("   [Backend].............planqc api_token is empty\n");
        return QDMI_ERROR_FATAL;
    }

    err = get_config(info, "backend_id", &g_backend_id);
    CHECK_ERR(err, "get_config backend_id");

    if (strlen(g_backend_id) == 0)
    {
        printf("   [Backend].............planqc backend_id is empty\n");
        return QDMI_ERROR_FATAL;
    }

    // Success
    printf("   [Backend].............planqc Backend %s initialized\n", g_backend_id);
    printf("   [Backend].............planqc Backend Connection URL: %s \n", g_api_client->basePath);

    backend_status_response_t *backend_status;
    err = planqc_device_status(&backend_status);
    if (err != QDMI_SUCCESS)
        return err;

    if (backend_status == NULL)
    {
        printf("   [Backend].............planqc Error: backend_status is NULL\n");
        return QDMI_ERROR_FATAL;
    }

    g_num_qubits = backend_status->number_of_qubits;

    printf("   [Backend].............planqc Backend version: %s \n", backend_status->version);
    printf("   [Backend].............planqc Backend status: %i \n", backend_status->status);
    printf("   [Backend].............planqc Backend number of qubits: %i \n", backend_status->number_of_qubits);

    backend_status_response_free(backend_status);

    return QDMI_SUCCESS;
}

// get status of device.
int QDMI_device_status(QDMI_Device dev, QInfo qinfo, int *status)
{
    printf("   [Backend].............planqc query device status\n");

    *status = DEVICE_STATUS_OFFLINE;

    backend_status_response_t *backend_status;
    int err = planqc_device_status(&backend_status);
    if (err != QDMI_SUCCESS)
        return err;

    if (backend_status->status != planqcapi_backend_status_response_STATUS_idle &&
        backend_status->status != planqcapi_backend_status_response_STATUS_running)
    {
        printf("   [Backend].............planqc backend is offline.\n");
        free(backend_status);
        return QDMI_SUCCESS;
    }
    printf("   [Backend].............planqc backend is online.\n");
    backend_status_response_free(backend_status);

    *status = DEVICE_STATUS_ONLINE;

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
    *numbits = g_num_qubits;
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
    char *qasmstr_base64 = base64((*frag)->qasmstr);
    if (!qasmstr_base64)
    {
        printf("   [Backend].............planqc Error: base64 encoding failed\n");
        return QDMI_ERROR_FATAL;
    }

    job_request_t *request = job_request_create(qasmstr_base64, numshots, origin, g_api_token);
    if (!request)
    {
        printf("   [Backend].............planqc Error: request creation failed\n");
        free(qasmstr_base64);
        return QDMI_ERROR_FATAL;
    }

    job_request_response_t *response = JobAPI_jobSubmit(g_api_client, g_backend_id, request);

    free(request);
    free(qasmstr_base64);

    if (g_api_client->response_code != 202)
    {
        printf("   [Backend].............planqc Error code {%li}: Server Error\n", g_api_client->response_code);
        return QDMI_ERROR_FATAL;
    }

    if (!response)
    {
        printf("   [Backend].............planqc Resquest not successful\n");
        return QDMI_ERROR_FATAL;
    }

    (*job)->task_id = ring_buffer_push(&g_ring_buffer, response->job_id);

    printf("   [Backend].............planqc response job_id: %s -> QDMI job.task_id: %i\n", response->job_id, (*job)->task_id);

    free(response);
    return QDMI_SUCCESS;
}

int QDMI_control_test(QDMI_Device dev, QDMI_Job *job, int *flag, QDMI_Status *status)
{
    char *job_id = ring_buffer_get(&g_ring_buffer, (*job)->task_id);
    job_response_t *response = JobAPI_job(g_api_client, g_backend_id, job_id);

    if (g_api_client->response_code != 200)
    {
        printf("Error code {%li}: Server Error\n", g_api_client->response_code);
        return QDMI_ERROR_FATAL;
    }

    if (!response)
    {
        printf("   [Backend].............planqc Resquest could not be parsed\n");
        return QDMI_ERROR_FATAL;
    }

    if (response->status->status == planqcapi_job_status_response_STATUS_NULL)
    {
        printf("Missing Status\n");
        return QDMI_ERROR_FATAL;
    }

    switch (response->status->status)
    {
    case planqcapi_job_status_response_STATUS_queued:
        *flag = QDMI_WAITING;
        break;
    case planqcapi_job_status_response_STATUS_running:
        *flag = QDMI_EXECUTING;
        break;
    case planqcapi_job_status_response_STATUS_completed:
        *flag = QDMI_COMPLETE;
        break;
    case planqcapi_job_status_response_STATUS_failed:
        *flag = QDMI_HALTED;
        break;
    default:
        printf("Unexpected status: %d\n", response->status->status);
        return QDMI_ERROR_FATAL;
    }

    return QDMI_SUCCESS;
}

const char *get_flag_status_str(int flag)
{
    switch (flag)
    {
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

        sleep(1);
    }

    return QDMI_SUCCESS;
}

void process_result(listEntry_t *entry, void *additionalData)
{
    size_t result_size = pow(2, g_num_qubits);
    char *binary_str = (char *)entry->data;
    int *num = (int *)additionalData;

    int result_index = (int)strtol(binary_str, NULL, 2);

    if (result_index < 0 || result_index >= result_size)
    {
        printf("   [Backend].............planqc Error: result index out of bounds\n");
        return;
    }

    num[result_index]++;
}

int QDMI_control_readout_raw_num(QDMI_Device dev, QDMI_Status *status, int task_id, int *num)
{
    printf("   [Backend].............planqc returning results\n");

    char *job_id = ring_buffer_get(&g_ring_buffer, task_id);
    job_response_t *response = JobAPI_job(g_api_client, g_backend_id, job_id);

    if (g_api_client->response_code != 200)
    {
        printf("Error code {%li}: Server Error\n", g_api_client->response_code);
        return QDMI_ERROR_FATAL;
    }

    if (!response)
    {
        printf("   [Backend].............planqc Resquest not successful\n");
        return QDMI_ERROR_FATAL;
    }

    if (response->status->status != planqcapi_job_status_response_STATUS_completed)
    {
        printf("   [Backend].............planqc Job not completed\n");
        return QDMI_ERROR_FATAL;
    }

    if (response->results == NULL)
    {
        printf("   [Backend].............planqc No results\n");
        return QDMI_ERROR_FATAL;
    }

    list_iterateThroughListForward(response->results, process_result, num);

    *status = QDMI_COMPLETE;

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
