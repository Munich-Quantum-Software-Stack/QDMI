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
 * @brief The QDMI device interface.
 * @details The purpose of the device interface is to provide a set of functions
 * that allow one to represent a physical quantum computer or classical quantum
 * simulator with a unified interface. The device interface provides functions
 * to establish sessions between a QDMI driver and a device, as well as to
 * interact with the device by querying its properties and submitting jobs to
 * the device.
 */

#pragma once

#include "qdmi/constants.h" // IWYU pragma: export

#ifdef __cplusplus
#include <cstddef>
#include <cstdint>

extern "C" {
#else
#include <stddef.h>
#include <stdint.h>
#endif

// The following clang-tidy warning cannot be addressed because this header is
// used from both C and C++ code.
// NOLINTBEGIN(performance-enum-size,modernize-use-using)

/**
 * @brief Initialize a device.
 * @details A device can expect that this function is called once in the
 * beginning and has returned before any other functions are invoked on that
 * device.
 * @return @ref QDMI_SUCCESS if the initialization was successful.
 * @return @ref QDMI_ERROR_FATAL if the initialization failed.
 */
int QDMI_device_initialize(void);

/**
 * @brief Finalize a device.
 * @details A device can expect that this function is called once at the end of
 * using the device and no other functions are invoked on that device after that
 * anymore.
 * @return @ref QDMI_SUCCESS if the finalization was successful.
 * @return @ref QDMI_ERROR_FATAL if the finalization failed, this could, e.g.,
 * be due to a job that is still running.
 */
int QDMI_device_finalize(void);

/** @defgroup device_session QDMI Device Session Interface
 *  The concept of sessions is used to establish a connection between a driver
 *  and a device. Sessions are used to authenticate with the device and to
 *  manage resources required for the interaction with the device.
 *  @{
 */

/**
 * @brief A handle for a QDMI device session.
 * @details An opaque pointer to an implementation of the QDMI device session
 * concept. The actual implementation is defined by the device.
 */
typedef struct QDMI_Device_Session_impl_d *QDMI_Device_Session;

/**
 * @brief Allocate a new QDMI device session.
 * @details The returned handle can be used in subsequent calls throughout the
 * client interface to refer to the session. However, the session must be
 * initialized with @ref QDMI_device_session_init before it can be used to
 * interact with the device.
 * @param[out] session A handle to the session that is allocated. Must not be
 * @c NULL. The session must be freed by calling @ref QDMI_device_session_free
 * when it is no longer needed.
 * @return @ref QDMI_SUCCESS if the session was allocated successfully.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p session is @c NULL.
 * @return @ref QDMI_ERROR_OUTOFMEM if memory space ran out.
 * @return @ref QDMI_ERROR_FATAL if an unexpected error occurred.
 * @see QDMI_device_session_init
 */
int QDMI_device_session_alloc(QDMI_Device_Session *session);

// TODO: seems like c&p error from the client interface. Needs to be adapted.
/**
 * @brief Initialize a QDMI device session.
 * @details This function initializes the session with the device. The session
 * must be initialized before it can be used to interact with the device. Some
 * devices may require authentication prior to using the session. The required
 * authentication information must be set using @ref
 * QDMI_device_session_set_parameter before calling this function. Functions
 * like @ref QDMI_device_query_property or @ref QDMI_job_create must not be
 * called before the session is initialized.
 * @param[in] session The session to initialize. Must not be @c NULL.
 * @return @ref QDMI_SUCCESS if the session was initialized successfully.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p session is @c NULL.
 * @return @ref QDMI_ERROR_FATAL if an unexpected error occurred.
 */
int QDMI_device_session_init(QDMI_Device_Session session);

/**
 * @brief Free a QDMI device session.
 * @details This function frees the memory that was allocated for the session.
 * Using a session handle after it was freed is undefined behavior.
 * @param[in] session The session to free.
 */
void QDMI_device_session_free(QDMI_Device_Session session);

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
 * @brief Set a parameter for a device session.
 * @param[in] session A handle to the session to set the parameter for. Must not
 * be @c NULL.
 * @param[in] param The parameter to set. Must be one of the values specified
 * for @ref QDMI_Device_Session_Parameter.
 * @param[in] size The size of the data pointed by @p value in bytes. Must not
 * be zero, except when @p value is @c NULL, in which case it is ignored.
 * @param[in] value The value to set the parameter to. If this is @c NULL, it is
 * ignored.
 * @return @ref QDMI_SUCCESS if the device supports the specified @ref
 * QDMI_Device_Session_Parameter and, when @p value is not @c NULL, the value of
 * the parameter was set successfully.
 * @return @ref QDMI_ERROR_NOTSUPPORTED if the device does not support the
 * parameter.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p session is @c NULL, if @p param
 * is invalid, if @p value is not @c NULL and @p size is
 * zero or not the expected size for the parameter (if specified by the @ref
 * QDMI_Device_Session_Parameter documentation).
 * @return @ref QDMI_ERROR_FATAL if an unexpected error occurred.
 *
 * @note By calling this function with @p value set to @c NULL, the function can
 * be used to check if the device supports the specified parameter without
 * setting a value.
 */
int QDMI_device_session_set_parameter(QDMI_Device_Session session,
                                      QDMI_Device_Session_Parameter param,
                                      size_t size, const void *value);

/** @} */ // end of device_session

/** @defgroup device_job QDMI Device Job Interface
 *  The job interface allows managing jobs on a device.
 *  @{
 */

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
 * @brief Create a job with a certain program on a device.
 * @param[in] session The session to create the job on. Must not be @c NULL.
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
int QDMI_device_job_create(QDMI_Device_Session session,
                           QDMI_Program_Format format, size_t size,
                           const void *prog, QDMI_Device_Job *job);

/**
 * @brief Free a job.
 * @details Free the resources associated with a job.
 * @param[in] job The job to free.
 */
void QDMI_device_job_free(QDMI_Device_Job job);

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
 * parameter.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p job is @c NULL, if @p param is
 * invalid, if @p value is not @c NULL and @p size is zero or not the expected
 * size for the parameter (if specified by the @ref QDMI_Device_Job_Parameter
 * documentation).
 * @return @ref QDMI_ERROR_FATAL if setting the parameter failed due to a fatal
 * error.
 *
 * @note By calling this function with @p value set to @c NULL, the function can
 * be used to check if the device supports the specified parameter without
 * setting the parameter and without the need to provide a value.
 */
int QDMI_device_job_set_parameter(QDMI_Device_Job job,
                                  QDMI_Device_Job_Parameter param, size_t size,
                                  const void *value);

/**
 * @brief Submit a job to the device.
 * @details This function can either be blocking until the job is finished or
 * non-blocking and return while the job is running. In the latter case, the
 * functions @ref QDMI_device_job_check and @ref QDMI_device_job_wait can be
 * used to check the status and wait for the job to finish.
 * @param[in] job The job to submit. Must not be @c NULL.
 * @return @ref QDMI_SUCCESS if the job was successfully submitted.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p job is @c NULL or in an invalid
 * state.
 * @return @ref QDMI_ERROR_FATAL if the job submission failed.
 */
int QDMI_device_job_submit(QDMI_Device_Job job);

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
int QDMI_device_job_cancel(QDMI_Device_Job job);

/**
 * @brief Check the status of a job.
 * @details This function is non-blocking and returns immediately with the job
 * status. It is not necessary to call this function before calling @ref
 * QDMI_device_job_get_data.
 * @param[in] job The job to check the status of. Must not be @c NULL.
 * @param[out] status The status of the job. Must not be @c NULL.
 * @return @ref QDMI_SUCCESS if the job status was successfully checked.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p job or @p status is @c NULL.
 */
int QDMI_device_job_check(QDMI_Device_Job job, QDMI_Job_Status *status);

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
int QDMI_device_job_wait(QDMI_Device_Job job);

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
 * result. The size of the buffer required to retrieve the result is returned in
 * @p size_ret if @p size_ret is not @c NULL.
 */
int QDMI_device_job_get_data(QDMI_Device_Job job, QDMI_Job_Result result,
                             size_t size, void *data, size_t *size_ret);

/** @} */ // end of device_job

/** @defgroup device_query QDMI Device Query Interface
 *  The query interface enables to query static and dynamic properties of the
 *  device in a unified fashion.
 *  @{
 */

/**
 * @brief Query a device property.
 * @param[in] session The session used for the query. Must not be @c NULL.
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
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p session is @c NULL, if @p prop
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
int QDMI_device_session_query_property(QDMI_Device_Session session,
                                       QDMI_Device_Property prop, size_t size,
                                       void *value, size_t *size_ret);

// TODO: probably needs more explicit docs on the site parameter and errors
/**
 * @brief Query a site property.
 * @param[in] session The session used for the query. Must not be @c NULL.
 * @param[in] site The site to query.
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
int QDMI_device_site_query_property(QDMI_Device_Session session, uint64_t site,
                                    QDMI_Site_Property prop, size_t size,
                                    void *value, size_t *size_ret);

// TODO: probably needs more explicit docs on the operation and sites parameter
//       as well as errors
/**
 * @brief Query a device operation property.
 * @param[in] session The session used for the query. Must not be @c NULL.
 * @param[in] operation The name of the operation to query. Must not be @c NULL.
 * @param[in] num_sites The number of sites that the operation is applied to.
 * @param[in] sites A list of sites that the operation is applied to. If this is
 * @c NULL, the property is queried for all sites, e.g., the average value of
 * all sites.
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
int QDMI_device_operation_query_property(
    QDMI_Device_Session session, const char *operation, size_t num_sites,
    const uint64_t *sites, QDMI_Operation_Property prop, size_t size,
    void *value, size_t *size_ret);

/** @} */ // end of device_query

// NOLINTEND(performance-enum-size,modernize-use-using)

#ifdef __cplusplus
} // extern "C"
#endif
