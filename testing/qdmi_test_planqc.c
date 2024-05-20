#include "qdmi_internal.h"
#include <qdmi.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#define CHECK_ERR(a,b) { if (a!=QDMI_SUCCESS) { printf("\n[Test] [Error]: %i at %s\n",a,b); return 1; }}


void* malloc_or_exit(size_t size) {
    void* ptr = malloc(size);
    if (!ptr) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    return ptr;
}


int main(int argc, char** argv)
{

    int err = 0, status = 0;
    int index = 0;
    int n_shots = 100;
    int max_readout_size = 10;
    QInfo info = NULL;
    QDMI_Session session = NULL;
    QDMI_Fragment frag = (QDMI_Fragment) malloc_or_exit(sizeof(*frag));
    QDMI_Device device = (QDMI_Device) malloc_or_exit(sizeof(*device));


    err = QInfo_create(&info);
    CHECK_ERR(err, "QInfo_create");

    err = QDMI_session_init(info, &session);
    CHECK_ERR(err, "QDMI_session_init");

    char** avaiable_devices = get_qdmi_library_list_names();

    float elapsed_time = 0;
    float sleep_duration = 1.0;
    float timeout = 10.0;

    while (avaiable_devices[index]) {
        QDMI_Library lib = find_library_by_name(avaiable_devices[index]);
        if(!lib){
            printf("Library is not found!\n");
            free(avaiable_devices[index++]);
            continue;
        }

        device->library = *lib;
        err = QDMI_device_status(device, device->library.info, &status);

        if(status){
            const char *device_name = strrchr(avaiable_devices[index], '/');
            printf("[Test] %s is found.\n", device_name);
            break;
        }

        sleep(sleep_duration);
        elapsed_time += sleep_duration;

        printf("[Test] Waiting for device. Elapsed time: %f\n", elapsed_time);

        if (elapsed_time >= timeout) {
            printf("[Test] Timeout reached. Device status did not become 1 after %f seconds.\n", timeout);
            exit(EXIT_FAILURE);
        }
    }

    // Bell state
    char* qasmstr = "OPENQASM 3.0;\ninclude \"stdgates.inc\";\nbit[2] c;\nqubit[2] q;\nh q[0];\ncx q[0], q[1];\nc[0] = measure q[0];\nc[1] = measure q[1];\n";


    err = QDMI_control_pack_qasm2(device, qasmstr, &frag);
    CHECK_ERR(err, "QDMI_control_pack_qasm2");

    QDMI_Job job = (QDMI_Job) malloc_or_exit(sizeof(*job));
    err = QDMI_control_submit(device, &frag, n_shots,
                                  device->library.info, &job);
    CHECK_ERR(err, "QDMI_control_submit");


    QDMI_Status qdmiStatus = 1; // Not used
    err = QDMI_control_wait(device, &job, &qdmiStatus);
    CHECK_ERR(err, "QDMI_control_wait");

    int numbits = -1;
    err = QDMI_control_readout_size(device, &qdmiStatus, &numbits);
    CHECK_ERR(err, "QDMI_control_readout_size");

    if (numbits < 0) {
        printf("[Test] Error: numbits < 0 \n");
        return 1;
    }

    if (numbits > max_readout_size)
    {
        printf("[Test] Error: numbits > %i, to large to allocate memory\n", max_readout_size);
        return 1;
    }

    size_t result_size = pow(2, numbits);
    int *result = (int *)calloc(result_size, sizeof(int));
    if (!result)
    {
        printf("[Test] Error: calloc failed for readout\n");
        return 1;
    }

    err = QDMI_control_readout_raw_num(device, &qdmiStatus, job->task_id, result);
    CHECK_ERR(err, "QDMI_control_readout_raw_num");

    free(job);
    for (int i = 0; i < result_size; i++)
    {
        printf("[Test] num[%d] = %d\n", i, result[i]);
    }

    free(result);
}

