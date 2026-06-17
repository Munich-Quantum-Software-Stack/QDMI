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

#include "orchestration_layer/device_core_bindings.hpp"

#include "od_qdmi/core.h"
#include "orchestration_layer/orchestration_layer.hpp"

#include <algorithm>
#include <cstddef>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <variant>

// NOLINTBEGIN(misc-include-cleaner)

namespace qdmi::orchestration_layer {
namespace {
/// @copydoc OD_QDMI_context_query_id
auto Context_query_id(DeviceContext *context, const size_t size, char *value,
                      size_t *size_ret) -> int {
  if (context == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  context->logger.log(QDMI_LOG_LEVEL_INFO, "[{}] Querying the device's ID",
                      context->device->id);
  const auto &id = context->device->id;
  if (value != nullptr) {
    if (size < id.size() + 1) {
      context->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Buffer too small",
                          context->device->id);
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
/// @copydoc OD_QDMI_context_query_name
auto Context_query_name(DeviceContext *context, const size_t size, char *value,
                        size_t *size_ret) -> int {
  if (context == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  context->logger.log(QDMI_LOG_LEVEL_INFO, "[{}] Querying the device's Name",
                      context->device->id);
  if (const auto ret = context->device->core_interface->context_query_name(
          context->device->context, size, value, size_ret);
      ret != QDMI_SUCCESS) {
    context->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Failed to query the device's name: {}",
                        context->device->id, ret);
    return ret;
  }
  return QDMI_SUCCESS;
}
/// @copydoc OD_QDMI_context_query_version
auto Context_query_version(DeviceContext *context, size_t *value) -> int {
  if (context == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  context->logger.log(QDMI_LOG_LEVEL_INFO, "[{}] Querying the device's version",
                      context->device->id);
  if (const auto ret = context->device->core_interface->context_query_version(
          context->device->context, value);
      ret != QDMI_SUCCESS) {
    context->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Failed to query the Device's version: {}",
                        context->device->id, ret);
    return ret;
  }
  return QDMI_SUCCESS;
}
/// @copydoc OD_QDMI_context_query_authentication_options
auto Context_query_authentication_options(DeviceContext *context,
                                          const size_t size,
                                          QDMI_Authentication_Option *value,
                                          size_t *size_ret) -> int {
  if (context == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  context->logger.log(
      QDMI_LOG_LEVEL_INFO,

      "[{}] Querying the OrchestrationLayer's authentication options",
      context->device->id);
  if (value != nullptr) {
    if (size < 1) {
      context->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Buffer too small",
                          context->device->id);
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    *value = QDMI_AUTHENTICATION_OPTION_NONE;
  }
  if (size_ret != nullptr) {
    *size_ret = 1;
  }
  return QDMI_SUCCESS;
}
/// @copydoc OD_QDMI_context_query_modules
auto Context_query_modules(DeviceContext *context, const size_t size,
                           DeviceModule **values, size_t *size_ret) -> int {
  if (context == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  context->logger.log(QDMI_LOG_LEVEL_INFO, "[{}] Querying the device's modules",
                      context->device->id);
  const auto &modules = OrchestrationLayer::get().device_modules();
  if (values != nullptr) {
    if (size < modules.size()) {
      context->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Buffer too small",
                          context->device->id);
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    std::ranges::copy(modules, values);
  }
  if (size_ret != nullptr) {
    *size_ret = modules.size();
  }
  return QDMI_SUCCESS;
}
/// @copydoc OD_QDMI_context_query_module_by_id
auto Context_query_module_by_id(DeviceContext *context, const char *id,
                                DeviceModule **module) -> int {
  if (context == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  context->logger.log(QDMI_LOG_LEVEL_INFO, "[{}] Querying a module by ID",
                      context->device->id);
  if (id == nullptr || module == nullptr) {
    context->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Invalid arguments",
                        context->device->id);
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (*module = OrchestrationLayer::get().device_module_by_id(id);
      *module == nullptr) {
    context->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID
                        "] Device module with the specified ID not found");
    return QDMI_ERROR_NOTFOUND;
  }
  return QDMI_SUCCESS;
}
/// @copydoc OD_QDMI_context_query_module_id
auto Context_query_module_id(DeviceContext *context, DeviceModule *module,
                             const size_t size, char *value, size_t *size_ret)
    -> int {
  if (context == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  context->logger.log(QDMI_LOG_LEVEL_INFO, "[{}] Querying a module's ID",
                      context->device->id);
  if (module == nullptr) {
    context->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Invalid arguments",
                        context->device->id);
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (value != nullptr) {
    const auto &id = module->id;
    if (size < id.size() + 1) {
      context->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Buffer too small",
                          context->device->id);
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    std::ranges::copy(id, value);
    /* Ensure null-termination */
    /* NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic) */
    value[id.size()] = '\0';
  }
  if (size_ret != nullptr) {
    *size_ret = module->id.size() + 1;
  }
  return QDMI_SUCCESS;
}
/// @copydoc OD_QDMI_context_get_module_interface
auto Context_get_module_interface(DeviceContext *context, DeviceModule *module,
                                  const void **interface_ptr) -> int {
  if (context == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  context->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[{}] Getting the module's interface",
                      context->device->id);
  if (module == nullptr || interface_ptr == nullptr) {
    context->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Invalid arguments",
                        context->device->id);
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  *interface_ptr = std::visit(
      [](const auto *interface) -> const void * {
        return static_cast<const void *>(interface);
      },
      module->interface);
  return QDMI_SUCCESS;
}
/// @copydoc OD_QDMI_context_allocate_session
auto Context_allocate_session(DeviceContext *context,
                              const QDMI_Log_Callback callback, void *user_data,
                              DeviceSession **session) -> int {
  if (context == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  context->logger.log(QDMI_LOG_LEVEL_INFO, "[{}] Allocating a session",
                      context->device->id);
  if ((callback == nullptr && user_data != nullptr) || session == nullptr) {
    context->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Invalid arguments",
                        context->device->id);
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  auto logger =
      callback != nullptr ? Logger{callback, user_data} : context->logger;
  auto session_ptr = std::make_unique<DeviceSession>(context, logger);
  *session = context->device->sessions
                 .emplace(session_ptr.get(), std::move(session_ptr))
                 .first->first;
  return QDMI_SUCCESS;
}
/// @copydoc OD_QDMI_session_set_token
auto Session_set_token(DeviceSession *session,
                       [[maybe_unused]] const char *value) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO, "[{}] Setting the session's token",
                      session->context->device->id);
  session->logger.log(QDMI_LOG_LEVEL_ERROR,
                      "[" QDMI_DEVICE_ID
                      "] Managed device does not require authentication");
  return QDMI_ERROR_NOTSUPPORTED;
}
/// @copydoc OD_QDMI_session_set_authentication_file
auto Session_set_authentication_file(DeviceSession *session,
                                     [[maybe_unused]] const char *value)
    -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID
                      "] Setting the session's authentication file");
  session->logger.log(QDMI_LOG_LEVEL_ERROR,
                      "[" QDMI_DEVICE_ID
                      "] Managed device does not require authentication");
  return QDMI_ERROR_NOTSUPPORTED;
}
/// @copydoc OD_QDMI_session_set_authentication_url
auto Session_set_authentication_url(DeviceSession *session,
                                    [[maybe_unused]] const char *value) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID
                      "] Setting the session's authentication URL");
  session->logger.log(QDMI_LOG_LEVEL_ERROR,
                      "[" QDMI_DEVICE_ID
                      "] Managed device does not require authentication");
  return QDMI_ERROR_NOTSUPPORTED;
}
/// @copydoc OD_QDMI_session_set_username
auto Session_set_username(DeviceSession *session,
                          [[maybe_unused]] const char *value) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[{}] Setting the session's username",
                      session->context->device->id);
  session->logger.log(QDMI_LOG_LEVEL_ERROR,
                      "[" QDMI_DEVICE_ID
                      "] Managed device does not require authentication");
  return QDMI_ERROR_NOTSUPPORTED;
}
/// @copydoc OD_QDMI_session_set_password
auto Session_set_password(DeviceSession *session,
                          [[maybe_unused]] const char *value) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[{}] Setting the session's password",
                      session->context->device->id);
  session->logger.log(QDMI_LOG_LEVEL_ERROR,
                      "[" QDMI_DEVICE_ID
                      "] Managed device does not require authentication");
  return QDMI_ERROR_NOTSUPPORTED;
}
/// @copydoc OD_QDMI_session_set_log_callback
auto Session_set_log_callback(DeviceSession *session,
                              const QDMI_Log_Callback callback, void *user_data)
    -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID
                      "] Setting the session's logging callback");
  if (callback == nullptr && user_data != nullptr) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID
                        "] User data provided without a callback function");
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger = Logger{callback, user_data};
  return QDMI_SUCCESS;
}
/// @copydoc OD_QDMI_session_initialize
auto Session_initialize(DeviceSession *session) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO, "[{}] Initializing the session",
                      session->context->device->id);
  if (session->status != DeviceSession::STATUS::ALLOCATED) {
    session->logger.log(
        QDMI_LOG_LEVEL_ERROR,
        "[{}] Session is in an invalid state for setting the token",
        session->context->device->id);
    return QDMI_ERROR_BADSTATE;
  }
  session->status = DeviceSession::STATUS::INITIALIZED;
  return QDMI_SUCCESS;
}
/// @copydoc OD_QDMI_session_free
auto Session_free(DeviceSession *session) -> void {
  if (session != nullptr) {
    session->logger.log(QDMI_LOG_LEVEL_INFO, "[{}] Freeing the session",
                        session->context->device->id);
    session->context->device->sessions.erase(session);
  }
}
} // namespace
auto Get_device_core_interface() -> OD_QDMI_Core_Interface {
  return {.context_query_id = &Context_query_id,
          .context_query_name = &Context_query_name,
          .context_query_version = &Context_query_version,
          .context_query_authentication_options =
              &Context_query_authentication_options,
          .context_query_modules = &Context_query_modules,
          .context_query_module_by_id = &Context_query_module_by_id,
          .context_query_module_id = &Context_query_module_id,
          .context_get_module_interface = &Context_get_module_interface,
          .context_allocate_session = &Context_allocate_session,
          .session_set_token = &Session_set_token,
          .session_set_authentication_file = &Session_set_authentication_file,
          .session_set_authentication_url = &Session_set_authentication_url,
          .session_set_username = &Session_set_username,
          .session_set_password = &Session_set_password,
          .session_set_log_callback = &Session_set_log_callback,
          .session_initialize = &Session_initialize,
          .session_free = &Session_free};
}
} // namespace qdmi::orchestration_layer

// NOLINTEND(misc-include-cleaner)
