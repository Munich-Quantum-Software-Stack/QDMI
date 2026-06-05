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

#include "orchestration_layer/orchestration_layer_bindings.hpp"

#include "o_qdmi/core.h"
#include "o_qdmi/orchestration_layer.h"
#include "od_qdmi/core.h"
#include "orchestration_layer/orchestration_layer.hpp"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <map>
#include <memory>
#include <ranges>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <variant>
#include <vector>

// NOLINTBEGIN(misc-include-cleaner)

namespace qdmi::orchestration_layer {
namespace {
template <class... Ts> struct overloaded : Ts... {
  using Ts::operator()...;
};
/// @copydoc O_QDMI_session_query_program_formats
auto Session_query_program_formats(Session *session, const size_t size,
                                   Format **value, size_t *size_ret) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID
                      "] Querying the supported program formats");
  if (session->status != Session::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Session is not initialized");
    return QDMI_ERROR_BADSTATE;
  }
  if (value != nullptr) {
    if (size < OrchestrationLayer::get().formats().size()) {
      session->logger.log(QDMI_LOG_LEVEL_ERROR,
                          "[" QDMI_DEVICE_ID
                          "] Provided buffer size is too small");
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    std::ranges::copy(OrchestrationLayer::get().formats(), value);
  }
  if (size_ret != nullptr) {
    *size_ret = OrchestrationLayer::get().formats().size();
  }
  return QDMI_SUCCESS;
}
/// @copydoc O_QDMI_session_query_program_format_by_id_and_version
auto Session_query_program_format_by_id_and_version(Session *session,
                                                    const char *id,
                                                    const size_t version,
                                                    Format **format) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID "] Querying the program format with "
                      "the specified ID and version");
  if (session->status != Session::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Session is not initialized");
    return QDMI_ERROR_BADSTATE;
  }
  if (id == nullptr || version == 0) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Invalid arguments");
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (auto *f = OrchestrationLayer::get().format_by_id_and_version(id, version);
      f != nullptr) {
    if (format != nullptr) {
      *format = f;
    }
  } else {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Program format not found: ");
    return QDMI_ERROR_NOTFOUND;
  }
  return QDMI_SUCCESS;
}
/// @copydoc O_QDMI_session_query_program_format_id
auto Session_query_program_format_id(Session *session, Format *format,
                                     const size_t size, char *value,
                                     size_t *size_ret) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID
                      "] Querying the ID of the program format");
  if (session->status != Session::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Session is not initialized");
    return QDMI_ERROR_BADSTATE;
  }
  const auto &id = format->id;
  if (value != nullptr) {
    if (size < id.size() + 1) {
      session->logger.log(QDMI_LOG_LEVEL_ERROR,
                          "[" QDMI_DEVICE_ID
                          "] Provided buffer size is too small");
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
/// @copydoc O_QDMI_session_query_program_format_string_support
auto Session_query_program_format_string_support(Session *session,
                                                 Format *format) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID "] Querying whether the program "
                      "format supports string payloads");
  if (session->status != Session::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Session is not initialized");
    return QDMI_ERROR_BADSTATE;
  }
  if (format == nullptr) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Program format is not specified");
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  return format->string_support ? QDMI_SUCCESS : QDMI_ERROR_NOTSUPPORTED;
}
/// @copydoc O_QDMI_session_query_program_format_binary_support
auto Session_query_program_format_binary_support(Session *session,
                                                 Format *format) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID "] Querying whether the program "
                      "format supports binary payloads");
  if (session->status != Session::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Session is not initialized");
    return QDMI_ERROR_BADSTATE;
  }
  if (format == nullptr) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Program format is not specified");
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  return format->binary_support ? QDMI_SUCCESS : QDMI_ERROR_NOTSUPPORTED;
}
/// @copydoc O_QDMI_session_query_program_format_version
auto Session_query_program_format_version(Session *session, Format *format,
                                          size_t *value) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID
                      "] Querying the version of the program format");
  if (session->status != Session::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Session is not initialized");
    return QDMI_ERROR_BADSTATE;
  }
  if (format == nullptr) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Program format is not specified");
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (value != nullptr) {
    *value = format->version;
  }
  return QDMI_SUCCESS;
}
/// @copydoc O_QDMI_session_create_job
auto Session_create_job(Session *session, const QDMI_Log_Callback callback,
                        void *user_data, Job **job) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID "] Creating a new job");
  if ((callback == nullptr && user_data != nullptr) || job == nullptr) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Invalid arguments");
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (session->status != Session::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Session is not initialized");
    return QDMI_ERROR_BADSTATE;
  }
  auto logger =
      callback != nullptr ? Logger{callback, user_data} : session->logger;
  *job = OrchestrationLayer::get().create_new_job(*session, logger);
  return QDMI_SUCCESS;
}
/// @copydoc O_QDMI_session_retrieve_job_by_id
auto Session_retrieve_job_by_id(Session *session,
                                const QDMI_Log_Callback callback,
                                void *user_data, const char *id, Job **job)
    -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID "] Retrieving a job by its ID");
  if ((callback == nullptr && user_data != nullptr) || id == nullptr ||
      job == nullptr) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Invalid arguments");
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (session->status != Session::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Session is not initialized");
    return QDMI_ERROR_BADSTATE;
  }
  auto logger =
      callback != nullptr ? Logger{callback, user_data} : session->logger;
  if (*job = OrchestrationLayer::get().retrieve_job_by_id(id, *session, logger);
      *job == nullptr) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Job not found.");
    return QDMI_ERROR_NOTFOUND;
  }
  return QDMI_SUCCESS;
}
/// @copydoc O_QDMI_session_query_devices
auto Session_query_devices(Session *session, size_t size, Device **value,
                           size_t *size_ret) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID "] Querying available devices");
  if (session->status != Session::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Session is not initialized");
    return QDMI_ERROR_BADSTATE;
  }
  const auto &devices = OrchestrationLayer::get().devices();
  if (value != nullptr) {
    if (size < devices.size()) {
      session->logger.log(QDMI_LOG_LEVEL_ERROR,
                          "[" QDMI_DEVICE_ID "] Buffer too small");
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    std::ranges::copy(devices, value);
  }
  if (size_ret != nullptr) {
    *size_ret = devices.size();
  }
  return QDMI_SUCCESS;
}
/// @copydoc O_QDMI_session_query_device_by_id
auto Session_query_device_by_id(Session *session, const char *id,
                                Device **device) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID "] Querying a device by its ID");
  if (session->status != Session::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Session is not initialized");
    return QDMI_ERROR_BADSTATE;
  }
  if (id == nullptr || device == nullptr) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Invalid arguments");
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (*device = OrchestrationLayer::get().device_by_id(id);
      *device == nullptr) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Device not found.");
    return QDMI_ERROR_NOTFOUND;
  }
  return QDMI_SUCCESS;
}
/// @copydoc O_QDMI_session_get_device_interface
auto Session_get_device_interface(Session *session, Device *device,
                                  OD_QDMI_Core_Interface const **interface_ptr)
    -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID "] Getting the device interface");
  if (session->status != Session::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Session is not initialized");
    return QDMI_ERROR_BADSTATE;
  }
  if (device == nullptr || interface_ptr == nullptr) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Invalid arguments");
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  *interface_ptr = &OrchestrationLayer::get().device_core_interface();
  return QDMI_SUCCESS;
}
/// @copydoc O_QDMI_session_query_device_context
auto Session_query_device_context(Session *session, Device *device,
                                  OD_QDMI_Context *context) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID "] Querying a device context");
  if (session->status != Session::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Session is not initialized");
    return QDMI_ERROR_BADSTATE;
  }
  if (device == nullptr || context == nullptr) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Invalid arguments");
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  auto context_uptr{std::make_unique<DeviceContext>(device, session->logger)};
  auto *context_ptr =
      device->contexts.emplace(context_uptr.get(), std::move(context_uptr))
          .first->second.get();
  *context = context_ptr;
  return QDMI_SUCCESS;
}
/// @copydoc O_QDMI_job_set_device
auto Job_set_device(Job *job, Device *device) -> int {
  if (job == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  job->logger.log(QDMI_LOG_LEVEL_INFO,
                  "[" QDMI_DEVICE_ID "] Setting the job's target device");
  if (device == nullptr) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR,
                    "[" QDMI_DEVICE_ID "] Device is null");
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (job->status != QDMI_JOB_STATUS_CREATED) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR,
                    "[" QDMI_DEVICE_ID
                    "] Cannot set device in the current job status");
    return QDMI_ERROR_BADSTATE;
  }
  job->device = device;
  return QDMI_SUCCESS;
}
/// @copydoc O_QDMI_job_set_payload_string
auto Job_set_payload_string(Job *job, Format *format, const size_t count,
                            const char *const *values) -> int {
  if (job == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  job->logger.log(QDMI_LOG_LEVEL_INFO,
                  "[" QDMI_DEVICE_ID "] Setting the job payload as strings");
  if (format == nullptr || count == 0) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR,
                    "[" QDMI_DEVICE_ID "] Invalid format or count");
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (job->status != QDMI_JOB_STATUS_CREATED) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR,
                    "[" QDMI_DEVICE_ID
                    "] Cannot set payload in the current job status");
    return QDMI_ERROR_BADSTATE;
  }
  if (!format->string_support) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR,
                    "[" QDMI_DEVICE_ID
                    "] Payload format does not support strings");
    return QDMI_ERROR_NOTSUPPORTED;
  }
  job->format = format;
  std::vector<std::string> payload{};
  payload.reserve(count);
  std::ranges::transform(std::span(values, count), std::back_inserter(payload),
                         [](const char *v) -> std::string { return {v}; });
  job->payload = std::move(payload);
  return QDMI_SUCCESS;
}
/// @copydoc O_QDMI_job_set_payload_binary
auto Job_set_payload_binary(Job *job, Format *format, const size_t count,
                            const size_t *sizes, const void *const *values)
    -> int {
  if (job == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  job->logger.log(QDMI_LOG_LEVEL_INFO,
                  "[" QDMI_DEVICE_ID
                  "] Setting the job payload as binary data");
  if (format == nullptr || count == 0) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR,
                    "[" QDMI_DEVICE_ID "] Invalid format or count");
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (job->status != QDMI_JOB_STATUS_CREATED) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR,
                    "[" QDMI_DEVICE_ID
                    "] Cannot set payload in the current job status");
    return QDMI_ERROR_BADSTATE;
  }
  if (!format->binary_support) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR,
                    "[" QDMI_DEVICE_ID
                    "] Payload format does not support binary data");
    return QDMI_ERROR_NOTSUPPORTED;
  }
  job->format = format;
  std::vector<std::vector<std::byte>> payload{};
  payload.reserve(count);
  std::ranges::transform(
      std::span(values, count), std::span(sizes, count),
      std::back_inserter(payload),
      [](const void *v, const size_t s) -> std::vector<std::byte> {
        const std::span source(static_cast<const std::byte *>(v), s);
        std::vector<std::byte> binary_data;
        binary_data.reserve(source.size());
        std::ranges::copy(source, std::back_inserter(binary_data));
        return binary_data;
      });
  job->payload = std::move(payload);
  return QDMI_SUCCESS;
}
/// @copydoc O_QDMI_job_set_shot_count
auto Job_set_shot_count(Job *job, const size_t value) -> int {
  if (job == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  job->logger.log(QDMI_LOG_LEVEL_INFO,
                  "[" QDMI_DEVICE_ID "] Setting the job shot count");
  if (value == 0) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR,
                    "[" QDMI_DEVICE_ID "] Shot count is zero");
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (job->status != QDMI_JOB_STATUS_CREATED) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR,
                    "[" QDMI_DEVICE_ID "] Cannot set shot count in the "
                    "current job status");
    return QDMI_ERROR_BADSTATE;
  }
  job->shot_count = value;
  return QDMI_SUCCESS;
}
/// @copydoc O_QDMI_job_set_log_callback
auto Job_set_log_callback(Job *job, const QDMI_Log_Callback callback,
                          void *user_data) -> int {
  if (job == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  job->logger.log(QDMI_LOG_LEVEL_INFO,
                  "[" QDMI_DEVICE_ID "] Setting the job log callback");
  if (callback == nullptr && user_data != nullptr) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR,
                    "[" QDMI_DEVICE_ID "] Invalid log callback or user data");
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  job->logger =
      callback != nullptr ? Logger{callback, user_data} : job->session->logger;
  return QDMI_SUCCESS;
}
/// @copydoc O_QDMI_job_query_id
auto Job_query_id(Job *job, const size_t size, char *value, size_t *size_ret)
    -> int {
  if (job == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  job->logger.log(QDMI_LOG_LEVEL_INFO,
                  "[" QDMI_DEVICE_ID "] Querying the job ID");
  const auto &id = job->id;
  if (value != nullptr) {
    if (size < id.size() + 1) {
      job->logger.log(QDMI_LOG_LEVEL_ERROR,
                      "[" QDMI_DEVICE_ID "] Buffer too small");
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
/// @copydoc O_QDMI_job_query_payload_program_format
auto Job_query_payload_program_format(Job *job, Format **format) -> int {
  if (job == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  job->logger.log(QDMI_LOG_LEVEL_INFO,
                  "[" QDMI_DEVICE_ID
                  "] Querying the program format of the job payload");
  if (format == nullptr) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR,
                    "[" QDMI_DEVICE_ID "] Format pointer is null");
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  *format = job->format;
  return QDMI_SUCCESS;
}
/// @copydoc O_QDMI_job_query_payload_string
auto Job_query_payload_string(Job *job, const size_t index, const size_t size,
                              char *value, size_t *size_ret) -> int {
  if (job == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  job->logger.log(QDMI_LOG_LEVEL_INFO,
                  "[" QDMI_DEVICE_ID "] Querying the job payload as strings");
  return std::visit(
      overloaded{[&job, index, size, value,
                  size_ret](const std::vector<std::string> &payload) -> int {
                   if (index >= payload.size()) {
                     job->logger.log(QDMI_LOG_LEVEL_ERROR,
                                     "[" QDMI_DEVICE_ID
                                     "] Payload index out of range");
                     return QDMI_ERROR_OUTOFRANGE;
                   }
                   const auto &program = payload.at(index);
                   if (value != nullptr) {
                     if (size < program.size() + 1) {
                       job->logger.log(QDMI_LOG_LEVEL_ERROR,
                                       "[" QDMI_DEVICE_ID "] Buffer too small");
                       return QDMI_ERROR_INVALIDARGUMENT;
                     }
                     std::ranges::copy(program, value);
                     /* Ensure null-termination */
                     /* NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                      */
                     value[program.size()] = '\0';
                   }
                   if (size_ret != nullptr) {
                     *size_ret = program.size() + 1;
                   }
                   return QDMI_SUCCESS;
                 },
                 [&job](const std::vector<std::vector<std::byte>> &) -> int {
                   job->logger.log(QDMI_LOG_LEVEL_ERROR,
                                   "[" QDMI_DEVICE_ID
                                   "] Job payload is not in string format");
                   return QDMI_ERROR_NOTSUPPORTED;
                 }},
      job->payload);
}
/// @copydoc O_QDMI_job_query_payload_binary
auto Job_query_payload_binary(Job *job, const size_t index, const size_t size,
                              void *value, size_t *size_ret) -> int {
  if (job == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  job->logger.log(QDMI_LOG_LEVEL_INFO,
                  "[" QDMI_DEVICE_ID
                  "] Querying the job payload as binary data");
  return std::visit(
      overloaded{
          [&job, index, size, value, size_ret](
              const std::vector<std::vector<std::byte>> &payload) -> int {
            if (index >= payload.size()) {
              job->logger.log(QDMI_LOG_LEVEL_ERROR,
                              "[" QDMI_DEVICE_ID
                              "] Payload index out of range");
              return QDMI_ERROR_OUTOFRANGE;
            }
            const auto &program = payload.at(index);
            if (value != nullptr) {
              if (size < program.size() + 1) {
                job->logger.log(QDMI_LOG_LEVEL_ERROR,
                                "[" QDMI_DEVICE_ID "] Buffer too small");
                return QDMI_ERROR_INVALIDARGUMENT;
              }
              std::ranges::copy(program, static_cast<std::byte *>(value));
            }
            if (size_ret != nullptr) {
              *size_ret = program.size() + 1;
            }
            return QDMI_SUCCESS;
          },
          [&job](const std::vector<std::string> &) -> int {
            job->logger.log(QDMI_LOG_LEVEL_ERROR,
                            "[" QDMI_DEVICE_ID
                            "] Job payload is not in binary format");
            return QDMI_ERROR_NOTSUPPORTED;
          }},
      job->payload);
}
/// @copydoc O_QDMI_job_query_shot_count
auto Job_query_shot_count(Job *job, size_t *value) -> int {
  if (job == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  job->logger.log(QDMI_LOG_LEVEL_INFO,
                  "[" QDMI_DEVICE_ID "] Querying the job shot count");
  if (value != nullptr) {
    *value = job->shot_count;
  }
  return QDMI_SUCCESS;
}
/// @copydoc O_QDMI_job_submit
auto Job_submit(Job *job) -> int {
  if (job == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  job->logger.log(QDMI_LOG_LEVEL_INFO,
                  "[" QDMI_DEVICE_ID "] Submitting the job");
  if (job->status != QDMI_JOB_STATUS_CREATED) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR,
                    "[" QDMI_DEVICE_ID
                    "] Job cannot be submitted in its current status");
    return QDMI_ERROR_BADSTATE;
  }
  if (std::visit([](const auto &programs) { return programs.empty(); },
                 job->payload) ||
      job->format == nullptr || job->shot_count == 0 ||
      job->device == nullptr) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR,
                    "[" QDMI_DEVICE_ID
                    "] Job is missing required fields (payload, format, shot "
                    "count, or device)");
    return QDMI_ERROR_BADSTATE;
  }
  try {
    OrchestrationLayer::get().submit_job(*job);
  } catch (const std::runtime_error &e) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR,
                    "[" QDMI_DEVICE_ID "] Failed to submit job: {}", e.what());
    return QDMI_ERROR_FATAL;
  }
  return QDMI_SUCCESS;
}
/// @copydoc O_QDMI_job_cancel
auto Job_cancel(Job *job) -> int {
  if (job == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  job->logger.log(QDMI_LOG_LEVEL_INFO,
                  "[" QDMI_DEVICE_ID "] Cancelling the job");
  if (job->status != QDMI_JOB_STATUS_SUBMITTED &&
      job->status != QDMI_JOB_STATUS_QUEUED &&
      job->status != QDMI_JOB_STATUS_RUNNING) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR,
                    "[" QDMI_DEVICE_ID
                    "] Job cannot be canceled in its current status");
    return QDMI_ERROR_BADSTATE;
  }
  job->status = QDMI_JOB_STATUS_CANCELED;
  return QDMI_SUCCESS;
}
/// @copydoc O_QDMI_job_check
auto Job_check(Job *job, QDMI_Job_Status *status) -> int {
  if (job == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  job->logger.log(QDMI_LOG_LEVEL_INFO,
                  "[" QDMI_DEVICE_ID "] Checking the job status");
  if (status == nullptr) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR,
                    "[" QDMI_DEVICE_ID "] Status pointer is null");
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  *status = job->status;
  return QDMI_SUCCESS;
}
/// @copydoc O_QDMI_job_wait
auto Job_wait(Job *job, [[maybe_unused]] const size_t timeout) -> int {
  if (job == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  job->logger.log(QDMI_LOG_LEVEL_INFO,
                  "[" QDMI_DEVICE_ID "] Waiting for the job to complete");
  // return immediately because the example implementation does not support
  // asynchronous execution.
  if (job->status != QDMI_JOB_STATUS_DONE) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR,
                    "[" QDMI_DEVICE_ID "] Job status is not done");
    return QDMI_ERROR_BADSTATE;
  }
  return QDMI_SUCCESS;
}
/// @copydoc O_QDMI_job_get_shots
auto Job_get_shots(Job *job, const size_t index, const size_t size, char *data,
                   size_t *size_ret) -> int {
  if (job == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  job->logger.log(QDMI_LOG_LEVEL_INFO,
                  "[" QDMI_DEVICE_ID "] Getting the shots of the job");
  const auto &results = job->results;
  if (index >= results.size()) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR,
                    "[" QDMI_DEVICE_ID "] Result index out of range");
    return QDMI_ERROR_OUTOFRANGE;
  }
  const auto &result = results.at(index);
  const size_t required_size = result.size() * (result.front().size() + 1);
  if (data != nullptr) {
    if (size < required_size) {
      job->logger.log(QDMI_LOG_LEVEL_ERROR,
                      "[" QDMI_DEVICE_ID "] Buffer too small");
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    for (auto it = result.begin(); it != result.end(); ++it) {
      data = std::ranges::copy(*it, data).out;
      if (std::next(it) != result.end()) {
        *data++ = ','; // Add comma separator
      } else {
        *data++ = '\0'; // Add null terminator at the end
      }
    }
  }
  if (size_ret != nullptr) {
    *size_ret = required_size;
  }
  return QDMI_SUCCESS;
}
/// @copydoc O_QDMI_job_get_histogram
auto Job_get_histogram(Job *job, const size_t index, const size_t size_keys,
                       char *keys, size_t *size_keys_ret,
                       const size_t size_values, size_t *values,
                       size_t *size_values_ret) -> int {
  if (job == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  job->logger.log(QDMI_LOG_LEVEL_INFO,
                  "[" QDMI_DEVICE_ID "] Getting the histogram of the job");
  const auto &results = job->results;
  if (index >= results.size()) {
    job->logger.log(QDMI_LOG_LEVEL_ERROR,
                    "[" QDMI_DEVICE_ID "] Result index out of range");
    return QDMI_ERROR_OUTOFRANGE;
  }
  const auto &result = results.at(index);
  // Count unique elements
  std::map<std::string, size_t> hist;
  for (const auto &shot : result) {
    hist[shot]++;
  }
  const size_t required_keys_size =
      hist.size() * (result.empty() ? 0 : (result.front().size() + 1));
  const size_t required_values_size = hist.size();
  if (keys != nullptr) {
    if (size_keys < required_keys_size) {
      job->logger.log(QDMI_LOG_LEVEL_ERROR,
                      "[" QDMI_DEVICE_ID "] Buffer too small");
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    for (const auto &bitstring : hist | std::views::keys) {
      std::ranges::copy(bitstring, keys);
      keys += bitstring.size();
      *keys++ = ',';
    }
    *(keys - 1) = '\0'; // Replace last comma with null terminator
  }
  if (values != nullptr) {
    if (size_values < required_values_size) {
      job->logger.log(QDMI_LOG_LEVEL_ERROR,
                      "[" QDMI_DEVICE_ID "] Buffer too small");
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    for (const auto &count : hist | std::views::values) {
      *values++ = count;
    }
  }
  if (size_keys_ret != nullptr) {
    *size_keys_ret = required_keys_size;
  }
  if (size_values_ret != nullptr) {
    *size_values_ret = required_values_size;
  }
  return QDMI_SUCCESS;
}
/// @copydoc O_QDMI_job_get_state_vector_dense
auto Job_get_state_vector_dense(Job *job, [[maybe_unused]] const size_t index,
                                [[maybe_unused]] const size_t size,
                                [[maybe_unused]] double *value,
                                [[maybe_unused]] size_t *size_ret) -> int {
  if (job == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  job->logger.log(QDMI_LOG_LEVEL_INFO,
                  "[" QDMI_DEVICE_ID "] Getting the state vector of the job");
  job->logger.log(QDMI_LOG_LEVEL_ERROR,
                  "[" QDMI_DEVICE_ID "] Dense state vector retrieval is not "
                  "supported in this example implementation");
  return QDMI_ERROR_NOTSUPPORTED;
}
/// @copydoc O_QDMI_job_get_probabilities_dense
auto Job_get_probabilities_dense(Job *job, [[maybe_unused]] const size_t index,
                                 [[maybe_unused]] const size_t size,
                                 [[maybe_unused]] double *value,
                                 [[maybe_unused]] size_t *size_ret) -> int {
  if (job == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  job->logger.log(QDMI_LOG_LEVEL_INFO, "Getting the probabilities of the job");
  job->logger.log(QDMI_LOG_LEVEL_ERROR,
                  "[" QDMI_DEVICE_ID "] Dense probability retrieval is not "
                  "supported in this example implementation");
  return QDMI_ERROR_NOTSUPPORTED;
}
/// @copydoc O_QDMI_job_get_state_vector_sparse
auto Job_get_state_vector_sparse(Job *job, [[maybe_unused]] const size_t index,
                                 [[maybe_unused]] const size_t size_keys,
                                 [[maybe_unused]] char *keys,
                                 [[maybe_unused]] size_t *size_keys_ret,
                                 [[maybe_unused]] const size_t size_values,
                                 [[maybe_unused]] double *values,
                                 [[maybe_unused]] size_t *size_values_ret)
    -> int {
  if (job == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  job->logger.log(QDMI_LOG_LEVEL_INFO,
                  "[" QDMI_DEVICE_ID "] Getting the state vector of the job");
  job->logger.log(QDMI_LOG_LEVEL_ERROR,
                  "[" QDMI_DEVICE_ID "] Sparse state vector retrieval is not "
                  "supported in this example implementation");
  return QDMI_ERROR_NOTSUPPORTED;
}
/// @copydoc O_QDMI_job_get_probabilities_sparse
auto Job_get_probabilities_sparse(Job *job, [[maybe_unused]] const size_t index,
                                  [[maybe_unused]] const size_t size_keys,
                                  [[maybe_unused]] char *keys,
                                  [[maybe_unused]] size_t *size_keys_ret,
                                  [[maybe_unused]] const size_t size_values,
                                  [[maybe_unused]] double *values,
                                  [[maybe_unused]] size_t *size_values_ret)
    -> int {
  if (job == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  job->logger.log(QDMI_LOG_LEVEL_INFO,
                  "[" QDMI_DEVICE_ID "] Getting the probabilities of the job");
  job->logger.log(QDMI_LOG_LEVEL_ERROR,
                  "[" QDMI_DEVICE_ID "] Sparse probability retrieval is not "
                  "supported in this example implementation");
  return QDMI_ERROR_NOTSUPPORTED;
}
/// @copydoc O_QDMI_job_free
auto Job_free(Job *job) -> void {
  if (job != nullptr) {
    job->logger.log(QDMI_LOG_LEVEL_INFO,
                    "[" QDMI_DEVICE_ID "] Freeing the job");
    OrchestrationLayer::get().remove_job(*job);
  }
}
} // namespace

auto Get_orchestration_layer_interface()
    -> const O_QDMI_OrchestrationLayer_Interface & {
  static constexpr O_QDMI_OrchestrationLayer_Interface INTERFACE{
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
      .session_query_devices = &Session_query_devices,
      .session_query_device_by_id = &Session_query_device_by_id,
      .session_get_device_interface = &Session_get_device_interface,
      .session_query_device_context = &Session_query_device_context,
      .job_set_device = &Job_set_device,
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
  return INTERFACE;
}
} // namespace qdmi::orchestration_layer

// NOLINTEND(misc-include-cleaner)
