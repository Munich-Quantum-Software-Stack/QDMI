/*------------------------------------------------------------------------------
Part of the MQSS Project, under the Apache License v2.0 with LLVM Exceptions.
See https://llvm.org/LICENSE.txt for license information.
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
------------------------------------------------------------------------------*/

/** @file
 * @brief A template of a device implementation in C++.
 * @details In the end, all QDMI_ERROR_NOT_IMPLEMENTED return codes should be
 * replaced by QDMI_ERROR_NOT_IMPLEMENTED return codes.
 * For the documentation of the functions, see the official documentation.
 */

#include "qdmi/device.h"

// The following line ignores the unused parameters in the functions.
// Additionally, it ignores warnings for easily swappable parameters.
// Please remove the `misc-unused-parameters` part while populating the
// functions.
// NOLINTBEGIN(misc-unused-parameters,clang-diagnostic-unused-parameter,bugprone-easily-swappable-parameters)

/**
 * @brief Implementation of the QDMI_Job structure.
 * @details This structure can, e.g., be used to store the job id.
 */
struct QDMI_Job_impl_d {};

int QDMI_query_get_sites_dev(int num_entries, QDMI_Site *sites,
                             int *num_sites) {
  return QDMI_ERROR_NOTIMPLEMENTED;
}

int QDMI_query_get_operations_dev(int num_entries, QDMI_Operation *operations,
                                  int *num_operations) {
  return QDMI_ERROR_NOTIMPLEMENTED;
}

int QDMI_query_device_property_dev(QDMI_Device_Property prop, int size,
                                   void *value, int *size_ret) {
  return QDMI_ERROR_NOTIMPLEMENTED;
}

int QDMI_query_site_property_dev(QDMI_Site site, QDMI_Site_Property prop,
                                 int size, void *value, int *size_ret) {
  return QDMI_ERROR_NOTIMPLEMENTED;
}

int QDMI_query_operation_property_dev(QDMI_Operation operation, int num_sites,
                                      const QDMI_Site *sites,
                                      QDMI_Operation_Property prop, int size,
                                      void *value, int *size_ret) {
  return QDMI_ERROR_NOTIMPLEMENTED;
}

int QDMI_control_create_job_dev(QDMI_Program_Format format, int size,
                                const void *prog, QDMI_Job *job) {
  return QDMI_ERROR_NOTIMPLEMENTED;
}

int QDMI_control_set_parameter_dev(QDMI_Job job, QDMI_Job_Parameter param,
                                   int size, const void *value) {
  return QDMI_ERROR_NOTIMPLEMENTED;
}

int QDMI_control_submit_job_dev(QDMI_Job job) {
  return QDMI_ERROR_NOTIMPLEMENTED;
}

int QDMI_control_cancel_dev(QDMI_Job job) { return QDMI_ERROR_NOTIMPLEMENTED; }

int QDMI_control_check_dev(QDMI_Job job, QDMI_Job_Status *status) {
  return QDMI_ERROR_NOTIMPLEMENTED;
}

int QDMI_control_wait_dev(QDMI_Job job) { return QDMI_ERROR_NOTIMPLEMENTED; }

int QDMI_control_get_data_dev(QDMI_Job job, QDMI_Job_Result result, int size,
                              void *data, int *size_ret) {
  return QDMI_ERROR_NOTIMPLEMENTED;
}

void QDMI_control_free_job_dev(QDMI_Job job) {}

int QDMI_control_initialize_dev() { return QDMI_ERROR_NOTIMPLEMENTED; }

int QDMI_control_finalize_dev() { return QDMI_ERROR_NOTIMPLEMENTED; }

// The following line ignores the unused parameters in the functions.
// Additionally, it ignores warnings for easily swappable parameters.
// Please remove the `misc-unused-parameters` part while populating the
// functions.
// NOLINTEND(misc-unused-parameters,clang-diagnostic-unused-parameter,bugprone-easily-swappable-parameters)
