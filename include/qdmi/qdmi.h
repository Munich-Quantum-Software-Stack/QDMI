/*------------------------------------------------------------------------------
  Part of the MQSS Project, under the Apache License v2.0 with LLVM Exceptions.
  See https://llvm.org/LICENSE.txt for license information.
  SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
------------------------------------------------------------------------------*/

#pragma once

#include <qinfo.h>

/*----------------------------------------*/
/* QDMI Constants for API usage*/

/* Timing Units */
#define QDMI_TIME_S 0
#define QDMI_TIME_MS 1
#define QDMI_TIME_US 2
#define QDMI_TIME_NS 3
#define QDMI_TIME_PS 4

/* Behavior of QDMI properties */
#define QDMI_PROPERTY_NOTEXIST -1
#define QDMI_PROPERTY_STATIC 1
#define QDMI_PROPERTY_DYNAMIC 2

/* Type of QDMI properties*/
#define QDMI_INT 0
#define QDMI_FLOAT 1
#define QDMI_DOUBLE 2
#define QDMI_CHARS 3
#define QDMI_BOOLEAN 4
#define QDMI_ARRAY 5
#define QDMI_OBJECT 6
#define QDMI_NULL 7

/* Status values to be used per job */
#define QDMI_COMPLETE 0
#define QDMI_EXECUTING 1
#define QDMI_WAITING 2
#define QDMI_HALTED 3

/* Error classes, i.e., status returned by API */
#define QDMI_WARN_NOBACKEND 2
#define QDMI_WARN_GENERAL 1
#define QDMI_SUCCESS 0
#define QDMI_ERROR_FATAL -1
#define QDMI_ERROR_OUTOFMEM -2
#define QDMI_ERROR_NOSESSION -3
#define QDMI_ERROR_NOCFGFILE -4
#define QDMI_ERROR_CONFIG -5
#define QDMI_ERROR_CFGFILE -6
#define QDMI_ERROR_BACKEND -7
#define QDMI_ERROR_NOTIMPL -8
#define QDMI_ERROR_NOTSUPPORTED -9

#define QDMI_IS_SUCCESS(err) (err == QDMI_SUCCESS)
#define QDMI_IS_WARNING(err) (err > QDMI_SUCCESS)
#define QDMI_IS_ERROR(err) (err != QDMI_SUCCESS)
#define QDMI_IS_FATAL(err) (err < QDMI_SUCCESS)

/*----------------------------------------*/
/* QDMI Constants to represent platform items*/

/*  Properties of the backend*/
#define BACKEND_NAME 0
#define BACKEND_VERSION 1
#define NUM_QUBITS 2
#define BASIS_GATES 3
#define GATES_INFO 4
#define COUPLING_MAP 5

/* Query interface: Global device properties */
#define QDMI_QUBIT_COUNT 0
#define QDMI_NATIVE_GATE_COUNT 1

/* Query Interface: Gate and Qubit properties*/
#define QDMI_FIDELITY 0
#define QDMI_T1_TIME 1
#define QDMI_T2_TIME 2
#define QDMI_T2_ECHO_TIME 3
#define QDMI_READOUT_ERROR 4
#define QDMI_READOUT_LENGTH 5
#define QDMI_QUBIT_COUPLING_MAP 6
#define QDMI_PROPERTY_NOT_DEFINED -9999999.999

/* Well known gate names */
/*
        QDMI_QUBIT
        QDMI_CNOT
        QDMI_H

    (reserved names for well defined gate types)
 */

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------*/
/* QDMI Type definitions */

typedef struct QDMI_Session_impl_d *QDMI_Session;
typedef struct QDMI_Device_impl_d *QDMI_Device;

typedef struct QDMI_Job_impl_d *QDMI_Job;
typedef struct QDMI_Fragment_d *QDMI_Fragment;

typedef int QDMI_Status;

typedef int QDMI_qubit_index;
typedef int QDMI_Gate_property;
// typedef int QDMI_Qubit_property;

typedef int QDMI_Device_property;
typedef struct QDMI_Qubit_property_impl_d *QDMI_Qubit_property;
typedef struct QDMI_Gate_impl_d *QDMI_Gate;
typedef struct QDMI_Unitary_impl_d *QDMI_Unitary;
typedef struct QDMI_Qubit_impl_d *QDMI_Qubit;

/*----------------------------------------*/
/* Function definitions */

/* Memory allocation routines */

QDMI_Device QDMI_allocate_device();
QDMI_Job QDMI_allocate_job();

/* Core Session, basic start/close routines */

int QDMI_session_init(QInfo info, QDMI_Session *session);
int QDMI_session_finalize(QDMI_Session session);

/* Core Interface */

int QDMI_core_version(QDMI_Session *session, int *major, int *minor);
int QDMI_core_device_count(QDMI_Session *session, int *count);
int QDMI_core_open_device(QDMI_Session *session, int idx, QInfo *info,
                          QDMI_Device *handle);
int QDMI_core_query_device(QDMI_Session *session, int idx, QInfo *info);
int QDMI_core_close_device(QDMI_Session *session, QDMI_Device handle);

// support linear queue with reordering without QPU splitting
// consider packing

/* Control Interface */

/* Pack routines */

int QDMI_control_pack_qasm2(QDMI_Device dev, char *qasmstr,
                            QDMI_Fragment *frag);
typedef int (*QDMI_control_pack_qasm2_t)(QDMI_Device dev, char *qasmstr,
                                         QDMI_Fragment *frag);

int QDMI_control_pack_qir(QDMI_Device dev, void *qirmod, QDMI_Fragment *frag);
typedef int (*QDMI_control_pack_qir_t)(QDMI_Device dev, void *qirmod,
                                       QDMI_Fragment *frag);

/* QC job control routines */

int QDMI_control_submit(QDMI_Device dev, QDMI_Fragment *frag, int numshots,
                        QInfo info, QDMI_Job *job);
typedef int (*QDMI_control_submit_t)(QDMI_Device dev, QDMI_Fragment *frag,
                                     int numshots, QInfo info, QDMI_Job *job);

int QDMI_control_cancel(QDMI_Device dev, QDMI_Job *job, QInfo info);
typedef int (*QDMI_control_cancel_t)(QDMI_Device dev, QDMI_Job *job,
                                     QInfo info);

int QDMI_control_pause(QDMI_Device dev, QDMI_Job *job, QInfo info);
typedef int (*QDMI_control_pause_t)(QDMI_Device dev, QDMI_Job *job, QInfo info);

int QDMI_control_test(QDMI_Device dev, QDMI_Job *job, int *flag,
                      QDMI_Status *status);
typedef int (*QDMI_control_test_t)(QDMI_Device dev, QDMI_Job *job, int *flag,
                                   QDMI_Status *status);

int QDMI_control_wait(QDMI_Device dev, QDMI_Job *job, QDMI_Status *status);
typedef int (*QDMI_control_wait_t)(QDMI_Device dev, QDMI_Job *job,
                                   QDMI_Status *status);

int QDMI_control_extract_state(QDMI_Device dev, QDMI_Status status, int *state);
typedef int (*QDMI_control_extract_state_t)(QDMI_Device dev, QDMI_Status status,
                                            int *state);

/* read out routines */

int QDMI_control_readout_size(QDMI_Device dev, QDMI_Status *status, QDMI_Job job,
                              int *numbits);
typedef int (*QDMI_control_readout_size_t)(QDMI_Device dev, QDMI_Status *status, QDMI_Job job,
                                           int *numbits);

/* Histogram */
int QDMI_control_readout_hist_size(QDMI_Device dev, QDMI_Status *status, QDMI_Job job,
                                   int *size);
typedef int (*QDMI_control_readout_hist_size_t)(QDMI_Device dev,QDMI_Status *status, 
                                  QDMI_Job job, int *size);
int QDMI_control_readout_hist_top(QDMI_Device dev, QDMI_Status *status, QDMI_Job job,
                                  int numhist, QInfo info, long *hist);
typedef int (*QDMI_control_readout_hist_top_t)(QDMI_Device dev,
                                               QDMI_Status *status, QDMI_Job job, int numhist,
                                               QInfo info, long *hist);

/* Full data */
int QDMI_control_readout_raw_num(QDMI_Device dev, QDMI_Status *status, 
                                QDMI_Job job, int *num);
typedef int (*QDMI_control_readout_raw_num_t)(QDMI_Device dev,
                                              QDMI_Status *status, QDMI_Job job,
                                              int *num);
int QDMI_control_readout_raw_sample(QDMI_Device dev, QDMI_Status *status,
                                    int numraw, QInfo info, long *hist);
typedef int (*QDMI_control_readout_raw_sample_t)(QDMI_Device dev,
                                                 QDMI_Status *status,
                                                 int numraw, QInfo info,
                                                 long *hist);

/* Query Interface */

// check if a specific device property exists
int QDMI_query_device_property_exists(QDMI_Device dev,
                                      QDMI_Device_property prop, int *scope);
typedef int (*QDMI_query_device_property_exists_t)(QDMI_Device dev,
                                                   QDMI_Device_property prop,
                                                   int *scope);
int QDMI_query_device_property_type(QDMI_Device dev, QDMI_Device_property prop);
typedef int (*QDMI_query_device_property_type_t)(QDMI_Device dev,
                                                 QDMI_Device_property prop);

// query device property as int, char, float, double
int QDMI_query_device_property_i(QDMI_Device dev, QDMI_Device_property prop,
                                 int *value);
typedef int (*QDMI_query_device_property_i_t)(QDMI_Device dev,
                                              QDMI_Device_property prop,
                                              int *value);
int QDMI_query_device_property_c(QDMI_Device dev, QDMI_Device_property prop,
                                 char **value);
typedef int (*QDMI_query_device_property_c_t)(QDMI_Device dev,
                                              QDMI_Device_property prop,
                                              char **value);
int QDMI_query_device_property_f(QDMI_Device dev, QDMI_Device_property prop,
                                 float *value);
typedef int (*QDMI_query_device_property_f_t)(QDMI_Device dev,
                                              QDMI_Device_property prop,
                                              float *value);
int QDMI_query_device_property_d(QDMI_Device dev, QDMI_Device_property prop,
                                 double *value);
typedef int (*QDMI_query_device_property_d_t)(QDMI_Device dev,
                                              QDMI_Device_property prop,
                                              double *value);

// routines to query gate set
int QDMI_query_gateset_num(QDMI_Device dev, int *num_gates);
typedef int (*QDMI_query_gateset_num_t)(QDMI_Device dev, int *num_gates);
int QDMI_query_qubits_num(QDMI_Device dev, int *num_qubits);
typedef int (*QDMI_query_qubits_num_t)(QDMI_Device dev, int *num_qubits);
int QDMI_query_all_gates(QDMI_Device dev, QDMI_Gate *gates);
typedef int (*QDMI_query_all_gates_t)(QDMI_Device dev, QDMI_Gate *gates);
int QDMI_query_byname(QDMI_Device dev, char *name, QDMI_Gate *gate);
typedef int (*QDMI_query_byname_t)(QDMI_Device dev, char *name,
                                   QDMI_Gate *gate);

int QDMI_query_gate_name(QDMI_Device dev, QDMI_Gate gate, char *name, int *len);
typedef int (*QDMI_query_gate_name_t)(QDMI_Device dev, QDMI_Gate gate,
                                      char *name, int *len);
int QDMI_query_gate_size(QDMI_Device dev, QDMI_Gate gate, int *gatesize);
typedef int (*QDMI_query_gate_size_t)(QDMI_Device dev, QDMI_Gate gate,
                                      int *gatesize);
int QDMI_query_gate_unitary(QDMI_Device dev, QDMI_Gate gate,
                            QDMI_Unitary *unitary);
typedef int (*QDMI_query_gate_unitary_t)(QDMI_Device dev, QDMI_Gate gate,
                                         QDMI_Unitary *unitary);

// routines to query gate properties
int QDMI_query_gate_property_exists(QDMI_Device dev, QDMI_Gate gate,
                                    QDMI_Gate_property prop, int *scope);
typedef int (*QDMI_query_gate_property_exists_t)(QDMI_Device dev,
                                                 QDMI_Gate gate,
                                                 QDMI_Gate_property prop,
                                                 int *scope);
int QDMI_query_gate_property_type(QDMI_Device dev, QDMI_Gate gate,
                                  QDMI_Gate_property prop);
typedef int (*QDMI_query_gate_property_type_t)(QDMI_Device dev, QDMI_Gate gate,
                                               QDMI_Gate_property prop);
int QDMI_query_gate_property_i(QDMI_Device dev, QDMI_Gate_property prop,
                               QDMI_Gate gate, int *coor, int *value);
typedef int (*QDMI_query_gate_property_i_t)(QDMI_Device dev,
                                            QDMI_Gate_property prop,
                                            QDMI_Gate gate, int *coor,
                                            int *value);
int QDMI_query_gate_property_f(QDMI_Device dev, QDMI_Gate_property prop,
                               QDMI_Gate gate, int *coor, float *value);
typedef int (*QDMI_query_gate_property_f_t)(QDMI_Device dev,
                                            QDMI_Gate_property prop,
                                            QDMI_Gate gate, int *coor,
                                            float *value);
int QDMI_query_gate_property_d(QDMI_Device dev, QDMI_Gate_property prop,
                               QDMI_Gate gate, int *coor, double *value);
typedef int (*QDMI_query_gate_property_d_t)(QDMI_Device dev,
                                            QDMI_Gate_property prop,
                                            QDMI_Gate gate, int *coor,
                                            double *value);

int QDMI_query_all_qubits(QDMI_Device dev, QDMI_Qubit *qubits);
typedef int (*QDMI_query_all_qubits_t)(QDMI_Device dev, QDMI_Qubit *qubits);
int QDMI_query_qubit_property_exists(QDMI_Device dev, QDMI_Qubit qubit,
                                     QDMI_Qubit_property prop, int *scope);
typedef int (*QDMI_query_qubit_property_exists_t)(QDMI_Device dev,
                                                  QDMI_Qubit qubit,
                                                  QDMI_Qubit_property prop,
                                                  int *scope);
int QDMI_query_qubit_property_type(QDMI_Device dev, QDMI_Qubit qubit,
                                   QDMI_Qubit_property prop);
typedef int (*QDMI_query_qubit_property_type_t)(QDMI_Device dev,
                                                QDMI_Qubit qubit,
                                                QDMI_Qubit_property prop);
// query qubit property as int, float, etc.
int QDMI_query_qubit_property_c(QDMI_Device dev, QDMI_Qubit qubit,
                                QDMI_Qubit_property prop, char *value);
typedef int (*QDMI_query_qubit_property_c_t)(QDMI_Device dev, QDMI_Qubit qubit,
                                             QDMI_Qubit_property prop,
                                             char *value);
int QDMI_query_qubit_property_i(QDMI_Device dev, QDMI_Qubit qubit,
                                QDMI_Qubit_property prop, int *value);
typedef int (*QDMI_query_qubit_property_i_t)(QDMI_Device dev, QDMI_Qubit qubit,
                                             QDMI_Qubit_property prop,
                                             int *value);
int QDMI_query_qubit_property_f(QDMI_Device dev, QDMI_Qubit qubit,
                                QDMI_Qubit_property prop, float *value);
typedef int (*QDMI_query_qubit_property_f_t)(QDMI_Device dev, QDMI_Qubit qubit,
                                             QDMI_Qubit_property prop,
                                             float *value);
int QDMI_query_qubit_property_d(QDMI_Device dev, QDMI_Qubit qubit,
                                QDMI_Qubit_property prop, double *value);
typedef int (*QDMI_query_qubit_property_d_t)(QDMI_Device dev, QDMI_Qubit qubit,
                                             QDMI_Qubit_property prop,
                                             double *value);

/* Device Interface */

/* general query */

int QDMI_device_status(QDMI_Device dev, QInfo info, int *status);
typedef int (*QDMI_device_status_t)(QDMI_Device dev, QInfo info, int *status);

/* calibration management */
int QDMI_device_quality_check(QDMI_Device dev, double *result);
typedef int (*QDMI_device_quality_check_t)(QDMI_Device dev, double *result);
int QDMI_device_quality_limit(QDMI_Device dev, double *result);
typedef int (*QDMI_device_quality_limit_t)(QDMI_Device dev, double *result);
int QDMI_device_quality_calibrate(QDMI_Device dev);
typedef int (*QDMI_device_quality_calibrate_t)(QDMI_Device dev);

#ifdef __cplusplus
} // extern "C"
#endif
