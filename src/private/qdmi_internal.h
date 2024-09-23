/*------------------------------------------------------------------------------
Part of the MQSS Project, under the Apache License v2.0 with LLVM Exceptions.
See https://llvm.org/LICENSE.txt for license information.
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
------------------------------------------------------------------------------*/

/* Internal Headerfile for the QDMI core implementation */

#ifndef QDMI_INTERNAL_H
#define QDMI_INTERNAL_H

#include "qdmi_backend.h"

#include <qinfo.h>
#include <stdio.h>
#include <stdlib.h>

/*--------------------------------w--------*/
/* Internal-use constants */

#define QDMI_CONFIG_FILE "QDMI_CONFIG_FILE"
#define QDMI_CONFIG_FILE_DEFAULT ".qdmi-config"

/*----------------------------------------*/
/* Internal-use types */

typedef struct QDMI_Devicelist_d {
  QDMI_Device device;
  struct QDMI_Devicelist_d *next;
} QDMI_Devicelist_t;

/*----------------------------------------*/
/* QDMI Type concretizations */

typedef struct QDMI_Library_impl_d {
  /* Info object */

  QInfo info;

  /* Name of the library to open */

  char *libname;

  /* Library handle */

  void *libhandle;

  /* All library management functions */

  QDMI_backend_init_t QDMI_backend_init;

  /* All public functions */

  QDMI_control_pack_qasm2_t QDMI_control_pack_qasm2;
  QDMI_control_pack_qir_t QDMI_control_pack_qir;
  QDMI_control_submit_t QDMI_control_submit;
  QDMI_control_cancel_t QDMI_control_cancel;
  QDMI_control_pause_t QDMI_control_pause;
  QDMI_control_test_t QDMI_control_test;
  QDMI_control_wait_t QDMI_control_wait;
  QDMI_control_extract_state_t QDMI_control_extract_state;
  QDMI_control_readout_size_t QDMI_control_readout_size;
  QDMI_control_readout_hist_size_t QDMI_control_readout_hist_size;
  QDMI_control_readout_hist_top_t QDMI_control_readout_hist_top;
  QDMI_control_readout_raw_num_t QDMI_control_readout_raw_num;
  QDMI_control_readout_raw_sample_t QDMI_control_readout_raw_sample;
  QDMI_query_device_property_exists_t QDMI_query_device_property_exists;
  QDMI_query_device_property_type_t QDMI_query_device_property_type;
  QDMI_query_device_property_c_t QDMI_query_device_property_c;
  QDMI_query_device_property_i_t QDMI_query_device_property_i;
  QDMI_query_device_property_f_t QDMI_query_device_property_f;
  QDMI_query_device_property_d_t QDMI_query_device_property_d;
  QDMI_query_gateset_num_t QDMI_query_gateset_num;
  QDMI_query_qubits_num_t QDMI_query_qubits_num;
  QDMI_query_all_gates_t QDMI_query_all_gates;
  QDMI_query_byname_t QDMI_query_byname;
  QDMI_query_gate_name_t QDMI_query_gate_name;
  QDMI_query_gate_size_t QDMI_query_gate_size;
  QDMI_query_gate_unitary_t QDMI_query_gate_unitary;
  QDMI_query_gate_property_exists_t QDMI_query_gate_property_exists;
  QDMI_query_gate_property_type_t QDMI_query_gate_property_type;
  QDMI_query_gate_property_i_t QDMI_query_gate_property_i;
  QDMI_query_gate_property_f_t QDMI_query_gate_property_f;
  QDMI_query_gate_property_d_t QDMI_query_gate_property_d;
  QDMI_query_all_qubits_t QDMI_query_all_qubits;
  QDMI_query_qubit_property_exists_t QDMI_query_qubit_property_exists;
  QDMI_query_qubit_property_type_t QDMI_query_qubit_property_type;
  QDMI_query_qubit_property_c_t QDMI_query_qubit_property_c;
  QDMI_query_qubit_property_i_t QDMI_query_qubit_property_i;
  QDMI_query_qubit_property_f_t QDMI_query_qubit_property_f;
  QDMI_query_qubit_property_d_t QDMI_query_qubit_property_d;
  QDMI_device_status_t QDMI_device_status;
  QDMI_device_quality_check_t QDMI_device_quality_check;
  QDMI_device_quality_limit_t QDMI_device_quality_limit;
  QDMI_device_quality_calibrate_t QDMI_device_quality_calibrate;

  /* Pointer to next library */

  struct QDMI_Library_impl_d *next;
} QDMI_Library_impl_t;
typedef QDMI_Library_impl_t *QDMI_Library;

typedef struct QDMI_Job_impl_d {
  QInfo info;
  char* task_id;
} QDMI_Job_impl_t;

typedef struct QDMI_Fragment_d {
  QInfo info;
  char *qasmstr;
  void *qirmod;
  size_t sizebuffer;
} QDMI_Fragment_t;

typedef struct QDMI_Session_impl_d {
  QInfo info;
  struct QDMI_Session_impl_d *next;
  QDMI_Library qdmi_library_list;
} QDMI_Session_impl_t;

typedef struct QDMI_Device_impl_d {
  QInfo libinfo;
  QInfo sessioninfo;
  QDMI_Library_impl_t library;
  void *device_state;
} QDMI_Device_impl_t;

typedef struct QDMI_Qubit_property_impl_d {
  int name; // for e.g. 15 for backend_name
  int type; // INT_PROPERTY, etc,
} QDMI_Qubit_property_impl_t;

typedef struct QDMI_Gate_impl_d {
  const char *name;
  QDMI_Gate_property **coupling_mapping;
  char *unitary;
  double fidelity;
  size_t size_coupling_map;
  size_t gate_size;
} QDMI_Gate_impl_t;

typedef struct QDMI_Qubit_impl_d {
  QDMI_qubit_index index;
  QDMI_qubit_index *coupling_mapping;
  int size_coupling_mapping;
  double t1;
  double t2;
  double readout_error;
  double readout_length;
} QDMI_Qubit_impl_t;

/*----------------------------------------*/
/* Anchors for global lists */

extern QDMI_Session qdmi_session_list;
extern QDMI_Devicelist_t *qdmi_device_list;

/*----------------------------------------*/
/* Internal-use Function definitions */

#ifdef __cplusplus
extern "C" {
#endif

/* Internal functions */

int qdmi_internal_translate_qinfo_error(int err);
char *trim_line(char *line);
QDMI_Library find_library_by_name(const char *libname);
char **get_qdmi_library_list_names(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
