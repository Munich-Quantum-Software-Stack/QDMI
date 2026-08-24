/*
 * Copyright (c) 2024 - 2026 QDMI Maintainers
 * All rights reserved.
 *
 * Licensed under the Apache License v2.0 with LLVM Exceptions (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * https://llvm.org/LICENSE.txt
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

/** @file
 * @brief A template of a device implementation in C++.
 * @details In the end, all QDMI_ERROR_NOTIMPLEMENTED return codes should be
 * replaced by proper return codes.
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
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif
#ifdef __GNUC__
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
 * @brief Implementation of the MY_QDMI_Site structure.
 * @details This structure can, e.g., be used to store the site id.
 */
struct MY_QDMI_Site_impl_d {};

/**
 * @brief Implementation of the MY_QDMI_Operation structure.
 * @details This structure can, e.g., be used to store the operation id.
 */
struct MY_QDMI_Operation_impl_d {};

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
                                         const size_t size, const void *value) {
  return QDMI_ERROR_NOTIMPLEMENTED;
}

int MY_QDMI_device_session_create_device_job(MY_QDMI_Device_Session session,
                                             MY_QDMI_Device_Job *job) {
  return QDMI_ERROR_NOTIMPLEMENTED;
}

int MY_QDMI_device_session_retrieve_device_job_by_id(
    MY_QDMI_Device_Session session, const char *job_id,
    MY_QDMI_Device_Job *job) {
  return QDMI_ERROR_NOTIMPLEMENTED;
}

void MY_QDMI_device_job_free(MY_QDMI_Device_Job job) {}

int MY_QDMI_device_job_set_parameter(MY_QDMI_Device_Job job,
                                     const QDMI_Device_Job_Parameter param,
                                     const size_t size, const void *value) {
  return QDMI_ERROR_NOTIMPLEMENTED;
}

int MY_QDMI_device_job_set_programs(MY_QDMI_Device_Job job,
                                    const QDMI_Program_Format *format,
                                    const size_t count, const size_t *sizes,
                                    const void *const *programs) {
  return QDMI_ERROR_NOTIMPLEMENTED;
}

int MY_QDMI_device_job_query_property(MY_QDMI_Device_Job job,
                                      const QDMI_Device_Job_Property prop,
                                      const size_t size, void *value,
                                      size_t *size_ret) {
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

int MY_QDMI_device_job_wait(MY_QDMI_Device_Job job, const size_t timeout) {
  return QDMI_ERROR_NOTIMPLEMENTED;
}

int MY_QDMI_device_job_get_results(MY_QDMI_Device_Job job,
                                   const size_t program_index,
                                   const QDMI_Job_Result result,
                                   const size_t size, void *data,
                                   size_t *size_ret) {
  return QDMI_ERROR_NOTIMPLEMENTED;
}

int MY_QDMI_device_session_query_device_property(
    MY_QDMI_Device_Session session, const QDMI_Device_Property prop,
    const size_t size, void *value, size_t *size_ret) {
  return QDMI_ERROR_NOTIMPLEMENTED;
}

int MY_QDMI_device_session_query_program_features(
    MY_QDMI_Device_Session session, const QDMI_Program_Format *format,
    size_t size, QDMI_Program_Feature *value, size_t *size_ret) {
  return QDMI_ERROR_NOTIMPLEMENTED;
}

int MY_QDMI_device_session_query_site_property(MY_QDMI_Device_Session session,
                                               MY_QDMI_Site site,
                                               const QDMI_Site_Property prop,
                                               const size_t size, void *value,
                                               size_t *size_ret) {
  return QDMI_ERROR_NOTIMPLEMENTED;
}

int MY_QDMI_device_session_query_operation_property(
    MY_QDMI_Device_Session session, MY_QDMI_Operation operation,
    const size_t num_sites, const MY_QDMI_Site *sites, const size_t num_params,
    const double *params, const QDMI_Operation_Property prop, const size_t size,
    void *value, size_t *size_ret) {
  return QDMI_ERROR_NOTIMPLEMENTED;
}

// The following line ignores the unused parameters in the functions.
// Please remove the following code block after populating the functions.
// NOLINTEND(misc-unused-parameters,clang-diagnostic-unused-parameter)
#ifdef __clang__
#pragma clang diagnostic pop
#endif
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
