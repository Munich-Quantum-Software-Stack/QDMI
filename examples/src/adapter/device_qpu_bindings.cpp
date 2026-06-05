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

#include "adapter/device_qpu_bindings.hpp"

#include "ad_qdmi/core.h"
#include "ad_qdmi/qpu.h"
#include "adapter/adapter.hpp"

#include <algorithm>
#include <complex>
#include <cstddef>
#include <map>
#include <ranges>
#include <utility>
#include <vector>

// NOLINTBEGIN(misc-include-cleaner)

namespace qdmi::adapter {
namespace {
/// @copydoc AD_QDMI_session_query_qpu_status
auto Session_query_qpu_status(DeviceSession *session, QDMI_QPU_Status *value)
    -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO, "[{}] Querying the QPU's status",
                      session->context->device->id);
  if (session->status != DeviceSession::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Session is not initialized",
                        session->context->device->id);
    return QDMI_ERROR_BADSTATE;
  }
  if (const auto ret{
          session->context->device->v1_library.session_query_device_property(
              session->v1_session, V1_DEVICE_PROPERTY::STATUS, sizeof(int),
              value, nullptr)};
      ret != SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Failed to query device property ({})",
                        session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_session_query_qubit_count
auto Session_query_qubit_count(DeviceSession *session, size_t *value) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[{}] Querying the number of qubits of the QPU",
                      session->context->device->id);
  if (session->status != DeviceSession::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Session is not initialized",
                        session->context->device->id);
    return QDMI_ERROR_BADSTATE;
  }
  if (const auto ret{
          session->context->device->v1_library.session_query_device_property(
              session->v1_session, V1_DEVICE_PROPERTY::QUBITSNUM,
              sizeof(size_t), value, nullptr)};
      ret != SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Failed to query device property ({})",
                        session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_session_query_program_formats
auto Session_query_program_formats(DeviceSession *session, const size_t size,
                                   DeviceFormat **value, size_t *size_ret)
    -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[{}] Querying the supported program formats",
                      session->context->device->id);
  if (session->status != DeviceSession::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Session is not initialized",
                        session->context->device->id);
    return QDMI_ERROR_BADSTATE;
  }
  if (!Ensure_formats_are_initialized(*session)) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Device formats could not be initialized",
                        session->context->device->id);
    return QDMI_ERROR_FATAL;
  }
  const auto &formats{*session->context->device->format_ptrs};
  if (value != nullptr) {
    if (size < formats.size()) {
      session->logger.log(QDMI_LOG_LEVEL_ERROR,
                          "[{}] Provided buffer size is too small",
                          session->context->device->id);
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    std::ranges::copy(formats, value);
  }
  if (size_ret != nullptr) {
    *size_ret = formats.size();
  }
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_session_query_program_format_by_id_and_version
auto Session_query_program_format_by_id_and_version(DeviceSession *session,
                                                    const char *id,
                                                    const size_t version,
                                                    DeviceFormat **format)
    -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(
      QDMI_LOG_LEVEL_INFO,
      "[{}] Querying the program format with the specified ID and version",
      session->context->device->id);
  if (session->status != DeviceSession::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Session is not initialized",
                        session->context->device->id);
    return QDMI_ERROR_BADSTATE;
  }
  if (id == nullptr || version == 0) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Invalid arguments",
                        session->context->device->id);
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (!Ensure_formats_are_initialized(*session)) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Device formats could not be initialized",
                        session->context->device->id);
    return QDMI_ERROR_FATAL;
  }
  if (const auto formats_it{session->context->device->formats->find(id)};
      formats_it != session->context->device->formats->end()) {
    if (const auto format_it{formats_it->second.find(version)};
        format_it != formats_it->second.end()) {
      if (format != nullptr) {
        *format = format_it->second.get();
      }
    } else {
      session->logger.log(
          QDMI_LOG_LEVEL_ERROR,
          "[{}] Program format with version {}.{}.{} ({}) not found",
          session->context->device->id, QDMI_MAJOR_VERSION(version),
          QDMI_MINOR_VERSION(version), QDMI_PATCH_VERSION(version), version);
      return QDMI_ERROR_NOTFOUND;
    }
  } else {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Program format with id {} not found",
                        session->context->device->id, id);
    return QDMI_ERROR_NOTFOUND;
  }
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_session_query_program_format_id
auto Session_query_program_format_id(DeviceSession *session,
                                     DeviceFormat *format, const size_t size,
                                     char *value, size_t *size_ret) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[{}] Querying the ID of the program format",
                      session->context->device->id);
  if (session->status != DeviceSession::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Session is not initialized",
                        session->context->device->id);
    return QDMI_ERROR_BADSTATE;
  }
  if (format == nullptr) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Program format is not specified",
                        session->context->device->id);
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  const auto &id = format->id;
  if (value != nullptr) {
    if (size < id.size() + 1) {
      session->logger.log(QDMI_LOG_LEVEL_ERROR,
                          "[{}] Provided buffer size is too small",
                          session->context->device->id);
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    std::ranges::copy(id, value);
    value[id.size()] = '\0';
  }
  if (size_ret != nullptr) {
    *size_ret = id.size() + 1;
  }
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_session_query_program_format_string_support
auto Session_query_program_format_string_support(DeviceSession *session,
                                                 DeviceFormat *format) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(
      QDMI_LOG_LEVEL_INFO,
      "[{}] Querying whether the program format supports string payloads",
      session->context->device->id);
  if (session->status != DeviceSession::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Session is not initialized",
                        session->context->device->id);
    return QDMI_ERROR_BADSTATE;
  }
  if (format == nullptr) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Program format is not specified",
                        session->context->device->id);
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  return format->string_support ? QDMI_SUCCESS : QDMI_ERROR_NOTSUPPORTED;
}
/// @copydoc AD_QDMI_session_query_program_format_binary_support
auto Session_query_program_format_binary_support(DeviceSession *session,
                                                 DeviceFormat *format) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(
      QDMI_LOG_LEVEL_INFO,
      "[{}] Querying whether the program format supports binary payloads",
      session->context->device->id);
  if (session->status != DeviceSession::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Session is not initialized",
                        session->context->device->id);
    return QDMI_ERROR_BADSTATE;
  }
  if (format == nullptr) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Program format is not specified",
                        session->context->device->id);
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  return format->binary_support ? QDMI_SUCCESS : QDMI_ERROR_NOTSUPPORTED;
}
/// @copydoc AD_QDMI_session_query_program_format_version
auto Session_query_program_format_version(DeviceSession *session,
                                          DeviceFormat *format, size_t *value)
    -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[{}] Querying the version of the program format",
                      session->context->device->id);
  if (session->status != DeviceSession::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Session is not initialized",
                        session->context->device->id);
    return QDMI_ERROR_BADSTATE;
  }
  if (format == nullptr) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Program format is not specified",
                        session->context->device->id);
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (value != nullptr) {
    *value = format->version;
  }
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_session_create_job
auto Session_create_job(DeviceSession *session,
                        const QDMI_Log_Callback callback, void *user_data,
                        DeviceJob **job) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO, "[{}] Creating a new job",
                      session->context->device->id);
  if ((callback == nullptr && user_data != nullptr) || job == nullptr) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Invalid arguments",
                        session->context->device->id);
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (session->status != DeviceSession::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Session is not initialized",
                        session->context->device->id);
    return QDMI_ERROR_BADSTATE;
  }
  auto logger =
      callback != nullptr ? Logger{callback, user_data} : session->logger;
  V1Job v1_job{};
  if (const auto ret{session->context->device->v1_library.session_create_job(
          session->v1_session, &v1_job)};
      ret != SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Failed to create a new job ({})",
                        session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  size_t size{};
  if (const auto ret{session->context->device->v1_library.job_query_property(
          v1_job, V1_JOB_PROPERTY::ID, 0, nullptr, &size)};
      ret != SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Failed to query job property (ID size) ({})",
                        session->context->device->id, ret);
    session->context->device->v1_library.job_free(v1_job);
    return Transform_return_codes(ret);
  }
  std::string id(size, '\0');
  if (const auto ret{session->context->device->v1_library.job_query_property(
          v1_job, V1_JOB_PROPERTY::ID, size, id.data(), nullptr)};
      ret != SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Failed to query job property (ID) ({})",
                        session->context->device->id, ret);
    session->context->device->v1_library.job_free(v1_job);
    return Transform_return_codes(ret);
  }
  auto job_uptr = std::make_unique<DeviceJob>(session, logger, v1_job, id);
  auto *job_ptr = session->context->device->jobs
                      .emplace(job_uptr.get(), std::move(job_uptr))
                      .first->first;
  session->context->device->jobs_by_id.emplace(job_ptr->id, job_ptr);
  *job = job_ptr;
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_session_retrieve_job_by_id
auto Session_retrieve_job_by_id(DeviceSession *session,
                                const QDMI_Log_Callback callback,
                                void *user_data, const char *id,
                                DeviceJob **job) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO, "[{}] Retrieving a job by its ID",
                      session->context->device->id);
  if ((callback == nullptr && user_data != nullptr) || id == nullptr ||
      job == nullptr) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Invalid arguments",
                        session->context->device->id);
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (session->status != DeviceSession::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Session is not initialized",
                        session->context->device->id);
    return QDMI_ERROR_BADSTATE;
  }
  auto logger =
      callback != nullptr ? Logger{callback, user_data} : session->logger;
  const auto job_it{session->context->device->jobs_by_id.find(id)};
  if (job_it == session->context->device->jobs_by_id.end()) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Job with ID {} not found",
                        session->context->device->id, id);
    return QDMI_ERROR_NOTFOUND;
  }
  *job = job_it->second;
  (*job)->session = session;
  (*job)->logger = logger;
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_job_set_payload_string
auto Job_set_payload_string(DeviceJob *job, DeviceFormat *format,
                            const size_t count, const char *const *values)
    -> int {
  if (job == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  job->logger.log(QDMI_LOG_LEVEL_INFO,
                  "[{}] Setting the job payload as a string",
                  job->session->context->device->id);
  if (format == nullptr || count == 0) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Invalid format or count",
                    job->session->context->device->id);
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (!format->string_support.has_value()) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR,
                    "[{}] Payload format does not support string data",
                    job->session->context->device->id);
    return QDMI_ERROR_NOTSUPPORTED;
  }
  if (count > 1) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR,
                    "[{}] Payload cannot contain multiple strings",
                    job->session->context->device->id);
    return QDMI_ERROR_NOTSUPPORTED;
  }
  if (const auto ret =
          job->session->context->device->v1_library.job_set_parameter(
              job->job_v1, V1_JOB_PARAMETER::PROGRAMFORMAT, sizeof(int),
              &*format->string_support);
      ret != SUCCESS) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR,
                    "[{}] Failed to set job parameter ({})",
                    job->session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  if (const auto ret =
          job->session->context->device->v1_library.job_set_parameter(
              job->job_v1, V1_JOB_PARAMETER::PROGRAM,
              std::strlen(values[0]) + 1, values[0]);
      ret != SUCCESS) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR,
                    "[{}] Failed to set job parameter ({})",
                    job->session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_job_set_payload_binary
auto Job_set_payload_binary(DeviceJob *job, DeviceFormat *format,
                            const size_t count, const size_t *sizes,
                            const void *const *values) -> int {
  if (job == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  job->logger.log(QDMI_LOG_LEVEL_INFO,
                  "[{}] Setting the job payload as binary data",
                  job->session->context->device->id);
  if (format == nullptr || count == 0) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Invalid format or count",
                    job->session->context->device->id);
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (!format->binary_support.has_value()) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR,
                    "[{}] Payload format does not support binary data",
                    job->session->context->device->id);
    return QDMI_ERROR_NOTSUPPORTED;
  }
  if (count > 1) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR,
                    "[{}] Payload cannot contain multiple binary data entries",
                    job->session->context->device->id);
    return QDMI_ERROR_NOTSUPPORTED;
  }
  if (const auto ret =
          job->session->context->device->v1_library.job_set_parameter(
              job->job_v1, V1_JOB_PARAMETER::PROGRAMFORMAT, sizeof(int),
              &*format->binary_support);
      ret != SUCCESS) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR,
                    "[{}] Failed to set job parameter ({})",
                    job->session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  if (const auto ret =
          job->session->context->device->v1_library.job_set_parameter(
              job->job_v1, V1_JOB_PARAMETER::PROGRAM, sizes[0], values[0]);
      ret != SUCCESS) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR,
                    "[{}] Failed to set job parameter ({})",
                    job->session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_job_set_shot_count
auto Job_set_shot_count(DeviceJob *job, const size_t value) -> int {
  if (job == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  job->logger.log(QDMI_LOG_LEVEL_INFO, "[{}] Setting the job shot count to {}",
                  job->session->context->device->id, value);
  if (value == 0) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR,
                    "[{}] Shot count must be greater than zero",
                    job->session->context->device->id);
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (const auto ret =
          job->session->context->device->v1_library.job_set_parameter(
              job->job_v1, V1_JOB_PARAMETER::SHOTSNUM, sizeof(size_t), &value);
      ret != SUCCESS) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR,
                    "[{}] Failed to set job parameter ({})",
                    job->session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_job_set_log_callback
auto Job_set_log_callback(DeviceJob *job, const QDMI_Log_Callback callback,
                          void *user_data) -> int {
  if (job == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  job->logger.log(QDMI_LOG_LEVEL_INFO, "[{}] Setting the job log callback",
                  job->session->context->device->id);
  if (callback == nullptr && user_data != nullptr) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR,
                    "[{}] Invalid log callback or user data",
                    job->session->context->device->id);
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  job->logger =
      callback != nullptr ? Logger{callback, user_data} : job->session->logger;
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_job_query_id
auto Job_query_id(DeviceJob *job, const size_t size, char *value,
                  size_t *size_ret) -> int {
  if (job == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  job->logger.log(QDMI_LOG_LEVEL_INFO, "[{}] Querying the job ID",
                  job->session->context->device->id);
  const auto &id = job->id;
  if (value != nullptr) {
    if (size < id.size() + 1) {
      job->logger.log(QDMI_LOG_LEVEL_ERROR,
                      "[{}] Provided buffer size is too small",
                      job->session->context->device->id);
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    std::ranges::copy(id, value);
    /* Ensure null-termination */
    /* NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic) */
    value[id.size()] = '\0';
  }
  if (size_ret != nullptr) {
    *size_ret = id.size() + 1;
  }
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_job_query_payload_program_format
auto Job_query_payload_program_format(DeviceJob *job, DeviceFormat **format)
    -> int {
  if (job == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  job->logger.log(QDMI_LOG_LEVEL_INFO,
                  "[{}] Querying the job payload program format",
                  job->session->context->device->id);
  if (format == nullptr) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Format pointer is null",
                    job->session->context->device->id);
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  V1_PROGRAM_FORMAT format_v1{};
  if (const auto ret{
          job->session->context->device->v1_library.job_query_property(
              job->job_v1, V1_JOB_PROPERTY::PROGRAMFORMAT, sizeof(int),
              &format_v1, nullptr)};
      ret != SUCCESS) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR,
                    "[{}] Failed to query job property ({})",
                    job->session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  *format = job->session->context->device->formats_v1->at(format_v1);
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_job_query_payload_string
auto Job_query_payload_string(DeviceJob *job, const size_t index,
                              const size_t size, char *value, size_t *size_ret)
    -> int {
  if (job == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  job->logger.log(QDMI_LOG_LEVEL_INFO,
                  "[{}] Querying the job payload as a string",
                  job->session->context->device->id);
  if (index > 0) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Payload index must be zero",
                    job->session->context->device->id);
    return QDMI_ERROR_OUTOFRANGE;
  }
  if (const auto ret{
          job->session->context->device->v1_library.job_query_property(
              job->job_v1, V1_JOB_PROPERTY::PROGRAM, size, value, size_ret)};
      ret != SUCCESS) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR,
                    "[{}] Failed to query job property ({})",
                    job->session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_job_query_payload_binary
auto Job_query_payload_binary(DeviceJob *job, const size_t index,
                              const size_t size, void *value, size_t *size_ret)
    -> int {
  if (job == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  job->logger.log(QDMI_LOG_LEVEL_INFO,
                  "[{}] Querying the job payload as binary data",
                  job->session->context->device->id);
  if (index > 0) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Payload index must be zero",
                    job->session->context->device->id);
    return QDMI_ERROR_OUTOFRANGE;
  }
  if (const auto ret{
          job->session->context->device->v1_library.job_query_property(
              job->job_v1, V1_JOB_PROPERTY::PROGRAM, size, value, size_ret)};
      ret != SUCCESS) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR,
                    "[{}] Failed to query job property ({})",
                    job->session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_job_query_shot_count
auto Job_query_shot_count(DeviceJob *job, size_t *value) -> int {
  if (job == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  job->logger.log(QDMI_LOG_LEVEL_INFO, "[{}] Querying the job's shot count",
                  job->session->context->device->id);
  if (value != nullptr) {
    if (const auto ret{
            job->session->context->device->v1_library.job_query_property(
                job->job_v1, V1_JOB_PROPERTY::SHOTSNUM, sizeof(size_t), value,
                nullptr)};
        ret != SUCCESS) {
      job->logger.log(QDMI_LOG_LEVEL_ERROR,
                      "[{}] Failed to query job property ({})",
                      job->session->context->device->id, ret);
      return Transform_return_codes(ret);
    }
  }
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_job_submit
auto Job_submit(DeviceJob *job) -> int {
  if (job == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  job->logger.log(QDMI_LOG_LEVEL_INFO, "[{}] Submitting the job",
                  job->session->context->device->id);
  if (const auto ret =
          job->session->context->device->v1_library.job_submit(job->job_v1);
      ret != SUCCESS) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Failed to submit the job ({})",
                    job->session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_job_cancel
auto Job_cancel(DeviceJob *job) -> int {
  if (job == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  job->logger.log(QDMI_LOG_LEVEL_INFO, "[{}] Canceling the job",
                  job->session->context->device->id);
  if (const auto ret =
          job->session->context->device->v1_library.job_cancel(job->job_v1);
      ret != SUCCESS) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Failed to cancel the job ({})",
                    job->session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_job_check
auto Job_check(DeviceJob *job, QDMI_Job_Status *status) -> int {
  if (job == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  job->logger.log(QDMI_LOG_LEVEL_INFO, "[{}] Checking the job's status",
                  job->session->context->device->id);
  if (status == nullptr) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Status pointer is null",
                    job->session->context->device->id);
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (const auto ret = job->session->context->device->v1_library.job_check(
          job->job_v1, reinterpret_cast<V1_JOB_STATUS *>(status));
      ret != SUCCESS) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR,
                    "[{}] Failed to check the job's status ({})",
                    job->session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_job_wait
auto Job_wait(DeviceJob *job, const size_t timeout) -> int {
  if (job == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  job->logger.log(QDMI_LOG_LEVEL_INFO, "[{}] Waiting for the job to complete",
                  job->session->context->device->id);
  if (const auto ret = job->session->context->device->v1_library.job_wait(
          job->job_v1, timeout);
      ret != SUCCESS) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR,
                    "[{}] Failed to wait for the job ({})",
                    job->session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_job_get_shots
auto Job_get_shots(DeviceJob *job, const size_t index, const size_t size,
                   char *data, size_t *size_ret) -> int {
  if (job == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  job->logger.log(QDMI_LOG_LEVEL_INFO, "[{}] Getting the shots of the job",
                  job->session->context->device->id);
  if (index > 0) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Result index out of range",
                    job->session->context->device->id);
    return QDMI_ERROR_OUTOFRANGE;
  }
  if (const auto ret{job->session->context->device->v1_library.job_get_results(
          job->job_v1, V1_JOB_RESULT::SHOTS, size, data, size_ret)};
      ret != SUCCESS) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Failed to get job results ({})",
                    job->session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_job_get_histogram
auto Job_get_histogram(DeviceJob *job, const size_t index,
                       const size_t size_keys, char *keys,
                       size_t *size_keys_ret, const size_t size_values,
                       size_t *values, size_t *size_values_ret) -> int {
  if (job == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  job->logger.log(QDMI_LOG_LEVEL_INFO, "Getting the histogram of the job",
                  job->session->context->device->id);
  if (index > 0) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR, "Result index out of range",
                    job->session->context->device->id);
    return QDMI_ERROR_OUTOFRANGE;
  }
  if (const auto ret{job->session->context->device->v1_library.job_get_results(
          job->job_v1, V1_JOB_RESULT::HIST_KEYS, size_keys, keys,
          size_keys_ret)};
      ret != SUCCESS) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR, "Failed to get job results ({})",
                    job->session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  if (const auto ret{job->session->context->device->v1_library.job_get_results(
          job->job_v1, V1_JOB_RESULT::HIST_VALUES, size_values, values,
          size_values_ret)};
      ret != SUCCESS) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR, "Failed to get job results ({})",
                    job->session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_job_get_state_vector_dense
auto Job_get_state_vector_dense(DeviceJob *job, const size_t index,
                                const size_t size, double *value,
                                size_t *size_ret) -> int {
  if (job == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  job->logger.log(QDMI_LOG_LEVEL_INFO, "Getting the state vector of the job",
                  job->session->context->device->id);
  if (index > 0) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR, "Result index out of range",
                    job->session->context->device->id);
    return QDMI_ERROR_OUTOFRANGE;
  }
  if (const auto ret{job->session->context->device->v1_library.job_get_results(
          job->job_v1, V1_JOB_RESULT::STATEVECTOR_DENSE, size, value,
          size_ret)};
      ret != SUCCESS) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR, "Failed to get job results ({})",
                    job->session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_job_get_probabilities_dense
auto Job_get_probabilities_dense(DeviceJob *job, const size_t index,
                                 const size_t size, double *value,
                                 size_t *size_ret) -> int {
  if (job == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  job->logger.log(QDMI_LOG_LEVEL_INFO, "Getting the probabilities of the job",
                  job->session->context->device->id);
  if (index > 0) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR, "Result index out of range",
                    job->session->context->device->id);
    return QDMI_ERROR_OUTOFRANGE;
  }
  if (const auto ret{job->session->context->device->v1_library.job_get_results(
          job->job_v1, V1_JOB_RESULT::PROBABILITIES_DENSE, size, value,
          size_ret)};
      ret != SUCCESS) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR, "Failed to get job results ({})",
                    job->session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_job_get_state_vector_sparse
auto Job_get_state_vector_sparse(DeviceJob *job, const size_t index,
                                 const size_t size_keys, char *keys,
                                 size_t *size_keys_ret,
                                 const size_t size_values, double *values,
                                 size_t *size_values_ret) -> int {
  if (job == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  job->logger.log(QDMI_LOG_LEVEL_INFO, "Getting the state vector of the job",
                  job->session->context->device->id);
  if (index > 0) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR, "Result index out of range",
                    job->session->context->device->id);
    return QDMI_ERROR_OUTOFRANGE;
  }
  if (const auto ret{job->session->context->device->v1_library.job_get_results(
          job->job_v1, V1_JOB_RESULT::STATEVECTOR_SPARSE_KEYS, size_keys, keys,
          size_keys_ret)};
      ret != SUCCESS) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR, "Failed to get job results ({})",
                    job->session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  if (const auto ret{job->session->context->device->v1_library.job_get_results(
          job->job_v1, V1_JOB_RESULT::STATEVECTOR_SPARSE_VALUES, size_values,
          values, size_values_ret)};
      ret != SUCCESS) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR, "Failed to get job results ({})",
                    job->session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_job_get_probabilities_sparse
auto Job_get_probabilities_sparse(DeviceJob *job, const size_t index,
                                  const size_t size_keys, char *keys,
                                  size_t *size_keys_ret,
                                  const size_t size_values, double *values,
                                  size_t *size_values_ret) -> int {
  if (job == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  job->logger.log(QDMI_LOG_LEVEL_INFO, "Getting the probabilities of the job",
                  job->session->context->device->id);
  if (index > 0) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR, "Result index out of range",
                    job->session->context->device->id);
    return QDMI_ERROR_OUTOFRANGE;
  }
  if (const auto ret{job->session->context->device->v1_library.job_get_results(
          job->job_v1, V1_JOB_RESULT::PROBABILITIES_SPARSE_KEYS, size_keys,
          keys, size_keys_ret)};
      ret != SUCCESS) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR, "Failed to get job results ({})",
                    job->session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  if (const auto ret{job->session->context->device->v1_library.job_get_results(
          job->job_v1, V1_JOB_RESULT::PROBABILITIES_SPARSE_VALUES, size_values,
          values, size_values_ret)};
      ret != SUCCESS) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR, "Failed to get job results ({})",
                    job->session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_job_free
auto Job_free(DeviceJob *job) -> void {
  if (job != nullptr) {
    job->logger.log(QDMI_LOG_LEVEL_INFO, "[{}] Freeing the job",
                    job->session->context->device->id);
    job->session->context->device->v1_library.job_free(job->job_v1);
    job->session->context->device->jobs_by_id.erase(job->id);
    job->session->context->device->jobs.erase(job);
  }
}
} // namespace

auto Get_device_qpu_interface() -> AD_QDMI_QPU_Interface {
  return {.session_query_qpu_status = &Session_query_qpu_status,
          .session_query_qubit_count = &Session_query_qubit_count,
          .session_query_program_formats = &Session_query_program_formats,
          .session_query_program_format_by_id_and_version =
              &Session_query_program_format_by_id_and_version,
          .session_query_program_format_id = &Session_query_program_format_id,
          .session_query_program_format_string_support =
              &Session_query_program_format_string_support,
          .session_query_program_format_binary_support =
              &Session_query_program_format_binary_support,
          .session_query_program_format_version =
              &Session_query_program_format_version,
          .session_create_job = &Session_create_job,
          .session_retrieve_job_by_id = &Session_retrieve_job_by_id,
          .job_set_payload_string = &Job_set_payload_string,
          .job_set_payload_binary = &Job_set_payload_binary,
          .job_set_shot_count = &Job_set_shot_count,
          .job_set_log_callback = &Job_set_log_callback,
          .job_query_id = &Job_query_id,
          .job_query_payload_program_format = &Job_query_payload_program_format,
          .job_query_payload_string = &Job_query_payload_string,
          .job_query_payload_binary = &Job_query_payload_binary,
          .job_query_shot_count = &Job_query_shot_count,
          .job_submit = &Job_submit,
          .job_cancel = &Job_cancel,
          .job_check = &Job_check,
          .job_wait = &Job_wait,
          .job_get_shots = &Job_get_shots,
          .job_get_histogram = &Job_get_histogram,
          .job_get_state_vector_dense = &Job_get_state_vector_dense,
          .job_get_probabilities_dense = &Job_get_probabilities_dense,
          .job_get_state_vector_sparse = &Job_get_state_vector_sparse,
          .job_get_probabilities_sparse = &Job_get_probabilities_sparse,
          .job_free = &Job_free};
}
} // namespace qdmi::adapter

// NOLINTEND(misc-include-cleaner)
