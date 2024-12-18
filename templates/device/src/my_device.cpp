/*------------------------------------------------------------------------------
Copyright 2024 Munich Quantum Software Stack Project

Licensed under the Apache License, Version 2.0 with LLVM Exceptions (the
"License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at

https://github.com/Munich-Quantum-Software-Stack/QDMI/blob/develop/LICENSE

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
License for the specific language governing permissions and limitations under
the License.

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
------------------------------------------------------------------------------*/

/** @file
 * @brief A template of a device implementation in C++.
 * @details In the end, all QDMI_ERROR_NOT_IMPLEMENTED return codes should be
 * replaced by QDMI_ERROR_NOT_IMPLEMENTED return codes.
 * For the documentation of the functions, see the official documentation.
 */

#include "my_qdmi/device.h"

#ifdef __cplusplus
#include <cstddef>
#else
#include <stddef.h>
#endif

// The following line ignores the unused parameters in the functions.
// Please remove the following code block after populating the functions.
// NOLINTBEGIN(misc-unused-parameters,clang-diagnostic-unused-parameter)
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
/**
 * @brief Implementation of the MY_QDMI_Device_Session structure.
 * @details This structure can, e.g., be used to store a token to access an API.
 */
struct MY_QDMI_Device_Session_impl_d {};

/**
 * @brief Implementation of the MY_QDMI_Device_Job structure.
 * @details This structure can, e.g., be used to store the job id.
 */
struct MY_QDMI_Device_Job_impl_d {};

/**
 * @brief Implementation of the MY_QDMI_Device_Site structure.
 * @details This structure can, e.g., be used to store the site id.
 */
struct MY_QDMI_Device_Site_impl_d {};

/**
 * @brief Implementation of the MY_QDMI_Device_Operation structure.
 * @details This structure can, e.g., be used to store the operation id.
 */
struct MY_QDMI_Device_Operation_impl_d {};

int MY_QDMI_device_initialize() { return QDMI_ERROR_NOTIMPLEMENTED; }

int MY_QDMI_device_finalize() { return QDMI_ERROR_NOTIMPLEMENTED; }

int MY_QDMI_device_session_alloc(MY_QDMI_Device_Session *session) {
  return QDMI_ERROR_NOTIMPLEMENTED;
}

int MY_QDMI_device_session_init(MY_QDMI_Device_Session session) {
  return QDMI_ERROR_NOTIMPLEMENTED;
}

void MY_QDMI_device_session_free(MY_QDMI_Device_Session session) {}

int MY_QDMI_device_session_set_parameter(MY_QDMI_Device_Session session,
                                         QDMI_Device_Session_Parameter param,
                                         size_t size, const void *value) {
  return QDMI_ERROR_NOTIMPLEMENTED;
}

int MY_QDMI_device_job_create(MY_QDMI_Device_Session session,
                              QDMI_Program_Format format, size_t size,
                              const void *prog, MY_QDMI_Device_Job *job) {
  return QDMI_ERROR_NOTIMPLEMENTED;
}

void MY_QDMI_device_job_free(MY_QDMI_Device_Job job) {}

int MY_QDMI_device_job_set_parameter(MY_QDMI_Device_Job job,
                                     QDMI_Device_Job_Parameter param,
                                     size_t size, const void *value) {
  return QDMI_ERROR_NOTIMPLEMENTED;
}

int MY_QDMI_device_job_submit(MY_QDMI_Device_Job job) {
  return QDMI_ERROR_NOTIMPLEMENTED;
}

int MY_QDMI_device_job_cancel(MY_QDMI_Device_Job job) {
  return QDMI_ERROR_NOTIMPLEMENTED;
}

int MY_QDMI_device_job_check(MY_QDMI_Device_Job job, QDMI_Job_Status *status) {
  return QDMI_ERROR_NOTIMPLEMENTED;
}

int MY_QDMI_device_job_wait(MY_QDMI_Device_Job job) {
  return QDMI_ERROR_NOTIMPLEMENTED;
}

int MY_QDMI_device_job_get_data(MY_QDMI_Device_Job job, QDMI_Job_Result result,
                                size_t size, void *data, size_t *size_ret) {
  return QDMI_ERROR_NOTIMPLEMENTED;
}

int MY_QDMI_device_session_query_property(MY_QDMI_Device_Session session,
                                          QDMI_Device_Property prop,
                                          size_t size, void *value,
                                          size_t *size_ret) {
  return QDMI_ERROR_NOTIMPLEMENTED;
}

int MY_QDMI_device_session_get_sites(MY_QDMI_Device_Session session,
                                     size_t num_entries,
                                     MY_QDMI_Device_Site *sites,
                                     size_t *num_sites) {
  return QDMI_ERROR_NOTIMPLEMENTED;
}

int MY_QDMI_device_session_get_operations(MY_QDMI_Device_Session session,
                                          size_t num_entries,
                                          MY_QDMI_Device_Operation *operations,
                                          size_t *num_operations) {
  return QDMI_ERROR_NOTIMPLEMENTED;
}

int MY_QDMI_device_site_query_property(MY_QDMI_Device_Site site,
                                       QDMI_Site_Property prop, size_t size,
                                       void *value, size_t *size_ret) {
  return QDMI_ERROR_NOTIMPLEMENTED;
}

int MY_QDMI_device_operation_query_property(MY_QDMI_Device_Operation operation,
                                            size_t num_sites,
                                            const MY_QDMI_Device_Site *sites,
                                            QDMI_Operation_Property prop,
                                            size_t size, void *value,
                                            size_t *size_ret) {
  return QDMI_ERROR_NOTIMPLEMENTED;
}

// The following line ignores the unused parameters in the functions.
// Please remove the following code block after populating the functions.
// NOLINTEND(misc-unused-parameters,clang-diagnostic-unused-parameter)
#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
