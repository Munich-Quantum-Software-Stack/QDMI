/*------------------------------------------------------------------------------
Part of the MQSS Project, under the Apache License v2.0 with LLVM Exceptions.
See https://llvm.org/LICENSE.txt for license information.
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <qdmi.h>
int QDMI_session_init(QInfo info, QDMI_Session *session);
int QDMI_session_finalize(QDMI_Session session);
//QDMI_Library find_library_by_name(const char *libname);

#define CHECK_ERR(a, b)                          \
    {                                            \
        if (a != QDMI_SUCCESS)                   \
        {                                        \
            printf("\n[Error]: %i at %s", a, b); \
            return 1;                            \
        }                                        \
    }

int main(int argc, char **argv)
{

    QInfo info;
    QDMI_Device* devices;
    QDMI_Session session = NULL;
    QDMI_Fragment frag;
    int err, count = 0;
    err = QInfo_create(&info);
    CHECK_ERR(err, "QInfo_create");

    err = QDMI_session_init(info, &session);
    CHECK_ERR(err, "QDMI_session_init");

    QDMI_core_device_count(&session, &count);

    printf("%d\n", count);
    devices = malloc(sizeof(QDMI_Device) * count);
    
    for(int i = 0; i < count; i++){
        err = QDMI_core_open_device(&session, i, &info, &devices[i]);
        CHECK_ERR(err, "QDMI_core_open_device");
        if(devices[i] == NULL)
            printf("HELLO WORLD!!!\n");
    }

    QDMI_control_pack_qasm2(devices[0], "testsfrt", &frag);
    


    /*
    putenv("TOKEN_WMI=../inputs/token.txt");
    QInfo info;
    QDMI_Session session = NULL;
    //QDMI_Library lib;
    QDMI_Fragment frag;
    QDMI_Device device;
    QDMI_Job job;
    int err, count = 0;

    job = malloc(sizeof(struct QDMI_Job_impl_d));
    if (device == NULL)
    {
        printf("\n[ERROR]: Job could not be created");
        exit(EXIT_FAILURE);
    }

    device = malloc(sizeof(struct QDMI_Device_impl_d));
    if (device == NULL)
    {
        printf("\n[ERROR]: Device could not be created");
        printf("\n[ERROR]: Device could not be created");
        exit(EXIT_FAILURE);
    }

    err = QInfo_create(&info);
    CHECK_ERR(err, "QInfo_create");

    // QDMI_session_init(info, &session) -> QDMI_internal_startup(info) -> QDMI_load_libraries(info) -> QInfo_create(&(newlib->info))

    QInfo info;    CHECK_ERR(err, "QDMI_session_init");

    frag = (QDMI_Fragment)malloc(sizeof(struct QDMI_Fragment_d));
    if (frag == NULL)
    {
        QDMI_session_finalize(session);

        printf("\n[ERROR]: The fragment could not be created");
        exit(EXIT_FAILURE);
    }

    // put readable qir in qirmod.
    char *buffer = 0;
    long length;
    FILE *f = fopen("../inputs/circuit_excited.bc", "rb");
    fseek(f, 0, SEEK_END);
    frag->sizebuffer = ftell(f);
    fseek(f, 0, SEEK_SET);
    frag->qirmod = malloc(frag->sizebuffer);
    fread(frag->qirmod, 1, frag->sizebuffer, f);
    fclose(f);

    lib = find_library_by_name("/home/martin/bin/lib/libbackend_wmi.so");
    if (!lib)
    {
        printf("\n[ERROR]: Library could not be found");
        exit(EXIT_FAILURE);
    }
    device->library = *lib;

    int num_qubits;
    err = QDMI_query_qubits_num(device, &num_qubits);
    CHECK_ERR(err, "QDMI_query_qubits_num");

    int status = 0;
    err = QDMI_device_status(device, device->library.info, &status);
    CHECK_ERR(err, "QDMI_device_status");

    // job_id as random int
    srand(time(NULL));
    int task_id = rand();
    job->task_id = task_id;
    err = QDMI_control_submit(device, &frag, 1024, device->library.info, &job);
    CHECK_ERR(err, "QDMI_control_submit");

    int wait_status = QDMI_control_wait(device, &job, &status);

    // init results array
    int state_space = 1;
    for (int i; i < num_qubits; i++)
    {
        state_space *= 2;
    }
    int num[state_space];
    for (int i = 0; i < state_space; i++)
    {
        num[i] = 0;
    }

    err = QDMI_control_readout_raw_num(device, &status, job->task_id, num);
    CHECK_ERR(err, "QDMI_control_readout_raw_num");

    for (int i = 0; i < state_space; i++)
    {
        printf("Measurement: %i counts:: %i\n", i, num[i]);
    }

    // QDMI_session_finalize(session) -> QDMI_internal_shutdown()
    err = QDMI_session_finalize(session);
    CHECK_ERR(err, "QDMI_session_finalize");

    err = QInfo_free(info);
    CHECK_ERR(err, "QInfo_free");

    free(frag->qirmod);
    free(frag);
    free(device);
    free(job);

    printf("\n[DEBUG]: Test Finished\n\n");

    */
    return 0;
}
