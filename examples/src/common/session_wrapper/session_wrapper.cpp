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

#include "common/session_wrapper/session_wrapper.hpp"

#include "qdmi/core.h"

#include <sstream>
#include <stdexcept>
#include <utility>

namespace qdmi {
SessionWrapper::SessionWrapper(QDMI_Context context,
                               const QDMI_Core_Interface *interface,
                               QDMI_Log_Callback callback, void *user_data)
    : callback_{callback}, user_data_{user_data}, context_{context},
      interface_{interface} {
  if (const auto ret = interface->context_allocate_session(
          context, callback_, user_data_, &session_);
      ret != QDMI_SUCCESS) {
    throw std::runtime_error(
        std::format("Failed to allocate device session: {}", ret));
  }
  // dummy authentication just setting an arbitrary token, ignore if the
  // function call fails, i.e., when no authentication is required.
  //
  // Temporarily disable logging because the following query may fail for
  // devices that do not require logging, and we want to avoid logging an error
  // in this case.
  interface->session_set_log_callback(session_, nullptr, nullptr);
  interface->session_set_token(session_, "abc123");
  interface->session_set_log_callback(session_, callback_, user_data_);

  if (const auto ret = interface->session_initialize(session_);
      ret != QDMI_SUCCESS) {
    throw std::runtime_error(
        std::format("Failed to initialize device session: {}", ret));
  }
}
SessionWrapper::SessionWrapper(const SessionWrapper &other)
    : SessionWrapper{other.context_, other.interface_, other.callback_,
                     other.user_data_} {}
SessionWrapper &SessionWrapper::operator=(const SessionWrapper &other) {
  if (this != &other) {
    SessionWrapper tmp{other};
    swap(tmp);
  }
  return *this;
}
SessionWrapper::SessionWrapper(SessionWrapper &&other) noexcept
    : callback_{other.callback_}, user_data_{other.user_data_},
      context_{other.context_}, interface_{other.interface_},
      session_{other.session_} {
  other.callback_ = nullptr;
  other.user_data_ = nullptr;
  other.context_ = nullptr;
  other.interface_ = nullptr;
  other.session_ = nullptr;
}
SessionWrapper &SessionWrapper::operator=(SessionWrapper &&other) noexcept {
  if (this != &other) {
    SessionWrapper tmp{std::move(other)};
    swap(tmp);
  }
  return *this;
}
SessionWrapper::~SessionWrapper() {
  if (interface_ != nullptr) {
    interface_->session_free(session_);
  }
}
auto SessionWrapper::get() const -> QDMI_Session {
  if (interface_ == nullptr) {
    return nullptr;
  }
  return session_;
}
auto SessionWrapper::swap(SessionWrapper &other) noexcept -> void {
  std::swap(callback_, other.callback_);
  std::swap(user_data_, other.user_data_);
  std::swap(context_, other.context_);
  std::swap(interface_, other.interface_);
  std::swap(session_, other.session_);
}
} // namespace qdmi
