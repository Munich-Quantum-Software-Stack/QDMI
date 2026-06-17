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
/// Defines the @ref orchestration_interface "orchestration layer interface".

#pragma once

#include "qdmi/job/functions.h"
#include "qdmi/job/types.h"
#include "qdmi/provider/functions.h"
#include "qdmi/provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

// NOLINTBEGIN(modernize-use-using)

/// @defgroup orchestration_interface QDMI Orchestration Layer Interface
///
/// Describes the functions to be implemented by an orchestration layer as an
/// entry point to a software stack with multiple quantum devices.
///
/// @{

/// @defgroup orchestration_job_interface QDMI Orchestration Layer's Job
/// Interface
///
/// The orchestration layer's job interface includes functions to set a job's
/// target device.
///
/// @{

/// Set the target device of a job.
/// @param[in] job is the job for which to set the target device. Must not be
/// @c NULL.
/// @param[in] device is the device to set as the target device for the job
/// retrieved, e.g., via @ref QDMI_session_query_devices. If this parameter is
/// set to @c NULL, the target device will be unset, and the job will run on a
/// default device determined by the orchestration layer.
///
/// @returns QDMI_SUCCESS if the target device was successfully set for the job.
/// @returns QDMI_ERROR_INVALIDARGUMENT if @p job is @c NULL.
/// @returns QDMI_ERROR_BADSTATE if the job is already submitted or finished.
/// @returns QDMI_ERROR_FATAL if an unexpected error occurred.
typedef int QDMI_job_set_device(QDMI_Job job, QDMI_Device device);

/** @} */

struct QDMI_OrchestrationLayer_Interface_impl_d {
  //===--------------------------------------------------------------------===//
  // Session interface
  //===--------------------------------------------------------------------===//
  /// @see QDMI_session_query_program_formats
  QDMI_session_query_program_formats *session_query_program_formats;
  /// @see QDMI_session_query_program_format_by_id_and_version
  QDMI_session_query_program_format_by_id_and_version
      *session_query_program_format_by_id_and_version;
  /// @see QDMI_session_query_program_format_id
  QDMI_session_query_program_format_id *session_query_program_format_id;
  /// @see QDMI_session_query_program_format_string_support
  QDMI_session_query_program_format_string_support
      *session_query_program_format_string_support;
  /// @see QDMI_session_query_program_format_binary_support
  QDMI_session_query_program_format_binary_support
      *session_query_program_format_binary_support;
  /// @see QDMI_session_query_program_format_version
  QDMI_session_query_program_format_version
      *session_query_program_format_version;
  /// @see QDMI_session_create_job
  QDMI_session_create_job *session_create_job;
  /// @see QDMI_session_retrieve_job_by_id
  QDMI_session_retrieve_job_by_id *session_retrieve_job_by_id;
  /// @see QDMI_session_query_devices
  QDMI_session_query_devices *session_query_devices;
  /// @see QDMI_session_query_device_by_id
  QDMI_session_query_device_by_id *session_query_device_by_id;
  /// @see QDMI_session_get_device_interface
  QDMI_session_get_device_interface *session_get_device_interface;
  /// @see QDMI_session_query_device_context
  QDMI_session_query_device_context *session_query_device_context;
  //===--------------------------------------------------------------------===//
  // Job interface
  //===--------------------------------------------------------------------===//
  /// @see QDMI_job_set_device
  QDMI_job_set_device *job_set_device;
  /// @see QDMI_job_set_payload_string
  QDMI_job_set_payload_string *job_set_payload_string;
  /// @see QDMI_job_set_payload_binary
  QDMI_job_set_payload_binary *job_set_payload_binary;
  /// @see QDMI_job_set_shot_count
  QDMI_job_set_shot_count *job_set_shot_count;
  /// @see QDMI_job_set_log_callback
  QDMI_job_set_log_callback *job_set_log_callback;
  /// @see QDMI_job_query_id
  QDMI_job_query_id *job_query_id;
  /// @see QDMI_job_query_payload_program_format
  QDMI_job_query_payload_program_format *job_query_payload_program_format;
  /// @see QDMI_job_query_payload_string
  QDMI_job_query_payload_string *job_query_payload_string;
  /// @see QDMI_job_query_payload_binary
  QDMI_job_query_payload_binary *job_query_payload_binary;
  /// @see QDMI_job_query_shot_count
  QDMI_job_query_shot_count *job_query_shot_count;
  /// @see QDMI_job_submit
  QDMI_job_submit *job_submit;
  /// @see QDMI_job_cancel
  QDMI_job_cancel *job_cancel;
  /// @see QDMI_job_check
  QDMI_job_check *job_check;
  /// @see QDMI_job_wait
  QDMI_job_wait *job_wait;
  /// @see QDMI_job_get_shots
  QDMI_job_get_shots *job_get_shots;
  /// @see QDMI_job_get_histogram
  QDMI_job_get_histogram *job_get_histogram;
  /// @see QDMI_job_get_state_vector_dense
  QDMI_job_get_state_vector_dense *job_get_state_vector_dense;
  /// @see QDMI_job_get_probabilities_dense
  QDMI_job_get_probabilities_dense *job_get_probabilities_dense;
  /// @see QDMI_job_get_state_vector_sparse
  QDMI_job_get_state_vector_sparse *job_get_state_vector_sparse;
  /// @see QDMI_job_get_probabilities_sparse
  QDMI_job_get_probabilities_sparse *job_get_probabilities_sparse;
  /// @see QDMI_job_free
  QDMI_job_free *job_free;
};

/** @} */ // end of orchestration_interface

// NOLINTEND(modernize-use-using)

#ifdef __cplusplus
} // extern "C"
#endif
