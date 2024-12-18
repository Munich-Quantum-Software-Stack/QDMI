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
 * @brief The QDMI client interface.
 * @details The purpose of the client interface is to provide a set of functions
 * that allow users to interact with QDMI devices. The client interface provides
 * functions to establish sessions with a QDMI driver, retrieve available QDMI
 * devices, and interact with the devices by querying their properties and
 * submitting jobs.
 */

#pragma once

#include "qdmi/device.h" // IWYU pragma: export

#ifdef __cplusplus
#include <cstddef>

extern "C" {
#else
#include <stddef.h>
#endif

// The following clang-tidy warning cannot be addressed because this header is
// used from both C and C++ code.
// NOLINTBEGIN(modernize-use-using)

typedef struct QDMI_Library_impl_d {
  /// Function pointer to @ref QDMI_device_initialize.
  QDMI_device_initialize_t *device_initialize;
  /// Function pointer to @ref QDMI_device_finalize.
  QDMI_device_finalize_t *device_finalize;
  /// Function pointer to @ref QDMI_device_session_alloc.
  QDMI_device_session_alloc_t *device_session_alloc;
  /// Function pointer to @ref QDMI_device_session_init.
  QDMI_device_session_init_t *device_session_init;
  /// Function pointer to @ref QDMI_device_session_free.
  QDMI_device_session_free_t *device_session_free;
  /// Function pointer to @ref QDMI_device_session_set_parameter.
  QDMI_device_session_set_parameter_t *device_session_set_parameter;
  /// Function pointer to @ref QDMI_device_job_create.
  QDMI_device_job_create_t *device_job_create;
  /// Function pointer to @ref QDMI_device_job_free.
  QDMI_device_job_free_t *device_job_free;
  /// Function pointer to @ref QDMI_device_job_set_parameter.
  QDMI_device_job_set_parameter_t *device_job_set_parameter;
  /// Function pointer to @ref QDMI_device_job_submit.
  QDMI_device_job_submit_t *device_job_submit;
  /// Function pointer to @ref QDMI_device_job_cancel.
  QDMI_device_job_cancel_t *device_job_cancel;
  /// Function pointer to @ref QDMI_device_job_check.
  QDMI_device_job_check_t *device_job_check;
  /// Function pointer to @ref QDMI_device_job_wait.
  QDMI_device_job_wait_t *device_job_wait;
  /// Function pointer to @ref QDMI_device_job_get_data.
  QDMI_device_job_get_data_t *device_job_get_data;
  /// Function pointer to @ref QDMI_device_session_query_property.
  QDMI_device_session_query_property_t *device_session_query_property;
  /// Function pointer to @ref QDMI_device_session_get_sites.
  QDMI_device_session_get_sites_t *device_session_get_sites;
  /// Function pointer to @ref QDMI_device_session_get_operations.
  QDMI_device_session_get_operations_t *device_session_get_operations;
  /// Function pointer to @ref QDMI_device_site_query_property.
  QDMI_device_site_query_property_t *device_site_query_property;
  /// Function pointer to @ref QDMI_device_operation_query_property.
  QDMI_device_operation_query_property_t *device_operation_query_property;
} QDMI_Library_impl_t;

typedef const QDMI_Library_impl_t *QDMI_Library;

typedef struct QDMI_Site_impl_d {
  QDMI_Library library;
} QDMI_Site_impl_t;

typedef struct QDMI_Operation_impl_d {
  QDMI_Library library;
} QDMI_Operation_impl_t;

// NOLINTEND(modernize-use-using)

#ifdef __cplusplus
} // extern "C"
#endif
