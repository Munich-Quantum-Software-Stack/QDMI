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

#include "common/library_wrapper/library_wrapper.hpp"

#include "common/dynamic_library/dynamic_library.hpp"
#include "qdmi/core.h"

#include <stdexcept>
#include <string>
#include <utility>

namespace qdmi {
LibraryWrapper::LibraryWrapper(Dylib dylib, const QDMI_Log_Callback callback,
                               void *user_data)
    : callback_{callback}, user_data_{user_data}, dylib_{std::move(dylib)} {
  init_prefix();
  init_context_and_library();
}
LibraryWrapper::LibraryWrapper(const LibraryWrapper &other)
    : LibraryWrapper{other.dylib_, other.prefix_, other.callback_,
                     other.user_data_} {}
LibraryWrapper &LibraryWrapper::operator=(const LibraryWrapper &other) {
  if (this != &other) {
    LibraryWrapper tmp{other};
    swap(tmp);
  }
  return *this;
}
LibraryWrapper::LibraryWrapper(LibraryWrapper &&other) noexcept
    : callback_{other.callback_}, user_data_{other.user_data_},
      dylib_{std::move(other.dylib_)}, prefix_{std::move(other.prefix_)},
      context_{other.context_}, library_{other.library_} {
  other.callback_ = nullptr;
  other.user_data_ = nullptr;
  other.context_ = nullptr;
  other.library_ = nullptr;
}
LibraryWrapper &LibraryWrapper::operator=(LibraryWrapper &&other) noexcept {
  if (this != &other) {
    LibraryWrapper tmp{std::move(other)};
    swap(tmp);
  }
  return *this;
}
LibraryWrapper::~LibraryWrapper() {
  if (library_ != nullptr) {
    library_->context_finalize(context_);
  }
}
auto LibraryWrapper::get() const -> const QDMI_Library * {
  if (library_ == nullptr) {
    throw std::runtime_error("Library not initialized");
  }
  return library_;
}
auto LibraryWrapper::context() const -> QDMI_Context {
  if (library_ == nullptr) {
    throw std::runtime_error("Library not initialized");
  }
  return context_;
}
auto LibraryWrapper::set_logging_callback(const QDMI_Log_Callback callback,
                                          void *user_data) -> void {
  if (library_ != nullptr) {
    callback_ = callback;
    user_data_ = user_data;
    library_->context_set_log_callback(context_, callback_, user_data_);
  }
}
LibraryWrapper::LibraryWrapper(Dylib dylib, std::string prefix,
                               const QDMI_Log_Callback callback,
                               void *user_data)
    : callback_{callback}, user_data_{user_data}, dylib_{std::move(dylib)},
      prefix_{std::move(prefix)} {
  init_context_and_library();
}
auto LibraryWrapper::init_prefix() -> void {
  auto *get_prefix{
      dylib_.get_symbol<decltype(QDMI_get_prefix)>("QDMI_get_prefix")};
  if (get_prefix == nullptr) {
    throw std::runtime_error("Failed to retrieve library prefix.");
  }
  prefix_ = get_prefix();
  if (prefix_.empty()) {
    throw std::runtime_error("Library prefix is empty.");
  }
}
auto LibraryWrapper::init_context_and_library() -> void {
  const auto symbol_name = prefix_ + "_QDMI_initialize";
  if (auto *initialize =
          dylib_.get_symbol<decltype(QDMI_initialize)>(symbol_name);
      initialize == nullptr ||
      // NOLINTNEXTLINE(misc-include-cleaner)
      initialize(QDMI_VERSION, callback_, user_data_, &context_, &library_) !=
          QDMI_SUCCESS) {
    throw std::runtime_error("Failed to initialize the device.");
  }
}
auto LibraryWrapper::swap(LibraryWrapper &other) noexcept -> void {
  std::swap(callback_, other.callback_);
  std::swap(user_data_, other.user_data_);
  std::swap(dylib_, other.dylib_);
  std::swap(prefix_, other.prefix_);
  std::swap(context_, other.context_);
  std::swap(library_, other.library_);
}
} // namespace qdmi
