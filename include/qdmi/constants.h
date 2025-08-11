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
 * @brief Defines all enums used within QDMI across the @ref client_interface
 * and the @ref device_interface.
 */

#pragma once

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
   * @brief The maximum value of the enum.
   * @details It can be used by devices for bounds checking and validation of
   * function parameters.
   *
   * @attention This value must remain the last regular member of the enum
   * besides the custom members and must be updated when new members are added.
   */
  QDMI_DEVICE_SESSION_PARAMETER_MAX = 6,
  /**
   * @brief This enum value is reserved for a custom parameter.
   * @details The device defines the meaning and the type of this parameter.
   * @attention The value of this enum member must not be changed to maintain
   * binary compatibility.
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
   * It may be used to recover a @ref QDMI_Device_Job handle upon device
   * failure.
   * It may, for example, correspond to the job ID provided by the
   * device's API or may be generated by the QDMI Device implementation.
   */
  QDMI_DEVICE_JOB_PROPERTY_ID = 0,
  /**
   * @brief @ref QDMI_Program_Format The format of the program to be executed.
   * @note This property returns the value of the @ref
   * QDMI_DEVICE_JOB_PARAMETER_PROGRAMFORMAT parameter.
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
   * @brief The maximum value of the enum.
   * @details It can be used by devices for bounds checking and validation of
   * function parameters.
   *
   * @attention This value must remain the last regular member of the enum
   * besides the custom members and must be updated when new members are added.
   */
  QDMI_DEVICE_JOB_PROPERTY_MAX = 4,
  /**
   * @brief This enum value is reserved for a custom parameter.
   * @details The device defines the meaning and the type of this parameter.
   * @attention The value of this enum member must not be changed to maintain
   * binary compatibility.
   */
  QDMI_DEVICE_JOB_PROPERTY_CUSTOM1 = 999999995,
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
   * @details This flag indicates whether the device needs calibration.
   * A value of zero indicates that the device does not need calibration, while
   * any non-zero value indicates that the device needs calibration. It is up
   * to the device to assign a specific meaning to the non-zero value.
   *
   * If a device reports that it needs calibration, a calibration run can be
   * triggered by submitting a job with the @ref QDMI_Program_Format set to @ref
   * QDMI_PROGRAM_FORMAT_CALIBRATION.
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
   * @see QDMI_DEVICE_PROPERTY_LENGTSCALEFACTOR
   */
  QDMI_DEVICE_PROPERTY_MINATOMDISTANCE = 14,
  /**
   * @brief The maximum value of the enum.
   * @details It can be used by devices for bounds checking and validation of
   * function parameters.
   *
   * @attention This value must remain the last regular member of the enum
   * besides the custom members and must be updated when new members are added.
   */
  QDMI_DEVICE_PROPERTY_MAX = 15,
  /**
   * @brief This enum value is reserved for a custom property.
   * @details The device defines the meaning and the type of this property.
   * @attention The value of this enum member must not be changed to maintain
   * binary compatibility.
   */
  QDMI_DEVICE_PROPERTY_CUSTOM1 = 999999995,
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
   * @see QDMI_DEVICE_PROPERTY_DURATIONSCALEFACTOR
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
   * @see QDMI_DEVICE_PROPERTY_DURATIONSCALEFACTOR
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
   * @see QDMI_DEVICE_PROPERTY_LENGTSCALEFACTOR
   * @see QDMI_SITE_PROPERTY_XCOORDINATE
   * @see QDMI_SITE_PROPERTY_YCOORDINATE
   * @see QDMI_SITE_PROPERTY_ZCOORDINATE
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
   * @see QDMI_DEVICE_PROPERTY_LENGTSCALEFACTOR
   * @see QDMI_SITE_PROPERTY_XCOORDINATE
   * @see QDMI_SITE_PROPERTY_YCOORDINATE
   * @see QDMI_SITE_PROPERTY_ZCOORDINATE
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
   * @see QDMI_DEVICE_PROPERTY_LENGTSCALEFACTOR
   * @see QDMI_SITE_PROPERTY_XCOORDINATE
   * @see QDMI_SITE_PROPERTY_YCOORDINATE
   * @see QDMI_SITE_PROPERTY_ZCOORDINATE
   */
  QDMI_SITE_PROPERTY_ZCOORDINATE = 6,
  /**
   * @brief `bool` Whether the site is a zone.
   * @details A zone is a site that has a spatial extent, i.e., it is not
   * just a point in space as a regular site. These kind of sites, namely zones,
   * are required to adequately represent global operations that act on all
   * qubits within a certain area, i.e., a zone.
   * @note Zones are typically used in neutral atom devices, where the atoms are
   * arranged in a 2D or 3D lattice, and operations can be applied to all
   * atoms within a certain zone.
   * @note This property defaults to `false`, i.e., if a device reports @ref
   * QDMI_ERROR_NOTSUPPORTED for this property, it is assumed that the site is
   * a regular site and not a zone.
   * @see QDMI_SITE_PROPERTY_XEXTENT
   * @see QDMI_SITE_PROPERTY_YEXTENT
   * @see QDMI_SITE_PROPERTY_ZEXTENT
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
   * @note This property is mainly required for neutral atom devices to
   * report the extent of zones, see @ref QDMI_SITE_PROPERTY_ISZONE.
   * @note If the site is not a zone, this property must return @ref
   * QDMI_ERROR_NOTSUPPORTED.
   * @see QDMI_DEVICE_PROPERTY_LENGTHUNIT
   * @see QDMI_DEVICE_PROPERTY_LENGTSCALEFACTOR
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
   * @note This property is mainly required for neutral atom devices to
   * report the extent of zones, see @ref QDMI_SITE_PROPERTY_ISZONE.
   * @note If the site is not a zone, this property must return @ref
   * QDMI_ERROR_NOTSUPPORTED.
   * @see QDMI_DEVICE_PROPERTY_LENGTHUNIT
   * @see QDMI_DEVICE_PROPERTY_LENGTSCALEFACTOR
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
   * @note This property is mainly required for neutral atom devices to
   * report the extent of zones, see @ref QDMI_SITE_PROPERTY_ISZONE.
   * @note If the site is not a zone, this property must return @ref
   * QDMI_ERROR_NOTSUPPORTED.
   * @see QDMI_DEVICE_PROPERTY_LENGTHUNIT
   * @see QDMI_DEVICE_PROPERTY_LENGTSCALEFACTOR
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
  QDMI_SITE_PROPERTY_CUSTOM1 = 999999995,
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
   * @see QDMI_DEVICE_PROPERTY_DURATIONSCALEFACTOR
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
   * @see QDMI_DEVICE_PROPERTY_LENGTSCALEFACTOR
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
   * @see QDMI_DEVICE_PROPERTY_LENGTHSCALEFACTOR
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
   * @see QDMI_OPERATION_PROPERTY_SITES
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
   * @see QDMI_DEVICE_PROPERTY_LENGTHSCALEFACTOR
   * @see QDMI_DEVICE_PROPERTY_DURATIONUNIT
   * @see QDMI_DEVICE_PROPERTY_DURATIONSCALEFACTOR
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
  QDMI_OPERATION_PROPERTY_CUSTOM1 = 999999995,
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

/**
 * @brief Enum of formats that can be submitted to the device.
 */
enum QDMI_PROGRAM_FORMAT_T {
  /**
   * @brief `char*` (string) An OpenQASM 2.0 program.
   * @details A text-based representation of a quantum circuit in the
   * [OpenQASM 2.0 language](https://arxiv.org/abs/1707.03429). Devices that
   * claim to support this format must accept programs conforming to the
   * following rules:
   * - The program contains exactly one quantum register named `q`.
   * - The number of qubits in the quantum register `q` matches the number of
   *   sites in the device.
   * - The program only contains gate identifiers that are reported by the
   *   @ref QDMI_OPERATION_PROPERTY_NAME property of the device's operations.
   *
   * @par
   * Given a program following these rules, the operations in the program
   * are expected to be performed on the physical sites of the device as queried
   * via @ref QDMI_DEVICE_PROPERTY_SITES.
   * Specifically, an operation on `q[i]` is performed on the i-th site in the
   * list of sites returned by the device.
   *
   * @note
   * Devices may decide to support more general OpenQASM 2.0 programs that
   * do not follow these rules, for example, using multiple qubit registers or
   * arbitrary gates. However, in that case, no guarantees can be made about the
   * mapping of qubits in the program to the physical sites of the device.
   */
  QDMI_PROGRAM_FORMAT_QASM2 = 0,
  /**
   * @brief `char*` (string) An OpenQASM 3 program.
   * @details A text-based representation of a quantum circuit in the
   * [OpenQASM 3 language](https://openqasm.com/). Devices that claim to support
   * this format must accept programs conforming to the same rules as for @ref
   * QDMI_PROGRAM_FORMAT_QASM2.
   *
   * @par
   * Besides the rules for OpenQASM 2.0 programs, OpenQASM 3 programs may
   * be written using physical qubits, which are denoted by `$[NUM]`, with
   * `[NUM]` being a non-negative integer denoting the physical qubit's index.
   * If a program uses physical qubits, the operations in the program must be
   * performed on the sites with indices corresponding to the physical qubits in
   * the program.
   *
   * @note
   * Devices may decide to support more general OpenQASM 3 programs that
   * do not follow these rules, for example, using multiple qubit registers or
   * arbitrary gates. However, in that case, no guarantees can be made about the
   * mapping of qubits in the program to the physical sites of the device.
   */
  QDMI_PROGRAM_FORMAT_QASM3 = 1,
  /**
   * @brief `char*` (string) A text-based QIR program complying to the QIR base
   * profile.
   * @details A text-based representation of a quantum circuit in the Quantum
   * Intermediate Representation (QIR) format; specifically, the [QIR base
   * profile](https://github.com/qir-alliance/qir-spec/blob/8b3fd47b7b70122a104e24733ef9de911576f7d6/specification/under_development/profiles/Base_Profile.md).
   * Devices that claim to support this format must accept programs that follow
   * the rules for the QIR base profile and that only contain operations that
   * are reported by the @ref QDMI_OPERATION_PROPERTY_NAME property of the
   * device's operations (for example, `@__quantum__qis__[NAME]__body`, where
   * `[NAME]` is the name of the operation).
   *
   * @par
   * QIR has a similar distinction between dynamically allocated and static
   * hardware qubits as @ref QDMI_PROGRAM_FORMAT_QASM3. The same rules apply for
   * the mapping of qubits in the program to the physical sites of the device.
   * Specifically, if the program only allocates a single register named `q`
   * with as many qubits as there are sites in the device, the operations in the
   * program are expected to be performed on the physical sites of the device as
   * queried via @ref QDMI_DEVICE_PROPERTY_SITES. If the program uses static
   * qubit addresses (for example, `ptr inttoptr (i64 1 to ptr)`), the
   * operations in the program must be performed on the sites with indices
   * corresponding to the static qubit addresses in the program.
   *
   * @note Devices may decide to support more general QIR programs that do not
   * follow these rules, for example, using multiple qubit registers or
   * arbitrary gates. However, in that case, no guarantees can be made about the
   * mapping of qubits in the program to the physical sites of the device.
   */
  QDMI_PROGRAM_FORMAT_QIRBASESTRING = 2,
  /**
   * @brief `void*` A QIR binary complying to the QIR base profile.
   * @details A binary representation of a quantum circuit in the Quantum
   * Intermediate Representation (QIR) format; specifically, the [QIR base
   * profile](https://github.com/qir-alliance/qir-spec/blob/8b3fd47b7b70122a104e24733ef9de911576f7d6/specification/under_development/profiles/Base_Profile.md).
   *
   * @see
   * QDMI_PROGRAM_FORMAT_QIRBASESTRING for more information on the QIR base
   * profile and the expected behavior of devices supporting this format.
   */
  QDMI_PROGRAM_FORMAT_QIRBASEMODULE = 3,
  /**
   * @brief `char*` (string) A text-based QIR program complying to the QIR
   * adaptive profile.
   * @details A text-based representation of a quantum circuit in the Quantum
   * Intermediate Representation (QIR) format; specifically, the [QIR adaptive
   * profile](https://github.com/qir-alliance/qir-spec/blob/8b3fd47b7b70122a104e24733ef9de911576f7d6/specification/under_development/profiles/Adaptive_Profile.md).
   *
   * @see QDMI_PROGRAM_FORMAT_QIRBASESTRING for more information on the QIR base
   * profile and the expected behavior of devices supporting this format.
   */
  QDMI_PROGRAM_FORMAT_QIRADAPTIVESTRING = 4,
  /**
   * @brief `void*` A QIR binary complying to the QIR adaptive profile.
   * @details A binary representation of a quantum circuit in the Quantum
   * Intermediate Representation (QIR) format; specifically, the [QIR adaptive
   * profile](https://github.com/qir-alliance/qir-spec/blob/8b3fd47b7b70122a104e24733ef9de911576f7d6/specification/under_development/profiles/Adaptive_Profile.md).
   *
   * @see QDMI_PROGRAM_FORMAT_QIRBASESTRING for more information on the QIR base
   * profile and the expected behavior of devices supporting this format.
   */
  QDMI_PROGRAM_FORMAT_QIRADAPTIVEMODULE = 5,
  /**
   * @brief `void*` A calibration program.
   * @details This program format is used to request the device to perform a
   * calibration run. Triggering a calibration run does not require a program to
   * be set via @ref QDMI_DEVICE_JOB_PARAMETER_PROGRAM.
   */
  QDMI_PROGRAM_FORMAT_CALIBRATION = 6,
  /**
   * @brief The maximum value of the enum.
   * @details It can be used by devices for bounds checking and validation of
   * function parameters.
   *
   * @attention This value must remain the last regular member of the enum
   * besides the custom members and must be updated when new members are added.
   */
  QDMI_PROGRAM_FORMAT_MAX = 7,
  /**
   * @brief This enum value is reserved for a custom program format.
   * @details The device defines the meaning and the type of this value.
   * @attention The value of this enum member must not be changed to maintain
   * binary compatibility.
   */
  QDMI_PROGRAM_FORMAT_CUSTOM1 = 999999995,
  /// @see QDMI_PROGRAM_FORMAT_CUSTOM1
  QDMI_PROGRAM_FORMAT_CUSTOM2 = 999999996,
  /// @see QDMI_PROGRAM_FORMAT_CUSTOM1
  QDMI_PROGRAM_FORMAT_CUSTOM3 = 999999997,
  /// @see QDMI_PROGRAM_FORMAT_CUSTOM1
  QDMI_PROGRAM_FORMAT_CUSTOM4 = 999999998,
  /// @see QDMI_PROGRAM_FORMAT_CUSTOM1
  QDMI_PROGRAM_FORMAT_CUSTOM5 = 999999999
};

/// Program format type.
typedef enum QDMI_PROGRAM_FORMAT_T QDMI_Program_Format;

/**
 * @brief Enum of the formats the results can be returned in.
 */
enum QDMI_JOB_RESULT_T {
  /**
   * @brief `char*` (string) The results of the individual shots as a
   * comma-separated list, for example, "0010,1101,0101,1100,1001,1100" for four
   * qubits and six shots.
   */
  QDMI_JOB_RESULT_SHOTS = 0,
  /**
   * @brief `char*` (string) The keys for the histogram of the results.
   * @details The histogram of the measurement results is represented as a
   * key-value mapping. This mapping is returned as a list of keys and an
   * equal-length list of values. The corresponding partners of keys and values
   * can be found at the same index in the lists.
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
   * parts. The real part of the amplitude is at index `2n` and the imaginary
   * part is at index `2n+1`. For example, the state vector of a 2-qubit system
   * with amplitudes `(0.5, 0.5), (0.5, -0.5), (-0.5, 0.5), (-0.5, -0.5)` would
   * be represented as `{0.5, 0.5, 0.5, -0.5, -0.5, 0.5, -0.5, -0.5}`.
   */
  QDMI_JOB_RESULT_STATEVECTOR_DENSE = 3,
  /**
   * @brief `double*` (`double` list) The probabilities of the result.
   * @details The probabilities are stored as a list of real numbers. The
   * probability of the state with index `n` is at index `n` in the list. For
   * example, the probabilities of a 2-qubit system with states `00, 01, 10, 11`
   * would be represented as `{0.25, 0.25, 0.25, 0.25}`.
   */
  QDMI_JOB_RESULT_PROBABILITIES_DENSE = 4,
  /**
   * @brief `char*` (string) The keys for the sparse state vector of the result.
   * @details The sparse state vector is represented as a key-value mapping.
   * This mapping is returned as a list of keys and an equal-length list of
   * values. The corresponding partners of keys and values can be found at the
   * same index in the lists.
   */
  QDMI_JOB_RESULT_STATEVECTOR_SPARSE_KEYS = 5,
  /**
   * @brief `double*` (`double` list) The values for the sparse state vector of
   * the result.
   * @details The complex amplitudes are stored in the same way as the dense
   * state vector, but only for the non-zero amplitudes.
   * @see QDMI_JOB_RESULT_STATEVECTOR_DENSE
   * @see QDMI_JOB_RESULT_STATEVECTOR_SPARSE_KEYS
   */
  QDMI_JOB_RESULT_STATEVECTOR_SPARSE_VALUES = 6,
  /**
   * @brief `char*` (string) The keys for the sparse probabilities of the
   * result.
   * @details The sparse probabilities are represented as a key-value mapping.
   * This mapping is returned as a list of keys and an equal-length list of
   * values. The corresponding partners of keys and values can be found at the
   * same index in the lists.
   */
  QDMI_JOB_RESULT_PROBABILITIES_SPARSE_KEYS = 7,
  /**
   * @brief `double*` (`double` list) The values for the sparse probabilities of
   * the result.
   * @details The probabilities are stored in the same way as the dense
   * probabilities, but only for the non-zero probabilities.
   * @see QDMI_JOB_RESULT_PROBABILITIES_DENSE
   * @see QDMI_JOB_RESULT_PROBABILITIES_SPARSE_KEYS
   */
  QDMI_JOB_RESULT_PROBABILITIES_SPARSE_VALUES = 8,
  /**
   * @brief The maximum value of the enum.
   * @details It can be used by devices for bounds checking and validation of
   * function parameters.
   *
   * @attention This value must remain the last regular member of the enum
   * besides the custom members and must be updated when new members are added.
   */
  QDMI_JOB_RESULT_MAX = 9,
  /**
   * @brief This enum value is reserved for a custom result.
   * @details The device defines the meaning and the type of this result.
   * @attention The value of this enum member must not be changed to maintain
   * binary compatibility.
   */
  QDMI_JOB_RESULT_CUSTOM1 = 999999995,
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
