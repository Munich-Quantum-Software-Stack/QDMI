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

#include "adapter/core_bindings.hpp"

#include "adapter/adapter.hpp"
#include "v2_qdmi/core.h"

#include <algorithm>
#include <cstddef>
#include <variant>

// NOLINTBEGIN(misc-include-cleaner)

namespace qdmi::adapter {
namespace {
/// @copydoc V2_QDMI_context_query_id
auto Context_query_id(Context *context, const size_t size, char *value,
                      size_t *size_ret) -> int {
  if (context == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  context->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID "] Querying the QPU's ID");
  const auto &id{Adapter::get().id()};
  if (value != nullptr) {
    if (size < id.size() + 1) {
      context->logger.log(QDMI_LOG_LEVEL_ERROR,
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
/// @copydoc V2_QDMI_context_query_name
auto Context_query_name(Context *context, const size_t size, char *value,
                        size_t *size_ret) -> int {
  if (context == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  context->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID "] Querying the QPU's name");
  const auto &name{Adapter::get().name()};
  if (value != nullptr) {
    if (size < name.size() + 1) {
      context->logger.log(QDMI_LOG_LEVEL_ERROR,
                          "[" QDMI_DEVICE_ID "] Buffer too small");
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
/// @copydoc V2_QDMI_context_query_version
auto Context_query_version(Context *context, size_t *value) -> int {
  if (context == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  context->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID "] Querying the QPU's version");
  if (value != nullptr) {
    *value = QDMI_VERSION;
  }
  return QDMI_SUCCESS;
}
auto Context_query_authentication_options(
    Context *context, [[maybe_unused]] const size_t size,
    [[maybe_unused]] QDMI_Authentication_Option *value,
    [[maybe_unused]] size_t *size_ret) -> int {
  if (context == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  context->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID
                      "] Querying the QPU's authentication options");
  const auto &options = Adapter::get().authentication_options();
  if (value != nullptr) {
    if (size < options.size()) {
      context->logger.log(QDMI_LOG_LEVEL_ERROR,
                          "[" QDMI_DEVICE_ID "] Buffer too small");
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    std::ranges::copy(options, value);
  }
  if (size_ret != nullptr) {
    *size_ret = options.size();
  }
  return QDMI_SUCCESS;
}
auto Context_query_modules(Context *context, const size_t size, Module **values,
                           size_t *size_ret) -> int {
  if (context == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  context->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID "] Querying the QPU's modules");
  const auto &modules = Adapter::get().modules();
  if (values != nullptr) {
    if (size < modules.size()) {
      context->logger.log(QDMI_LOG_LEVEL_ERROR,
                          "[" QDMI_DEVICE_ID "] Buffer too small");
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    std::ranges::copy(modules, values);
  }
  if (size_ret != nullptr) {
    *size_ret = modules.size();
  }
  return QDMI_SUCCESS;
}
auto Context_query_module_by_id(Context *context, const char *id,
                                Module **module) -> int {
  if (context == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  context->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID "] Querying a module by ID");
  if (id == nullptr || module == nullptr) {
    context->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Invalid arguments");
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (*module = Adapter::get().module_by_id(id); *module == nullptr) {
    context->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Module with ID '{}' not found.",
                        id);
    return QDMI_ERROR_NOTFOUND;
  }
  return QDMI_SUCCESS;
}
auto Context_query_module_id(Context *context, Module *module,
                             const size_t size, char *value, size_t *size_ret)
    -> int {
  if (context == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  context->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID "] Querying a module's ID");
  if (module == nullptr) {
    context->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Invalid arguments");
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (value != nullptr) {
    const auto &id = module->id;
    if (size < id.size() + 1) {
      context->logger.log(QDMI_LOG_LEVEL_ERROR,
                          "[" QDMI_DEVICE_ID "] Buffer too small");
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
/// @copydoc V2_QDMI_context_get_module_interface
auto Context_get_module_interface(Context *context, Module *module,
                                  const void **interface_ptr) -> int {
  if (context == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  context->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID "] Querying a module's interface");
  if (module == nullptr || interface_ptr == nullptr) {
    context->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Invalid arguments");
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  *interface_ptr = std::visit(
      [](const auto *interface) -> const void * {
        return static_cast<const void *>(interface);
      },
      module->interface);
  return QDMI_SUCCESS;
}
/// @copydoc V2_QDMI_context_allocate_session
auto Context_allocate_session(Context *context,
                              const QDMI_Log_Callback callback, void *user_data,
                              Session **session) -> int {
  if (context == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  context->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID "] Allocating a new session");
  if ((callback == nullptr && user_data != nullptr) || session == nullptr) {
    context->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Invalid arguments");
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  const auto logger =
      callback != nullptr ? Logger{callback, user_data} : context->logger;
  *session = Adapter::get().create_new_session(context, logger);
  return QDMI_SUCCESS;
}
/// @copydoc V2_QDMI_session_set_token
auto Session_set_token(Session *session, const char *value) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID "] Setting the session's token");
  if (session->status != Session::STATUS::ALLOCATED) {
    session->logger.log(
        QDMI_LOG_LEVEL_ERROR,
        "[" QDMI_DEVICE_ID
        "] Session is in an invalid state for setting the token");
    return QDMI_ERROR_BADSTATE;
  }
  if (value == nullptr) {
    session->token.clear();
  } else {
    session->token = value;
  }
  return QDMI_SUCCESS;
}
/// @copydoc V2_QDMI_session_set_authentication_file
auto Session_set_authentication_file(Session *session,
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
                      "] Authentication file support is not implemented");
  return QDMI_ERROR_NOTSUPPORTED;
}
/// @copydoc V2_QDMI_session_set_authentication_url
auto Session_set_authentication_url(Session *session,
                                    [[maybe_unused]] const char *value) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID
                      "] Setting the session's authentication URL");
  session->logger.log(QDMI_LOG_LEVEL_ERROR,
                      "[" QDMI_DEVICE_ID
                      "] Authentication URL support is not implemented");
  return QDMI_ERROR_NOTSUPPORTED;
}
/// @copydoc V2_QDMI_session_set_username
auto Session_set_username(Session *session, [[maybe_unused]] const char *value)
    -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID "] Setting the session's username");
  session->logger.log(QDMI_LOG_LEVEL_ERROR,
                      "[" QDMI_DEVICE_ID
                      "] Username support is not implemented");
  return QDMI_ERROR_NOTSUPPORTED;
}
/// @copydoc V2_QDMI_session_set_password
auto Session_set_password(Session *session, [[maybe_unused]] const char *value)
    -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID "] Setting the session's password");
  session->logger.log(QDMI_LOG_LEVEL_ERROR,
                      "[" QDMI_DEVICE_ID
                      "] Password support is not implemented");
  return QDMI_ERROR_NOTSUPPORTED;
}
/// @copydoc V2_QDMI_session_set_log_callback
auto Session_set_log_callback(Session *session,
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
/// @copydoc V2_QDMI_session_initialize
auto Session_initialize(Session *session) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID "] Initializing the session");
  if (session->status != Session::STATUS::ALLOCATED) {
    session->logger.log(
        QDMI_LOG_LEVEL_ERROR,
        "[" QDMI_DEVICE_ID
        "] Session is in an invalid state for setting the token");
    return QDMI_ERROR_BADSTATE;
  }
  session->status = Session::STATUS::INITIALIZED;
  return QDMI_SUCCESS;
}
/// @copydoc V2_QDMI_session_free
auto Session_free(Session *session) -> void {
  if (session != nullptr) {
    session->logger.log(QDMI_LOG_LEVEL_INFO,
                        "[" QDMI_DEVICE_ID "] Freeing the session");
    Adapter::get().remove_session(session);
  }
}
/// @copydoc V2_QDMI_context_finalize
auto Context_finalize(Context *context) -> int {
  if (context == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  context->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID "] Finalizing the context");
  Adapter::get().remove_context(context);
  return QDMI_SUCCESS;
}
/// @copydoc V2_QDMI_context_set_log_callback
auto Context_set_log_callback(Context *context,
                              const QDMI_Log_Callback callback, void *user_data)
    -> int {
  if (context == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  context->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID
                      "] Setting the logging callback for the context");
  if (callback == nullptr && user_data != nullptr) {
    context->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID
                        "] User data provided without a callback function");
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  context->logger =
      callback != nullptr ? Logger{callback, user_data} : Logger{};
  return QDMI_SUCCESS;
}
/// @copydoc V2_QDMI_get_interface
auto Get_interface(V2_QDMI_Core_Interface const **interface_ptr) -> int {
  if (interface_ptr == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  *interface_ptr = &Adapter::get().core_interface();
  return QDMI_SUCCESS;
}
} // namespace
auto Get_core_interface() -> V2_QDMI_Core_Interface {
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
auto Get_library() -> V2_QDMI_Library {
  return {.get_interface = &Get_interface,
          .context_set_log_callback = &Context_set_log_callback,
          .context_finalize = &Context_finalize};
}
} // namespace qdmi::adapter
int V2_QDMI_initialize(const size_t version, QDMI_Log_Callback callback,
                       void *user_data, qdmi::adapter::Context **context,
                       V2_QDMI_Library const **interface_ptr) {
  const qdmi::adapter::Logger logger{callback, user_data};
  logger.log(QDMI_LOG_LEVEL_INFO,
             "[" QDMI_DEVICE_ID "] Initializing the QDMI interface");
  if ((callback == nullptr && user_data != nullptr) || context == nullptr ||
      interface_ptr == nullptr) {
    logger.log(QDMI_LOG_LEVEL_ERROR, "[" QDMI_DEVICE_ID "] Invalid arguments");
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (version != QDMI_VERSION) {
    logger.log(QDMI_LOG_LEVEL_ERROR,
               "[" QDMI_DEVICE_ID
               "] Version mismatch: expected {}.{}.{} ({}), got {}.{}.{} ({})",
               QDMI_MAJOR_VERSION(QDMI_VERSION),
               QDMI_MINOR_VERSION(QDMI_VERSION),
               QDMI_PATCH_VERSION(QDMI_VERSION), QDMI_VERSION,
               QDMI_MAJOR_VERSION(version), QDMI_MINOR_VERSION(version),
               QDMI_PATCH_VERSION(version), version);
    return QDMI_ERROR_VERSIONMISMATCH;
  }
  *context = qdmi::adapter::Adapter::get().create_new_context(logger);
  *interface_ptr = &qdmi::adapter::Adapter::get().library();
  return QDMI_SUCCESS;
}
#ifndef QDMI_STATIC_DEFINE
const char *QDMI_get_prefix() { return "V2"; }
#endif // QDMI_STATIC_DEFINE
// NOLINTEND(misc-include-cleaner)
