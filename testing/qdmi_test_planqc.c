#include "qdmi_internal.h"
#include <qdmi.h>
#include <stdio.h>
#include <llvm-c/Core.h>
#include <llvm-c/IRReader.h>
#include <string.h>

#define CHECK_ERR(a,b) { if (a!=QDMI_SUCCESS) { printf("\n[Error]: %i at %s\n",a,b); return 1; }}


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
    QInfo info = NULL;
    QDMI_Session session = NULL;
    QDMI_Fragment frag = (QDMI_Fragment) malloc_or_exit(sizeof(*frag));
    QDMI_Device device = (QDMI_Device) malloc_or_exit(sizeof(*device));

    LLVMModuleRef qirModule;

    err = QInfo_create(&info);
    CHECK_ERR(err, "QInfo_create");

    err = QDMI_session_init(info, &session);
    CHECK_ERR(err, "QDMI_session_init");

    char** avaiable_devices = get_qdmi_library_list_names();

    while (avaiable_devices[index]) {
        QDMI_Library lib = find_library_by_name(avaiable_devices[index]);
        if(!lib){
            printf("Library is not found!");
            free(avaiable_devices[index++]);
            continue;
        }

        device->library = *lib;
        err = QDMI_device_status(device, device->library.info, &status);

        if(status){
            const char *device_name = strrchr(avaiable_devices[index], '/');
            printf("%s is found", device_name);
            break;
        }

    }
    char* qasmstr = "OPENQASM 2.0;\ninclude \"qelib1.inc\";\nqreg q[2];\ncreg c[2];\nh q[0];\ncx q[0],q[1];\nmeasure q[0] -> c[0];\nmeasure q[1] -> c[1];\n";

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
        printf("Error: numbits < 0");
        return 1;
    }

    int* result = malloc_or_exit(numbits * sizeof(int));

    err = QDMI_control_readout_raw_num(device, &qdmiStatus, job->task_id, result);
    CHECK_ERR(err, "QDMI_control_readout_raw_num");

    free(job);
    for (int i = 0; i < numbits; i++) {
        printf("num[%d] = %d\n", i, result[i]);
    }

    free(result);
}

