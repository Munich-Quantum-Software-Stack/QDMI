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
 * @brief Defines all enums used within QDMI.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// The following clang-tidy warning cannot be addressed because this header is
// used from both C and C++ code.
// NOLINTBEGIN(performance-enum-size)

/// Enum of the device properties that can be queried.
enum QDMI_DEVICE_PROPERTY_T {
  QDMI_DEVICE_PROPERTY_NAME, ///< `char*` (string) The name of the device.
  /// `char*` (string) The version of the device.
  QDMI_DEVICE_PROPERTY_VERSION,
  /// `int` The @ref QDMI_Device_Status of the device.
  QDMI_DEVICE_PROPERTY_STATUS,
  /// `char*` (string) The implemented version of QDMI.
  QDMI_DEVICE_PROPERTY_LIBRARYVERSION,
  /// `size_t` The number of qubits in the device.
  QDMI_DEVICE_PROPERTY_QUBITSNUM,
  /**
   * @brief `int*` (int list) The coupling map of the device.
   * @details The returned list contains pairs of qubits that are coupled. The
   * pairs in the list are flattened such that the first qubit of the pair is at
   * index 2n and the second qubit is at index 2n+1. For example, a 2-qubit
   * device with a coupling map of (0, 1) would return `{0, 1}`. A
   * 3-qubit device with a coupling map of (0, 1), (1, 2) would return
   * `{0, 1, 1, 2}`.
   */
  QDMI_DEVICE_PROPERTY_COUPLINGMAP,
  /**
   * @brief This property is reserved for a custom property.
   * @details The meaning and the type of this property is defined by the
   * device.
   */
  QDMI_DEVICE_PROPERTY_CUSTOM_1,
  /// @see QDMI_DEVICE_PROPERTY_CUSTOM_1
  QDMI_DEVICE_PROPERTY_CUSTOM_2,
  /// @see QDMI_DEVICE_PROPERTY_CUSTOM_1
  QDMI_DEVICE_PROPERTY_CUSTOM_3,
  /// @see QDMI_DEVICE_PROPERTY_CUSTOM_1
  QDMI_DEVICE_PROPERTY_CUSTOM_4,
  /// @see QDMI_DEVICE_PROPERTY_CUSTOM_1
  QDMI_DEVICE_PROPERTY_CUSTOM_5,
  /**
   * @brief The maximum value of the enum.
   * @details This value can be used for bounds checks by the devices.
   * @note This value should always be updated to be the last and maximum value
   * of the enum.
   */
  QDMI_DEVICE_PROPERTY_MAX
};

/// Enum of different status the device can be in.
enum QDMI_DEVICE_STATUS_T {
  QDMI_DEVICE_STATUS_OFFLINE,     ///< The device is offline.
  QDMI_DEVICE_STATUS_IDLE,        ///< The device is idle.
  QDMI_DEVICE_STATUS_BUSY,        ///< The device is busy.
  QDMI_DEVICE_STATUS_ERROR,       ///< The device is in an error state.
  QDMI_DEVICE_STATUS_MAINTENANCE, ///< The device is in maintenance.
  QDMI_DEVICE_STATUS_CALIBRATION, ///< The device is in calibration.
  /**
   * @brief The maximum value of the enum.
   * @details This value can be used for bounds checks by the devices.
   * @note This value should always be updated to be the last and maximum value
   * of the enum.
   */
  QDMI_DEVICE_STATUS_MAX
};

/// Enum of the site properties that can be queried.
enum QDMI_SITE_PROPERTY_T {
  QDMI_SITE_PROPERTY_TIME_T1, ///< `double` The T1 time of a site in µs.
  QDMI_SITE_PROPERTY_TIME_T2, ///< `double` The T2 time of a site in µs.
  /**
   * @brief This property is reserved for a custom property.
   * @details The meaning and the type of this property is defined by the
   * device.
   */
  QDMI_SITE_PROPERTY_CUSTOM_1,
  /// @see QDMI_SITE_PROPERTY_CUSTOM_1
  QDMI_SITE_PROPERTY_CUSTOM_2,
  /// @see QDMI_SITE_PROPERTY_CUSTOM_1
  QDMI_SITE_PROPERTY_CUSTOM_3,
  /// @see QDMI_SITE_PROPERTY_CUSTOM_1
  QDMI_SITE_PROPERTY_CUSTOM_4,
  /// @see QDMI_SITE_PROPERTY_CUSTOM_1
  QDMI_SITE_PROPERTY_CUSTOM_5,
  /**
   * @brief The maximum value of the enum.
   * @details This value can be used for bounds checks by the devices.
   * @note This value should always be updated to be the last and maximum value
   * of the enum.
   */
  QDMI_SITE_PROPERTY_MAX
};

/// Enum of the operation properties that can be queried.
enum QDMI_OPERATION_PROPERTY_T {
  /// `char*` (string) The string identifier of the operation.
  QDMI_OPERATION_PROPERTY_NAME,
  /// `size_t` The number of qubits in the operation.
  QDMI_OPERATION_PROPERTY_QUBITSNUM,
  /// `double` The duration of an operation in µs.
  QDMI_OPERATION_PROPERTY_DURATION,
  /// `double` The fidelity of an operation.
  QDMI_OPERATION_PROPERTY_FIDELITY,
  /**
   * @brief This property is reserved for a custom property.
   * @details The meaning and the type of this property is defined by the
   * device.
   */
  QDMI_OPERATION_PROPERTY_CUSTOM_1,
  /// @see QDMI_OPERATION_PROPERTY_CUSTOM_1
  QDMI_OPERATION_PROPERTY_CUSTOM_2,
  /// @see QDMI_OPERATION_PROPERTY_CUSTOM_1
  QDMI_OPERATION_PROPERTY_CUSTOM_3,
  /// @see QDMI_OPERATION_PROPERTY_CUSTOM_1
  QDMI_OPERATION_PROPERTY_CUSTOM_4,
  /// @see QDMI_OPERATION_PROPERTY_CUSTOM_1
  QDMI_OPERATION_PROPERTY_CUSTOM_5,
  /**
   * @brief The maximum value of the enum.
   * @details This value can be used for bounds checks by the devices.
   * @note This value should always be updated to be the last and maximum value
   * of the enum.
   */
  QDMI_OPERATION_PROPERTY_MAX
};

/**
 * @brief Enum of the status a job can have.
 * @details See also @ref rationale.md for a description of the job's lifecycle.
 */
enum QDMI_JOB_STATUS_T {
  /**
   * @brief The job was created and can be configured via @ref
   * QDMI_control_set_parameter.
   */
  QDMI_JOB_STATUS_CREATED,
  /// The job was submitted and is waiting to be executed
  QDMI_JOB_STATUS_SUBMITTED,
  /// The job is done, and the result can be retrieved.
  QDMI_JOB_STATUS_DONE,
  /// The job is running, and the result is not yet available.
  QDMI_JOB_STATUS_RUNNING,
  /// The job was cancelled and the result is not available.
  QDMI_JOB_STATUS_CANCELLED
};

/**
 * @brief Enum of the modes a device can be opened in.
 * @note The values of this enum are meant to be used as bitflags. Hence, their
 * values must be powers of 2.
 */
enum QDMI_DEVICE_MODE_T {
  QDMI_DEVICE_MODE_READONLY = 0,  ///< To open the device in read-only mode.
  QDMI_DEVICE_MODE_READWRITE = 1, ///< To open the device in read-write mode.
};

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
};

/**
 * @brief Enum of formats that can be submitted to the device.
 */
enum QDMI_PROGRAM_FORMAT_T {
  /// `char*`(string) The OpenQASM 2.0 program to run.
  QDMI_PROGRAM_FORMAT_QASM2,
  /// `char*`(string) The OpenQASM 3 program to run.
  QDMI_PROGRAM_FORMAT_QASM3,
  /// `char*`(string) The QIR program to run as a string.
  QDMI_PROGRAM_FORMAT_QIRSTRING,
  /// `void*` The QIR program as a binary module.
  QDMI_PROGRAM_FORMAT_QIRMODULE,
  /**
   * @brief This value is reserved for a custom format.
   * @details The meaning and the type of this format is defined by the device.
   */
  QDMI_PROGRAM_FORMAT_CUSTOM_1,
  /// @see QDMI_PROGRAM_FORMAT_CUSTOM_1
  QDMI_PROGRAM_FORMAT_CUSTOM_2,
  /// @see QDMI_PROGRAM_FORMAT_CUSTOM_1
  QDMI_PROGRAM_FORMAT_CUSTOM_3,
  /// @see QDMI_PROGRAM_FORMAT_CUSTOM_1
  QDMI_PROGRAM_FORMAT_CUSTOM_4,
  /// @see QDMI_PROGRAM_FORMAT_CUSTOM_1
  QDMI_PROGRAM_FORMAT_CUSTOM_5,
  /**
   * @brief The maximum value of the enum.
   * @details This value can be used for bounds checks by the devices.
   * @note This value should always be updated to be the last and maximum value
   * of the enum.
   */
  QDMI_PROGRAM_FORMAT_MAX
};

/**
 * @brief Enum of the job parameters that can be set.
 */
enum QDMI_JOB_PARAMETER_T {
  /// `size_t` The number of shots to take.
  QDMI_JOB_PARAMETER_SHOTS_NUM,
  /**
   * @brief This property is reserved for a custom property.
   * @details The meaning and the type of this property is defined by the
   * device.
   */
  QDMI_JOB_PARAMETER_CUSTOM_1,
  /// @see QDMI_JOB_PARAMETER_CUSTOM_1
  QDMI_JOB_PARAMETER_CUSTOM_2,
  /// @see QDMI_JOB_PARAMETER_CUSTOM_1
  QDMI_JOB_PARAMETER_CUSTOM_3,
  /// @see QDMI_JOB_PARAMETER_CUSTOM_1
  QDMI_JOB_PARAMETER_CUSTOM_4,
  /// @see QDMI_JOB_PARAMETER_CUSTOM_1
  QDMI_JOB_PARAMETER_CUSTOM_5,
  /**
   * @brief The maximum value of the enum.
   * @details This value can be used for bounds checks by the devices.
   * @note This value should always be updated to be the last and maximum value
   * of the enum.
   */
  QDMI_JOB_PARAMETER_MAX
};

/**
 * @brief Enum of the formats the results can be returned in.
 */
enum QDMI_JOB_RESULT_T {
  /**
   * @brief `char*`(string) The results of the individual shots as a
   * comma-separated list, e.g., "0010,1101,0101,1100,1001,1100" for four qubits
   * and six shots.
   */
  QDMI_JOB_RESULT_SHOTS,
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
  QDMI_JOB_RESULT_HIST_KEYS,
  /**
   * @brief `size_t*` (int list) The values for the histogram of the results.
   * @see QDMI_JOB_RESULT_HIST_KEY
   */
  QDMI_JOB_RESULT_HIST_VALUES,
  /**
   * @brief `double*` (double list) The state vector of the result.
   * @details The complex amplitudes are stored as a list of real and imaginary
   * parts. The real part of the amplitude is at index 2n and the imaginary part
   * is at index 2n+1. For example, the state vector of a 2-qubit system with
   * amplitudes (0.5, 0.5), (0.5, -0.5), (-0.5, 0.5), (-0.5, -0.5) would be
   * represented as `{0.5, 0.5, 0.5, -0.5, -0.5, 0.5, -0.5, -0.5}`.
   */
  QDMI_JOB_RESULT_STATEVECTOR_DENSE,
  /**
   * @brief `double*` (double list) The probabilities of the result.
   * @details The probabilities are stored as a list of real numbers. The
   * probability of the state with index n is at index n in the list. For
   * example, the probabilities of a 2-qubit system with states 00, 01, 10, 11
   * would be represented as `{0.25, 0.25, 0.25, 0.25}`.
   */
  QDMI_JOB_RESULT_PROBABILITIES_DENSE,
  /**
   * @brief `char*`(string) The keys for the sparse state vector of the result.
   * @details The sparse state vector is represented as a key-value mapping.
   * This mapping is returned as a list of keys and an equal-length list of
   * values. The corresponding partners of keys and values can be found at the
   * same index in the lists.
   */
  QDMI_JOB_RESULT_STATEVECTOR_SPARSE_KEYS,
  /**
   * @brief `double*` (double list) The values for the sparse state vector of
   * the result.
   * @details The complex amplitudes are stored in the same way as the dense
   * state vector only that the values are only stored for the non-zero
   * amplitudes.
   * @see QDMI_JOB_RESULT_STATEVECTOR_DENSE
   * @see QDMI_JOB_RESULT_STATEVECTOR_SPARSE_KEYS
   */
  QDMI_JOB_RESULT_STATEVECTOR_SPARSE_VALUES,
  /**
   * @brief `char*`(string) The keys for the sparse probabilities of the result.
   * @details The sparse probabilities are represented as a key-value mapping.
   * This mapping is returned as a list of keys and an equal-length list of
   * values. The corresponding partners of keys and values can be found at the
   * same index in the lists.
   */
  QDMI_JOB_RESULT_PROBABILITIES_SPARSE_KEYS,
  /**
   * @brief `double*` (double list) The values for the sparse probabilities of
   * the result.
   * @details The probabilities are stored in the same way as the dense
   * probabilities only that the values are only stored for the non-zero
   * probabilities.
   * @see QDMI_JOB_RESULT_PROBABILITIES_DENSE
   * @see QDMI_JOB_RESULT_PROBABILITIES_SPARSE_KEYS
   */
  QDMI_JOB_RESULT_PROBABILITIES_SPARSE_VALUES,
  /**
   * @brief This property is reserved for a custom property.
   * @details The meaning and the type of this property is defined by the
   * device.
   */
  QDMI_JOB_RESULT_CUSTOM_1,
  /// @see QDMI_JOB_RESULT_CUSTOM_1
  QDMI_JOB_RESULT_CUSTOM_2,
  /// @see QDMI_JOB_RESULT_CUSTOM_1
  QDMI_JOB_RESULT_CUSTOM_3,
  /// @see QDMI_JOB_RESULT_CUSTOM_1
  QDMI_JOB_RESULT_CUSTOM_4,
  /// @see QDMI_JOB_RESULT_CUSTOM_1
  QDMI_JOB_RESULT_CUSTOM_5,
  /**
   * @brief The maximum value of the enum.
   * @details This value can be used for bounds checks by the devices.
   * @note This value should always be updated to be the last and maximum value
   * of the enum.
   */
  QDMI_JOB_RESULT_MAX
};

/**
 * @brief Enum of the session parameters that can be set.
 */
enum QDMI_SESSION_PARAMETER_T {
  /// `char*` (string) The owner of the session.
  QDMI_SESSION_PARAMETER_OWNER,
  /// `char*` (string) The access token for the session.
  QDMI_SESSION_PARAMETER_TOKEN,
  /**
   * @brief This property is reserved for a custom property.
   * @details The meaning and the type of this property is defined by the
   * driver.
   */
  QDMI_SESSION_PARAMETER_CUSTOM_1,
  /// @see QDMI_SESSION_PARAMETER_CUSTOM_1
  QDMI_SESSION_PARAMETER_CUSTOM_2,
  /// @see QDMI_SESSION_PARAMETER_CUSTOM_1
  QDMI_SESSION_PARAMETER_CUSTOM_3,
  /// @see QDMI_SESSION_PARAMETER_CUSTOM_1
  QDMI_SESSION_PARAMETER_CUSTOM_4,
  /// @see QDMI_SESSION_PARAMETER_CUSTOM_1
  QDMI_SESSION_PARAMETER_CUSTOM_5,
  /**
   * @brief The maximum value of the enum.
   * @details This value can be used for bounds checks by the drivers.
   * @note This value should always be updated to be the last and maximum value
   * of the enum.
   */
  QDMI_SESSION_PARAMETER_MAX
};

// NOLINTEND(performance-enum-size)

#ifdef __cplusplus
} // extern "C"
#endif
