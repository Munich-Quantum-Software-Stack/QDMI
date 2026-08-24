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
 * @brief Defines constants and value types used within QDMI across the @ref
 * client_interface and the @ref device_interface.
 */

#ifndef QDMI_CONSTANTS_H
#define QDMI_CONSTANTS_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// The following clang-tidy warnings cannot be addressed because this header is
// used from both C and C++ code.
// NOLINTBEGIN(performance-enum-size, modernize-use-using)

/**
 * @brief Status codes returned by the API.
 */
enum QDMI_STATUS {
  QDMI_WARN_GENERAL = 1,            ///< A general warning.
  QDMI_SUCCESS = 0,                 ///< The operation was successful.
  QDMI_ERROR_FATAL = -1,            ///< A fatal error.
  QDMI_ERROR_OUTOFMEM = -2,         ///< Out of memory.
  QDMI_ERROR_NOTIMPLEMENTED = -3,   ///< Not implemented.
  QDMI_ERROR_LIBNOTFOUND = -4,      ///< Library not found.
  QDMI_ERROR_NOTFOUND = -5,         ///< Element not found.
  QDMI_ERROR_OUTOFRANGE = -6,       ///< Out of range.
  QDMI_ERROR_INVALIDARGUMENT = -7,  ///< Invalid argument.
  QDMI_ERROR_PERMISSIONDENIED = -8, ///< Permission denied.
  QDMI_ERROR_NOTSUPPORTED = -9,     ///< Operation is not supported.
  /// Resource is in the wrong state for the operation.
  QDMI_ERROR_BADSTATE = -10,
  QDMI_ERROR_TIMEOUT = -11, ///< Operation timed out.
};

/**
 * @brief First value in every provider-defined enum range.
 * @details Every value from @ref QDMI_CUSTOM_ENUM_VALUE_MIN through @ref
 * QDMI_CUSTOM_ENUM_VALUE_MAX is syntactically valid. The `CUSTOM1` through
 * `CUSTOM5` members of each extensible enum preserve names for the first five
 * values. Values from such an enum's regular `MAX` member up to, but excluding,
 * @ref QDMI_CUSTOM_ENUM_VALUE_MIN are invalid. An implementation returns @ref
 * QDMI_ERROR_NOTSUPPORTED for a valid custom value that it does not support.
 */
#define QDMI_CUSTOM_ENUM_VALUE_MIN 999999995
/** @brief Last value in every provider-defined enum range. */
#define QDMI_CUSTOM_ENUM_VALUE_MAX INT32_MAX

/**
 * @brief Enum of the device session parameters that can be set via @ref
 * QDMI_device_session_set_parameter.
 * @details If not noted otherwise, parameters are optional and devices must not
 * require them to be set.
 */
enum QDMI_DEVICE_SESSION_PARAMETER_T {
  /**
   * @brief `char*` (string) The baseURL or API endpoint to be used for
   * accessing the device within the session.
   * @details If this parameter is set and the device supports it, the device
   * must use the specified baseURL or API endpoint for the session. Devices may
   * use this parameter to switch between different versions of the API or
   * different endpoints for testing or production environments.
   */
  QDMI_DEVICE_SESSION_PARAMETER_BASEURL = 0,
  /**
   * @brief `char*` (string) A token to be used in the session initialization
   * for authenticating with the device.
   * @details A token could be an API key. The device documentation *must*
   * document what kind of token is required and how it is used. If the device
   * requires authentication via a token, this parameter must be set before
   * calling @ref QDMI_device_session_init.
   */
  QDMI_DEVICE_SESSION_PARAMETER_TOKEN = 1,
  /**
   * @brief `char*` (string) A file path to a file containing authentication
   * information.
   * @details The file may contain a token or other authentication information
   * required for the session. The device documentation *must* document
   * whether the implementation requires this parameter to be set and what
   * kind of authentication information is expected in the file.
   */
  QDMI_DEVICE_SESSION_PARAMETER_AUTHFILE = 2,
  /**
   * @brief `char*` (string) The URL to an authentication server used as part of
   * the authentication procedure.
   * @details This parameter might be used as part of an authentication scheme
   * where an API token is received from an authentication server. This may,
   * additionally, require a username and a password, which can be set via the
   * @ref QDMI_DEVICE_SESSION_PARAMETER_USERNAME and @ref
   * QDMI_DEVICE_SESSION_PARAMETER_PASSWORD parameters.
   *
   * @par The device documentation *must* document if the implementation
   * requires this parameter to be set and which additional parameters need to
   * be set in case this authentication method is used.
   */
  QDMI_DEVICE_SESSION_PARAMETER_AUTHURL = 3,
  /**
   * @brief `char*` (string) The username to use for the device session.
   * @details The username is used for authentication within the session. The
   * device documentation *must* document when the implementation requires this
   * parameter to be set.
   */
  QDMI_DEVICE_SESSION_PARAMETER_USERNAME = 4,
  /**
   * @brief `char*` (string) The password to use for the session.
   * @details The password is used for authentication within the session. The
   * device documentation *must* document if the implementation requires this
   * parameter to be set.
   */
  QDMI_DEVICE_SESSION_PARAMETER_PASSWORD = 5,
  /**
   * @brief `QDMI_Child_Device` The child device to establish the session with.
   * @details If the device manages child devices, a QDMI driver can establish
   * a session with those child devices by setting this session parameter to the
   * respective @ref QDMI_Child_Device handle.
   * @par
   * After initialization of this session, the device will forward any function
   * call on this session to the job or query interface of the child device.
   * @note This parameter can be unset by setting this parameter to `NULL`.
   * @see QDMI_DEVICE_PROPERTY_CHILDDEVICES
   */
  QDMI_DEVICE_SESSION_PARAMETER_CHILDDEVICE = 6,
  /**
   * @brief The maximum value of the enum.
   * @details It can be used by devices for bounds checking and validation of
   * function parameters.
   *
   * @attention This value must remain the last regular member of the enum
   * besides the custom members and must be updated when new members are added.
   */
  QDMI_DEVICE_SESSION_PARAMETER_MAX = 7,
  /**
   * @brief This enum value is reserved for a custom parameter.
   * @details The device defines the meaning and the type of this parameter.
   * @attention The value of this enum member must not be changed to maintain
   * binary compatibility.
   */
  QDMI_DEVICE_SESSION_PARAMETER_CUSTOM1 = QDMI_CUSTOM_ENUM_VALUE_MIN,
  /// @see QDMI_DEVICE_SESSION_PARAMETER_CUSTOM1
  QDMI_DEVICE_SESSION_PARAMETER_CUSTOM2 = 999999996,
  /// @see QDMI_DEVICE_SESSION_PARAMETER_CUSTOM1
  QDMI_DEVICE_SESSION_PARAMETER_CUSTOM3 = 999999997,
  /// @see QDMI_DEVICE_SESSION_PARAMETER_CUSTOM1
  QDMI_DEVICE_SESSION_PARAMETER_CUSTOM4 = 999999998,
  /// @see QDMI_DEVICE_SESSION_PARAMETER_CUSTOM1
  QDMI_DEVICE_SESSION_PARAMETER_CUSTOM5 = 999999999
};

/// Device session parameter type.
typedef enum QDMI_DEVICE_SESSION_PARAMETER_T QDMI_Device_Session_Parameter;

/**
 * @brief Enum of the device job parameters that can be set via @ref
 * QDMI_device_job_set_parameter.
 * @details If not noted otherwise, parameters are optional and devices must not
 * require them to be set.
 */
enum QDMI_DEVICE_JOB_PARAMETER_T {
  /**
   * @brief @ref QDMI_Program_Format The format of the program to be executed.
   * @details This parameter is required. The device must support the specified
   * program format. If the device does not support the specified program
   * format, the @ref QDMI_device_job_set_parameter function must return @ref
   * QDMI_ERROR_NOTSUPPORTED.
   */
  QDMI_DEVICE_JOB_PARAMETER_PROGRAMFORMAT = 0,
  /**
   * @brief `void*` The program to be executed.
   * @details This parameter is required. The program must be in the format
   * specified by the @ref QDMI_DEVICE_JOB_PARAMETER_PROGRAMFORMAT parameter.
   * A text program contains exactly one trailing NUL and no earlier NUL; @c
   * size includes that NUL. A binary program is a nonempty arbitrary byte
   * sequence.
   * If the program is invalid, the @ref QDMI_device_job_set_parameter function
   * must return @ref QDMI_ERROR_INVALIDARGUMENT. If the program is valid, but
   * the device cannot execute it, the @ref QDMI_device_job_set_parameter
   * function must return @ref QDMI_ERROR_NOTSUPPORTED.
   */
  QDMI_DEVICE_JOB_PARAMETER_PROGRAM = 1,
  /**
   * @brief `size_t` The number of shots to execute for a quantum circuit job.
   * @details If this parameter is not set, a device-specific default is used.
   */
  QDMI_DEVICE_JOB_PARAMETER_SHOTSNUM = 2,
  /**
   * @brief The maximum value of the enum.
   * @details It can be used by devices for bounds checking and validation of
   * function parameters.
   *
   * @attention This value must remain the last regular member of the enum
   * besides the custom members and must be updated when new members are added.
   */
  QDMI_DEVICE_JOB_PARAMETER_MAX = 3,
  /**
   * @brief This enum value is reserved for a custom parameter.
   * @details The device defines the meaning and the type of this parameter.
   * @attention The value of this enum member must not be changed to maintain
   * binary compatibility.
   */
  QDMI_DEVICE_JOB_PARAMETER_CUSTOM1 = QDMI_CUSTOM_ENUM_VALUE_MIN,
  /// @see QDMI_DEVICE_JOB_PARAMETER_CUSTOM1
  QDMI_DEVICE_JOB_PARAMETER_CUSTOM2 = 999999996,
  /// @see QDMI_DEVICE_JOB_PARAMETER_CUSTOM1
  QDMI_DEVICE_JOB_PARAMETER_CUSTOM3 = 999999997,
  /// @see QDMI_DEVICE_JOB_PARAMETER_CUSTOM1
  QDMI_DEVICE_JOB_PARAMETER_CUSTOM4 = 999999998,
  /// @see QDMI_DEVICE_JOB_PARAMETER_CUSTOM1
  QDMI_DEVICE_JOB_PARAMETER_CUSTOM5 = 999999999
};

/// Device job parameter type.
typedef enum QDMI_DEVICE_JOB_PARAMETER_T QDMI_Device_Job_Parameter;

/**
 * @brief Enum of the device job properties that can be queried via @ref
 * QDMI_device_job_query_property as part of the @ref
 * device_interface "device interface".
 * @details In particular, every parameter's value that can be set via @ref
 * QDMI_device_job_set_parameter can be queried.
 */
enum QDMI_DEVICE_JOB_PROPERTY_T {
  /**
   * @brief `char*` (string) The job's ID.
   * @details The ID must uniquely identify a job for the specific device.
   * It should generally be universally unique (such as a UUID), to avoid
   * conflicts with other devices' job IDs.
   * It may be used with @ref QDMI_device_session_retrieve_device_job_by_id to
   * obtain a new @ref QDMI_Device_Job handle for an existing remote job.
   * It may, for example, correspond to the job ID provided by the
   * device's API or may be generated by the QDMI Device implementation.
   */
  QDMI_DEVICE_JOB_PROPERTY_ID = 0,
  /**
   * @brief @ref QDMI_Program_Format The format of the program to be executed.
   * @note This property returns the descriptor executed by the device. A driver
   * can expose a different client-submitted descriptor when it converts the
   * payload before setting @ref QDMI_DEVICE_JOB_PARAMETER_PROGRAMFORMAT.
   */
  QDMI_DEVICE_JOB_PROPERTY_PROGRAMFORMAT = 1,
  /**
   * @brief `void*` The program to be executed.
   * @note This property returns the value of the @ref
   * QDMI_DEVICE_JOB_PARAMETER_PROGRAM parameter.
   */
  QDMI_DEVICE_JOB_PROPERTY_PROGRAM = 2,
  /**
   * @brief `size_t` The number of shots to execute for a quantum circuit job.
   * @note This property returns the value of the @ref
   * QDMI_DEVICE_JOB_PARAMETER_SHOTSNUM parameter.
   */
  QDMI_DEVICE_JOB_PROPERTY_SHOTSNUM = 3,
  /**
   * @brief `size_t` The current number of jobs ahead of this job in its queue.
   * @details Querying this property must refresh the job's status and queue
   * position. The property can only be queried while the refreshed status is
   * @ref QDMI_JOB_STATUS_QUEUED; otherwise, the query must return @ref
   * QDMI_ERROR_BADSTATE.
   * @par
   * If the provider only exposes a lower bound, the implementation reports
   * that lower bound. For example, a provider value of `>50` is reported as
   * `50`.
   * @par
   * The property may yield @ref QDMI_ERROR_NOTSUPPORTED if the implementation
   * cannot obtain a trustworthy queue position.
   */
  QDMI_DEVICE_JOB_PROPERTY_QUEUEPOSITION = 4,
  /**
   * @brief The maximum value of the enum.
   * @details It can be used by devices for bounds checking and validation of
   * function parameters.
   *
   * @attention This value must remain the last regular member of the enum
   * besides the custom members and must be updated when new members are added.
   */
  QDMI_DEVICE_JOB_PROPERTY_MAX = 5,
  /**
   * @brief This enum value is reserved for a custom parameter.
   * @details The device defines the meaning and the type of this parameter.
   * @attention The value of this enum member must not be changed to maintain
   * binary compatibility.
   */
  QDMI_DEVICE_JOB_PROPERTY_CUSTOM1 = QDMI_CUSTOM_ENUM_VALUE_MIN,
  /// @see QDMI_DEVICE_JOB_PROPERTY_CUSTOM1
  QDMI_DEVICE_JOB_PROPERTY_CUSTOM2 = 999999996,
  /// @see QDMI_DEVICE_JOB_PROPERTY_CUSTOM1
  QDMI_DEVICE_JOB_PROPERTY_CUSTOM3 = 999999997,
  /// @see QDMI_DEVICE_JOB_PROPERTY_CUSTOM1
  QDMI_DEVICE_JOB_PROPERTY_CUSTOM4 = 999999998,
  /// @see QDMI_DEVICE_JOB_PROPERTY_CUSTOM1
  QDMI_DEVICE_JOB_PROPERTY_CUSTOM5 = 999999999
};

/// Device job property type.
typedef enum QDMI_DEVICE_JOB_PROPERTY_T QDMI_Device_Job_Property;

/**
 * Enum of the device properties that can be queried via @ref
 * QDMI_device_session_query_device_property as part of the @ref
 * device_interface "device interface" and via @ref
 * QDMI_device_query_device_property as part of the @ref client_interface
 * "client interface".
 */
enum QDMI_DEVICE_PROPERTY_T {
  /// `char*` (string) The name of the device.
  QDMI_DEVICE_PROPERTY_NAME = 0,
  /// `char*` (string) The version of the device.
  QDMI_DEVICE_PROPERTY_VERSION = 1,
  /// @ref QDMI_Device_Status The status of the device.
  QDMI_DEVICE_PROPERTY_STATUS = 2,
  /// `char*` (string) The implemented version of QDMI.
  QDMI_DEVICE_PROPERTY_LIBRARYVERSION = 3,
  /// `size_t` The number of qubits in the device.
  QDMI_DEVICE_PROPERTY_QUBITSNUM = 4,
  /**
   * @brief `QDMI_Site*` (@ref QDMI_Site list) The sites of the device.
   * @details The returned @ref QDMI_Site handles may be used to query site
   * and operation properties. The list need not be sorted based on the @ref
   * QDMI_SITE_PROPERTY_INDEX.
   * @par
   * The list returned by this property contains all sites of the device, i.e.,
   * regular and zone sites (see @ref QDMI_SITE_PROPERTY_ISZONE). To filter out
   * regular or zone sites, use the function @ref
   * QDMI_device_query_site_property.
   */
  QDMI_DEVICE_PROPERTY_SITES = 5,
  /**
   * @brief `QDMI_Operation*` (@ref QDMI_Operation list) The operations
   * supported by the device.
   * @details The returned @ref QDMI_Operation handles may be used to query
   * operation properties.
   */
  QDMI_DEVICE_PROPERTY_OPERATIONS = 6,
  /**
   * @brief `QDMI_Site*` (@ref QDMI_Site list) The coupling map of the device.
   * @details The returned list contains pairs of sites that are coupled. The
   * pairs in the list are flattened such that the first site of the pair is at
   * index `2n` and the second site is at index `2n+1`.
   *
   * The sites returned in that list are represented as @ref QDMI_Site handles.
   * For example, consider a 3-site device with a coupling map `(0, 1), (1, 2)`.
   * Additionally, assume `site_i` is the handle for the i-th site. Then,
   * `{site_0, site_1, site_1, site_2}` would be returned.
   */
  QDMI_DEVICE_PROPERTY_COUPLINGMAP = 7,
  /**
   * @brief `size_t` Whether the device needs calibration.
   * @details Zero means that the device does not need calibration. A nonzero
   * value means that the device needs calibration. The device defines the
   * meaning of each nonzero value. QDMI does not define a portable way to
   * trigger calibration.
   */
  QDMI_DEVICE_PROPERTY_NEEDSCALIBRATION = 8,
  /**
   * @brief @ref QDMI_Device_Pulse_Support_Level Whether the device supports
   * pulse-level control.
   * @details This property indicates the level of pulse-level control.
   * If a device supports pulse-level control, it may provide additional
   * functionality for pulse-level programming and execution.
   */
  QDMI_DEVICE_PROPERTY_PULSESUPPORT = 9,
  /**
   * @brief `char*` (string) The length unit reported by the device.
   * @details The device implementation must report a known SI unit (e.g., "mm",
   * "um", or "nm") for this property. A client querying a length value must
   * first scale it using @ref QDMI_DEVICE_PROPERTY_LENGTHSCALEFACTOR. The
   * resulting value is then interpreted in the unit specified by this property.
   * @note If the device reports any length values, this property must be set.
   */
  QDMI_DEVICE_PROPERTY_LENGTHUNIT = 10,
  /**
   * @brief `double` A scale factor for all length values.
   * @details The device implementation reports this scale factor. A client must
   * multiply any raw length value received from the device by this factor to
   * obtain the physical length. The unit of the physical length is given by
   * @ref QDMI_DEVICE_PROPERTY_LENGTHUNIT.
   * @note If querying this property returns @ref QDMI_ERROR_NOTSUPPORTED, a
   * client should assume a default value of `1.0`.
   */
  QDMI_DEVICE_PROPERTY_LENGTHSCALEFACTOR = 11,
  /**
   * @brief `char*` (string) The duration unit reported by the device.
   * @details The device implementation must report a known SI unit (e.g., "ms",
   * "us", or "ns") for this property. A client querying a duration value must
   * first scale it using @ref QDMI_DEVICE_PROPERTY_DURATIONSCALEFACTOR. The
   * resulting value is then interpreted in the unit specified by this property.
   * @note If the device reports any duration values, this property must be set.
   */
  QDMI_DEVICE_PROPERTY_DURATIONUNIT = 12,
  /**
   * @brief `double` A scale factor for all duration values.
   * @details The device implementation reports this scale factor. A client must
   * multiply any raw duration value received from the device by this factor to
   * obtain the physical duration. The unit of the physical duration is given by
   * @ref QDMI_DEVICE_PROPERTY_DURATIONUNIT.
   * @note If querying this property returns @ref QDMI_ERROR_NOTSUPPORTED, a
   * client should assume a default value of `1.0`.
   */
  QDMI_DEVICE_PROPERTY_DURATIONSCALEFACTOR = 13,
  /**
   * @brief `uint64_t` The raw, unscaled minimum required distance between
   * qubits during quantum computation.
   * @details For neutral atom-based devices, qubits (atoms) can be repositioned
   * dynamically. However, a minimum separation must be maintained to prevent
   * collisions and loss of atoms. This property specifies the minimum atom
   * distance.
   * @par
   * To obtain the physical minimum atom distance, a client must scale the raw
   * value of this property. The physical minimum atom distance is calculated
   * as: `raw_value * scale_factor`, where `scale_factor` is the value of the
   * @ref QDMI_DEVICE_PROPERTY_LENGTHSCALEFACTOR property. The resulting value
   * is in units of @ref QDMI_DEVICE_PROPERTY_LENGTHUNIT.
   * @note Primarily relevant for neutral atom devices supporting dynamic atom
   * arrangement.
   * @see QDMI_DEVICE_PROPERTY_LENGTHUNIT
   *      QDMI_DEVICE_PROPERTY_LENGTSCALEFACTOR
   */
  QDMI_DEVICE_PROPERTY_MINATOMDISTANCE = 14,
  /**
   * @brief `QDMI_Program_Format*` (@ref QDMI_Program_Format list) The exact
   * program formats supported by the device.
   * @details Every returned descriptor can be passed unchanged as the job's
   * program-format parameter. Descriptors with different versions, profiles,
   * or encodings are independent formats. The list is ordered from most to
   * least preferred by the device provider.
   */
  QDMI_DEVICE_PROPERTY_SUPPORTEDPROGRAMFORMATS = 15,
  /**
   * @brief `QDMI_Child_Device*` (@ref QDMI_Child_Device list) A list of device
   * handles corresponding to the device's child devices managed by this device.
   * @details Some devices may manage multiple child devices, e.g., a
   * multi-device system or a device with multiple processing units. This
   * property provides access to the child devices as separate
   * @ref QDMI_Child_Device handles.
   * @par
   * The property may yield @ref QDMI_ERROR_NOTSUPPORTED if the device does not
   * have any child devices.
   * @note Devices with child devices may have special job submission handling.
   * Check the concrete device's job interface documentation.
   */
  QDMI_DEVICE_PROPERTY_CHILDDEVICES = 16,
  /**
   * @brief `size_t` The current number of jobs waiting to access the device.
   * @details This property is a snapshot of the device's queue length and does
   * not include jobs that are currently executing. If a provider exposes
   * multiple queues for the device, the implementation reports the sum of the
   * waiting jobs across those queues.
   * @par
   * If the provider only exposes a lower bound, the implementation reports
   * that lower bound. For example, a provider value of `>50` is reported as
   * `50`.
   * @par
   * The property may yield @ref QDMI_ERROR_NOTSUPPORTED if the implementation
   * cannot obtain a trustworthy queue length.
   */
  QDMI_DEVICE_PROPERTY_QUEUELENGTH = 17,
  /**
   * @brief The maximum value of the enum.
   * @details It can be used by devices for bounds checking and validation of
   * function parameters.
   *
   * @attention This value must remain the last regular member of the enum
   * besides the custom members and must be updated when new members are added.
   */
  QDMI_DEVICE_PROPERTY_MAX = 18,
  /**
   * @brief This enum value is reserved for a custom property.
   * @details The device defines the meaning and the type of this property.
   * @attention The value of this enum member must not be changed to maintain
   * binary compatibility.
   */
  QDMI_DEVICE_PROPERTY_CUSTOM1 = QDMI_CUSTOM_ENUM_VALUE_MIN,
  /// @see QDMI_DEVICE_PROPERTY_CUSTOM1
  QDMI_DEVICE_PROPERTY_CUSTOM2 = 999999996,
  /// @see QDMI_DEVICE_PROPERTY_CUSTOM1
  QDMI_DEVICE_PROPERTY_CUSTOM3 = 999999997,
  /// @see QDMI_DEVICE_PROPERTY_CUSTOM1
  QDMI_DEVICE_PROPERTY_CUSTOM4 = 999999998,
  /// @see QDMI_DEVICE_PROPERTY_CUSTOM1
  QDMI_DEVICE_PROPERTY_CUSTOM5 = 999999999
};

/// Device property type.
typedef enum QDMI_DEVICE_PROPERTY_T QDMI_Device_Property;

/// Enum of different status the device can be in.
enum QDMI_DEVICE_STATUS_T {
  QDMI_DEVICE_STATUS_OFFLINE = 0,     ///< The device is offline.
  QDMI_DEVICE_STATUS_IDLE = 1,        ///< The device is idle.
  QDMI_DEVICE_STATUS_BUSY = 2,        ///< The device is busy.
  QDMI_DEVICE_STATUS_ERROR = 3,       ///< The device is in an error state.
  QDMI_DEVICE_STATUS_MAINTENANCE = 4, ///< The device is in maintenance.
  QDMI_DEVICE_STATUS_CALIBRATION = 5, ///< The device is in calibration.
  /**
   * @brief The maximum value of the enum.
   * @details It can be used by devices for bounds checking and validation of
   * function parameters.
   *
   * @attention This value must remain the last regular member of the enum
   * besides the custom members and must be updated when new members are added.
   */
  QDMI_DEVICE_STATUS_MAX = 6
};

/// Device status type.
typedef enum QDMI_DEVICE_STATUS_T QDMI_Device_Status;

/// Enum of the site properties that can be queried via @ref
/// QDMI_device_session_query_site_property as part of the @ref device_interface
/// "device interface" and via @ref QDMI_device_query_site_property as part of
/// the @ref client_interface "client interface".
enum QDMI_SITE_PROPERTY_T {
  /**
   * @brief `size_t` The unique index (or ID) to identify the site in a program.
   * @details The index of a site is used to link the qubits used in a quantum
   * program to the physical sites of the device that can be queried via this
   * interface. Indices may be non-consecutive and need not start at 0.
   * See @ref QDMI_Program_Format for more information on how the site indices
   * map to the qubits in a program.
   *
   * @par This property must be available for all sites since it is used to
   * address the sites in a program.
   */
  QDMI_SITE_PROPERTY_INDEX = 0,
  /**
   * @brief `uint64_t` The raw, unscaled T1 time of a site.
   * @details To obtain the physical T1 time, a client must scale the raw value
   * of this property. The physical T1 time is calculated as: `raw_value *
   * scale_factor`, where `scale_factor` is the value of the
   * @ref QDMI_DEVICE_PROPERTY_DURATIONSCALEFACTOR property. The resulting value
   * is in units of @ref QDMI_DEVICE_PROPERTY_DURATIONUNIT.
   * @see QDMI_DEVICE_PROPERTY_DURATIONUNIT
   *      QDMI_DEVICE_PROPERTY_DURATIONSCALEFACTOR
   */
  QDMI_SITE_PROPERTY_T1 = 1,
  /**
   * @brief `uint64_t` The raw, unscaled T2 time of a site.
   * @details To obtain the physical T2 time, a client must scale the raw value
   * of this property. The physical T2 time is calculated as: `raw_value *
   * scale_factor`, where `scale_factor` is the value of the
   * @ref QDMI_DEVICE_PROPERTY_DURATIONSCALEFACTOR property. The resulting value
   * is in units of @ref QDMI_DEVICE_PROPERTY_DURATIONUNIT.
   * @see QDMI_DEVICE_PROPERTY_DURATIONUNIT
   *      QDMI_DEVICE_PROPERTY_DURATIONSCALEFACTOR
   */
  QDMI_SITE_PROPERTY_T2 = 2,
  /**
   * `char*` (string) The name of a site, e.g., another identifier of the site
   * given by the device.
   */
  QDMI_SITE_PROPERTY_NAME = 3,
  /**
   * @brief `int64_t` The raw, unscaled X-coordinate of the site.
   * @details The X-coordinate is measured relative to some unique origin of the
   * device, i.e., the triple of X-, Y-, and Z-coordinate must be unique to the
   * site.
   * @par
   * To obtain the physical X-coordinate of the site, a client must scale the
   * raw value of this property. The physical X-coordinate of the site is
   * calculated as: `raw_value * scale_factor`, where `scale_factor` is the
   * value of the @ref QDMI_DEVICE_PROPERTY_LENGTHSCALEFACTOR property. The
   * resulting value is in units of @ref QDMI_DEVICE_PROPERTY_LENGTHUNIT.
   * @note This property is mainly required for neutral atom devices to report
   * the location of sites.
   * @see QDMI_DEVICE_PROPERTY_LENGTHUNIT
   *      QDMI_DEVICE_PROPERTY_LENGTSCALEFACTOR
   *      QDMI_SITE_PROPERTY_XCOORDINATE
   *      QDMI_SITE_PROPERTY_YCOORDINATE
   *      QDMI_SITE_PROPERTY_ZCOORDINATE
   */
  QDMI_SITE_PROPERTY_XCOORDINATE = 4,
  /**
   * @brief `int64_t` The raw, unscaled Y-coordinate of the site.
   * @details The Y-coordinate is measured relative to some unique origin of the
   * device, i.e., the triple of X-, Y-, and Z-coordinate must be unique to the
   * site.
   * @par
   * To obtain the physical Y-coordinate of the site, a client must scale the
   * raw value of this property. The physical Y-coordinate of the site is
   * calculated as: `raw_value * scale_factor`, where `scale_factor` is the
   * value of the @ref QDMI_DEVICE_PROPERTY_LENGTHSCALEFACTOR property. The
   * resulting value is in units of @ref QDMI_DEVICE_PROPERTY_LENGTHUNIT.
   * @note This property is mainly required for neutral atom devices to report
   * the location of sites.
   * @see QDMI_DEVICE_PROPERTY_LENGTHUNIT
   *      QDMI_DEVICE_PROPERTY_LENGTSCALEFACTOR
   *      QDMI_SITE_PROPERTY_XCOORDINATE
   *      QDMI_SITE_PROPERTY_YCOORDINATE
   *      QDMI_SITE_PROPERTY_ZCOORDINATE
   */
  QDMI_SITE_PROPERTY_YCOORDINATE = 5,
  /**
   * @brief `int64_t` The raw, unscaled Z-coordinate of the site.
   * @details The Z-coordinate is measured relative to some unique origin of the
   * device, i.e., the triple of X-, Y-, and Z-coordinate must be unique to the
   * site.
   * @par
   * To obtain the physical Z-coordinate of the site, a client must scale the
   * raw value of this property. The physical Z-coordinate of the site is
   * calculated as: `raw_value * scale_factor`, where `scale_factor` is the
   * value of the @ref QDMI_DEVICE_PROPERTY_LENGTHSCALEFACTOR property. The
   * resulting value is in units of @ref QDMI_DEVICE_PROPERTY_LENGTHUNIT.
   * @note This property is mainly required for neutral atom devices to report
   * the location of sites.
   * @see QDMI_DEVICE_PROPERTY_LENGTHUNIT
   *      QDMI_DEVICE_PROPERTY_LENGTSCALEFACTOR
   *      QDMI_SITE_PROPERTY_XCOORDINATE
   *      QDMI_SITE_PROPERTY_YCOORDINATE
   *      QDMI_SITE_PROPERTY_ZCOORDINATE
   */
  QDMI_SITE_PROPERTY_ZCOORDINATE = 6,
  /**
   * @brief `bool` Whether the site is a zone.
   * @details A zone is a site that has a spatial extent, i.e., it is not
   * just a point in space as a regular site. These kind of sites, namely zones,
   * are required to adequately represent global operations that act on all
   * qubits within a certain area, i.e., a zone.
   * @note @parblock Zones are typically used in neutral atom devices, where the
   * atoms are arranged in a 2D or 3D lattice, and operations can be applied to
   * all atoms within a certain zone. This property defaults to `false`, i.e.,
   * if a device reports @ref QDMI_ERROR_NOTSUPPORTED for this property, it is
   * assumed that the site is a regular site and not a zone.
   * @endparblock
   * @see QDMI_SITE_PROPERTY_XEXTENT
   *      QDMI_SITE_PROPERTY_YEXTENT
   *      QDMI_SITE_PROPERTY_ZEXTENT
   */
  QDMI_SITE_PROPERTY_ISZONE = 7,
  /**
   * @brief `uint64_t` The raw, unscaled extent of a zone along the X-axis.
   * @details To obtain the physical extent of a zone along the X-axis, a client
   * must scale the raw value of this property. The physical extent of a zone
   * along the X-axis is calculated as: `raw_value * scale_factor`, where
   * `scale_factor` is the value of the @ref
   * QDMI_DEVICE_PROPERTY_LENGTHSCALEFACTOR property. The resulting value is in
   * units of @ref QDMI_DEVICE_PROPERTY_LENGTHUNIT.
   * @note @parblock This property is mainly required for neutral atom devices
   * to report the extent of zones, see @ref QDMI_SITE_PROPERTY_ISZONE. If the
   * site is not a zone, this property must return @ref QDMI_ERROR_NOTSUPPORTED.
   * @endparblock
   * @see QDMI_DEVICE_PROPERTY_LENGTHUNIT
   *      QDMI_DEVICE_PROPERTY_LENGTSCALEFACTOR
   */
  QDMI_SITE_PROPERTY_XEXTENT = 8,
  /**
   * @brief `uint64_t` The raw, unscaled extent of a zone along the Y-axis.
   * @details To obtain the physical extent of a zone along the Y-axis, a client
   * must scale the raw value of this property. The physical extent of a zone
   * along the Y-axis is calculated as: `raw_value * scale_factor`, where
   * `scale_factor` is the value of the @ref
   * QDMI_DEVICE_PROPERTY_LENGTHSCALEFACTOR property. The resulting value is in
   * units of @ref QDMI_DEVICE_PROPERTY_LENGTHUNIT.
   * @note @parblock This property is mainly required for neutral atom devices
   * to report the extent of zones, see @ref QDMI_SITE_PROPERTY_ISZONE. If the
   * site is not a zone, this property must return @ref QDMI_ERROR_NOTSUPPORTED.
   * @endparblock
   * @see QDMI_DEVICE_PROPERTY_LENGTHUNIT
   *      QDMI_DEVICE_PROPERTY_LENGTSCALEFACTOR
   */
  QDMI_SITE_PROPERTY_YEXTENT = 9,
  /**
   * @brief `uint64_t` The raw, unscaled extent of a zone along the Z-axis.
   * @details To obtain the physical extent of a zone along the Z-axis, a client
   * must scale the raw value of this property. The physical extent of a zone
   * along the Z-axis is calculated as: `raw_value * scale_factor`, where
   * `scale_factor` is the value of the @ref
   * QDMI_DEVICE_PROPERTY_LENGTHSCALEFACTOR property. The resulting value is in
   * units of @ref QDMI_DEVICE_PROPERTY_LENGTHUNIT.
   * @note @parblock This property is mainly required for neutral atom devices
   * to report the extent of zones, see @ref QDMI_SITE_PROPERTY_ISZONE. If the
   * site is not a zone, this property must return @ref QDMI_ERROR_NOTSUPPORTED.
   * @endparblock
   * @see QDMI_DEVICE_PROPERTY_LENGTHUNIT
   *      QDMI_DEVICE_PROPERTY_LENGTSCALEFACTOR
   */
  QDMI_SITE_PROPERTY_ZEXTENT = 10,
  /**
   * @brief `uint64_t` an unsigned integer that uniquely identifies the module.
   * @details A module is a logical grouping of sites, e.g., one part on a
   * superconducting chip or an array of sites in a neutral atom-based device.
   */
  QDMI_SITE_PROPERTY_MODULEINDEX = 11,
  /**
   * @brief `uint64_t` an unsigned integer uniquely identifying the submodule
   * within a module.
   * @details A submodule is a repetitive substructure of sites within a
   * module. E.g., for a module (@ref QDMI_SITE_PROPERTY_MODULEINDEX), where the
   * sites are arranged in pairs and the pairs are arranged in a grid, the
   * submodule index would be the index of the pair within the module.
   */
  QDMI_SITE_PROPERTY_SUBMODULEINDEX = 12,
  /**
   * @brief The maximum value of the enum.
   * @details It can be used by devices for bounds checking and validation of
   * function parameters.
   *
   * @attention This value must remain the last regular member of the enum
   * besides the custom members and must be updated when new members are added.
   */
  QDMI_SITE_PROPERTY_MAX = 13,
  /**
   * @brief This enum value is reserved for a custom property.
   * @details The device defines the meaning and the type of this property.
   * @attention The value of this enum member must not be changed to maintain
   * binary compatibility.
   */
  QDMI_SITE_PROPERTY_CUSTOM1 = QDMI_CUSTOM_ENUM_VALUE_MIN,
  /// @see QDMI_SITE_PROPERTY_CUSTOM1
  QDMI_SITE_PROPERTY_CUSTOM2 = 999999996,
  /// @see QDMI_SITE_PROPERTY_CUSTOM1
  QDMI_SITE_PROPERTY_CUSTOM3 = 999999997,
  /// @see QDMI_SITE_PROPERTY_CUSTOM1
  QDMI_SITE_PROPERTY_CUSTOM4 = 999999998,
  /// @see QDMI_SITE_PROPERTY_CUSTOM1
  QDMI_SITE_PROPERTY_CUSTOM5 = 999999999
};

/// Site property type.
typedef enum QDMI_SITE_PROPERTY_T QDMI_Site_Property;

/// Enum of the operation properties that can be queried via @ref
/// QDMI_device_session_query_operation_property as part of the @ref
/// device_interface "device interface" and via @ref
/// QDMI_device_query_operation_property as part of the @ref client_interface
/// "client interface".
enum QDMI_OPERATION_PROPERTY_T {
  /// `char*` (string) The string identifier of the operation.
  QDMI_OPERATION_PROPERTY_NAME = 0,
  /// `size_t` The number of qubits involved in the operation.
  QDMI_OPERATION_PROPERTY_QUBITSNUM = 1,
  /// `size_t` The number of floating point parameters the operation takes.
  QDMI_OPERATION_PROPERTY_PARAMETERSNUM = 2,
  /**
   * @brief `uint64_t` The raw, unscaled duration of an operation.
   * @details To obtain the physical duration, a client must scale the raw value
   * of this property. The physical duration is calculated as: `raw_value *
   * scale_factor`, where `scale_factor` is the value of the
   * @ref QDMI_DEVICE_PROPERTY_DURATIONSCALEFACTOR property. The resulting value
   * is in units of @ref QDMI_DEVICE_PROPERTY_DURATIONUNIT.
   * @see QDMI_DEVICE_PROPERTY_DURATIONUNIT
   *      QDMI_DEVICE_PROPERTY_DURATIONSCALEFACTOR
   */
  QDMI_OPERATION_PROPERTY_DURATION = 3,
  /// `double` The fidelity of an operation.
  QDMI_OPERATION_PROPERTY_FIDELITY = 4,
  /**
   * @brief `uint64_t` The raw, unscaled interaction radius of the operation.
   * @details The interaction radius is the maximum distance between two
   * qubits that can be involved in the operation. It only applies to
   * multi-qubit gates.
   * @par
   * To obtain the physical interaction radius, a client must scale the raw
   * value of this property. The physical interaction radius is calculated as:
   * `raw_value * scale_factor`, where `scale_factor` is the value of the @ref
   * QDMI_DEVICE_PROPERTY_LENGTHSCALEFACTOR property. The resulting value is in
   * units of @ref QDMI_DEVICE_PROPERTY_LENGTHUNIT.
   * @note This property is mainly required for neutral atom devices where
   * atoms representing qubits can be at arbitrary locations. Hence, it is
   * infeasible to define a coupling map. Instead, the coupling of atoms is
   * defined by the interaction radius of the operation.
   * @see QDMI_DEVICE_PROPERTY_LENGTHUNIT
   *      QDMI_DEVICE_PROPERTY_LENGTSCALEFACTOR
   */
  QDMI_OPERATION_PROPERTY_INTERACTIONRADIUS = 5,
  /**
   * @brief `uint64_t` The raw, unscaled blocking radius of the operation.
   * @details The blocking radius is the minimum distance between two
   * qubits that should not be involved in the operation to avoid crosstalk.
   * It only applies to multi-qubit gates.
   * @par
   * To obtain the physical blocking radius, a client must scale the raw value
   * of this property. The physical blocking radius is calculated as: `raw_value
   * * scale_factor`, where `scale_factor` is the value of the @ref
   * QDMI_DEVICE_PROPERTY_LENGTHSCALEFACTOR property. The resulting value is in
   * units of @ref QDMI_DEVICE_PROPERTY_LENGTHUNIT.
   * @note This property is mainly required for neutral atom devices where
   * atoms representing qubits can be at arbitrary locations. To avoid
   * crosstalk, the blocking radius of the operation must be respected when
   * scheduling operations.
   * @see QDMI_DEVICE_PROPERTY_LENGTHUNIT
   *      QDMI_DEVICE_PROPERTY_LENGTHSCALEFACTOR
   */
  QDMI_OPERATION_PROPERTY_BLOCKINGRADIUS = 6,
  /**
   * @brief `double` Fidelity of qubits idling during a global operation.
   * @details This property measures the fidelity of qubits that are within the
   * affected area of a global multi-qubit operation but do not actively
   * participate (i.e., they lack an interaction partner within their radius).
   * Even though these qubits undergo an identity operation, errors may still
   * occur, resulting in lower fidelity compared to qubits that are simply
   * idling and not exposed to the operation.
   * @note This is especially relevant for neutral atom devices, where global
   * operations (e.g., laser pulses) can impact all atoms in the array,
   * including those not interacting.
   */
  QDMI_OPERATION_PROPERTY_IDLINGFIDELITY = 7,
  /**
   * @brief `bool` Whether the operation is a zoned (global) operation.
   * @details A zoned (or global) operation is an operation that can be applied
   * simultaneously to all qubits within a specific zone. If this property is
   * `true`, the operation is considered zoned. If it is `false` or returns @ref
   * QDMI_ERROR_NOTSUPPORTED, the operation is considered local. The
   * applicability of a zoned operation to specific zones is detailed in @ref
   * QDMI_OPERATION_PROPERTY_SITES.
   * @note This property is primarily relevant for neutral atom devices, where a
   * laser can illuminate an entire array of atoms representing qubits.
   * @see QDMI_SITE_PROPERTY_ISZONE
   *      QDMI_OPERATION_PROPERTY_SITES
   */
  QDMI_OPERATION_PROPERTY_ISZONED = 8,
  /**
   * @brief `QDMI_Site*` (list) The sites to which the operation is applicable.
   * @details
   * - For local operations (see @ref QDMI_OPERATION_PROPERTY_ISZONED), this
   * property returns a list of tuples. Each tuple contains sites from the list
   * provided by @ref QDMI_DEVICE_PROPERTY_SITES and represents a valid
   * combination for the operation. The number of sites in each tuple matches
   * the value of @ref QDMI_OPERATION_PROPERTY_QUBITSNUM.
   * - For global operations (see @ref QDMI_OPERATION_PROPERTY_ISZONED), this
   * property returns a list of zone sites, i.e., zones where the operation can
   * be applied.
   */
  QDMI_OPERATION_PROPERTY_SITES = 9,
  /**
   * @brief `uint64_t` The raw, unscaled mean shuttling speed of an operation.
   * @details To obtain the physical speed, a client must scale the raw value of
   * this property. The physical speed is calculated as: `raw_value *
   * length_scale_factor / duration_scale_factor`. The `length_scale_factor` is
   * the value of @ref QDMI_DEVICE_PROPERTY_LENGTHSCALEFACTOR and the
   * `duration_scale_factor` is the value of @ref
   * QDMI_DEVICE_PROPERTY_DURATIONSCALEFACTOR. The resulting value is in units
   * of @ref QDMI_DEVICE_PROPERTY_LENGTHUNIT per @ref
   * QDMI_DEVICE_PROPERTY_DURATIONUNIT.
   * @note This property is mainly required for neutral atom devices where atoms
   * representing qubits can be moved to different sites.
   * @see QDMI_DEVICE_PROPERTY_LENGTHUNIT
   *      QDMI_DEVICE_PROPERTY_LENGTHSCALEFACTOR
   *      QDMI_DEVICE_PROPERTY_DURATIONUNIT
   *      QDMI_DEVICE_PROPERTY_DURATIONSCALEFACTOR
   */
  QDMI_OPERATION_PROPERTY_MEANSHUTTLINGSPEED = 10,
  /**
   * @brief The maximum value of the enum.
   * @details It can be used by devices for bounds checking and validation of
   * function parameters.
   *
   * @attention This value must remain the last regular member of the enum
   * besides the custom members and must be updated when new members are added.
   */
  QDMI_OPERATION_PROPERTY_MAX = 11,
  /**
   * @brief This enum value is reserved for a custom property.
   * @details The device defines the meaning and the type of this property.
   * @attention The value of this enum member must not be changed to maintain
   * binary compatibility.
   */
  QDMI_OPERATION_PROPERTY_CUSTOM1 = QDMI_CUSTOM_ENUM_VALUE_MIN,
  /// @see QDMI_OPERATION_PROPERTY_CUSTOM1
  QDMI_OPERATION_PROPERTY_CUSTOM2 = 999999996,
  /// @see QDMI_OPERATION_PROPERTY_CUSTOM1
  QDMI_OPERATION_PROPERTY_CUSTOM3 = 999999997,
  /// @see QDMI_OPERATION_PROPERTY_CUSTOM1
  QDMI_OPERATION_PROPERTY_CUSTOM4 = 999999998,
  /// @see QDMI_OPERATION_PROPERTY_CUSTOM1
  QDMI_OPERATION_PROPERTY_CUSTOM5 = 999999999
};

/// Operation property type.
typedef enum QDMI_OPERATION_PROPERTY_T QDMI_Operation_Property;

/**
 * @brief Enum of the status a job can have.
 * @details See also @ref client_job_interface for a description of the job's
 * lifecycle.
 */
enum QDMI_JOB_STATUS_T {
  /**
   * @brief The job was created and can be configured via @ref
   * QDMI_job_set_parameter.
   */
  QDMI_JOB_STATUS_CREATED = 0,
  /// The job was submitted.
  QDMI_JOB_STATUS_SUBMITTED = 1,
  /// The job was received, and is waiting to be executed.
  QDMI_JOB_STATUS_QUEUED = 2,
  /// The job is running, and the result is not yet available.
  QDMI_JOB_STATUS_RUNNING = 3,
  /// The job is done, and the result can be retrieved.
  QDMI_JOB_STATUS_DONE = 4,
  /// The job was canceled, and the result is not available.
  QDMI_JOB_STATUS_CANCELED = 5,
  /// An error occurred in the job's lifecycle.
  QDMI_JOB_STATUS_FAILED = 6
};

/// Job status type.
typedef enum QDMI_JOB_STATUS_T QDMI_Job_Status;

/// Maximum bytes, including the terminating NUL, in a format or profile ID.
#define QDMI_PROGRAM_ID_SIZE 64U

/**
 * @brief Pack a Semantic Versioning major, minor, and patch release into a
 * 32-bit exact version value.
 * @details The major and minor components must each fit in 10 bits. The patch
 * component must fit in 12 bits. Prerelease and build metadata are not part of
 * a program-format descriptor.
 */
#define QDMI_MAKE_VERSION(major, minor, patch)                                 \
  ((((uint32_t)(major) & 0x3FFU) << 22U) |                                     \
   (((uint32_t)(minor) & 0x3FFU) << 12U) | ((uint32_t)(patch) & 0xFFFU))

/// Extract the Semantic Versioning major component of a packed version.
#define QDMI_VERSION_MAJOR(version) (((uint32_t)(version) >> 22U) & 0x3FFU)
/// Extract the Semantic Versioning minor component of a packed version.
#define QDMI_VERSION_MINOR(version) (((uint32_t)(version) >> 12U) & 0x3FFU)
/// Extract the Semantic Versioning patch component of a packed version.
#define QDMI_VERSION_PATCH(version) ((uint32_t)(version) & 0xFFFU)

/** @brief Encoding of a submitted payload. */
enum QDMI_PROGRAM_ENCODING_T {
  /// Text with exactly one trailing NUL and no earlier NUL.
  QDMI_PROGRAM_ENCODING_TEXT = 1,
  /// A nonempty arbitrary byte sequence.
  QDMI_PROGRAM_ENCODING_BINARY = 2
};

/// Program encoding type.
typedef enum QDMI_PROGRAM_ENCODING_T QDMI_Program_Encoding;

/**
 * @brief Exact program format accepted by a device.
 * @details All fields take part in identity. `version` is the nonzero, exact
 * packed major, minor, and patch release of the payload specification. Devices
 * list every accepted descriptor separately. Clients must not infer
 * compatibility between versions, profiles, or encodings.
 *
 * The `id` and `profile` arrays must be NUL-terminated, and every byte after
 * the first NUL must be zero. Every API compares descriptor values, not their
 * addresses. A caller may reconstruct a canonical value from its fields; use
 * @c QDMI_program_format_equal to compare two values. IDs are case-sensitive.
 * QDMI reserves
 * unqualified IDs for standard formats. The standard IDs are `openqasm` and
 * `qir`. Vendor formats use `<vendor>.<custom-format-identifier>` IDs, such as
 * `iqm.circuit`. The vendor component is an identifier, not a reverse domain
 * name.
 * QDMI does not define vendor-format versions, profiles, wire formats, or
 * result semantics. Providers must document each vendor descriptor and its
 * payload and result contract. An empty profile identifies a format without a
 * named profile. QIR uses `base` and `adaptive`; OpenQASM uses an empty
 * profile.
 *
 * In the following mappings, the regular-site list is the subsequence of @ref
 * QDMI_DEVICE_PROPERTY_SITES for which @ref QDMI_SITE_PROPERTY_ISZONE is false,
 * in provider order. Zone sites are not program-addressable qubits. Standard
 * descriptors require the regular-site list size to equal @ref
 * QDMI_DEVICE_PROPERTY_QUBITSNUM. Standard mappings cover local operations
 * only; a vendor extension must define how a payload selects a zoned operation.
 *
 * Standard descriptors have the following portable mappings:
 * - OpenQASM descriptors use text encoding and an empty profile. A program has
 *   exactly one quantum register named `q`, with one qubit for each regular
 *   site. `q[i]` maps to the i-th regular site. OpenQASM 3 physical qubit `$i`
 *   maps to the regular site whose @ref QDMI_SITE_PROPERTY_INDEX is `i`.
 *   Except for format-mandated measurement and output primitives, each quantum
 *   instruction name, number of quantum operands, and number of parameters must
 *   equal @ref QDMI_OPERATION_PROPERTY_NAME,
 *   @ref QDMI_OPERATION_PROPERTY_QUBITSNUM, and
 *   @ref QDMI_OPERATION_PROPERTY_PARAMETERSNUM for one reported local
 *   operation.
 * - QIR descriptors use LLVM assembly for text encoding and LLVM bitcode for
 *   binary encoding. They use the `base` or `adaptive` profile. Descriptor
 *   version `N.M.P` identifies the QIR specification release and is independent
 *   of the output-schema version in @ref QDMI_JOB_RESULT_PROGRAMOUTPUT. Except
 *   for profile-mandated measurement, output, and runtime functions, a QIS
 *   function named `__quantum__qis__NAME__body` maps to the reported local
 *   operation whose name is `NAME`; its qubit and parameter operands must match
 *   that operation's qubit and parameter counts. A statically identified qubit
 *   with integer value `i` maps to the regular site whose @ref
 *   QDMI_SITE_PROPERTY_INDEX is `i`. A device that advertises QIR must assign
 *   the regular-site index set to `[0, N)`, where `N` is @ref
 *   QDMI_DEVICE_PROPERTY_QUBITSNUM. Dynamic qubit allocation has no portable
 *   site mapping.
 * A provider may accept a wider format subset, but clients must not rely on a
 * portable mapping outside these rules.
 * Advertising a standard descriptor guarantees only its normative baseline and
 * its reported optional program features, not every construct expressible in
 * the format.
 *
 * The QIR Adaptive baseline includes mid-circuit measurement, measured-qubit
 * reuse, measurement-result use, Boolean computation, and forward branching.
 * Devices report optional QIR module flags as program features. Other standard
 * descriptors have an empty program-feature baseline.
 */
typedef struct QDMI_PROGRAM_FORMAT_T {
  uint32_t version;  ///< Exact version packed with @ref QDMI_MAKE_VERSION.
  uint32_t encoding; ///< One @ref QDMI_Program_Encoding value.
  char id[QDMI_PROGRAM_ID_SIZE];      ///< NUL-terminated format ID.
  char profile[QDMI_PROGRAM_ID_SIZE]; ///< NUL-terminated profile ID.
} QDMI_Program_Format;

#ifdef __cplusplus
static_assert(sizeof(QDMI_Program_Format) == 136U);
static_assert(alignof(QDMI_Program_Format) == 4U);
static_assert(offsetof(QDMI_Program_Format, version) == 0U);
static_assert(offsetof(QDMI_Program_Format, encoding) == 4U);
static_assert(offsetof(QDMI_Program_Format, id) == 8U);
static_assert(offsetof(QDMI_Program_Format, profile) == 72U);
#else
_Static_assert(sizeof(QDMI_Program_Format) == 136U,
               "QDMI_Program_Format must be 136 bytes");
_Static_assert(_Alignof(QDMI_Program_Format) == 4U,
               "QDMI_Program_Format must have four-byte alignment");
_Static_assert(offsetof(QDMI_Program_Format, version) == 0U,
               "QDMI_Program_Format.version must start at byte 0");
_Static_assert(offsetof(QDMI_Program_Format, encoding) == 4U,
               "QDMI_Program_Format.encoding must start at byte 4");
_Static_assert(offsetof(QDMI_Program_Format, id) == 8U,
               "QDMI_Program_Format.id must start at byte 8");
_Static_assert(offsetof(QDMI_Program_Format, profile) == 72U,
               "QDMI_Program_Format.profile must start at byte 72");
#endif

/**
 * @brief Compare two exact program-format values.
 * @param[in] lhs The first descriptor, or @c NULL.
 * @param[in] rhs The second descriptor, or @c NULL.
 * @return Nonzero if every field and array byte is equal; otherwise, zero.
 */
static inline int
QDMI_program_format_equal(const QDMI_Program_Format *const lhs,
                          const QDMI_Program_Format *const rhs) {
  if (lhs == NULL || rhs == NULL || lhs->version != rhs->version ||
      lhs->encoding != rhs->encoding) {
    return 0;
  }
  for (size_t index = 0U; index < QDMI_PROGRAM_ID_SIZE; ++index) {
    if (lhs->id[index] != rhs->id[index] ||
        lhs->profile[index] != rhs->profile[index]) {
      return 0;
    }
  }
  return 1;
}

/// Maximum bytes, including the terminating NUL, in a feature ID.
#define QDMI_PROGRAM_FEATURE_ID_SIZE 64U

/// Maximum bytes, including the terminating NUL, in a constraint ID.
#define QDMI_PROGRAM_CONSTRAINT_ID_SIZE 64U

/**
 * @brief One optional feature guarantee for an exact program format.
 * @details `id` and `constraint_id` must be NUL-terminated, and every byte
 * after the first NUL must be zero. QDMI reserves unqualified IDs for standard
 * features and constraints. Vendor-defined IDs must be namespaced.
 *
 * Records with the same `id` and `value` describe one feature group. An empty
 * `constraint_id` means that the group is unrestricted and
 * `constraint_value` must be zero. An unrestricted group contains exactly one
 * record. Otherwise, every record in the group is one constraint and all
 * constraints are conjunctive. A constrained group must not repeat a
 * constraint ID. Different values for the same feature ID are alternatives,
 * such as supported integer widths.
 *
 * A client that does not understand a feature ignores it. A client that
 * understands a feature must treat a group as unusable if it contains an
 * unknown constraint or if any record violates a representation or grouping
 * rule above. A group is also unusable if it applies a known constraint to a
 * feature for which that constraint is not defined, or if a constraint
 * documented as positive has value zero. Future QDMI revisions can add new
 * typed constraints without weakening this fail-closed rule.
 */
typedef struct QDMI_PROGRAM_FEATURE_T {
  char id[QDMI_PROGRAM_FEATURE_ID_SIZE]; ///< NUL-terminated feature ID.
  uint64_t value;                        ///< Feature-specific value.
  char constraint_id[QDMI_PROGRAM_CONSTRAINT_ID_SIZE];
  ///< NUL-terminated constraint ID, or empty for no constraint.
  uint64_t constraint_value; ///< Constraint-specific value.
} QDMI_Program_Feature;

/**
 * @brief Initialize an unrestricted program feature for C or C++.
 * @param feature_id String-literal feature ID.
 * @param feature_value Feature-specific value.
 */
#define QDMI_PROGRAM_FEATURE_UNCONSTRAINED(feature_id, feature_value)          \
  {feature_id, feature_value, "", 0U}

/**
 * @brief Measure a qubit before the end of one execution.
 * @details This Boolean feature uses value zero. It does not by itself
 * guarantee same-execution use of the result or reuse of the measured qubit.
 */
#define QDMI_PROGRAM_FEATURE_MID_CIRCUIT_MEASUREMENT "mid-circuit-measurement"
/**
 * @brief Apply a quantum operation to a measured qubit in the same execution.
 * @details This Boolean feature uses value zero.
 */
#define QDMI_PROGRAM_FEATURE_MEASURED_QUBIT_REUSE "measured-qubit-reuse"
/**
 * @brief Read a measurement result during the same execution.
 * @details This Boolean feature uses value zero. Control flow based on the
 * result also requires the relevant branching or loop feature.
 */
#define QDMI_PROGRAM_FEATURE_MEASUREMENT_RESULT_USE "measurement-result-use"
/**
 * @brief Compute Boolean values during execution.
 * @details This Boolean feature uses value zero. It includes Boolean logic and
 * comparisons whose operand types are otherwise supported by the descriptor.
 */
#define QDMI_PROGRAM_FEATURE_BOOLEAN_COMPUTATION "boolean-computation"
/**
 * @brief Select a later program region from a value computed during execution.
 * @details This Boolean feature uses value zero and does not include a backward
 * branch.
 */
#define QDMI_PROGRAM_FEATURE_FORWARD_BRANCHING "forward-branching"
/**
 * @brief Repeat a program region a known finite number of times.
 * @details This Boolean feature uses value zero. The trip count does not depend
 * on a value produced by the repeated region.
 */
#define QDMI_PROGRAM_FEATURE_COUNTED_ITERATION "counted-iteration"
/**
 * @brief Repeat a program region based on a value computed during execution.
 * @details This Boolean feature uses value zero and includes a backward branch.
 */
#define QDMI_PROGRAM_FEATURE_CONDITIONAL_LOOP "conditional-loop"
/**
 * @brief Select one of more than two program regions during execution.
 * @details This Boolean feature uses value zero. The selector type requires its
 * corresponding computation feature.
 */
#define QDMI_PROGRAM_FEATURE_MULTIWAY_BRANCHING "multiway-branching"
/**
 * @brief Compute integer values during execution.
 * @details The feature value is one exact supported integer width in bits.
 * Devices return one feature group for each supported width.
 */
#define QDMI_PROGRAM_FEATURE_INTEGER_COMPUTATION "integer-computation"
/**
 * @brief Compute floating-point values during execution.
 * @details The feature value is one exact supported floating-point width in
 * bits. Devices return one feature group for each supported width.
 */
#define QDMI_PROGRAM_FEATURE_FLOAT_COMPUTATION "float-computation"
/**
 * @brief Define and call functions beyond the entry point and format-mandated
 * declarations or runtime calls.
 * @details This Boolean feature uses value zero.
 */
#define QDMI_PROGRAM_FEATURE_IR_FUNCTIONS "ir-functions"
/**
 * @brief Use more than one return point in one function or entry point.
 * @details This Boolean feature uses value zero.
 */
#define QDMI_PROGRAM_FEATURE_MULTIPLE_RETURN_POINTS "multiple-return-points"
/**
 * @brief Allocate and release qubits during execution.
 * @details This Boolean feature uses value zero.
 */
#define QDMI_PROGRAM_FEATURE_DYNAMIC_QUBIT_MANAGEMENT "dynamic-qubit-management"
/**
 * @brief Allocate and release result handles during execution.
 * @details This Boolean feature uses value zero.
 */
#define QDMI_PROGRAM_FEATURE_DYNAMIC_RESULT_MANAGEMENT                         \
  "dynamic-result-management"
/**
 * @brief Create and access arrays during execution.
 * @details This Boolean feature uses value zero. Element types must be
 * supported by the descriptor or another reported computation feature.
 */
#define QDMI_PROGRAM_FEATURE_ARRAYS "arrays"

/**
 * @brief Limit lexical control-flow nesting depth.
 * @details The positive constraint value is the maximum number of enclosing
 * branch and loop constructs, counting the outermost construct as depth one.
 * This constraint applies to branching and iteration features.
 */
#define QDMI_PROGRAM_CONSTRAINT_MAX_CONTROL_FLOW_NESTING_DEPTH                 \
  "max-control-flow-nesting-depth"
/**
 * @brief Limit the trip count of each loop.
 * @details The positive constraint value is the inclusive maximum number of
 * times one loop body may execute. A compiler must prove this upper bound. This
 * constraint applies to counted iteration and conditional loops.
 */
#define QDMI_PROGRAM_CONSTRAINT_MAX_ITERATION_COUNT "max-iteration-count"
/**
 * @brief Limit the number of explicit cases in each multiway branch.
 * @details The positive constraint value counts explicit cases and excludes a
 * default case. This constraint applies to multiway branching.
 */
#define QDMI_PROGRAM_CONSTRAINT_MAX_CASE_COUNT "max-case-count"

/**
 * @brief Enum of the formats the results can be returned in.
 */
enum QDMI_JOB_RESULT_T {
  /**
   * @brief `char*` (string) The results of the individual shots as a
   * comma-separated list.
   * @details Each bit string contains every flat bit output declared by the
   * submitted payload. The following rules assign logical output slots starting
   * at zero:
   * - OpenQASM 2 uses `creg` declarations in source order and increasing bit
   *   index within each declaration.
   * - OpenQASM 3 uses bit-valued output declarations in source order and
   *   increasing bit index within each declaration. If the program has no
   *   explicit output declaration, the OpenQASM 3 implicit-output rules select
   *   the outputs before this ordering is applied.
   * - QIR uses primitive result-recording calls in execution order. A result
   *   array contributes its elements in memory order. Container recording
   *   calls do not add bits.
   * The string writes the highest-numbered slot first and slot zero at the
   * right. For example, logical slot values `[1, 0, 0]` produce `"001"`.
   * If the payload output cannot be represented losslessly as one fixed-width
   * bit string per shot, queries for shots and histogram results return @ref
   * QDMI_ERROR_NOTSUPPORTED. Clients can query @ref
   * QDMI_JOB_RESULT_PROGRAMOUTPUT when the submitted descriptor defines a
   * native output representation.
   *
   * The payload output schema owns the slots. Their width and order are
   * independent of the device sites and physical-site order. For example,
   * `"0010,1101,0101"` represents three shots of four declared bit outputs.
   */
  QDMI_JOB_RESULT_SHOTS = 0,
  /**
   * @brief `char*` (string) The keys for the histogram of the results.
   * @details The histogram of the measurement results is represented as a
   * key-value mapping. This mapping is returned as a list of keys and an
   * equal-length list of values. The corresponding partners of keys and values
   * can be found at the same index in the lists. Each key uses the same
   * payload-declared logical bit order as @ref QDMI_JOB_RESULT_SHOTS.
   *
   * This constant denotes the list of keys, @ref QDMI_JOB_RESULT_HIST_VALUES
   * denotes the list of values.
   */
  QDMI_JOB_RESULT_HIST_KEYS = 1,
  /**
   * @brief `size_t*` (`size_t` list) The values for the histogram of the
   * results.
   * @see QDMI_JOB_RESULT_HIST_KEY
   */
  QDMI_JOB_RESULT_HIST_VALUES = 2,
  /**
   * @brief `double*` (`double` list) The state vector of the result.
   * @details The complex amplitudes are stored as a list of real and imaginary
   * parts. Logical qubit zero is the least-significant bit of basis index `n`.
   * The real part of the amplitude is at index `2n` and the imaginary part is
   * at index `2n+1`. For example, the state vector of a 2-qubit system
   * with amplitudes `(0.5, 0.5), (0.5, -0.5), (-0.5, 0.5), (-0.5, -0.5)` would
   * be represented as `{0.5, 0.5, 0.5, -0.5, -0.5, 0.5, -0.5, -0.5}`.
   */
  QDMI_JOB_RESULT_STATEVECTOR_DENSE = 3,
  /**
   * @brief `double*` (`double` list) The probabilities of the result.
   * @details The probabilities are stored as a list of real numbers. Logical
   * qubit zero is the least-significant bit of basis index `n`. The probability
   * of that state is at index `n` in the list. For
   * example, the probabilities of a 2-qubit system with states `00, 01, 10, 11`
   * would be represented as `{0.25, 0.25, 0.25, 0.25}`.
   */
  QDMI_JOB_RESULT_PROBABILITIES_DENSE = 4,
  /**
   * @brief `char*` (string) The keys for the sparse state vector of the result.
   * @details The sparse state vector is represented as a key-value mapping.
   * This mapping is returned as a list of keys and an equal-length list of
   * values. The corresponding partners of keys and values can be found at the
   * same index in the lists. Keys write the highest-numbered logical qubit at
   * the left and logical qubit zero at the right.
   */
  QDMI_JOB_RESULT_STATEVECTOR_SPARSE_KEYS = 5,
  /**
   * @brief `double*` (`double` list) The values for the sparse state vector of
   * the result.
   * @details The complex amplitudes are stored in the same way as the dense
   * state vector, but only for the non-zero amplitudes.
   * @see QDMI_JOB_RESULT_STATEVECTOR_DENSE
   *      QDMI_JOB_RESULT_STATEVECTOR_SPARSE_KEYS
   */
  QDMI_JOB_RESULT_STATEVECTOR_SPARSE_VALUES = 6,
  /**
   * @brief `char*` (string) The keys for the sparse probabilities of the
   * result.
   * @details The sparse probabilities are represented as a key-value mapping.
   * This mapping is returned as a list of keys and an equal-length list of
   * values. The corresponding partners of keys and values can be found at the
   * same index in the lists. Keys write the highest-numbered logical qubit at
   * the left and logical qubit zero at the right.
   */
  QDMI_JOB_RESULT_PROBABILITIES_SPARSE_KEYS = 7,
  /**
   * @brief `double*` (`double` list) The values for the sparse probabilities of
   * the result.
   * @details The probabilities are stored in the same way as the dense
   * probabilities, but only for the non-zero probabilities.
   * @see QDMI_JOB_RESULT_PROBABILITIES_DENSE
   *      QDMI_JOB_RESULT_PROBABILITIES_SPARSE_KEYS
   */
  QDMI_JOB_RESULT_PROBABILITIES_SPARSE_VALUES = 8,
  /**
   * @brief `void*` The complete format-native program output.
   * @details The exact descriptor defines the byte representation. The output
   * is an arbitrary byte sequence and need not be NUL-terminated. For a QIR
   * specification that defines an output schema, the sequence is a complete
   * output-schema stream, including its required headers and every shot record.
   * Devices that accept such a QIR descriptor must support this result. A
   * device may return @ref QDMI_ERROR_NOTSUPPORTED when the submitted
   * descriptor does not define a native output representation.
   */
  QDMI_JOB_RESULT_PROGRAMOUTPUT = 9,
  /**
   * @brief The maximum value of the enum.
   * @details It can be used by devices for bounds checking and validation of
   * function parameters.
   *
   * @attention This value must remain the last regular member of the enum
   * besides the custom members and must be updated when new members are added.
   */
  QDMI_JOB_RESULT_MAX = 10,
  /**
   * @brief This enum value is reserved for a custom result.
   * @details The device defines the meaning and the type of this result.
   * @attention The value of this enum member must not be changed to maintain
   * binary compatibility.
   */
  QDMI_JOB_RESULT_CUSTOM1 = QDMI_CUSTOM_ENUM_VALUE_MIN,
  /// @see QDMI_JOB_RESULT_CUSTOM1
  QDMI_JOB_RESULT_CUSTOM2 = 999999996,
  /// @see QDMI_JOB_RESULT_CUSTOM1
  QDMI_JOB_RESULT_CUSTOM3 = 999999997,
  /// @see QDMI_JOB_RESULT_CUSTOM1
  QDMI_JOB_RESULT_CUSTOM4 = 999999998,
  /// @see QDMI_JOB_RESULT_CUSTOM1
  QDMI_JOB_RESULT_CUSTOM5 = 999999999
};

/// Job result type.
typedef enum QDMI_JOB_RESULT_T QDMI_Job_Result;

/**
 * @brief Enum to indicate the level of pulse support a device has.
 */
enum QDMI_DEVICE_PULSE_SUPPORT_LEVEL_T {
  /// The device does not support pulse-level control.
  QDMI_DEVICE_PULSE_SUPPORT_LEVEL_NONE = 0,
  /**
   * @brief The device supports pulse-level control at an abstraction level of
   * @ref QDMI_Site.
   * @details This means that the device can execute pulse-level
   * instructions on the sites of the device.
   * This level of support is sufficient for most devices that can execute
   * quantum circuits with pulse-level control, as it allows the device to
   * execute pulse-level instructions on the sites of the device.
   * @see QDMI_Site for more information on the site abstraction.
   */
  QDMI_DEVICE_PULSE_SUPPORT_LEVEL_SITE = 1,
  /**
   * @brief The device supports pulse-level control at an abstraction level of
   * `QDMI_Pulse_Channel`.
   * @details This means that the device can execute pulse-level instructions on
   * the channels of the device.
   * This level of support is sufficient for devices that can execute quantum
   * circuits with pulse-level control on a channel basis, such as devices that
   * use a single channel for all sites.
   */
  QDMI_DEVICE_PULSE_SUPPORT_LEVEL_CHANNEL = 2,
  /**
   * @brief The device supports pulse-level control at an abstraction level of
   * @ref QDMI_Site and `QDMI_Pulse_Channel`.
   * @details This means that the device can execute pulse-level instructions on
   * both the sites and channels of the device.
   */
  QDMI_DEVICE_PULSE_SUPPORT_LEVEL_SITEANDCHANNEL = 3,
};

/// Pulse support level type.
typedef enum QDMI_DEVICE_PULSE_SUPPORT_LEVEL_T QDMI_Device_Pulse_Support_Level;

// NOLINTEND(performance-enum-size, modernize-use-using)

#ifdef __cplusplus
} // extern "C"
#endif

#endif // QDMI_CONSTANTS_H
