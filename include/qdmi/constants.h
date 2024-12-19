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
 * @brief Defines all enums used within QDMI across the client and device
 * interface.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// The following clang-tidy warnings cannot be addressed because this header is
// used from both C and C++ code.
// NOLINTBEGIN(performance-enum-size,modernize-use-using)

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
};

/**
 * @brief Enum of the device session parameters that can be set.
 * @details If not noted otherwise, parameters are optional and devices must not
 * require them to be set.
 */
enum QDMI_DEVICE_SESSION_PARAMETER_T {
  /**
   * @brief `char*` (string) A token to be used in the session initialization
   * for authenticating with the device.
   * @details A token could be a password, an API key, or any other form of
   * authentication that the device requires. The device documentation *must*
   * document what kind of token is required and how it is used. If the device
   * requires authentication via a token, this parameter must be set before
   * calling @ref QDMI_device_session_init.
   */
  QDMI_DEVICE_SESSION_PARAMETER_TOKEN = 0,
  /**
   * @brief `char*` (string) The baseURL or API endpoint to be used for
   * accessing the device within the session.
   * @details If this parameter is set, and the device supports it, the device
   * must use the specified baseURL or API endpoint for the session. Devices may
   * use this parameter to switch between different versions of the API or
   * different endpoints for testing or production environments.
   */
  QDMI_DEVICE_SESSION_PARAMETER_BASEURL = 1,
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
 * @details If not noted otherwise, parameters are optional and devices must not
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

/// Enum of the device properties that can be queried.
enum QDMI_DEVICE_PROPERTY_T {
  /// `char*` (string) The name of the device.
  QDMI_DEVICE_PROPERTY_NAME = 0,
  /// `char*` (string) The version of the device.
  QDMI_DEVICE_PROPERTY_VERSION = 1,
  /// `int` The @ref QDMI_Device_Status of the device.
  QDMI_DEVICE_PROPERTY_STATUS = 2,
  /// `char*` (string) The implemented version of QDMI.
  QDMI_DEVICE_PROPERTY_LIBRARYVERSION = 3,
  /// `size_t` The number of qubits in the device.
  QDMI_DEVICE_PROPERTY_QUBITSNUM = 4,
  /**
   * @brief `QDMI_Site*` (@ref QDMI_Site list) The coupling map of the device.
   * @details The returned list contains pairs of sites that are coupled. The
   * pairs in the list are flattened such that the first site of the pair is at
   * index 2n and the second site is at index 2n+1.
   *
   * The sites returned in that list are represented as @ref QDMI_Site handles.
   * For example, consider a 3-site device with a coupling map `(0, 1), (1, 2)`.
   * Additionally, assume `site_i` is the handle for the i-th site. Then,
   * `{site_0, site_1, site_1, site_2}` would be returned.
   */
  QDMI_DEVICE_PROPERTY_COUPLINGMAP = 5,
  /**
   * @brief The maximum value of the enum.
   * @details It can be used by devices for bounds checking and validation of
   * function parameters. This value must remain the last regular member of the
   * enum besides the custom members and must be updated when new members are
   * added.
   */
  QDMI_DEVICE_PROPERTY_MAX = 6,
  /**
   * @brief This property is reserved for a custom property.
   * @details The meaning and the type of this property are defined by the
   * device. To maintain binary compatibility, the value of this enum member
   * must not be changed.
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

/// Type of the device properties.
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
   * function parameters. This value must remain the last regular member of the
   * enum besides the custom members and must be updated when new members are
   * added.
   */
  QDMI_DEVICE_STATUS_MAX = 6
};

/// Type of the device status.
typedef enum QDMI_DEVICE_STATUS_T QDMI_Device_Status;

/// Enum of the site properties that can be queried.
enum QDMI_SITE_PROPERTY_T {
  /**
   * @brief `size_t` The unique ID to identify the site in a program.
   * @details The ID of a site is used to link the qubits used in a quantum
   * program to the physical sites of the device that can be queried via this
   * interface. IDs may be non-consecutive and need not start at 0.
   * See @ref QDMI_Program_Format for more information on how the site IDs map
   * to the qubits in a program.
   */
  QDMI_SITE_PROPERTY_ID = 0,
  /// `double` The T1 time of a site in µs.
  QDMI_SITE_PROPERTY_TIME_T1 = 1,
  /// `double` The T2 time of a site in µs.
  QDMI_SITE_PROPERTY_TIME_T2 = 2,
  /**
   * @brief The maximum value of the enum.
   * @details It can be used by devices for bounds checking and validation of
   * function parameters. This value must remain the last regular member of the
   * enum besides the custom members and must be updated when new members are
   * added.
   */
  QDMI_SITE_PROPERTY_MAX = 3,
  /**
   * @brief This property is reserved for a custom property.
   * @details The meaning and the type of this property are defined by the
   * device. To maintain binary compatibility, the value of this enum member
   * must not be changed.
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

/// Type of the site properties.
typedef enum QDMI_SITE_PROPERTY_T QDMI_Site_Property;

/// Enum of the operation properties that can be queried.
enum QDMI_OPERATION_PROPERTY_T {
  /// `char*` (string) The string identifier of the operation.
  QDMI_OPERATION_PROPERTY_NAME = 0,
  /// `size_t` The number of qubits in the operation.
  QDMI_OPERATION_PROPERTY_QUBITSNUM = 1,
  /// `double` The duration of an operation in µs.
  QDMI_OPERATION_PROPERTY_DURATION = 2,
  /// `double` The fidelity of an operation.
  QDMI_OPERATION_PROPERTY_FIDELITY = 3,
  /**
   * @brief The maximum value of the enum.
   * @details It can be used by devices for bounds checking and validation of
   * function parameters. This value must remain the last regular member of the
   * enum besides the custom members and must be updated when new members are
   * added.
   */
  QDMI_OPERATION_PROPERTY_MAX = 4,
  /**
   * @brief This property is reserved for a custom property.
   * @details The meaning and the type of this property are defined by the
   * device. To maintain binary compatibility, the value of this enum member
   * must not be changed.
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

/// Type of the operation properties.
typedef enum QDMI_OPERATION_PROPERTY_T QDMI_Operation_Property;

/**
 * @brief Enum of the status a job can have.
 * @details See also @ref rationale.md for a description of the job's lifecycle.
 */
enum QDMI_JOB_STATUS_T {
  /**
   * @brief The job was created and can be configured via @ref
   * QDMI_control_set_parameter.
   */
  QDMI_JOB_STATUS_CREATED = 0,
  /// The job was submitted and is waiting to be executed
  QDMI_JOB_STATUS_SUBMITTED = 1,
  /// The job is done, and the result can be retrieved.
  QDMI_JOB_STATUS_DONE = 2,
  /// The job is running, and the result is not yet available.
  QDMI_JOB_STATUS_RUNNING = 3,
  /// The job was cancelled and the result is not available.
  QDMI_JOB_STATUS_CANCELLED = 4
};

/// Type of the device status.
typedef enum QDMI_JOB_STATUS_T QDMI_Job_Status;

/**
 * @brief Enum of formats that can be submitted to the device.
 */
enum QDMI_PROGRAM_FORMAT_T {
  /// `char*`(string) An OpenQASM 2.0 program.
  QDMI_PROGRAM_FORMAT_QASM2 = 0,
  /// `char*`(string) An OpenQASM 3.0 program.
  QDMI_PROGRAM_FORMAT_QASM3 = 1,
  /// `char*`(string) A text-based QIR program complying to the QIR base
  /// profile.
  QDMI_PROGRAM_FORMAT_QIRBASESTRING = 2,
  /// `void*` A QIR binary complying to the QIR base profile.
  QDMI_PROGRAM_FORMAT_QIRBASEMODULE = 3,
  /// `char*`(string) A text-based QIR program complying to the QIR adaptive
  /// profile.
  QDMI_PROGRAM_FORMAT_QIRADAPTIVESTRING = 4,
  /// `void*` A QIR binary complying to the QIR adaptive profile.
  QDMI_PROGRAM_FORMAT_QIRADAPTIVEMODULE = 5,
  /**
   * @brief The maximum value of the enum.
   * @details It can be used by devices for bounds checking and validation of
   * function parameters. This value must remain the last regular member of the
   * enum besides the custom members and must be updated when new members are
   * added.
   */
  QDMI_PROGRAM_FORMAT_MAX = 6,
  /**
   * @brief This property is reserved for a custom property.
   * @details The meaning and the type of this property are defined by the
   * device. To maintain binary compatibility, the value of this enum member
   * must not be changed.
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

/// Type of the program format.
typedef enum QDMI_PROGRAM_FORMAT_T QDMI_Program_Format;

/**
 * @brief Enum of the formats the results can be returned in.
 */
enum QDMI_JOB_RESULT_T {
  /**
   * @brief `char*`(string) The results of the individual shots as a
   * comma-separated list, e.g., "0010,1101,0101,1100,1001,1100" for four qubits
   * and six shots.
   */
  QDMI_JOB_RESULT_SHOTS = 0,
  /**
   * @brief `char*`(string) The keys for the histogram of the results.
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
   * @brief `size_t*` (int list) The values for the histogram of the results.
   * @see QDMI_JOB_RESULT_HIST_KEY
   */
  QDMI_JOB_RESULT_HIST_VALUES = 2,
  /**
   * @brief `double*` (double list) The state vector of the result.
   * @details The complex amplitudes are stored as a list of real and imaginary
   * parts. The real part of the amplitude is at index 2n and the imaginary part
   * is at index 2n+1. For example, the state vector of a 2-qubit system with
   * amplitudes (0.5, 0.5), (0.5, -0.5), (-0.5, 0.5), (-0.5, -0.5) would be
   * represented as `{0.5, 0.5, 0.5, -0.5, -0.5, 0.5, -0.5, -0.5}`.
   */
  QDMI_JOB_RESULT_STATEVECTOR_DENSE = 3,
  /**
   * @brief `double*` (double list) The probabilities of the result.
   * @details The probabilities are stored as a list of real numbers. The
   * probability of the state with index n is at index n in the list. For
   * example, the probabilities of a 2-qubit system with states 00, 01, 10, 11
   * would be represented as `{0.25, 0.25, 0.25, 0.25}`.
   */
  QDMI_JOB_RESULT_PROBABILITIES_DENSE = 4,
  /**
   * @brief `char*`(string) The keys for the sparse state vector of the result.
   * @details The sparse state vector is represented as a key-value mapping.
   * This mapping is returned as a list of keys and an equal-length list of
   * values. The corresponding partners of keys and values can be found at the
   * same index in the lists.
   */
  QDMI_JOB_RESULT_STATEVECTOR_SPARSE_KEYS = 5,
  /**
   * @brief `double*` (double list) The values for the sparse state vector of
   * the result.
   * @details The complex amplitudes are stored in the same way as the dense
   * state vector only that the values are only stored for the non-zero
   * amplitudes.
   * @see QDMI_JOB_RESULT_STATEVECTOR_DENSE
   * @see QDMI_JOB_RESULT_STATEVECTOR_SPARSE_KEYS
   */
  QDMI_JOB_RESULT_STATEVECTOR_SPARSE_VALUES = 6,
  /**
   * @brief `char*`(string) The keys for the sparse probabilities of the result.
   * @details The sparse probabilities are represented as a key-value mapping.
   * This mapping is returned as a list of keys and an equal-length list of
   * values. The corresponding partners of keys and values can be found at the
   * same index in the lists.
   */
  QDMI_JOB_RESULT_PROBABILITIES_SPARSE_KEYS = 7,
  /**
   * @brief `double*` (double list) The values for the sparse probabilities of
   * the result.
   * @details The probabilities are stored in the same way as the dense
   * probabilities only that the values are only stored for the non-zero
   * probabilities.
   * @see QDMI_JOB_RESULT_PROBABILITIES_DENSE
   * @see QDMI_JOB_RESULT_PROBABILITIES_SPARSE_KEYS
   */
  QDMI_JOB_RESULT_PROBABILITIES_SPARSE_VALUES = 8,
  /**
   * @brief The maximum value of the enum.
   * @details It can be used by devices for bounds checking and validation of
   * function parameters. This value must remain the last regular member of the
   * enum besides the custom members and must be updated when new members are
   * added.
   */
  QDMI_JOB_RESULT_MAX = 9,
  /**
   * @brief This property is reserved for a custom property.
   * @details The meaning and the type of this property are defined by the
   * device. To maintain binary compatibility, the value of this enum member
   * must not be changed.
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

/// Type of the job result.
typedef enum QDMI_JOB_RESULT_T QDMI_Job_Result;

// NOLINTEND(performance-enum-size,modernize-use-using)

#ifdef __cplusplus
} // extern "C"
#endif
