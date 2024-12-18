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

#include "qdmi/constants.h" // IWYU pragma: export

#ifdef __cplusplus
#include <cstddef>

extern "C" {
#else
#include <stddef.h>
#endif

// The following clang-tidy warning cannot be addressed because this header is
// used from both C and C++ code.
// NOLINTBEGIN(performance-enum-size,modernize-use-using)

/**
 * @brief A handle for a device.
 * @details An opaque pointer to an implementation of the QDMI device interface.
 * The actual implementation is defined by the driver.
 */
typedef struct QDMI_Device_impl_d *QDMI_Device;

/** @defgroup client_session QDMI Client Session Interface
 *  The driver manages resources provided to clients in sessions TODO: Expand
 *  @{
 */

/**
 * @brief A handle for a QDMI session.
 * @details An opaque pointer to an implementation of the QDMI session interface
 * that makes @ref QDMI_Device handles available to clients. The actual
 * implementation is defined by the driver.
 */
typedef struct QDMI_Session_impl_d *QDMI_Session;

/**
 * @brief Allocate a new QDMI session.
 * @details The returned handle can be used in subsequent calls to @ref
 * QDMI_session_get_devices to get the devices available to the client.
 * Prior to using the session, it must be initialized using @ref
 * QDMI_session_init.
 * @param[out] session A handle to the session that is allocated. Must not be
 * @c NULL. The session must be freed by calling @ref QDMI_session_free
 * when it is no longer needed.
 * @return @ref QDMI_SUCCESS if the session was allocated successfully.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p session is @c NULL.
 * @return @ref QDMI_ERROR_OUTOFMEM if memory space ran out.
 * @return @ref QDMI_ERROR_FATAL if an unexpected error occurred.
 * @see QDMI_session_init
 */
int QDMI_session_alloc(QDMI_Session *session);

/**
 * @brief Initialize a QDMI session.
 * @details This function initializes the session and prepares it for use. The
 * session must be initialized before it can be used in @ref
 * QDMI_session_get_devices. Some devices may require authentication prior to
 * initializing the session. The required authentication information must be set
 * using @ref QDMI_session_set_parameter before calling this function.
 * @param[in] session the session to initialize.
 * @return @ref QDMI_SUCCESS if the session was initialized successfully.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p session is @c NULL.
 * @return @ref QDMI_ERROR_FATAL if an unexpected error occurred.
 */
int QDMI_session_init(QDMI_Session session);

/**
 * @brief Free a QDMI session.
 * @details This function frees the memory that was allocated for the session.
 * Accessing a (dangling) handle to a device that was attached to the session
 * after the session was freed is undefined behavior.
 * @param[in] session the session to free.
 */
void QDMI_session_free(QDMI_Session session);

/**
 * @brief Enum of the session parameters that can be set.
 * @details If not noted otherwise, parameters are optional and drivers must not
 * require them to be set.
 */
enum QDMI_SESSION_PARAMETER_T {
  /**
   * @brief `char*` (string) The token to use for the session.
   * @details The token is used for authentication within the session. The
   * driver documentation *must* document if the implementation requires this
   * parameter to be set.
   */
  QDMI_SESSION_PARAMETER_TOKEN = 0,
  /**
   * @brief `char*` (string) The username to use for the session.
   * @details The username is used for authentication within the session. The
   * driver documentation *must* document if the implementation requires this
   * parameter to be set.
   */
  QDMI_SESSION_PARAMETER_USERNAME = 1,
  /**
   * @brief `char*` (string) The project ID to use for the session.
   * @details Can be used to associate the job with a certain project, e.g., for
   * accounting purposes. The driver documentation *must* document if the
   * implementation requires this parameter to be set.
   */
  QDMI_SESSION_PARAMETER_PROJECTID = 2,
  /**
   * @brief The maximum value of the enum.
   * @details It can be used by drivers for bounds checking and validation of
   * function parameters. This value must remain the last regular member of the
   * enum besides the custom members and must be updated when new members are
   * added.
   */
  QDMI_SESSION_PARAMETER_MAX = 3,
  /**
   * @brief This property is reserved for a custom property.
   * @details The meaning and the type of this property are defined by the
   * driver. To maintain binary compatibility, the value of this enum member
   * must not be changed.
   */
  QDMI_SESSION_PARAMETER_CUSTOM1 = 999999995,
  /// @see QDMI_SESSION_PARAMETER_CUSTOM1
  QDMI_SESSION_PARAMETER_CUSTOM2 = 999999996,
  /// @see QDMI_SESSION_PARAMETER_CUSTOM1
  QDMI_SESSION_PARAMETER_CUSTOM3 = 999999997,
  /// @see QDMI_SESSION_PARAMETER_CUSTOM1
  QDMI_SESSION_PARAMETER_CUSTOM4 = 999999998,
  /// @see QDMI_SESSION_PARAMETER_CUSTOM1
  QDMI_SESSION_PARAMETER_CUSTOM5 = 999999999
};

/// Type of the session parameter.
typedef enum QDMI_SESSION_PARAMETER_T QDMI_Session_Parameter;

/**
 * @brief Set a parameter for a session.
 * @param[in] session A handle to the session to set the parameter for. Must not
 * be @c NULL.
 * @param[in] param The parameter to set. Must be one of the values specified
 * for @ref QDMI_Session_Parameter.
 * @param[in] size The size of the data pointed by @p value in bytes. Must not
 * be zero, except when @p value is @c NULL, in which case it is ignored.
 * @param[in] value The value to set the parameter to. If this is @c NULL, it is
 * ignored.
 * @return @ref QDMI_SUCCESS if the driver supports the specified @ref
 * QDMI_Session_Parameter and, when @p value is not @c NULL, the value of
 * the parameter was set successfully.
 * @return @ref QDMI_ERROR_NOTSUPPORTED if the driver does not support the
 * parameter.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p session is @c NULL, if @p param
 * is invalid, if @p value is not @c NULL and @p size is
 * zero or not the expected size for the parameter (if specified by the @ref
 * QDMI_Session_Parameter documentation).
 * @return @ref QDMI_ERROR_FATAL if an unexpected error occurred.
 *
 * @note By calling this function with @p value set to @c NULL, the function can
 * be used to check if the driver supports the specified parameter without
 * setting a value.
 */
int QDMI_session_set_parameter(QDMI_Session session,
                               QDMI_Session_Parameter param, size_t size,
                               const void *value);
/**
 * @brief Query the devices associated with @p session.
 * @param[in] session The session to query. Must not be @c NULL.
 * @param[in] num_entries The number of entries that can be added to @p devices.
 * Must be greater than zero, except when @p devices is @c NULL, in which case
 * it is ignored.
 * @param[out] devices A pointer to a list of handles where the devices
 * available to the client will be stored. If this is @c NULL, it is ignored.
 * The number of devices returned is the minimum of the value specified by
 * @p num_entries and the number of devices found.
 * @param[out] num_devices The number of devices available. If this is @c NULL,
 * it is ignored.
 * @return @ref QDMI_SUCCESS if the function is executed successfully.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p session is @c NULL, if @p
 * num_entries is zero and @p devices is not @c NULL or if both @p devices and
 * @p num_devices are @c NULL.
 * @return @ref QDMI_ERROR_FATAL if an unexpected error occurred.
 *
 * @note By calling this function with @p devices set to @c NULL, the function
 * can be used to query the number of devices available without retrieving the
 * devices.
 */
int QDMI_session_get_devices(QDMI_Session session, size_t num_entries,
                             QDMI_Device *devices, size_t *num_devices);

/** @} */ // end of client_session

/** @defgroup client_job QDMI Client Job Interface
 *  The job interface allows managing jobs on a device.
 *  @{
 */

/**
 * @brief A handle for a QDMI job.
 * @details An opaque pointer to an implementation of the QDMI job concept. The
 * actual implementation is defined by the driver. Most implementations will
 * want to store the device handle used to create the job in the job handle to
 * be able to access the device information when needed.
 *
 * @see QDMI_Device_Job
 */
typedef struct QDMI_Job_impl_d *QDMI_Job;

/**
 * @brief Create a job with a certain program on a device.
 * @param[in] dev The device to create the job on. Must not be @c NULL.
 * @param[in] format The format of the program. Must be one of the values
 * specified for @ref QDMI_Program_Format.
 * @param[in] size The size of the program in bytes. Must not be zero, except
 * when @p prog is @c NULL, in which case it is ignored.
 * @param[in] prog The program to run. If this is @c NULL, it is ignored.
 * @param[out] job A pointer to a handle that will store the created job.
 * Must not be @c NULL, except when @p prog is @c NULL, in which case it is
 * ignored.
 * @return @ref QDMI_SUCCESS if the device supports the specified @ref
 * QDMI_Program_Format @p format and, when @p prog is not @c NULL, the job was
 * successfully created.
 * @return @ref QDMI_ERROR_NOTSUPPORTED if the device does not support the
 * specified program format @p format.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p session is @c NULL, if @p
 * format is invalid, if @p prog is not @c NULL and @p size is zero or @job is
 * @c NULL, or if the program @p prog is invalid (e.g., contains a syntax
 * error).
 * @return @ref QDMI_ERROR_FATAL if the job creation failed due to a fatal
 * error.
 *
 * @note By calling this function with @p prog and @p job set to @c NULL, the
 * function can be used to check if the device supports the specified program
 * format without creating a job and without the need to provide a program.
 */
int QDMI_job_create(QDMI_Device dev, QDMI_Program_Format format, size_t size,
                    const void *prog, QDMI_Job *job);

/**
 * @brief Free a job.
 * @details Free the resources associated with a job.
 * @param[in] job The job to free.
 */
void QDMI_job_free(QDMI_Job job);

/**
 * @brief Enum of the job parameters that can be set.
 * @details If not noted otherwise, parameters are optional and drivers must not
 * require them to be set.
 */
enum QDMI_JOB_PARAMETER_T {
  /**
   * @brief `size_t` The number of shots to execute for a quantum circuit job.
   * @details If this parameter is not set, a device-specific default number of
   * shots is used.
   */
  QDMI_JOB_PARAMETER_SHOTS_NUM = 0,
  /**
   * @brief The maximum value of the enum.
   * @details It can be used by drivers for bounds checking and validation of
   * function parameters. This value must remain the last regular member of the
   * enum besides the custom members and must be updated when new members are
   * added.
   */
  QDMI_JOB_PARAMETER_MAX = 1,
  /**
   * @brief This property is reserved for a custom property.
   * @details The meaning and the type of this property is defined by the
   * device.
   */
  QDMI_JOB_PARAMETER_CUSTOM1 = 999999995,
  /// @see QDMI_JOB_PARAMETER_CUSTOM1
  QDMI_JOB_PARAMETER_CUSTOM2 = 999999996,
  /// @see QDMI_JOB_PARAMETER_CUSTOM1
  QDMI_JOB_PARAMETER_CUSTOM3 = 999999997,
  /// @see QDMI_JOB_PARAMETER_CUSTOM1
  QDMI_JOB_PARAMETER_CUSTOM4 = 999999998,
  /// @see QDMI_JOB_PARAMETER_CUSTOM1
  QDMI_JOB_PARAMETER_CUSTOM5 = 999999999
};

/// Type of the job parameter.
typedef enum QDMI_JOB_PARAMETER_T QDMI_Job_Parameter;

/**
 * @brief Set a parameter for a job.
 * @param[in] job A handle to a job for which to set @p param. Must not be @c
 * NULL.
 * @param[in] param The parameter whose value will be set. Must be one of the
 * values specified for @ref QDMI_Job_Parameter.
 * @param[in] size The size of the data pointed to by @p value in bytes. Must
 * not be zero, except when @p value is @c NULL, in which case it is ignored.
 * @param[in] value A pointer to the memory location that contains the value of
 * the parameter to be set. The data pointed to by @p value is copied and can be
 * safely reused after this function returns. If this is @c NULL, it is ignored.
 * @return @ref QDMI_SUCCESS if the driver supports the specified @ref
 * QDMI_Job_Parameter @p param and, when @p value is not @c NULL, the
 * parameter was successfully set.
 * @return @ref QDMI_ERROR_NOTSUPPORTED if the driver does not support the
 * parameter.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p job is @c NULL, if @p param is
 * invalid, if @p value is not @c NULL and @p size is zero or not the expected
 * size for the parameter (if specified by the @ref QDMI_Job_Parameter
 * documentation).
 * @return @ref QDMI_ERROR_FATAL if setting the parameter failed due to a fatal
 * error.
 *
 * @note By calling this function with @p value set to @c NULL, the function can
 * be used to check if the driver supports the specified parameter without
 * setting the parameter and without the need to provide a value.
 */
int QDMI_job_set_parameter(QDMI_Job job, QDMI_Job_Parameter param, size_t size,
                           const void *value);

/**
 * @brief Submit a job to the device.
 * @details This function can either be blocking until the job is finished or
 * non-blocking and return while the job is running. In the latter case, the
 * functions @ref QDMI_job_check and @ref QDMI_job_wait can be used to check the
 * status and wait for the job to finish.
 * @param[in] job The job to submit. Must not be @c NULL.
 * @return @ref QDMI_SUCCESS if the job was successfully submitted.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p job is @c NULL or in an invalid
 * state.
 * @return @ref QDMI_ERROR_FATAL if the job submission failed.
 */
int QDMI_job_submit(QDMI_Job job);

/**
 * @brief Cancel an already submitted job.
 * @details Remove the job from the queue of waiting jobs. This changes the
 * status of the job to @ref QDMI_JOB_STATUS_CANCELLED.
 * @param[in] job The job to cancel. Must not be @c NULL.
 * @return @ref QDMI_SUCCESS if the job was successfully cancelled.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @job is @c NULL or the job already
 * has the status @ref QDMI_JOB_STATUS_DONE.
 * @return @ref QDMI_ERROR_FATAL if the job could not be cancelled.
 */
int QDMI_job_cancel(QDMI_Job job);

/**
 * @brief Check the status of a job.
 * @details This function is non-blocking and returns immediately with the job
 * status. It is not necessary to call this function before calling @ref
 * QDMI_job_get_data.
 * @param[in] job The job to check the status of. Must not be @c NULL.
 * @param[out] status The status of the job. Must not be @c NULL.
 * @return @ref QDMI_SUCCESS if the job status was successfully checked.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p job or @p status is @c NULL.
 */
int QDMI_job_check(QDMI_Job job, QDMI_Job_Status *status);

/**
 * @brief Wait for a job to finish.
 * @details This function blocks until the job has either finished or has been
 * cancelled.
 * @param[in] job The job to wait for. Must not be @c NULL.
 * @return @ref QDMI_SUCCESS if the job is finished or cancelled.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p job is @c NULL.
 * @return @ref QDMI_ERROR_FATAL if the job could not be waited for and this
 * function returns before the job has finished or has been cancelled.
 */
int QDMI_job_wait(QDMI_Job job);

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
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if the job has not finished yet, was
 * cancelled, or does not exist.
 * @return @ref QDMI_ERROR_FATAL if an error occurred during the retrieval.
 *
 * @note By calling this function with @p data set to @c NULL, the function can
 * be used to check if the device supports the specified result without
 * retrieving the result and without the need to provide a buffer for the
 * result. The size of the buffer needed to retrieve the result is returned in
 * @p size_ret if @p size_ret is not @c NULL.
 */
int QDMI_job_get_data(QDMI_Job job, QDMI_Job_Result result, size_t size,
                      void *data, size_t *size_ret);

/** @} */ // end of client_job

/** @defgroup client_query QDMI Client Query Interface
 *  The query interface enables to query static and dynamic properties of the
 *  device in a unified fashion.
 *  @{
 */

/**
 * @brief Query a device property.
 * @param[in] device The device to query. Must not be @c NULL.
 * @param[in] prop The property to query. Must be one of the values specified
 * for @ref QDMI_Device_Property.
 * @param[in] size The size of the memory pointed to by @p value in bytes. Must
 * be greater or equal to the size of the return type specified for the @ref
 * QDMI_Device_Property @p prop, except when @p value is @c NULL, in which case
 * it is ignored.
 * @param[out] value A pointer to the memory location where the value of the
 * property will be stored. If this is @c NULL, it is ignored.
 * @param[out] size_ret The actual size of the data being queried in bytes. If
 * this is @c NULL, it is ignored.
 * @return @ref QDMI_SUCCESS if the device supports the specified property and,
 * when @p value is not @c NULL, the property was successfully retrieved.
 * @return @ref QDMI_ERROR_NOTSUPPORTED if the property is not supported by the
 * device.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p device is @c NULL, if @p prop
 * is invalid, if the size in bytes specified by @p size is less than the size
 * of the data being queried as specified for the @ref QDMI_Device_Property @p
 * prop and @p value is not @c NULL, or if both @p value and @p size_ret are @c
 * NULL.
 * @return @ref QDMI_ERROR_FATAL if an unexpected error occurred.
 *
 * @note By calling this function with @p value set to @c NULL, the function can
 * be used to check if the device supports the specified property without
 * retrieving the property and without the need to provide a buffer for it. The
 * size of the buffer needed to retrieve the property is returned in @p size_ret
 * if @p size_ret is not @c NULL.
 */
int QDMI_device_query_property(QDMI_Device device, QDMI_Device_Property prop,
                               size_t size, void *value, size_t *size_ret);

/**
 * @brief Get the sites associated with @p device.
 * @param[in] device The device to query. Must not be @c NULL.
 * @param[in] num_entries The number of entries that can be added to @p sites.
 * Must be greater than zero, except when @p sites is @c NULL, in which case it
 * is ignored.
 * @param[out] sites A pointer to a list of handles where the sites available on
 * the device will be stored. If this is @c NULL, it is ignored. The number of
 * sites returned is the minimum of the value specified by @p num_entries and
 * the number of sites found.
 * @param[out] num_sites The number of sites available. If this is @c NULL, it
 * is ignored.
 * @return @ref QDMI_SUCCESS if the function is executed successfully.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p device is @c NULL, if @p
 * num_entries is zero and @p sites is not @c NULL or if both @p sites and @p
 * num_sites are @c NULL.
 * @return @ref QDMI_ERROR_FATAL if an unexpected error occurred.
 *
 * @note By calling this function with @p sites set to @c NULL, the function can
 * be used to query the number of sites available without retrieving the sites.
 */
int QDMI_device_get_sites(QDMI_Device device, size_t num_entries,
                          QDMI_Site *sites, size_t *num_sites);

/**
 * @brief Get the operations available on @p device.
 * @param[in] device The device to query. Must not be @c NULL.
 * @param[in] num_entries The number of entries that can be added to @p
 * operations. Must be greater than zero, except when @p operations is @c NULL,
 * in which case it is ignored.
 * @param[out] operations A pointer to a list of handles where the operations
 * available on the device will be stored. If this is @c NULL, it is ignored.
 * The number of operations returned is the minimum of the value specified by
 * @p num_entries and the number of operations found.
 * @param[out] num_operations The number of operations available. If this is @c
 * NULL, it is ignored.
 * @return @ref QDMI_SUCCESS if the function is executed successfully.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p device is @c NULL, if @p
 * num_entries is zero and @p operations is not @c NULL or if both @p operations
 * and @p num_operations are @c NULL.
 * @return @ref QDMI_ERROR_FATAL if an unexpected error occurred.
 *
 * @note By calling this function with @p operations set to @c NULL, the
 * function can be used to query the number of operations available without
 * retrieving the operations.
 */
int QDMI_device_get_operations(QDMI_Device device, size_t num_entries,
                               QDMI_Operation *operations,
                               size_t *num_operations);

/**
 * @brief Query a site property.
 * @param[in] site The site to query. Must not be @c NULL.
 * @param[in] prop The property to query. Must be one of the values specified
 * for @ref QDMI_Site_Property.
 * @param[in] size The size of the memory pointed to by @p value in bytes. Must
 * be greater or equal to the size of the return type specified for the @ref
 * QDMI_Site_Property @p prop, except when @p value is @c NULL, in which case it
 * is ignored.
 * @param[out] value A pointer to the memory location where the value of the
 * property will be stored. If this is @c NULL, it is ignored.
 * @param[out] size_ret The actual size of the data being queried in bytes. If
 * this is @c NULL, it is ignored.
 * @return @ref QDMI_SUCCESS if the device supports the specified property and,
 * when @p value is not @c NULL, the property was successfully retrieved.
 * @return @ref QDMI_ERROR_NOTSUPPORTED if the property is not supported by the
 * device.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p site is @c NULL, if @p prop is
 * invalid, if the size in bytes specified by @p size is less than the size of
 * the data being queried as specified for the @ref QDMI_Site_Property @p prop
 * and @p value is not @c NULL, or if both @p value and @p size_ret are @c NULL.
 * @return @ref QDMI_ERROR_FATAL if an unexpected error occurred.
 *
 * @note By calling this function with @p value set to @c NULL, the function can
 * be used to check if the device supports the specified property without
 * retrieving the property and without the need to provide a buffer for it.
 */
int QDMI_site_query_property(QDMI_Site site, QDMI_Site_Property prop,
                             size_t size, void *value, size_t *size_ret);

/**
 * @brief Query a device operation property.
 * @param[in] operation The operation to query. Must not be @c NULL.
 * @param[in] num_sites The number of sites that the operation is applied to.
 * @param[in] sites A pointer to a list of handles where the sites that the
 * operation is applied to are stored. If this is @c NULL, the property is
 * queried for all site, e.g., the average value of all sites.
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
 * device or if the queried property cannot be provided for the given sites.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p operation is @c NULL, if @p
 * prop is invalid, if the size in bytes specified by @p size is less than the
 * size of the data being queried as specified for the @ref
 * QDMI_Operation_Property @p prop and @p value is not @c NULL, or if both @p
 * value and @p size_ret are @c NULL.
 * @return @ref QDMI_ERROR_FATAL if an unexpected error occurred.
 *
 * @note By calling this function with @p value set to @c NULL, the function can
 * be used to check if the device supports the specified property without
 * retrieving the property and without the need to provide a buffer for it.
 *
 * @note If a device reports @ref QDMI_ERROR_NOTSUPPORTED for a property, it may
 * still be possible to query the property for all sites by passing @c NULL to
 * @p sites. In this case, the device may return the average value of the
 * property for all sites.
 */
int QDMI_operation_query_property(QDMI_Operation operation, size_t num_sites,
                                  const QDMI_Site *sites,
                                  QDMI_Operation_Property prop, size_t size,
                                  void *value, size_t *size_ret);

/** @} */ // end of client_query

// NOLINTEND(performance-enum-size,modernize-use-using)

#ifdef __cplusplus
} // extern "C"
#endif
