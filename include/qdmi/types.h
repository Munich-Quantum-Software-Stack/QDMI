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

#ifdef __cplusplus
#include <cstddef>

extern "C" {
#else
#include <stddef.h>
#endif

// The following clang-tidy warning cannot be addressed because this header is
// used from both C and C++ code.
// NOLINTBEGIN(modernize-use-using)

/**
 * @brief A handle for a QDMI device session.
 * @details An opaque pointer to an implementation of the QDMI device session
 * concept. The actual implementation is defined by the device.
 */
typedef struct QDMI_Device_Session_impl_d *QDMI_Device_Session;

/**
 * @brief A handle for a QDMI device job.
 * @details An opaque pointer to an implementation of the QDMI device job
 * concept. The actual implementation is defined by the device.
 * Most implementation will want to store the session handle used to create the
 * job in the job handle to be able to access the session information when
 * needed.
 *
 * A job is a program that is executed on a device. The program can be a quantum
 * circuit (in various formats) or some other routine that the device can run.
 * Jobs are created using the @ref QDMI_device_job_create function. After
 * creating a job, additional parameters can be set using @ref
 * QDMI_device_job_set_parameter. Then, the job must be submitted for execution
 * using @ref QDMI_device_job_submit. The status of the job can be checked using
 * @ref QDMI_device_job_status. The job can be waited for using @ref
 * QDMI_device_job_wait. The job can be canceled using @ref
 * QDMI_device_job_cancel. The results of the job can be retrieved using @ref
 * QDMI_device_job_get_result. Finally, the job must be freed using @ref
 * QDMI_device_job_free.
 */
typedef struct QDMI_Device_Job_impl_d *QDMI_Device_Job;

/**
 * @brief Enum of the session parameters that can be set.
 * @details If not noted otherwise, parameters are optional and devices must not
 * require them to be set.
 */
enum QDMI_DEVICE_SESSION_PARAMETER_T {
  /**
   * @brief `char*` (string) The token to be used for authentication within the
   * session.
   * @details If the device authentication via a token, this parameter must be
   * set prior to calling @ref QDMI_device_session_init.
   */
  QDMI_DEVICE_SESSION_PARAMETER_TOKEN = 0,
  /**
   * @brief `char*` (string) The baseURL or API endpoint to be used for
   * accessing the device within the session.
   * @details If this parameter is set and the device supports it, the device
   * must use the specified baseURL or API endpoint for the session. If this
   * parameter is not set, the device must use a reasonable default value.
   */
  QDMI_DEVICE_SESSION_PARAMETER_BASEURL = 0,
  /**
   * @brief The maximum value of the enum.
   * @details It can be used by devices for bounds checking and validation of
   * function parameters. This value must remain the last regular member of the
   * enum besides the custom members and must be updated when new members are
   * added.
   */
  QDMI_DEVICE_SESSION_PARAMETER_MAX = 2,
  /**
   * @brief This property is reserved for a custom property.
   * @details The meaning and the type of this property are defined by the
   * device. To maintain binary compatibility, the value of this enum member
   * must not be changed.
   */
  QDMI_DEVICE_SESSION_PARAMETER_CUSTOM1 = 999999995,
  /// @see QDMI_DEVICE_SESSION_PARAMETER_CUSTOM1
  QDMI_DEVICE_SESSION_PARAMETER_CUSTOM2 = 999999996,
  /// @see QDMI_DEVICE_SESSION_PARAMETER_CUSTOM1
  QDMI_DEVICE_SESSION_PARAMETER_CUSTOM3 = 999999997,
  /// @see QDMI_DEVICE_SESSION_PARAMETER_CUSTOM1
  QDMI_DEVICE_SESSION_PARAMETER_CUSTOM4 = 999999998,
  /// @see QDMI_DEVICE_SESSION_PARAMETER_CUSTOM1
  QDMI_DEVICE_SESSION_PARAMETER_CUSTOM5 = 999999999
};

/// Type of the device session parameter.
typedef enum QDMI_DEVICE_SESSION_PARAMETER_T QDMI_Device_Session_Parameter;

/**
 * @brief Enum of the device job parameters that can be set.
 * @details If not noted otherwise, parameters are optional and drivers must not
 * require them to be set.
 */
enum QDMI_DEVICE_JOB_PARAMETER_T {
  /**
   * @brief `size_t` The number of shots to execute for a quantum circuit job.
   * @details If this parameter is not set, a device-specific default number of
   * shots is used.
   */
  QDMI_DEVICE_JOB_PARAMETER_SHOTS_NUM = 0,
  /**
   * @brief The maximum value of the enum.
   * @details It can be used by devices for bounds checking and validation of
   * function parameters. This value must remain the last regular member of the
   * enum besides the custom members and must be updated when new members are
   * added.
   */
  QDMI_DEVICE_JOB_PARAMETER_MAX = 1,
  /**
   * @brief This property is reserved for a custom property.
   * @details The meaning and the type of this property is defined by the
   * device.
   */
  QDMI_DEVICE_JOB_PARAMETER_CUSTOM1 = 999999995,
  /// @see QDMI_DEVICE_JOB_PARAMETER_CUSTOM1
  QDMI_DEVICE_JOB_PARAMETER_CUSTOM2 = 999999996,
  /// @see QDMI_DEVICE_JOB_PARAMETER_CUSTOM1
  QDMI_DEVICE_JOB_PARAMETER_CUSTOM3 = 999999997,
  /// @see QDMI_DEVICE_JOB_PARAMETER_CUSTOM1
  QDMI_DEVICE_JOB_PARAMETER_CUSTOM4 = 999999998,
  /// @see QDMI_DEVICE_JOB_PARAMETER_CUSTOM1
  QDMI_DEVICE_JOB_PARAMETER_CUSTOM5 = 999999999
};

/// Type of the device job parameter.
typedef enum QDMI_DEVICE_JOB_PARAMETER_T QDMI_Device_Job_Parameter;

typedef int QDMI_device_initialize_t(void);

typedef int QDMI_device_finalize_t(void);

typedef int QDMI_device_session_alloc_t(QDMI_Device_Session *session);

typedef int QDMI_device_session_init_t(QDMI_Device_Session session);

typedef void QDMI_device_session_free_t(QDMI_Device_Session session);

typedef int
QDMI_device_session_set_parameter_t(QDMI_Device_Session session,
                                    QDMI_Device_Session_Parameter param,
                                    size_t size, const void *value);

typedef int QDMI_device_job_create_t(QDMI_Device_Session session,
                                     QDMI_Program_Format format, size_t size,
                                     const void *prog, QDMI_Device_Job *job);

typedef void QDMI_device_job_free_t(QDMI_Device_Job job);

typedef int QDMI_device_job_set_parameter_t(QDMI_Device_Job job,
                                            QDMI_Device_Job_Parameter param,
                                            size_t size, const void *value);

typedef int QDMI_device_job_submit_t(QDMI_Device_Job job);

typedef int QDMI_device_job_cancel_t(QDMI_Device_Job job);

typedef int QDMI_device_job_check_t(QDMI_Device_Job job,
                                    QDMI_Job_Status *status);

typedef int QDMI_device_job_wait_t(QDMI_Device_Job job);

typedef int QDMI_device_job_get_data_t(QDMI_Device_Job job,
                                       QDMI_Job_Result result, size_t size,
                                       void *data, size_t *size_ret);

typedef int QDMI_device_session_query_property_t(QDMI_Device_Session session,
                                                 QDMI_Device_Property prop,
                                                 size_t size, void *value,
                                                 size_t *size_ret);

typedef int QDMI_device_session_get_sites_t(QDMI_Device_Session session,
                                            size_t num_entries,
                                            QDMI_Site *sites,
                                            size_t *num_sites);

typedef int QDMI_device_site_query_property_t(QDMI_Site site,
                                              QDMI_Site_Property prop,
                                              size_t size, void *value,
                                              size_t *size_ret);

typedef int QDMI_device_session_get_operations_t(QDMI_Device_Session session,
                                                 size_t num_entries,
                                                 QDMI_Operation *operations,
                                                 size_t *num_operations);

typedef int QDMI_device_operation_query_property_t(
    QDMI_Operation operation, size_t num_sites, const QDMI_Site *sites,
    QDMI_Operation_Property prop, size_t size, void *value, size_t *size_ret);

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
