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

#include "adapter/provider_bindings.hpp"

#include "ad_qdmi/core.h"
#include "adapter/adapter.hpp"
#include "v2_qdmi/provider.h"

#include <algorithm>
#include <cstddef>
#include <sstream>
#include <stdexcept>

// NOLINTBEGIN(misc-include-cleaner)

namespace qdmi::adapter {
namespace {
/// @copydoc V2_QDMI_session_query_devices
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
  const auto &devices = Adapter::get().devices();
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
/// @copydoc V2_QDMI_session_query_device_by_id
auto Session_query_device_by_id(Session *session, const char *id,
                                Device **device) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID "] Querying device by ID: {}", id);
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
  if (*device = Adapter::get().device_by_id(id); *device == nullptr) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Device with ID '{}' not found.",
                        id);
    return QDMI_ERROR_NOTFOUND;
  }
  return QDMI_SUCCESS;
}
/// @copydoc V2_QDMI_session_get_device_interface
auto Session_get_device_interface(Session *session, Device *device,
                                  AD_QDMI_Core_Interface const **interface_ptr)
    -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID
                      "] Querying a device's core interface");
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
  *interface_ptr = &Adapter::get().device_core_interface();
  return QDMI_SUCCESS;
}
/// @copydoc V2_QDMI_session_query_device_context
auto Session_query_device_context(Session *session, Device *device,
                                  AD_QDMI_Context *context) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID "] Querying the device's context");
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
} // namespace

auto Get_provider_interface() -> V2_QDMI_Provider_Interface {
  return {.session_query_devices = &Session_query_devices,
          .session_query_device_by_id = &Session_query_device_by_id,
          .session_get_device_interface = &Session_get_device_interface,
          .session_query_device_context = &Session_query_device_context};
}
} // namespace qdmi::adapter

// NOLINTEND(misc-include-cleaner)
