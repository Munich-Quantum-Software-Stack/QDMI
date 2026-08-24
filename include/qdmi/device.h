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
 * @brief Defines the @ref device_interface.
 */

#pragma once

#include "qdmi/constants.h" // IWYU pragma: export
#include "qdmi/export.h"    // IWYU pragma: export
#include "qdmi/types.h"     // IWYU pragma: export

#ifdef __cplusplus
#include <cstddef>

extern "C" {
#else
#include <stddef.h>
#endif

// The following clang-tidy warning cannot be addressed because this header is
// used from both C and C++ code.
// NOLINTBEGIN(performance-enum-size,modernize-use-using,modernize-redundant-void-arg)

/** @defgroup device_interface QDMI Device Interface
 *  @brief Describes the functions to be implemented by a device or backend to
 *  be used with QDMI.
 *  @details This is an interface between the QDMI driver and the device.
 *  It includes functions to initialize and finalize a device, as well as to
 *  manage sessions between a QDMI driver and a device, query properties of the
 *  device, and submit jobs to the device.
 *
 *  The device interface is split into three parts:
 *  - The @ref device_session_interface "device session interface" for managing
 * sessions between a QDMI driver and a device.
 *  - The @ref device_query_interface "device query interface" for querying
 * properties of the device.
 *  - The @ref device_job_interface "device job interface" for submitting jobs
 * to the device.
 *
 * @{
 */

/**
 * @brief Initialize a device.
 * @details A device can expect that this function is called exactly  once in
 * the beginning and has returned before any other functions are invoked on that
 * device.
 * @return @ref QDMI_SUCCESS if the device was initialized successfully.
 * @return @ref QDMI_ERROR_FATAL if an unexpected error occurred.
 */
QDMI_EXPORT int QDMI_device_initialize(void);

/**
 * @brief Finalize a device.
 * @details A device can expect that this function is called exactly once at the
 * end of using the device, and no other functions are invoked on that device
 * afterward.
 * @return @ref QDMI_SUCCESS if the device was finalized successfully.
 * @return @ref QDMI_ERROR_FATAL if the finalization failed, this could, for
 * example, be due to a job that is still running.
 */
QDMI_EXPORT int QDMI_device_finalize(void);

/** @defgroup device_session_interface QDMI Device Session Interface
 *  @brief Provides functions to manage sessions between the driver and device.
 *  @details A device session is a connection between a driver and a device that
 *  allows the driver to interact with the device.
 *  Sessions are used to authenticate with the device and to manage resources
 *  required for the interaction with the device.
 *
 *  The typical workflow for a device session is as follows:
 *  - Allocate a session with @ref QDMI_device_session_alloc.
 *  - Set parameters for the session with @ref
 * QDMI_device_session_set_parameter.
 *  - Initialize the session with @ref QDMI_device_session_init.
 *  - Run code to interact with the device using the @ref device_query_interface
 *    "device query interface" and the @ref device_job_interface
 *    "device job interface".
 *  - Free the session with @ref QDMI_device_session_free when it is no longer
 *    needed.
 *
 *  @{
 */

/**
 * @brief A handle for a device session.
 * @details An opaque pointer to a type defined by the device that encapsulates
 * all information about a session between a driver and a device.
 */
typedef struct QDMI_Device_Session_impl_d *QDMI_Device_Session;

/**
 * @brief Allocate a new device session.
 * @details This is the main entry point for a driver to establish a session
 * with a device. The returned handle can be used throughout the @ref
 * device_session_interface "device session interface" to refer to the session.
 * @param[out] session A handle to the session that is allocated. Must not be
 * @c NULL. The session must be freed by calling @ref QDMI_device_session_free
 * when it is no longer used.
 * @return @ref QDMI_SUCCESS if the session was allocated successfully.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p session is @c NULL.
 * @return @ref QDMI_ERROR_OUTOFMEM if memory space ran out.
 * @return @ref QDMI_ERROR_FATAL if an unexpected error occurred.
 * @see QDMI_device_session_set_parameter
 *      QDMI_device_session_init
 */
QDMI_EXPORT int QDMI_device_session_alloc(QDMI_Device_Session *session);

/**
 * @brief Set a parameter for a device session.
 * @param[in] session A handle to the session to set the parameter for. Must not
 * be @c NULL.
 * @param[in] param The parameter to set. Must be one of the values specified
 * for @ref QDMI_Device_Session_Parameter.
 * @param[in] size The size of the data pointed by @p value in bytes. Must not
 * be zero, except when @p value is @c NULL, in which case it is ignored.
 * @param[in] value A pointer to the memory location that contains the value of
 * the parameter to be set. The data pointed to by @p value is copied and can be
 * safely reused after this function returns. If this is @c NULL, it is ignored.
 * @return @ref QDMI_SUCCESS if the device supports the specified @ref
 * QDMI_Device_Session_Parameter and, when @p value is not @c NULL, the value of
 * the parameter was set successfully.
 * @return @ref QDMI_ERROR_NOTSUPPORTED if the device does not support the
 * parameter or the value of the parameter.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if
 *  - @p session is @c NULL,
 *  - @p param is invalid, or
 *  - @p value is not @c NULL and @p size is zero or not the expected size for
 *    the parameter (if specified by the @ref QDMI_Device_Session_Parameter
 *    documentation).
 * @return @ref QDMI_ERROR_BADSTATE if the parameter cannot be set in the
 * current state of the session, for example, because the session is already
 * initialized.
 * @return @ref QDMI_ERROR_FATAL if an unexpected error occurred.
 * @see QDMI_device_session_init
 *
 * @note @parblock By calling this function with @p value set to @c NULL, the
 * function can be used to check if the device supports the specified parameter
 * without setting a value.
 *
 * For example, to check whether the device supports setting a token for
 * authentication, the following code pattern can be used:
 *
 * @code{.cpp}
 * // Check if the device supports setting a token.
 * auto ret = QDMI_device_session_set_parameter(
 *   session, QDMI_DEVICE_SESSION_PARAMETER_TOKEN, 0, nullptr);
 * if (ret == QDMI_ERROR_NOTSUPPORTED) {
 *   // The device does not support setting a token.
 *   ...
 * }
 *
 * // Set the token.
 * std::string token = "token";
 * ret = QDMI_device_session_set_parameter(
 *   session, QDMI_DEVICE_SESSION_PARAMETER_TOKEN, token.size() + 1,
 *   token.c_str());
 * @endcode
 * @endparblock
 */
QDMI_EXPORT int
QDMI_device_session_set_parameter(QDMI_Device_Session session,
                                  QDMI_Device_Session_Parameter param,
                                  size_t size, const void *value);

/**
 * @brief Initialize a device session.
 * @details This function initializes the device session and prepares it for
 * use.
 * The session must be initialized before it can be used as part of the @ref
 * device_query_interface "device query interface" or the @ref
 * device_job_interface "device job interface". If a device requires
 * authentication, the required authentication information must be set using
 * @ref QDMI_device_session_set_parameter before calling this function. A
 * session may only be successfully initialized once.
 * @param[in] session The session to initialize. Must not be @c NULL.
 * @return @ref QDMI_SUCCESS if the session was initialized successfully.
 * @return @ref QDMI_ERROR_PERMISSIONDENIED if the session could not be
 * initialized due to missing permissions. This could be due to missing
 * authentication information that should be set using @ref
 * QDMI_device_session_set_parameter.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p session is @c NULL.
 * @return @ref QDMI_ERROR_BADSTATE if the session is not in a state allowing
 * initialization, for example, because the session is already initialized.
 * @return @ref QDMI_ERROR_FATAL if an unexpected error occurred.
 * @see QDMI_device_session_set_parameter
 *      QDMI_device_session_query_device_property
 *      QDMI_device_session_query_site_property
 *      QDMI_device_session_query_operation_property
 *      QDMI_device_session_create_device_job
 */
QDMI_EXPORT int QDMI_device_session_init(QDMI_Device_Session session);

/**
 * @brief Free a QDMI device session.
 * @details This function frees the memory allocated for the session.
 * Using a session handle after it was freed is undefined behavior.
 * @param[in] session The session to free.
 */
QDMI_EXPORT void QDMI_device_session_free(QDMI_Device_Session session);

/** @} */ // end of device_session_interface

/** @defgroup device_query_interface QDMI Device Query Interface
 *  @brief Provides functions to query properties of a device.
 *  @brief The query interface enables to query static and dynamic properties of
 *  a device and its constituents in a unified fashion. It operates on @ref
 *  QDMI_Device_Session handles created via the @ref device_session_interface
 *  "device session interface".
 *
 *  @{
 */

/**
 * @brief Query a device property.
 * @param[in] session The session used for the query. Must not be @c NULL.
 * @param[in] prop The property to query. Must be one of the values specified
 * for @ref QDMI_Device_Property.
 * @param[in] size The size of the memory pointed to by @p value in bytes. Must
 * be greater or equal to the size of the return type specified for @p prop,
 * except when @p value is @c NULL, in which case it is ignored.
 * @param[out] value A pointer to the memory location where the value of the
 * property will be stored. If this is @c NULL, it is ignored.
 * @param[out] size_ret The actual size of the data being queried in bytes. If
 * this is @c NULL, it is ignored.
 * @return @ref QDMI_SUCCESS if the device supports the specified property and,
 * when @p value is not @c NULL, the property was successfully retrieved.
 * @return @ref QDMI_ERROR_NOTSUPPORTED if the device does not support the
 * property.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if
 *  - @p session is @c NULL,
 *  - @p prop is invalid, or
 *  - @p value is not @c NULL and @p size is less than the size of the data
 *    being queried.
 * @return @ref QDMI_ERROR_BADSTATE if the property cannot be queried in the
 * current state of the session, for example, because the session is not
 * initialized.
 * @return @ref QDMI_ERROR_FATAL if an unexpected error occurred.
 *
 * @note @parblock By calling this function with @p value set to @c NULL, the
 * function can be used to check if the device supports the specified property
 * without retrieving the property and without the need to provide a buffer for
 * it. Additionally, the size of the buffer needed to retrieve the property is
 * returned in @p size_ret if @p size_ret is not @c NULL.
 *
 * For example, to query the name of a device implementation, the
 * following code pattern can be used:
 * @code{.cpp}
 * // Query the size of the property.
 * size_t size;
 * QDMI_device_session_query_device_property(
 *   session, QDMI_DEVICE_PROPERTY_NAME, 0, nullptr, &size);
 *
 * // Allocate memory for the property.
 * auto name = std::string(size - 1, '\0');
 *
 * // Query the property.
 * QDMI_device_session_query_device_property(
 *   session, QDMI_DEVICE_PROPERTY_NAME, size, name.data(), nullptr);
 * @endcode
 *
 * @endparblock
 * @attention May only be called after the session has been initialized with
 * @ref QDMI_device_session_init.
 */
QDMI_EXPORT int QDMI_device_session_query_device_property(
    QDMI_Device_Session session, QDMI_Device_Property prop, size_t size,
    void *value, size_t *size_ret);

/**
 * @brief Query the complete optional feature guarantees for one exact program
 * format.
 * @param[in] session The initialized session used for the query. Must not be
 * @c NULL.
 * @param[in] format A canonical descriptor equal to one returned by @ref
 * QDMI_DEVICE_PROPERTY_SUPPORTEDPROGRAMFORMATS. The value may be reconstructed
 * by the caller. Must not be @c NULL.
 * @param[in] size The size of @p value in bytes. Ignored when @p value is
 * @c NULL.
 * @param[out] value Storage for a list of @ref QDMI_Program_Feature records,
 * including all constraints, or @c NULL to query the required size.
 * @param[out] size_ret The required list size in bytes, or @c NULL.
 * @return @ref QDMI_SUCCESS when the complete optional set is available. A
 * zero-byte result means that the format supports only its normative baseline.
 * @return @ref QDMI_ERROR_NOTSUPPORTED when optional feature metadata is
 * unknown for @p format.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT for a null handle or descriptor, an
 * invalid descriptor, or an insufficient output buffer.
 * @return @ref QDMI_ERROR_BADSTATE when @p session is not initialized.
 * @return @ref QDMI_ERROR_FATAL if an unexpected error occurred.
 */
QDMI_EXPORT int QDMI_device_session_query_program_features(
    QDMI_Device_Session session, const QDMI_Program_Format *format, size_t size,
    QDMI_Program_Feature *value, size_t *size_ret);

/**
 * @brief Query a site property.
 * @param[in] session The session used for the query. Must not be @c NULL.
 * @param[in] site The site to query. Must not be @c NULL.
 * @param[in] prop The property to query. Must be one of the values specified
 * for @ref QDMI_Site_Property.
 * @param[in] size The size of the memory pointed to by @p value in bytes. Must
 * be greater or equal to the size of the return type specified for @p prop,
 * except when @p value is @c NULL, in which case it is ignored.
 * @param[out] value A pointer to the memory location where the value of the
 * property will be stored. If this is @c NULL, it is ignored.
 * @param[out] size_ret The actual size of the data being queried in bytes. If
 * this is @c NULL, it is ignored.
 * @return @ref QDMI_SUCCESS if the device supports the specified property and,
 * when @p value is not @c NULL, the property was successfully retrieved.
 * @return @ref QDMI_ERROR_NOTSUPPORTED if the device does not support the
 * property.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if
 *  - @p session or @p site is @c NULL,
 *  - @p prop is invalid, or
 *  - @p value is not @c NULL and @p size is less than the size of the data
 *  being queried.
 * @return @ref QDMI_ERROR_BADSTATE if the property cannot be queried in the
 * current state of the session, for example, because the session is not
 * initialized.
 * @return @ref QDMI_ERROR_FATAL if an unexpected error occurred.
 *
 * @note @parblock By calling this function with @p value set to @c NULL, the
 * function can be used to check if the device supports the specified property
 * without retrieving the property and without the need to provide a buffer for
 * it. Additionally, the size of the buffer needed to retrieve the property is
 * returned in @p size_ret if @p size_ret is not @c NULL.
 *
 * For example, to query the T1 time of a site, the following code
 * pattern can be used:
 * @code{.cpp}
 * // Check if the device supports the property.
 * auto ret = QDMI_device_session_query_site_property(
 *   session, site, QDMI_SITE_PROPERTY_T1, 0, nullptr, nullptr);
 * if (ret == QDMI_ERROR_NOTSUPPORTED) {
 *   // The device does not support the property.
 *   ...
 * }
 *
 * // Query the property.
 * uint64_t t1;
 * QDMI_device_session_query_site_property(
 *   session, site, QDMI_SITE_PROPERTY_T1, sizeof(uint64_t), &t1, nullptr);
 * @endcode
 *
 * @endparblock
 * @attention May only be called after the session has been initialized with
 * @ref QDMI_device_session_init.
 */
QDMI_EXPORT int QDMI_device_session_query_site_property(
    QDMI_Device_Session session, QDMI_Site site, QDMI_Site_Property prop,
    size_t size, void *value, size_t *size_ret);

/**
 * @brief Query an operation property.
 * @param[in] session The session used for the query. Must not be @c NULL.
 * @param[in] operation The operation to query. Must not be @c NULL.
 * @param[in] num_sites The number of sites that the operation is applied to.
 * @param[in] sites A pointer to a list of handles where the sites that the
 * operation is applied to are stored. If this is @c NULL, it is ignored.
 * @param[in] num_params The number of parameters that the operation takes.
 * @param[in] params A pointer to a list of parameters the operation takes. If
 * this is @c NULL, it is ignored.
 * @param[in] prop The property to query. Must be one of the values specified
 * for @ref QDMI_Operation_Property.
 * @param[in] size The size of the memory pointed to by @p value in bytes. Must
 * be greater or equal to the size of the return type specified for the @ref
 * QDMI_Operation_Property @p prop, except when @p value is @c NULL, in which
 * case it is ignored.
 * @param[out] value A pointer to the memory location where the value of the
 * property will be stored. If this is @c NULL, it is ignored.
 * @param[out] size_ret The actual size of the data being queried in bytes. If
 * this is @c NULL, it is ignored.
 * @return @ref QDMI_SUCCESS if the device supports the specified property and,
 * when @p value is not @c NULL, the property was successfully retrieved.
 * @return @ref QDMI_ERROR_NOTSUPPORTED if the property is not supported by the
 * device or if the queried property cannot be provided for the given sites or
 * parameters.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if
 *  - @p session or @p operation are @c NULL,
 *  - @p prop is invalid, or
 *  - @p value is not @c NULL and @p size is less than the size of the data
 *    being queried.
 * @return @ref QDMI_ERROR_BADSTATE if the property cannot be queried in the
 * current state of the session, for example, because the session is not
 * initialized.
 * @return @ref QDMI_ERROR_FATAL if an unexpected error occurred.
 *
 * @note @parblock By calling this function with @p sites set to @c NULL, the
 * function can be used to query properties of the device that are independent
 * of the sites. A device will return @ref QDMI_ERROR_NOTSUPPORTED if the
 * queried property is site-dependent and @p sites is @c NULL.
 *
 * By calling this function with @p params set to @c NULL, the function
 * can be used to query properties of the device that are independent of the
 * values of the parameters. A device will return @ref QDMI_ERROR_NOTSUPPORTED
 * if the queried property is parameter-dependent and @p params is @c NULL.
 *
 * By calling this function with @p value set to @c NULL, the function
 * can be used to check if the device supports the specified property without
 * retrieving the property and without the need to provide a buffer for it.
 * Additionally, the size of the buffer needed to retrieve the property is
 * returned in @p size_ret if @p size_ret is not @c NULL.
 *
 * For example, to query the site-independent fidelity of an operation
 * without parameters, the following code snippet can be used:
 * @code{.cpp}
 * // Check if the device supports the property.
 * auto ret = QDMI_device_session_query_operation_property(
 *   session, operation, 0, nullptr, 0, nullptr,
 *   QDMI_OPERATION_PROPERTY_FIDELITY, 0, nullptr, nullptr);
 * if (ret == QDMI_ERROR_NOTSUPPORTED) {
 *   // The device does not support the site-independent property.
 *   // Check if the device supports the site-dependent property.
 *   ...
 * }
 *
 * // Query the property.
 * double fidelity;
 * QDMI_device_session_query_operation_property(
 *   session, operation, 0, nullptr, 0, nullptr,
 *   QDMI_OPERATION_PROPERTY_FIDELITY, sizeof(double), &fidelity, nullptr);
 * @endcode
 *
 * @endparblock
 * @attention May only be called after the session has been initialized with
 * @ref QDMI_device_session_init.
 */
QDMI_EXPORT int QDMI_device_session_query_operation_property(
    QDMI_Device_Session session, QDMI_Operation operation, size_t num_sites,
    const QDMI_Site *sites, size_t num_params, const double *params,
    QDMI_Operation_Property prop, size_t size, void *value, size_t *size_ret);

/** @} */ // end of device_query_interface

/** @defgroup device_job_interface QDMI Device Job Interface
 *  @brief Provides functions to manage jobs on a device.
 *  @details A job is a task submitted to a device for execution.
 *  Most jobs are quantum circuits to be executed on a quantum device.
 *
 *  The typical workflow for a device job is as follows:
 *  - Create a job with @ref QDMI_device_session_create_device_job.
 *  - Set parameters for the job with @ref QDMI_device_job_set_parameter.
 *  - Submit the job with @ref QDMI_device_job_submit.
 *  - Check the status of the job with @ref QDMI_device_job_check.
 *  - Wait for the job to finish with @ref QDMI_device_job_wait.
 *  - Retrieve the results of the job with @ref QDMI_device_job_get_results.
 *  - Free the job with @ref QDMI_device_job_free when it is no longer used.
 *
 *  Alternatively, a driver may retrieve a previously submitted job with @ref
 *  QDMI_device_session_retrieve_device_job_by_id and continue managing it
 *  through the same interface.
 *
 *  @{
 */

/**
 * @brief A handle for a device job.
 * @details An opaque pointer to a type defined by the device that encapsulates
 * all information about a job on a device.
 * @remark Implementations of the underlying type will want to store the session
 * handle used to create the job in the job handle to be able to access the
 * session information when needed.
 * @see QDMI_Job for the client-side job handle.
 */
typedef struct QDMI_Device_Job_impl_d *QDMI_Device_Job;

/**
 * @brief Create a job.
 * @details This is the main entry point for a driver to create a job for a
 * device. The returned handle can be used throughout the @ref
 * device_job_interface "device job interface" to refer to the job.
 * @param[in] session The session to create the job on. Must not be @c NULL.
 * @param[out] job A pointer to a handle that will store the created job.
 * Must not be @c NULL. The job must be freed by calling @ref
 * QDMI_device_job_free when it is no longer used.
 * @return @ref QDMI_SUCCESS if the job was successfully created.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p session or @p job are @c NULL.
 * @return @ref QDMI_ERROR_BADSTATE if the session is not in a state allowing
 * the creation of a job, for example, because the session is not initialized.
 * @return @ref QDMI_ERROR_PERMISSIONDENIED if the device does not allow using
 * the @ref device_job_interface "device job interface" for the current session.
 * @return @ref QDMI_ERROR_FATAL if job creation failed due to a fatal error.
 *
 * @attention May only be called after the session has been initialized with
 * @ref QDMI_device_session_init.
 */
QDMI_EXPORT int
QDMI_device_session_create_device_job(QDMI_Device_Session session,
                                      QDMI_Device_Job *job);

/**
 * @brief Retrieve an existing device job by its ID.
 * @details Creates a new local device-job handle for the existing remote job
 * identified by @p job_id. Retrieving a job does not submit, clone, or
 * otherwise modify the remote job. The returned handle can be used to query
 * properties, check or wait for completion, cancel the job, and retrieve
 * results.
 *
 * The job is accessed with the credentials and configuration of @p session.
 * The job ID is an identifier, not an authentication credential.
 * Parameters cannot be set on a retrieved job, and a retrieved job cannot be
 * submitted again.
 *
 * The retrieved job's properties describe the historical execution. Its exact
 * program descriptor may no longer appear in the device's current
 * supported-format list. A device that cannot reconstruct the historical
 * descriptor losslessly returns @ref QDMI_ERROR_NOTSUPPORTED.
 *
 * @param[in] session The initialized session with which to retrieve the job.
 * Must not be @c NULL.
 * @param[in] job_id The nonempty, null-terminated ID returned by
 * @ref QDMI_DEVICE_JOB_PROPERTY_ID. Must not be @c NULL.
 * @param[out] job A pointer to a handle that will store the retrieved job.
 * Must not be @c NULL. The handle must be freed by calling @ref
 * QDMI_device_job_free when it is no longer used.
 * @return @ref QDMI_SUCCESS if the job was successfully retrieved.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p session, @p job_id, or @p job
 * is @c NULL, or if @p job_id is empty.
 * @return @ref QDMI_ERROR_NOTSUPPORTED if the device does not support
 * retrieving existing jobs.
 * @return @ref QDMI_ERROR_NOTFOUND if no accessible job with @p job_id exists.
 * @return @ref QDMI_ERROR_BADSTATE if @p session is not initialized.
 * @return @ref QDMI_ERROR_PERMISSIONDENIED if @p session is not permitted to
 * access the job.
 * @return @ref QDMI_ERROR_FATAL if retrieving the job failed due to a fatal
 * error.
 */
QDMI_EXPORT int QDMI_device_session_retrieve_device_job_by_id(
    QDMI_Device_Session session, const char *job_id, QDMI_Device_Job *job);

/**
 * @brief Set a parameter for a job.
 * @param[in] job A handle to a job for which to set @p param. Must not be @c
 * NULL.
 * @param[in] param The parameter whose value will be set. Must be one of the
 * values specified for @ref QDMI_Device_Job_Parameter.
 * @param[in] size The size of the data pointed to by @p value in bytes. Must
 * not be zero, except when @p value is @c NULL, in which case it is ignored.
 * @param[in] value A pointer to the memory location that contains the value of
 * the parameter to be set. The data pointed to by @p value is copied and can be
 * safely reused after this function returns. If this is @c NULL, it is ignored.
 * @return @ref QDMI_SUCCESS if the device supports the specified @ref
 * QDMI_Device_Job_Parameter @p param and, when @p value is not @c NULL, the
 * parameter was successfully set.
 * @return @ref QDMI_ERROR_NOTSUPPORTED if the device does not support the
 * parameter or the value of the parameter.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if
 *  - @p job is @c NULL,
 *  - @p param is invalid, or
 *  - @p value is not @c NULL and @p size is zero or not the expected size for
 *    the parameter (if specified by the @ref QDMI_Device_Job_Parameter
 *    documentation).
 * @return @ref QDMI_ERROR_BADSTATE if the parameter cannot be set in the
 * current state of the job, for example, because the job is already submitted.
 * @return @ref QDMI_ERROR_PERMISSIONDENIED if the device does not allow using
 * the @ref device_job_interface "device job interface" for the current session.
 * @return @ref QDMI_ERROR_FATAL if setting the parameter failed due to a fatal
 * error.
 *
 * @note @parblock By calling this function with @p value set to @c NULL, the
 * function can be used to check if the device supports the specified parameter
 * without setting the parameter and without the need to provide a value.
 *
 * For example, to check whether the device supports setting the number of
 * shots for a quantum circuit job, the following code pattern can be used:
 * @code{.cpp}
 * // Check if the device supports setting the number of shots.
 * auto ret = QDMI_device_job_set_parameter(
 *   job, QDMI_DEVICE_JOB_PARAMETER_SHOTSNUM, 0, nullptr);
 * if (ret == QDMI_ERROR_NOTSUPPORTED) {
 *   // The device does not support setting the number of shots.
 *   ...
 * }
 *
 * // Set the number of shots.
 * size_t shots = 8192;
 * QDMI_device_job_set_parameter(
 *   job, QDMI_DEVICE_JOB_PARAMETER_SHOTSNUM, sizeof(size_t), &shots);
 * @endcode
 * @endparblock
 */
QDMI_EXPORT int QDMI_device_job_set_parameter(QDMI_Device_Job job,
                                              QDMI_Device_Job_Parameter param,
                                              size_t size, const void *value);

/**
 * @brief Query a job property.
 * @param[in] job A handle to a job for which to query @p prop. Must not be @c
 * NULL.
 * @param[in] prop The property to query. Must be one of the values specified
 * for @ref QDMI_Device_Job_Property.
 * @param[in] size The size of the memory pointed to by @p value in bytes. Must
 * be greater or equal to the size of the return type specified for @p prop,
 * except when @p value is @c NULL, in which case it is ignored.
 * @param[out] value A pointer to the memory location where the value of the
 * property will be stored. If this is @c NULL, it is ignored.
 * @param[out] size_ret The actual size of the data being queried in bytes. If
 * this is @c NULL, it is ignored.
 * @return @ref QDMI_SUCCESS if the job supports the specified property and,
 * when @p value is not @c NULL, the property was successfully retrieved.
 * @return @ref QDMI_ERROR_NOTSUPPORTED if the job does not support the
 * property.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if
 *  - @p job is @c NULL,
 *  - @p prop is invalid, or
 *  - @p value is not @c NULL and @p size is less than the size of the data
 *    being queried.
 * @return @ref QDMI_ERROR_BADSTATE if the property cannot be queried in the
 * current state of the job, for example, because the job failed or the property
 * is not initialized because it has no default value and was not set.
 * @return @ref QDMI_ERROR_FATAL if an unexpected error occurred.
 *
 * @note @parblock By calling this function with @p value set to @c NULL, the
 * function can be used to check if the job supports the specified property
 * without retrieving the property and without the need to provide a buffer for
 * it. Additionally, the size of the buffer needed to retrieve the property is
 * returned in @p size_ret if @p size_ret is not @c NULL.
 *
 * For example, to query the ID of a job, the following code pattern can
 * be used:
 * @code{.cpp}
 * // Query the size of the property.
 * size_t size;
 * QDMI_device_job_query_property(
 *   job, QDMI_DEVICE_JOB_PROPERTY_ID, 0, nullptr, &size);
 *
 * // Allocate memory for the property.
 * auto id = std::string(size - 1, '\0');
 *
 * // Query the property.
 * QDMI_device_job_query_property(
 *   job, QDMI_DEVICE_JOB_PROPERTY_ID, size, id.data(), nullptr);
 * @endcode
 * @endparblock
 */
QDMI_EXPORT int QDMI_device_job_query_property(QDMI_Device_Job job,
                                               QDMI_Device_Job_Property prop,
                                               size_t size, void *value,
                                               size_t *size_ret);

/**
 * @brief Submit a job to the device.
 * @details This function can either be blocking until the job is finished or
 * non-blocking and return while the job is running. In the latter case, the
 * functions @ref QDMI_device_job_check and @ref QDMI_device_job_wait can be
 * used to check the status and wait for the job to finish.
 * @param[in] job The job to submit. Must not be @c NULL.
 * @return @ref QDMI_SUCCESS if the job was successfully submitted.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p job is @c NULL.
 * @return @ref QDMI_ERROR_BADSTATE if the job was retrieved with @ref
 * QDMI_device_session_retrieve_device_job_by_id.
 * @return @ref QDMI_ERROR_PERMISSIONDENIED if the device does not allow using
 * the @ref device_job_interface "device job interface" for the current session.
 * @return @ref QDMI_ERROR_FATAL if the job submission failed.
 */
QDMI_EXPORT int QDMI_device_job_submit(QDMI_Device_Job job);

/**
 * @brief Cancel an already submitted job.
 * @details Remove the job from the queue of waiting jobs. This changes the
 * status of the job to @ref QDMI_JOB_STATUS_CANCELED.
 * @param[in] job The job to cancel. Must not be @c NULL.
 * @return @ref QDMI_SUCCESS if the job was successfully canceled.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p job is @c NULL or the job
 * already has the status @ref QDMI_JOB_STATUS_DONE.
 * @return @ref QDMI_ERROR_PERMISSIONDENIED if the device does not allow using
 * the @ref device_job_interface "device job interface" for the current session.
 * @return @ref QDMI_ERROR_FATAL if the job could not be canceled.
 */
QDMI_EXPORT int QDMI_device_job_cancel(QDMI_Device_Job job);

/**
 * @brief Check the status of a job.
 * @details This function is non-blocking and returns immediately with the job
 * status. It is not required to call this function before calling @ref
 * QDMI_device_job_get_results.
 * @param[in] job The job to check the status of. Must not be @c NULL.
 * @param[out] status The status of the job. Must not be @c NULL.
 * @return @ref QDMI_SUCCESS if the job status was successfully checked.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p job or @p status is @c NULL.
 * @return @ref QDMI_ERROR_PERMISSIONDENIED if the device does not allow using
 * the @ref device_job_interface "device job interface" for the current session.
 * @return @ref QDMI_ERROR_FATAL if the job status could not be checked.
 */
QDMI_EXPORT int QDMI_device_job_check(QDMI_Device_Job job,
                                      QDMI_Job_Status *status);

/**
 * @brief Wait for a job to finish.
 * @details This function blocks until the job has either finished, has been
 * canceled, or the timeout has been reached.
 * If @p timeout is not zero, this function returns latest after the specified
 * number of seconds.
 * @param[in] job The job to wait for. Must not be @c NULL.
 * @param[in] timeout The timeout in seconds.
 * If this is zero, the function waits indefinitely until the job has finished.
 * @return @ref QDMI_SUCCESS if the job is finished or canceled.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p job is @c NULL.
 * @return @ref QDMI_ERROR_PERMISSIONDENIED if the device does not allow using
 * the @ref device_job_interface "device job interface" for the current session.
 * @return @ref QDMI_ERROR_TIMEOUT if @p timeout is not zero and the job did not
 * finish within the specified time.
 * @return @ref QDMI_ERROR_FATAL if the job could not be waited for and this
 * function returns before the job has finished or has been canceled.
 */
QDMI_EXPORT int QDMI_device_job_wait(QDMI_Device_Job job, size_t timeout);

/**
 * @brief Retrieve the results of a job.
 * @param[in] job The job to retrieve the results from. Must not be @c NULL.
 * @param[in] result The result to retrieve. Must be one of the values specified
 * for @ref QDMI_Job_Result.
 * @param[in] size The size of the buffer pointed to by @p data in bytes. Must
 * be greater or equal to the size of the return type specified for the @ref
 * QDMI_Job_Result @p result, except when @p data is @c NULL, in which case it
 * is ignored.
 * @param[out] data A pointer to the memory location where the results will be
 * stored. If this is @c NULL, it is ignored.
 * @param[out] size_ret The actual size of the data being queried in bytes. If
 * this is @c NULL, it is ignored.
 * @return @ref QDMI_SUCCESS if the device supports the specified result and,
 * when @p data is not @c NULL, the results were successfully retrieved.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if
 *  - @p job is @c NULL,
 *  - @p job has not finished,
 *  - @p job was canceled,
 *  - @p result is invalid, or
 *  - @p data is not @c NULL and @p size is smaller than the size of the data
 *    being queried.
 * @return @ref QDMI_ERROR_PERMISSIONDENIED if the device does not allow using
 * the @ref device_job_interface "device job interface" for the current session.
 * @return @ref QDMI_ERROR_FATAL if an error occurred during the retrieval.
 *
 * @note @parblock By calling this function with @p data set to @c NULL, the
 * function can be used to check if the device supports the specified result
 * without retrieving the result and without the need to provide a buffer for
 * the result. Additionally, the size of the buffer needed to retrieve the
 * result is returned in @p size_ret if @p size_ret is not @c NULL.
 *
 * For example, to query the measurement results of a quantum circuit job,
 * the following code pattern can be used:
 * @code{.cpp}
 * // Query the size of the result.
 * size_t size;
 * auto ret = QDMI_device_job_get_results(
 *   job, QDMI_JOB_RESULT_SHOTS, 0, nullptr, &size);
 *
 * // Allocate memory for the result.
 * std::string shots(size - 1, '\0');
 *
 * // Query the result.
 * QDMI_device_job_get_results(
 *   job, QDMI_JOB_RESULT_SHOTS, size, shots.data(), nullptr);
 * @endcode
 * @endparblock
 */
QDMI_EXPORT int QDMI_device_job_get_results(QDMI_Device_Job job,
                                            QDMI_Job_Result result, size_t size,
                                            void *data, size_t *size_ret);

/**
 * @brief Free a job.
 * @details Free the resources associated with a job. Using a job handle after
 * it was freed is undefined behavior. Freeing a job handle does not
 * necessarily cancel or delete the underlying job; this behavior is
 * device-specific.
 * @param[in] job The job to free.
 */
QDMI_EXPORT void QDMI_device_job_free(QDMI_Device_Job job);

/** @} */ // end of device_job_interface

/**
 * @brief A handle for a child device.
 * @details An opaque pointer to an implementation of the QDMI child device
 * concept. A child device generally represents a core or processing unit
 * of a multicore device.
 * Each implementation of the @ref device_interface "QDMI Device Interface"
 * may define the actual implementation of the concept.
 *
 * @note Only authors of a multicore device library that want to facilitate
 * job execution on a dedicated core and/or need to expose device properties on
 * a child device level must implement the concept.
 *
 * A simple example of an implementation is a struct that merely contains an
 * index, which can be used to identify the respective core / processing unit.
 * @code{.cpp}
 * struct QDMI_Child_Device_impl_d {
 *   size_t id;
 * };
 * @endcode
 * @see QDMI_DEVICE_PROPERTY_CHILDDEVICES
 * @see QDMI_DEVICE_SESSION_PARAMETER_CHILDDEVICE
 */
typedef struct QDMI_Child_Device_impl_d *QDMI_Child_Device;

/** @} */ // end of device_interface

// NOLINTEND(performance-enum-size,modernize-use-using,modernize-redundant-void-arg)

#ifdef __cplusplus
} // extern "C"
#endif
