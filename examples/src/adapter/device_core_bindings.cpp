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

#include "adapter/device_core_bindings.hpp"

#include "ad_qdmi/core.h"
#include "adapter/adapter.hpp"

#include <algorithm>
#include <cstddef>
#include <sstream>
#include <stdexcept>
#include <variant>

// NOLINTBEGIN(misc-include-cleaner)

namespace qdmi::adapter {
namespace {
/// @copydoc AD_QDMI_context_query_id
auto Context_query_id(DeviceContext *context, const size_t size, char *value,
                      size_t *size_ret) -> int {
  if (context == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  const auto &id{context->device->id};
  context->logger.log(QDMI_LOG_LEVEL_INFO, "[{}] Querying the QPU's ID", id);
  if (value != nullptr) {
    if (size < id.size() + 1) {
      context->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Buffer too small", id);
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
/// @copydoc AD_QDMI_context_query_name
auto Context_query_name(DeviceContext *context, const size_t size, char *value,
                        size_t *size_ret) -> int {
  if (context == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  context->logger.log(QDMI_LOG_LEVEL_INFO, "[{}] Querying the QPU's name",
                      context->device->id);
  const auto &name{context->device->name};
  if (value != nullptr) {
    if (size < name.size() + 1) {
      context->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Buffer too small",
                          context->device->id);
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    std::ranges::copy(name, value);
    /* Ensure null-termination */
    /* NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic) */
    value[name.size()] = '\0';
  }
  if (size_ret != nullptr) {
    *size_ret = name.size() + 1;
  }
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_context_query_version
auto Context_query_version(DeviceContext *context, size_t *value) -> int {
  if (context == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  context->logger.log(QDMI_LOG_LEVEL_INFO, "[{}] Querying the QPU's version",
                      context->device->id);
  if (value != nullptr) {
    *value = QDMI_VERSION;
  }
  return QDMI_SUCCESS;
}
auto Context_query_authentication_options(
    DeviceContext *context, [[maybe_unused]] const size_t size,
    [[maybe_unused]] QDMI_Authentication_Option *value,
    [[maybe_unused]] size_t *size_ret) -> int {
  if (context == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  context->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[{}] Querying the QPU's authentication options",
                      context->device->id);
  context->logger.log(QDMI_LOG_LEVEL_ERROR,
                      "[{}] Authentication is not supported on this QPU",
                      context->device->id);
  return QDMI_ERROR_NOTSUPPORTED;
}
auto Context_query_modules(DeviceContext *context, const size_t size,
                           DeviceModule **values, size_t *size_ret) -> int {
  if (context == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  context->logger.log(QDMI_LOG_LEVEL_INFO, "[{}] Querying the QPU's modules",
                      context->device->id);
  const auto &modules = Adapter::get().device_modules();
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
  if (*module = Adapter::get().device_module_by_id(id); *module == nullptr) {
    context->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Module with ID '{}' not found.",
                        context->device->id, id);
    return QDMI_ERROR_NOTFOUND;
  }
  return QDMI_SUCCESS;
}
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
/// @copydoc AD_QDMI_context_get_module_interface
auto Context_get_module_interface(DeviceContext *context, DeviceModule *module,
                                  const void **interface_ptr) -> int {
  if (context == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  context->logger.log(QDMI_LOG_LEVEL_INFO, "[{}] Querying a module's interface",
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
/// @copydoc AD_QDMI_context_allocate_session
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
  if (const auto ret{
          context->device->v1_library.session_alloc(&session_ptr->v1_session)};
      ret != SUCCESS) {
    context->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Failed to allocate a session ({})",
                        context->device->id, ret);
    return Transform_return_codes(ret);
  }
  *session = context->device->sessions
                 .emplace(session_ptr.get(), std::move(session_ptr))
                 .first->first;
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_session_set_token
auto Session_set_token(DeviceSession *session, const char *value) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO, "[{}] Setting the session's token",
                      session->context->device->id);
  if (session->status != DeviceSession::STATUS::ALLOCATED) {
    session->logger.log(
        QDMI_LOG_LEVEL_ERROR,
        "[{}] Session is in an invalid state for setting the token",
        session->context->device->id);
    return QDMI_ERROR_BADSTATE;
  }
  if (const auto ret{session->context->device->v1_library.session_set_parameter(
          session->v1_session, V1_SESSION_PARAMETER::TOKEN,
          value != nullptr ? std::strlen(value) + 1 : 0, value)};
      ret != SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Failed to set the session's token ({})",
                        session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_session_set_authentication_file
auto Session_set_authentication_file(DeviceSession *session, const char *value)
    -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[{}] Setting the session's authentication file",
                      session->context->device->id);
  if (session->status != DeviceSession::STATUS::ALLOCATED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Session is in an invalid state for setting the "
                        "authentication file",
                        session->context->device->id);
    return QDMI_ERROR_BADSTATE;
  }
  if (const auto ret{session->context->device->v1_library.session_set_parameter(
          session->v1_session, V1_SESSION_PARAMETER::AUTH_FILE,
          value != nullptr ? std::strlen(value) + 1 : 0, value)};
      ret != SUCCESS) {
    session->logger.log(
        QDMI_LOG_LEVEL_ERROR,
        "[{}] Failed to set the session's authentication file ({})",
        session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_session_set_authentication_url
auto Session_set_authentication_url(DeviceSession *session, const char *value)
    -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[{}] Setting the session's authentication URL",
                      session->context->device->id);
  if (session->status != DeviceSession::STATUS::ALLOCATED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Session is in an invalid state for setting the "
                        "authentication URL",
                        session->context->device->id);
    return QDMI_ERROR_BADSTATE;
  }
  if (const auto ret{session->context->device->v1_library.session_set_parameter(
          session->v1_session, V1_SESSION_PARAMETER::AUTH_URL,
          value != nullptr ? std::strlen(value) + 1 : 0, value)};
      ret != SUCCESS) {
    session->logger.log(
        QDMI_LOG_LEVEL_ERROR,
        "[{}] Failed to set the session's authentication URL ({})",
        session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_session_set_username
auto Session_set_username(DeviceSession *session, const char *value) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[{}] Setting the session's username",
                      session->context->device->id);
  if (session->status != DeviceSession::STATUS::ALLOCATED) {
    session->logger.log(
        QDMI_LOG_LEVEL_ERROR,
        "[{}] Session is in an invalid state for setting the username",
        session->context->device->id);
    return QDMI_ERROR_BADSTATE;
  }
  if (const auto ret{session->context->device->v1_library.session_set_parameter(
          session->v1_session, V1_SESSION_PARAMETER::USERNAME,
          value != nullptr ? std::strlen(value) + 1 : 0, value)};
      ret != SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Failed to set the session's username ({})",
                        session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_session_set_password
auto Session_set_password(DeviceSession *session, const char *value) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[{}] Setting the session's password",
                      session->context->device->id);
  if (session->status != DeviceSession::STATUS::ALLOCATED) {
    session->logger.log(
        QDMI_LOG_LEVEL_ERROR,
        "[{}] Session is in an invalid state for setting the password",
        session->context->device->id);
    return QDMI_ERROR_BADSTATE;
  }
  if (const auto ret{session->context->device->v1_library.session_set_parameter(
          session->v1_session, V1_SESSION_PARAMETER::PASSWORD,
          value != nullptr ? std::strlen(value) + 1 : 0, value)};
      ret != SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Failed to set the session's password ({})",
                        session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_session_set_log_callback
auto Session_set_log_callback(DeviceSession *session,
                              const QDMI_Log_Callback callback, void *user_data)
    -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[{}] Setting the session's logging callback",
                      session->context->device->id);
  if (callback == nullptr && user_data != nullptr) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] User data provided without a callback function",
                        session->context->device->id);
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger = Logger{callback, user_data};
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_session_initialize
auto Session_initialize(DeviceSession *session) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO, "[{}] Initializing the session",
                      session->context->device->id);
  if (session->status != DeviceSession::STATUS::ALLOCATED) {
    session->logger.log(
        QDMI_LOG_LEVEL_ERROR,
        "[{}] Session is in an invalid state for initialization",
        session->context->device->id);
    return QDMI_ERROR_BADSTATE;
  }
  if (const auto ret{session->context->device->v1_library.session_init(
          session->v1_session)};
      ret != SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Failed to initialize the session ({})",
                        session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  session->status = DeviceSession::STATUS::INITIALIZED;
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_session_free
auto Session_free(DeviceSession *session) -> void {
  if (session != nullptr) {
    session->logger.log(QDMI_LOG_LEVEL_INFO, "[{}] Freeing the session",
                        session->context->device->id);
    session->context->device->v1_library.session_free(session->v1_session);
    session->context->device->sessions.erase(session);
  }
}
} // namespace
auto Get_device_core_interface() -> AD_QDMI_Core_Interface {
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
} // namespace qdmi::adapter
// NOLINTEND(misc-include-cleaner)
