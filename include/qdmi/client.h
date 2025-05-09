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
 * @brief Defines the @ref client_interface.
 */

#pragma once

#include "qdmi/constants.h" // IWYU pragma: export
#include "qdmi/types.h"     // IWYU pragma: export

#ifdef __cplusplus
#include <cstddef>

extern "C" {
#else
#include <stddef.h>
#endif

// The following clang-tidy warning cannot be addressed because this header is
// used from both C and C++ code.
// NOLINTBEGIN(performance-enum-size, modernize-use-using)

/** @defgroup client_interface QDMI Client Interface
 *  @brief Describes the functions accessible to clients or users of QDMI.
 *  @details This is an interface between the QDMI driver and the client.
 *  It includes functions to establish sessions between a QDMI driver and a
 *  client, as well as to interact with the devices managed by the driver.
 *
 *  The client interface is split into three parts:
 *  - The @ref client_session_interface "client session interface" for managing
 * sessions between a QDMI driver and a client.
 *  - The @ref client_query_interface "client query interface" for querying
 * properties of devices.
 *  - The @ref client_job_interface "client job interface" for submitting jobs
 * to devices.
 *  - The @ref client_environment_query_interface "client environment query
 * interface" for querying environmental variables.
 *
 * @{
 */

/**
 * @brief A handle for a device implementing the
 * @ref device_interface "QDMI Device Interface".
 * @details An opaque pointer to a type defined by the driver that encapsulates
 * an implementation of the  @ref device_interface "QDMI Device Interface".
 */
typedef struct QDMI_Device_impl_d *QDMI_Device;

/** @defgroup client_session_interface QDMI Client Session Interface
 *  @brief Provides functions to manage sessions between the client and driver.
 *  @details A session is a connection between a client and a QDMI driver that
 *  allows the client to interact with the driver and the devices it manages.
 *
 *  The typical workflow for a client session is as follows:
 *  - Allocate a session with @ref QDMI_session_alloc.
 *  - Set parameters for the session with @ref QDMI_session_set_parameter.
 *  - Initialize the session with @ref QDMI_session_init.
 *  - Query the available devices with @ref QDMI_session_query_session_property.
 *  - Run client code to interact with the retrieved @ref QDMI_Device handles
 *    using the @ref client_query_interface "client query interface" and the
 * @ref client_job_interface "client job interface".
 *  - Free the session with @ref QDMI_session_free when it is no longer needed.
 *
 *  @{
 */

/**
 * @brief A handle for a session.
 * @details An opaque pointer to a type defined by the driver that encapsulates
 * all information about a session between a client and a QDMI driver.
 */
typedef struct QDMI_Session_impl_d *QDMI_Session;

/**
 * @brief Allocate a new session.
 * @details This is the main entry point for a client to establish a session
 * with a QDMI driver. The returned handle can be used throughout the
 * @ref client_session_interface "client session interface" to refer to the
 * session.
 * @param[out] session A handle to the session that is allocated. Must not be
 * @c NULL. The session must be freed by calling @ref QDMI_session_free
 * when it is no longer used.
 * @return @ref QDMI_SUCCESS if the session was allocated successfully.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p session is @c NULL.
 * @return @ref QDMI_ERROR_OUTOFMEM if memory space ran out.
 * @return @ref QDMI_ERROR_FATAL if an unexpected error occurred.
 * @see QDMI_session_set_parameter
 * @see QDMI_session_init
 */
int QDMI_session_alloc(QDMI_Session *session);

/**
 * @brief Enum of the session parameters that can be set via @ref
 * QDMI_session_set_parameter.
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
   * @details Can be used to associate the session with a certain project, for
   * example, for accounting purposes. The driver documentation *must* document
   * if the implementation requires this parameter to be set.
   */
  QDMI_SESSION_PARAMETER_PROJECTID = 2,
  /**
   * @brief The maximum value of the enum.
   * @details It can be used by drivers for bounds checking and validation of
   * function parameters.
   *
   * @attention This value must remain the last regular member of the enum
   * besides the custom members and must be updated when new members are added.
   */
  QDMI_SESSION_PARAMETER_MAX = 3,
  /**
   * @brief This enum value is reserved for a custom parameter.
   * @details The driver defines the meaning and the type of this parameter.
   * @attention The value of this enum member must not be changed to maintain
   * binary compatibility.
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

/// Session parameter type.
typedef enum QDMI_SESSION_PARAMETER_T QDMI_Session_Parameter;

/**
 * @brief Set a parameter for a session.
 * @param[in] session A handle to the session to set the parameter for. Must not
 * be @c NULL.
 * @param[in] param The parameter to set. Must be one of the values specified
 * for @ref QDMI_Session_Parameter.
 * @param[in] size The size of the data pointed to by @p value in bytes. Must
 * not be zero, except when @p value is @c NULL, in which case it is ignored.
 * @param[in] value A pointer to the memory location that contains the value of
 * the parameter to be set. The data pointed to by @p value is copied and can be
 * safely reused after this function returns. If this is @c NULL, it is ignored.
 * @return @ref QDMI_SUCCESS if the driver supports the specified @p param and,
 * when @p value is not @c NULL, the value of the parameter was set
 * successfully.
 * @return @ref QDMI_ERROR_NOTSUPPORTED if the driver does not support the
 * parameter or the value of the parameter.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if
 *  - @p session is @c NULL,
 *  - @p param is invalid, or
 *  - @p value is not @c NULL and @p size is zero or not the expected size for
 *    the parameter (if specified by the @ref QDMI_Session_Parameter
 *    documentation).
 * @return @ref QDMI_ERROR_BADSTATE if the parameter cannot be set in the
 * current state of the session, for example, because the session is already
 * initialized.
 * @return @ref QDMI_ERROR_FATAL if an unexpected error occurred.
 * @see QDMI_session_init
 *
 * @note By calling this function with @p value set to @c NULL, the function can
 * be used to check if the driver supports the specified parameter without
 * setting a value.
 *
 * @note For example, to check whether the driver supports setting a token for
 * authentication, the following code pattern can be used:
 * ```
 * // Check if the driver supports setting a token.
 * auto ret = QDMI_session_set_parameter(
 *   session, QDMI_SESSION_PARAMETER_TOKEN, 0, nullptr);
 * if (ret == QDMI_ERROR_NOTSUPPORTED) {
 *  // The driver does not support setting a token.
 * }
 *
 * // Set the token.
 * std::string token = "token";
 * ret = QDMI_session_set_parameter(
 *   session, QDMI_SESSION_PARAMETER_TOKEN, token.size() + 1, token.c_str());
 * ```
 */
int QDMI_session_set_parameter(QDMI_Session session,
                               QDMI_Session_Parameter param, size_t size,
                               const void *value);

/**
 * @brief Initialize a session.
 * @details This function initializes the session and prepares it for use. The
 * session must be initialized before properties can be queried using @ref
 * QDMI_session_query_session_property. Some devices may require authentication
 * information to be set using @ref QDMI_session_set_parameter before calling
 * this function. A session may only be successfully initialized once.
 * @param[in] session The session to initialize. Must not be @c NULL.
 * @return @ref QDMI_SUCCESS if the session was initialized successfully.
 * @return @ref QDMI_ERROR_PERMISSIONDENIED if the session could not be
 * initialized due to missing permissions. This could be due to missing
 * authentication information that should be set using @ref
 * QDMI_session_set_parameter.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p session is @c NULL.
 * @return @ref QDMI_ERROR_BADSTATE if the session is not in a state allowing
 * initialization, for example, because the session is already initialized.
 * @return @ref QDMI_ERROR_FATAL if an unexpected error occurred.
 * @see QDMI_session_set_parameter
 * @see QDMI_session_query_session_property
 */
int QDMI_session_init(QDMI_Session session);

/**
 * @brief Enum of the session properties that can be queried via @ref
 * QDMI_session_query_session_property.
 * @details If not noted otherwise, properties are optional and drivers must not
 * require them to be set.
 */
enum QDMI_SESSION_PROPERTY_T {
  /**
   * @brief `QDMI_Device*` (@ref QDMI_Device list) The devices the client has
   * access to.
   */
  QDMI_SESSION_PROPERTY_DEVICES = 0,
  /**
   * @brief The maximum value of the enum.
   * @details It can be used by drivers for bounds checking and validation of
   * function parameters.
   * @attention This value must remain the last regular member of the enum
   * besides the custom members and must be updated when new members are added.
   */
  QDMI_SESSION_PROPERTY_MAX = 1,
  /**
   * @brief This enum value is reserved for a custom property.
   * @details The driver defines the meaning and the type of this property.
   * @attention The value of this enum member must not be changed to maintain
   * binary compatibility.
   */
  QDMI_SESSION_PROPERTY_CUSTOM1 = 999999995,
  /// @see QDMI_SESSION_PROPERTY_CUSTOM1
  QDMI_SESSION_PROPERTY_CUSTOM2 = 999999996,
  /// @see QDMI_SESSION_PROPERTY_CUSTOM1
  QDMI_SESSION_PROPERTY_CUSTOM3 = 999999997,
  /// @see QDMI_SESSION_PROPERTY_CUSTOM1
  QDMI_SESSION_PROPERTY_CUSTOM4 = 999999998,
  /// @see QDMI_SESSION_PROPERTY_CUSTOM1
  QDMI_SESSION_PROPERTY_CUSTOM5 = 999999999
};

/// Session property type.
typedef enum QDMI_SESSION_PROPERTY_T QDMI_Session_Property;

/**
 * @brief Query a property of a session.
 * @param[in] session The session to query. Must not be @c NULL.
 * @param[in] prop The property to query. Must be one of the values specified
 * for @ref QDMI_Session_Property.
 * @param[in] size The size of the memory pointed to by @p value in bytes. Must
 * be greater or equal to the size of the return type specified for the @ref
 * QDMI_Session_Property @p prop, except when @p value is @c NULL, in which case
 * it is ignored.
 * @param[out] value A pointer to the memory location where the value of the
 * property will be stored. If this is @c NULL, it is ignored.
 * @param[out] size_ret The actual size of the data being queried in bytes. If
 * this is @c NULL, it is ignored.
 * @return @ref QDMI_SUCCESS if the driver supports the specified property and,
 * when @p value is not @c NULL, the property was successfully retrieved.
 * @return @ref QDMI_ERROR_NOTSUPPORTED if the driver does not support the
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
 * @note By calling this function with @p value set to @c NULL, the function
 * can be used to check if the driver supports the specified property without
 * retrieving the property and without the need to provide a buffer for it.
 * Additionally, the size of the buffer needed to retrieve the property will be
 * returned in @p size_ret if @p size_ret is not @c NULL.
 *
 * @note
 * For example, to query the devices available in a session, the following code
 * pattern can be used:
 * ```c
 * // Query the size of the property.
 * size_t size;
 * auto ret = QDMI_session_query_session_property(
 *   session, QDMI_SESSION_PROPERTY_DEVICES, 0, nullptr, &size);
 *
 * // Allocate memory for the property.
 * auto devices = std::vector<QDMI_Device>(size / sizeof(QDMI_Device));
 *
 * // Query the property.
 * ret = QDMI_session_query_session_property(
 *   session, prop, size, static_cast<void*>(devices.data()), nullptr);
 * ```
 *
 * @attention May only be called after the session has been successfully
 * initialized with @ref QDMI_session_init.
 */
int QDMI_session_query_session_property(QDMI_Session session,
                                        QDMI_Session_Property prop, size_t size,
                                        void *value, size_t *size_ret);

/**
 * @brief Free a session.
 * @details This function frees the memory allocated for the session.
 * Accessing a (dangling) handle to a device that was attached to the session
 * after the session was freed is undefined behavior.
 * @param[in] session The session to free.
 */
void QDMI_session_free(QDMI_Session session);

/** @} */ // end of client_session_interface

/** @defgroup client_query_interface QDMI Client Query Interface
 *  @brief Provides functions to query properties of devices.
 *  @details The query interface enables to query static and dynamic properties
 *  of devices and their constituents in a unified fashion. It operates on @ref
 *  QDMI_Device handles queried from a @ref QDMI_Session via @ref
 *  QDMI_session_query_session_property.
 *
 *  @{
 */

/**
 * @brief Query a device property.
 * @param[in] device The device to query. Must not be @c NULL.
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
 *  - @p device is @c NULL,
 *  - @p prop is invalid, or
 *  - @p value is not @c NULL and @p size is less than the size of the data
 *    being queried.
 * @return @ref QDMI_ERROR_FATAL if an unexpected error occurred.
 *
 * @note By calling this function with @p value set to @c NULL, the function can
 * be used to check if the device supports the specified property without
 * retrieving the property and without the need to provide a buffer for it.
 * Additionally, the size of the buffer needed to retrieve the property is
 * returned in @p size_ret if @p size_ret is not @c NULL.
 *
 * @note For example, to query the name of a device, the following code pattern
 * can be used:
 * ```
 * // Query the size of the property.
 * size_t size;
 * QDMI_device_query_device_property(
 *   device, QDMI_DEVICE_PROPERTY_NAME, 0, nullptr, &size);
 *
 * // Allocate memory for the property.
 * auto name = std::string(size - 1, '\0');
 *
 * // Query the property.
 * QDMI_device_query_device_property(
 *   device, QDMI_DEVICE_PROPERTY_NAME, size, name.data(), nullptr);
 * ```
 */
int QDMI_device_query_device_property(QDMI_Device device,
                                      QDMI_Device_Property prop, size_t size,
                                      void *value, size_t *size_ret);

/**
 * @brief Query a site property.
 * @param[in] device The device to query. Must not be @c NULL.
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
 *  - @p device or @p site is @c NULL,
 *  - @p prop is invalid, or
 *  - @p value is not @c NULL and @p size is less than the size of the data
 *    being queried.
 * @return @ref QDMI_ERROR_FATAL if an unexpected error occurred.
 *
 * @note By calling this function with @p value set to @c NULL, the function can
 * be used to check if the device supports the specified property without
 * retrieving the property and without the need to provide a buffer for it.
 * Additionally, the size of the buffer needed to retrieve the property is
 * returned in @p size_ret if @p size_ret is not @c NULL.
 *
 * @note For example, to query the T1 time of a site, the following code pattern
 * can be used:
 * ```
 * // Check if the device supports the property.
 * auto ret = QDMI_device_query_site_property(
 *   device, site, QDMI_SITE_PROPERTY_T1, 0, nullptr, nullptr);
 * if (ret == QDMI_ERROR_NOTSUPPORTED) {
 *   // The device does not support the property.
 *   ...
 * }
 *
 * // Query the property.
 * double t1;
 * QDMI_device_query_site_property(
 *   device, site, QDMI_SITE_PROPERTY_T1, sizeof(double), &t1, nullptr);
 * ```
 *
 * @remark @ref QDMI_Site handles may be queried via @ref
 * QDMI_device_query_device_property with @ref QDMI_DEVICE_PROPERTY_SITES.
 */
int QDMI_device_query_site_property(QDMI_Device device, QDMI_Site site,
                                    QDMI_Site_Property prop, size_t size,
                                    void *value, size_t *size_ret);

/**
 * @brief Query an operation property.
 * @param[in] device The device to query. Must not be @c NULL.
 * @param[in] operation The operation to query. Must not be @c NULL.
 * @param[in] num_sites The number of sites that the operation is applied to.
 * @param[in] sites A pointer to a list of handles where the sites that the
 * operation is applied to are stored. If this is @c NULL, it is ignored.
 * @param[in] num_params The number of parameters that the operation takes.
 * @param[in] params A pointer to a list of parameters that the operation takes.
 * If this is @c NULL, it is ignored.
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
 * @return @ref QDMI_ERROR_NOTSUPPORTED if
 *  - the device does not support the property,
 *  - the queried property cannot be provided for the given sites, or
 *  - the queried property cannot be provided for the given parameters.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if
 *  - @p device or @p operation are @c NULL,
 *  - @p prop is invalid,
 *  - @p num_sites is zero and @p sites is not @c NULL,
 *  - @p num_params is zero and @p params is not @c NULL, or
 *  - @p value is not @c NULL and @p size is less than the size of the data
 *    being queried.
 * @return @ref QDMI_ERROR_FATAL if an unexpected error occurred.
 *
 * @note By calling this function with @p sites set to @c NULL, the function can
 * be used to query properties of the device that are independent of the sites.
 * A device will return @ref QDMI_ERROR_NOTSUPPORTED if the queried property is
 * site-dependent and @p sites is @c NULL.
 *
 * @note By calling this function with @p params set to @c NULL, the function
 * can be used to query properties of the device that are independent of the
 * values of the parameters. A device will return @ref QDMI_ERROR_NOTSUPPORTED
 * if the queried property is parameter-dependent and @p params is @c NULL.
 *
 * @note By calling this function with @p value set to @c NULL, the function can
 * be used to check if the device supports the specified property without
 * retrieving the property and without the need to provide a buffer for it.
 * Additionally, the size of the buffer needed to retrieve the property is
 * returned in @p size_ret if @p size_ret is not @c NULL.
 *
 * @note For example, to query the site-independent fidelity of an operation
 * without parameters, the following code snippet can be used:
 * ```
 * // Check if the device supports the property.
 * auto ret = QDMI_device_query_operation_property(
 *   device, operation, 0, nullptr, 0, nullptr,
 *   QDMI_OPERATION_PROPERTY_FIDELITY, 0, nullptr, nullptr);
 * if (ret == QDMI_ERROR_NOTSUPPORTED) {
 *   // The device does not support the site-independent property.
 *   // Check if the device supports the site-dependent property.
 *   ...
 * }
 *
 * // Query the property.
 * double fidelity;
 * QDMI_device_query_operation_property(
 *   device, operation, 0, nullptr, 0, nullptr,
 *   QDMI_OPERATION_PROPERTY_FIDELITY, sizeof(double), &fidelity, nullptr);
 * ```
 *
 * @remark @ref QDMI_Operation and @ref QDMI_Site handles may be queried via
 * @ref QDMI_device_query_device_property with @ref
 * QDMI_DEVICE_PROPERTY_OPERATIONS and @ref QDMI_DEVICE_PROPERTY_SITES,
 * respectively.
 *
 * @remark The number of operands and parameters of an operation can be queried
 * via @ref QDMI_device_query_operation_property with @ref
 * QDMI_OPERATION_PROPERTY_QUBITSNUM and @ref
 * QDMI_OPERATION_PROPERTY_PARAMETERSNUM, respectively.
 */
int QDMI_device_query_operation_property(
    QDMI_Device device, QDMI_Operation operation, size_t num_sites,
    const QDMI_Site *sites, size_t num_params, const double *params,
    QDMI_Operation_Property prop, size_t size, void *value, size_t *size_ret);

/**
 * @brief Query an environment property.
 * @param[in] device The device to query. Must not be @c NULL.
 * @param[in] environment The environment to query. Must not be @c NULL.
 * @param[in] prop The property to query. Must be one of the values specified
 * for @ref QDMI_Environment_Property.
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
 *  - @p device or @p environment is @c NULL,
 *  - @p prop is invalid, or
 *  - @p value is not @c NULL and @p size is less than the size of the data
 *    being queried.
 * @return @ref QDMI_ERROR_FATAL if an unexpected error occurred.
 *
 * @note By calling this function with @p value set to @c NULL, the function can
 * be used to check if the environment supports the specified property without
 * retrieving the property and without the need to provide a buffer for it.
 * Additionally, the size of the buffer needed to retrieve the property is
 * returned in @p size_ret if @p size_ret is not @c NULL.
 *
 * @note For example, to query the unit of an environment, the following code
 pattern
 * can be used:
 * ```
 * // Check if the device supports the property.
 * auto ret = QDMI_device_query_environment_property(
 *   device, environment, QDMI_ENVIRONMENT_PROPERTY_UNIT, 0, nullptr, nullptr);
 * if (ret == QDMI_ERROR_NOTSUPPORTED) {
 *   // The device does not support the property.
 *   ...
 * }
 *
 * // Query the size of the property and the property.
 * size_t environment_unit_size = 0;
 * auto ret = QDMI_device_query_environment_property(
 *     device, environment, QDMI_ENVIRONMENT_PROPERTY_UNIT, 0, nullptr,
 *     &environment_unit_size);
 * if (ret != QDMI_SUCCESS) {
 *   // An error occured.
 *   ...
 * }
 * std::string environment_unit(environment_unit_size - 1, '\0');
 * ret = QDMI_device_query_environment_property(
 *      device, environment, QDMI_ENVIRONMENT_PROPERTY_UNIT,
 *      environment_unit.size() + 1, environment_unit.data(), nullptr);
 * ```
 *
 * @remark @ref QDMI_Environment handles may be queried via @ref
 * QDMI_device_query_device_property with @ref
 QDMI_DEVICE_PROPERTY_ENVIRONMENTVARIABLES.
 */
int QDMI_device_query_environment_property(QDMI_Device device,
                                           QDMI_Environment environment,
                                           QDMI_Environment_Property prop,
                                           size_t size, void *value,
                                           size_t *size_ret);

/** @} */ // end of client_query_interface

/** @defgroup client_job_interface QDMI Client Job Interface
 *  @brief Provides functions to manage client-side jobs.
 *  @details A job is a task submitted by a client to a device for execution.
 *  Most jobs are quantum circuits to be executed on a quantum device.
 *  However, jobs can also be a different type of task, such as calibration.
 *
 *  The typical workflow for a client job is as follows:
 *  - Create a job with @ref QDMI_device_create_job.
 *  - Set parameters for the job with @ref QDMI_job_set_parameter.
 *  - Submit the job to the device with @ref QDMI_job_submit.
 *  - Check the status of the job with @ref QDMI_job_check.
 *  - Wait for the job to finish with @ref QDMI_job_wait.
 *  - Retrieve the results of the job with @ref QDMI_job_get_results.
 *  - Free the job with @ref QDMI_job_free when it is no longer used.
 *
 *  @{
 */

/**
 * @brief A handle for a client-side job.
 * @details An opaque pointer to a type defined by the driver that encapsulates
 * all information about a job submitted to a device by a client.
 * @remark Implementations of the underlying type will want to store the device
 * handle used to create the job in the job handle to be able to access the
 * device when needed.
 * @see QDMI_Device_Job for the device-side job handle.
 */
typedef struct QDMI_Job_impl_d *QDMI_Job;

/**
 * @brief Create a job.
 * @details This is the main entry point for a client to submit a job to a
 * device. The returned handle can be used throughout the @ref
 * client_job_interface "client job interface" to refer to the job.
 * @param[in] device The device to create the job on. Must not be @c NULL.
 * @param[out] job A pointer to a handle that will store the created job.
 * Must not be @c NULL. The job must be freed by calling @ref QDMI_job_free
 * when it is no longer used.
 * @return @ref QDMI_SUCCESS if the job was successfully created.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p device or @p job are @c NULL.
 * @return @ref QDMI_ERROR_PERMISSIONDENIED if the driver does not allow using
 * the @ref client_job_interface "client job interface" for the device in the
 * current session.
 * @return @ref QDMI_ERROR_FATAL if job creation failed due to a fatal error.
 */
int QDMI_device_create_job(QDMI_Device device, QDMI_Job *job);

/**
 * @brief Enum of the job parameters that can be set.
 * @details If not noted otherwise, parameters are optional and drivers must not
 * require them to be set.
 */
enum QDMI_JOB_PARAMETER_T {
  /**
   * @brief @ref QDMI_Program_Format The format of the program to be executed.
   * @details This parameter is required. If the device does not support the
   * specified program format, it is up to the driver to decide whether to
   * return @ref QDMI_ERROR_NOTSUPPORTED from @ref QDMI_job_set_parameter or to
   * convert the program to a supported format.
   */
  QDMI_JOB_PARAMETER_PROGRAMFORMAT = 0,
  /**
   * @brief `void*` The program to be executed.
   * @details This parameter is required. The program must be in the format
   * specified by the @ref QDMI_JOB_PARAMETER_PROGRAMFORMAT parameter.
   * If the program is invalid, the @ref QDMI_job_set_parameter function
   * must return @ref QDMI_ERROR_INVALIDARGUMENT. If the program is valid, but
   * the device cannot execute it, the @ref QDMI_job_set_parameter function must
   * return @ref QDMI_ERROR_NOTSUPPORTED.
   */
  QDMI_JOB_PARAMETER_PROGRAM = 1,
  /**
   * @brief `size_t` The number of shots to execute for a quantum circuit job.
   * @details If this parameter is not set, a device-specific default is used.
   */
  QDMI_JOB_PARAMETER_SHOTSNUM = 2,
  /**
   * @brief The maximum value of the enum.
   * @details It can be used by drivers for bounds checking and validation of
   * function parameters.
   * @attention This value must remain the last regular member of the enum
   * besides the custom members and must be updated when new members are added.
   */
  QDMI_JOB_PARAMETER_MAX = 3,
  /**
   * @brief This enum value is reserved for a custom parameter.
   * @details The driver defines the meaning and the type of this parameter.
   * @attention The value of this enum member must not be changed to maintain
   * binary compatibility.
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

/// Job parameter type.
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
 * parameter or the value of the parameter.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if
 *  - @p job is @c NULL,
 *  - @p param is invalid, or
 *  - @p value is not @c NULL and @p size is zero or not the expected size for
 *  the parameter (if specified by the @ref QDMI_Job_Parameter documentation).
 * @return @ref QDMI_ERROR_BADSTATE if the parameter cannot be set in the
 * current state of the job, for example, because the job is already submitted.
 * @return @ref QDMI_ERROR_PERMISSIONDENIED if the driver does not allow using
 * the @ref client_job_interface "client job interface" for the device in the
 * current session.
 * @return @ref QDMI_ERROR_FATAL if setting the parameter failed due to a fatal
 * error.
 *
 * @note By calling this function with @p value set to @c NULL, the function can
 * be used to check if the driver supports the specified parameter without
 * setting the parameter and without the need to provide a value.
 *
 * @note For example, to check whether the device supports setting the number of
 * shots for a quantum circuit job, the following code pattern can be used:
 * ```
 * // Check if the device supports setting the number of shots.
 * auto ret = QDMI_job_set_parameter(
 *   job, QDMI_JOB_PARAMETER_SHOTSNUM, 0, nullptr);
 * if (ret == QDMI_ERROR_NOTSUPPORTED) {
 *   // The device does not support setting the number of shots.
 *   ...
 * }
 *
 * // Set the number of shots.
 * size_t shots = 8192;
 * QDMI_job_set_parameter(
 *   job, QDMI_JOB_PARAMETER_SHOTSNUM, sizeof(size_t), &shots);
 * ```
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
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p job is @c NULL.
 * @return @ref QDMI_ERROR_BADSTATE if the job is in an invalid state.
 * @return @ref QDMI_ERROR_PERMISSIONDENIED if the driver does not allow using
 * the @ref client_job_interface "client job interface" for the device in the
 * current session.
 * @return @ref QDMI_ERROR_FATAL if the job submission failed.
 */
int QDMI_job_submit(QDMI_Job job);

/**
 * @brief Cancel an already submitted job.
 * @details Remove the job from the queue of waiting jobs. This changes the
 * status of the job to @ref QDMI_JOB_STATUS_CANCELED.
 * @param[in] job The job to cancel. Must not be @c NULL.
 * @return @ref QDMI_SUCCESS if the job was successfully canceled.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p job is @c NULL or the job
 * already has the status @ref QDMI_JOB_STATUS_DONE.
 * @return @ref QDMI_ERROR_PERMISSIONDENIED if the driver does not allow using
 * the @ref client_job_interface "client job interface" for the device in the
 * current session.
 * @return @ref QDMI_ERROR_FATAL if the job could not be canceled.
 */
int QDMI_job_cancel(QDMI_Job job);

/**
 * @brief Check the status of a job.
 * @details This function is non-blocking and returns immediately with the job
 * status. It is not required to call this function before calling @ref
 * QDMI_job_get_results.
 * @param[in] job The job to check the status of. Must not be @c NULL.
 * @param[out] status The status of the job. Must not be @c NULL.
 * @return @ref QDMI_SUCCESS if the job status was successfully checked.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p job or @p status is @c NULL.
 * @return @ref QDMI_ERROR_PERMISSIONDENIED if the driver does not allow using
 * the @ref client_job_interface "client job interface" for the device in the
 * current session.
 * @return @ref QDMI_ERROR_FATAL if the job status could not be checked.
 */
int QDMI_job_check(QDMI_Job job, QDMI_Job_Status *status);

/**
 * @brief Wait for a job to finish.
 * @details This function blocks until the job has either finished or has been
 * canceled.
 * @param[in] job The job to wait for. Must not be @c NULL.
 * @return @ref QDMI_SUCCESS if the job is finished or canceled.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p job is @c NULL.
 * @return @ref QDMI_ERROR_PERMISSIONDENIED if the driver does not allow using
 * the @ref client_job_interface "client job interface" for the device in the
 * current session.
 * @return @ref QDMI_ERROR_FATAL if the job could not be waited for and this
 * function returns before the job has finished or has been canceled.
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
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if
 *  - @p job is @c NULL,
 *  - @p job has not finished,
 *  - @p job was canceled,
 *  - @p result is invalid, or
 *  - @p data is not @c NULL and @p size is smaller than the size of the data
 *    being queried.
 * @return @ref QDMI_ERROR_PERMISSIONDENIED if the driver does not allow using
 * the @ref client_job_interface "client job interface" for the device in the
 * current session.
 * @return @ref QDMI_ERROR_FATAL if an error occurred during the retrieval.
 *
 * @note By calling this function with @p data set to @c NULL, the function can
 * be used to check if the device supports the specified result without
 * retrieving the result and without the need to provide a buffer for the
 * result.
 * Additionally, the size of the buffer needed to retrieve the result is
 * returned in @p size_ret if @p size_ret is not @c NULL.
 *
 * @note For example, to query the measurement results of a quantum circuit job,
 * the following code pattern can be used:
 * ```
 * // Query the size of the result.
 * size_t size;
 * auto ret = QDMI_job_get_results(
 *   job, QDMI_JOB_RESULT_SHOTS, 0, nullptr, &size);
 *
 * // Allocate memory for the result.
 * std::string shots(size-1, '\0');
 *
 * // Query the result.
 * QDMI_job_get_results(
 *   job, QDMI_JOB_RESULT_SHOTS, size, shots.data(), nullptr);
 * ```
 */
int QDMI_job_get_results(QDMI_Job job, QDMI_Job_Result result, size_t size,
                         void *data, size_t *size_ret);

/**
 * @brief Free a job.
 * @details Free the resources associated with a job. Using a job handle after
 * it has been freed is undefined behavior.
 * @param[in] job The job to free.
 */
void QDMI_job_free(QDMI_Job job);

/** @} */ // end of client_job_interface

/** @defgroup client_environment_query_interface QDMI Client Environment Query
 * Interface
 *  @brief Provides functions to query environmental variables.
 *  @details An environment query is a task submitted by a client to a device
 *  for querying environmental variables, i.e. temperature or power.
 *
 *  The typical workflow for a client environment query is as follows:
 *  - Create an environment query with @ref
 * QDMI_device_create_environment_query.
 *  - Set parameters for the environment query with @ref
 * QDMI_environment_query_set_parameter
 *  - Submit the environment query to the device with @ref
 * QDMI_environment_query_submit.
 *  - Check the status of the environment query with @ref
 * QDMI_environment_query_check_status.
 *  - Wait for the environment query to finish with @ref
 * QDMI_environment_query_wait.
 *  - Retrieve the results of the environment query with @ref
 * QDMI_environment_query_get_results.
 *  - Free the environment query with @ref QDMI_environment_query_free when it
 * is no longer used.
 *
 *  @{
 */

/**
 * @brief A handle for a client-side environment query.
 * @details An opaque pointer to a type defined by the driver that encapsulates
 * all information about an environment query submitted to a device by a client.
 * @remark Implementations of the underlying type will want to store the device
 * handle used to create the environment query in the handle to be able to
 * access the device when needed.
 * @see QDMI_Device_Environment_Query for the device-side the environment query
 * handle.
 */
typedef struct QDMI_Environment_Query_impl_d *QDMI_Environment_Query;

/**
 * @brief Create an environment query.
 * @details This is the main entry point for a client to submit an environment
 * query to a device. The returned handle can be used throughout the @ref
 * client_environment_query_interface "client environment query interface" to
 * refer to the environment query.
 * @param[in] device The device to create the environment query on. Must not be
 * @c NULL.
 * @param[out] query A pointer to a handle that will store the created
 * environment query. Must not be @c NULL. The job must be freed by calling @ref
 * QDMI_environment_query_free when it is no longer used.
 * @return @ref QDMI_SUCCESS if the job was successfully created.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p device or @p query are @c NULL.
 * @return @ref QDMI_ERROR_PERMISSIONDENIED if the driver does not allow using
 * the @ref client_environment_query_interface "client environment query
 * interface" for the device in the current session.
 * @return @ref QDMI_ERROR_FATAL if the environment query creation failed due to
 * a fatal error.
 */
int QDMI_device_create_environment_query(QDMI_Device device,
                                         QDMI_Environment_Query *query);

/**
 * @brief Enum of the environment query parameters that can be set.
 * @details If not noted otherwise, parameters are mandatory and drivers must
 * require them to be set.
 */

enum QDMI_ENVIRONMENT_QUERY_PARAMETER_T {

  /// The start time of the environment query interval.
  QDMI_ENVIRONMENT_QUERY_PARAMETER_START_TIME = 0,

  /// The end time of the environment query interval.
  QDMI_ENVIRONMENT_QUERY_PARAMETER_END_TIME = 1,

  /// The environment for the environment query.
  QDMI_ENVIRONMENT_QUERY_PARAMETER_ENVIRONMENT = 2,

  /// The maximum value of the enum.
  QDMI_ENVIRONMENT_QUERY_PARAMETER_MAX = 3,

  /**
   * @brief This enum value is reserved for a custom parameter.
   * @details The driver defines the meaning and the type of this parameter.
   * @attention The value of this enum member must not be changed to maintain
   * binary compatibility.
   */
  QDMI_ENVIRONMENT_QUERY_PARAMETER_CUSTOM1 = 999999995,

  /// @see QDMI_ENVIRONMENT_QUERY_PARAMETER_CUSTOM1
  QDMI_ENVIRONMENT_QUERY_PARAMETER_CUSTOM2 = 999999996,

  /// @see QDMI_ENVIRONMENT_QUERY_PARAMETER_CUSTOM1
  QDMI_ENVIRONMENT_QUERY_PARAMETER_CUSTOM3 = 999999997,

  /// @see QDMI_ENVIRONMENT_QUERY_PARAMETER_CUSTOM1
  QDMI_ENVIRONMENT_QUERY_PARAMETER_CUSTOM4 = 999999998,

  /// @see QDMI_ENVIRONMENT_QUERY_PARAMETER_CUSTOM1
  QDMI_ENVIRONMENT_QUERY_PARAMETER_CUSTOM5 = 999999999
};

/// Environment query parameter type.
typedef enum QDMI_ENVIRONMENT_QUERY_PARAMETER_T
    QDMI_Environment_Query_Parameter;
/**
 * @brief Set a parameter for an environment.
 * @param[in] query A handle to a job for which to set @p param. Must not be @c
 * NULL.
 * @param[in] param The parameter whose value will be set. Must be one of the
 * values specified for @ref QDMI_Environment_Query_Parameter.
 * @param[in] size The size of the data pointed to by @p value in bytes. Must
 * not be zero, except when @p value is @c NULL, in which case it is ignored.
 * @param[in] value A pointer to the memory location that contains the value of
 * the parameter to be set. The data pointed to by @p value is copied and can be
 * safely reused after this function returns. If this is @c NULL, it is ignored.
 * @return @ref QDMI_SUCCESS if the driver supports the specified @ref
 * QDMI_Environment_Query_Parameter @p param and, when @p value is not @c NULL,
 * the parameter was successfully set.
 * @return @ref QDMI_ERROR_NOTSUPPORTED if the driver does not support the
 * parameter or the value of the parameter.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if
 *  - @p query is @c NULL,
 *  - @p param is invalid, or
 *  - @p value is not @c NULL and @p size is zero or not the expected size for
 *  the parameter (if specified by the @ref QDMI_Environment_Query_Parameter
 * documentation).
 * @return @ref QDMI_ERROR_BADSTATE if the parameter cannot be set in the
 * current state of the job, for example, because the query is already
 * submitted.
 * @return @ref QDMI_ERROR_PERMISSIONDENIED if the driver does not allow using
 * the @ref client_environment_query_interface "client environment query
 * interface" for the device in the current session.
 * @return @ref QDMI_ERROR_FATAL if setting the parameter failed due to a fatal
 * error.
 *
 * @note By calling this function with @p value set to @c NULL, the function can
 * be used to check if the driver supports the specified parameter without
 * setting the parameter and without the need to provide a value.
 *
 * @note For example, to check whether the device supports setting the start
 * time for an envronment query, the following code pattern can be
 * used:
 * ```
 * // Check if the device supports setting the start time
 * auto ret = QDMI_environment_query_set_parameter(
 *   query, QDMI_ENVIRONMENT_QUERY_PARAMETER_START_TIME, 0, nullptr);
 * if (ret == QDMI_ERROR_NOTSUPPORTED) {
 *   // The device does not support setting the start time.
 *   ...
 * }
 *
 * // Set the start time.
 * uint64_t start_ts = 1744970400;
 * QDMI_environment_query_set_parameter(
 *   query, QDMI_ENVIRONMENT_QUERY_PARAMETER_START_TIME, sizeof(uint64_t),
 * &start_ts);
 * ```
 */
int QDMI_environment_query_set_parameter(QDMI_Environment_Query query,
                                         QDMI_Environment_Query_Parameter param,
                                         size_t size, const void *value);
/**
 * @brief Submit an environment query to the device.
 * @details This function can either be blocking until the environment query is
 * finished or non-blocking and return while the environment query is running.
 * In the latter case, the functions @ref QDMI_environment_query_check_status
 * and @ref QDMI_environment_query_wait can be used to check the status and wait
 * for the environment query to finish.
 * @param[in] query The environment query to submit. Must not be @c NULL.
 * @return @ref QDMI_SUCCESS if the environment query was successfully
 * submitted.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p environment query is @c NULL.
 * @return @ref QDMI_ERROR_BADSTATE if the environment query is in an invalid
 * state.
 * @return @ref QDMI_ERROR_PERMISSIONDENIED if the driver does not allow using
 * the @ref client_environment_query_interface "client environment query
 * interface" for the device in the current session.
 * @return @ref QDMI_ERROR_FATAL if the environment query submission failed.
 */
int QDMI_environment_query_submit(QDMI_Environment_Query query);

/**
 * @brief Cancel an already submitted environment query.
 * @details Remove the environment query from the queue of waiting environment
 * query. This changes the status of the environment query to @ref
 * QDMI_ENVIRONMENT_QUERY_STATUS_CANCELED.
 * @param[in] query The environment query to cancel. Must not be @c NULL.
 * @return @ref QDMI_SUCCESS if the environment query was successfully canceled.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p query is @c NULL or the
 * environment query already has the status @ref
 * QDMI_ENVIRONMENT_QUERY_STATUS_DONE.
 * @return @ref QDMI_ERROR_PERMISSIONDENIED if the driver does not allow using
 * the @ref client_environment_query_interface "client environment query
 * interface" for the device in the current session.
 * @return @ref QDMI_ERROR_FATAL if the environment query could not be canceled.
 */
int QDMI_environment_query_cancel(QDMI_Environment_Query query);

/**
 * @brief Check the status of an environment query.
 * @details This function is non-blocking and returns immediately with the
 * environment query status. It is not required to call this function before
 * calling @ref QDMI_environment_query_get_results.
 * @param[in] query The environment query to check the status of. Must not be @c
 * NULL.
 * @param[out] status The status of the environment query. Must not be @c NULL.
 * @return @ref QDMI_SUCCESS if the environment query status was successfully
 * checked.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p query or @p status is @c NULL.
 * @return @ref QDMI_ERROR_PERMISSIONDENIED if the driver does not allow using
 * the @ref client_environment_query_interface "client environment query
 * interface" for the device in the current session.
 * @return @ref QDMI_ERROR_FATAL if the environment query status could not be
 * checked.
 */
int QDMI_environment_query_check_status(QDMI_Environment_Query query,
                                        QDMI_Environment_Query_Status *status);

/**
 * @brief Wait for an environment query to finish.
 * @details This function blocks until the environment query has either finished
 * or has been canceled.
 * @param[in] query The environment query to wait for. Must not be @c NULL.
 * @return @ref QDMI_SUCCESS if the environment query is finished or canceled.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p query is @c NULL.
 * @return @ref QDMI_ERROR_PERMISSIONDENIED if the driver does not allow using
 * the @ref client_environment_query_interface "client environment query
 * interface" for the device in the current session.
 * @return @ref QDMI_ERROR_FATAL if the environment query could not be waited
 * for and this function returns before the environment query has finished or
 * has been canceled.
 */
int QDMI_environment_query_wait(QDMI_Environment_Query query);

/**
 * @brief Retrieve the results of an environment query.
 * @param[in] query The environment query to retrieve the results from. Must not
 * be @c NULL.
 * @param[in] result The result to retrieve. Must be one of the values specified
 * for @ref QDMI_Environment_Query_Result.
 * @param[in] size The size of the buffer pointed to by @p data in bytes. Must
 * be greater or equal to the size of the return type specified for the @ref
 * QDMI_Environment_Query_Result @p result, except when @p data is @c NULL, in
 * which case it is ignored.
 * @param[out] data A pointer to the memory location where the results will be
 * stored. If this is @c NULL, it is ignored.
 * @param[out] size_ret The actual size of the data being queried in bytes. If
 * this is @c NULL, it is ignored.
 * @return @ref QDMI_SUCCESS if the device supports the specified result and,
 * when @p data is not @c NULL, the results were successfully retrieved.
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if
 *  - @p query is @c NULL,
 *  - @p query has not finished,
 *  - @p query was canceled,
 *  - @p result is invalid, or
 *  - @p data is not @c NULL and @p size is smaller than the size of the data
 *    being queried.
 * @return @ref QDMI_ERROR_PERMISSIONDENIED if the driver does not allow using
 * the @ref client_environment_query_interface "client environment query
 * interface" for the device in the current session.
 * @return @ref QDMI_ERROR_FATAL if an error occurred during the retrieval.
 *
 * @note By calling this function with @p data set to @c NULL, the function can
 * be used to check if the device supports the specified result without
 * retrieving the result and without the need to provide a buffer for the
 * result.
 * Additionally, the size of the buffer needed to retrieve the result is
 * returned in @p size_ret if @p size_ret is not @c NULL.
 *
 * @note For example, to query the results of a environment query,
 * the following code pattern can be used:
 * ```
 * // Query the size of the result.
 * size_t size;
 * auto ret = QDMI_environment_query_get_results(
 *   query, QDMI_ENVIRONMENT_QUERY_RESULT_VALUES, 0, nullptr, &size);
 *
 * // Allocate memory for the result.
 * std::vector<float> values;
 * values.reserve(size);
 *
 * // Query the result.
 * QDMI_environment_query_get_results(
 *   query, QDMI_ENVIRONMENT_QUERY_RESULT_VALUES, size, values.data(), nullptr);
 * ```
 */
int QDMI_environment_query_get_results(QDMI_Environment_Query query,
                                       QDMI_Environment_Query_Result result,
                                       size_t size, void *data,
                                       size_t *size_ret);
/**
 * @brief Free an environment query.
 * @details Free the resources associated with a environment query. Using a
 * environment query handle after it has been freed is undefined behavior.
 * @param[in] query The environment query to free.
 */
void QDMI_environment_query_free(QDMI_Environment_Query query);

/** @} */ // end of client_environment_query_interface

/** @} */ // end of client_interface

// NOLINTEND(performance-enum-size, modernize-use-using)

#ifdef __cplusplus
} // extern "C"
#endif
