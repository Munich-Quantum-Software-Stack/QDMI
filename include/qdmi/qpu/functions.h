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

/// @file
/// @brief Defines the @ref qpu_interface "QPU interface".

#pragma once

#include "qdmi/core.h"
#include "qdmi/job/functions.h"
#include "qdmi/qpu/constants.h"

#ifdef __cplusplus
#include <cstddef>
#else
#include <stddef.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

// The following clang-tidy warning cannot be addressed because this header is
// used from both C and C++ code.
// NOLINTBEGIN(modernize-use-using, modernize-redundant-void-arg)

/// @defgroup qpu_interface QDMI QPU Interface
///
/// Describes the functions to be implemented by a quantum processing unit
/// (QPU) or backend to be used with QDMI.
///
/// The QPU interface includes functions to query properties of the QPU, such as
/// its status and the number of qubits, and to create and manage jobs on the
/// QPU. To this end, it includes the functions of the @ref job_interface "job
/// interface" as well.
///
/// Similar to the @ref core_interface "core interface", the QPU interface is
/// split into two parts:
/// - The @ref qpu_session_interface "QPU's session interface" and
///   job_session_interface "Job's session interface" for additional
///   functionality on the session level.
/// - The @ref job_job_interface "job interface" for managing jobs on the QPU.
///
/// @{

/// @defgroup qpu_session_interface QPU Session Interface
///
/// The QPU session interface includes functions to query properties of the
/// QPU, such as its status and the number of qubits. It extends the @ref
/// core_session_interface "core session interface" with QPU-specific functions.
///
/// @{

/// Query the status of a QPU.
///
/// @param[in] session The session used for the query. Must not be @c NULL.
/// @param[out] value A pointer to the memory location where the status will be
/// stored. If this is @c NULL, it is ignored.
///
/// @returns @ref QDMI_SUCCESS if the QPU supports querying a status, and, when
/// the @p value is not @c NULL, the property was retrieved successfully.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if @p session is @c NULL.
/// @returns @ref QDMI_ERROR_NOTSUPPORTED if the arguments are valid and the
/// QPU does not support a status, or the status cannot be retrieved.
/// @returns @ref QDMI_ERROR_BADSTATE if the arguments are valid, and the
/// property cannot be queried in the current state of the session, for example,
/// because the session is not initialized with @ref QDMI_session_initialize.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
///
/// @note Calling this function with the @p value set to @c NULL is expected to
/// allow checking whether the QPU supports querying a status without
/// retrieving the status without the need to provide a buffer for it.
///
/// @attention May only be called after the session has been initialized with
/// @ref QDMI_session_initialize.
typedef int QDMI_session_query_qpu_status(QDMI_Session session,
                                          QDMI_QPU_Status *value);

/// Query a QPU's number of qubits.
///
/// @param[in] session The session used for the query. Must not be @c NULL.
/// @param[out] value A pointer to the memory location where the number of
/// qubits will be stored. If this is @c NULL, it is ignored.
///
/// @returns @ref QDMI_SUCCESS if the QPU supports querying the number of
/// qubits, and, when the @p value is not @c NULL, the property was retrieved
/// successfully.
/// @returns @ref QDMI_ERROR_INVALIDARGUMENT if @p session is @c NULL.
/// @returns @ref QDMI_ERROR_NOTSUPPORTED if the arguments are valid and the
/// QPU does not support querying the number of qubits, or the number of qubits
/// cannot be retrieved.
/// @returns @ref QDMI_ERROR_BADSTATE if the arguments are valid, and the
/// property cannot be queried in the current state of the session, for example,
/// because the session is not initialized with @ref QDMI_session_initialize.
/// @returns @ref QDMI_ERROR_FATAL if an unexpected error occurred.
///
/// @attention May only be called after the session has been initialized with
/// @ref QDMI_session_initialize.
typedef int QDMI_session_query_qubit_count(QDMI_Session session, size_t *value);

/** @} */ // end of qpu_session_interface

struct QDMI_QPU_Interface_impl_d {
  //===--------------------------------------------------------------------===//
  // Session Interface
  //===--------------------------------------------------------------------===//
  /// @see QDMI_session_query_qpu_status
  QDMI_session_query_qpu_status *session_query_qpu_status{};
  /// @see QDMI_session_query_qubit_count
  QDMI_session_query_qubit_count *session_query_qubit_count{};
  /// @see QDMI_session_query_program_formats
  QDMI_session_query_program_formats *session_query_program_formats{};
  /// @see QDMI_session_query_program_format_by_id_and_version
  QDMI_session_query_program_format_by_id_and_version
      *session_query_program_format_by_id_and_version{};
  /// @see QDMI_session_query_program_format_id
  QDMI_session_query_program_format_id *session_query_program_format_id{};
  /// @see QDMI_session_query_program_format_string_support
  QDMI_session_query_program_format_string_support
      *session_query_program_format_string_support{};
  /// @see QDMI_session_query_program_format_binary_support
  QDMI_session_query_program_format_binary_support
      *session_query_program_format_binary_support{};
  /// @see QDMI_session_query_program_format_version
  QDMI_session_query_program_format_version
      *session_query_program_format_version{};
  /// @see QDMI_session_create_job
  QDMI_session_create_job *session_create_job{};
  /// @see QDMI_session_retrieve_job_by_id
  QDMI_session_retrieve_job_by_id *session_retrieve_job_by_id{};
  //===--------------------------------------------------------------------===//
  // Job interface
  //===--------------------------------------------------------------------===//
  /// @see QDMI_job_set_payload_string
  QDMI_job_set_payload_string *job_set_payload_string{};
  /// @see QDMI_job_set_payload_binary
  QDMI_job_set_payload_binary *job_set_payload_binary{};
  /// @see QDMI_job_set_shot_count
  QDMI_job_set_shot_count *job_set_shot_count{};
  /// @see QDMI_job_set_log_callback
  QDMI_job_set_log_callback *job_set_log_callback{};
  /// @see QDMI_job_query_id
  QDMI_job_query_id *job_query_id{};
  /// @see QDMI_job_query_payload_program_format
  QDMI_job_query_payload_program_format *job_query_payload_program_format{};
  /// @see QDMI_job_query_payload_string
  QDMI_job_query_payload_string *job_query_payload_string{};
  /// @see QDMI_job_query_payload_binary
  QDMI_job_query_payload_binary *job_query_payload_binary{};
  /// @see QDMI_job_query_shot_count
  QDMI_job_query_shot_count *job_query_shot_count{};
  /// @see QDMI_job_submit
  QDMI_job_submit *job_submit{};
  /// @see QDMI_job_cancel
  QDMI_job_cancel *job_cancel{};
  /// @see QDMI_job_check
  QDMI_job_check *job_check{};
  /// @see QDMI_job_wait
  QDMI_job_wait *job_wait{};
  /// @see QDMI_job_get_shots
  QDMI_job_get_shots *job_get_shots{};
  /// @see QDMI_job_get_histogram
  QDMI_job_get_histogram *job_get_histogram{};
  /// @see QDMI_job_get_state_vector_dense
  QDMI_job_get_state_vector_dense *job_get_state_vector_dense{};
  /// @see QDMI_job_get_probabilities_dense
  QDMI_job_get_probabilities_dense *job_get_probabilities_dense{};
  /// @see QDMI_job_get_state_vector_sparse
  QDMI_job_get_state_vector_sparse *job_get_state_vector_sparse{};
  /// @see QDMI_job_get_probabilities_sparse
  QDMI_job_get_probabilities_sparse *job_get_probabilities_sparse{};
  /// @see QDMI_job_free
  QDMI_job_free *job_free{};
};

/** @} */ // end of qpu_interface

// NOLINTEND(modernize-use-using, modernize-redundant-void-arg)

#ifdef __cplusplus
} // extern "C"
#endif
