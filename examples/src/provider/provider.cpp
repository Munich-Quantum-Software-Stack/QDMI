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

#include "provider/provider.hpp"

#include "common/dynamic_library/dynamic_library.hpp"
#include "common/library_wrapper/library_wrapper.hpp"
#include "p_qdmi/core.h"
#include "p_qdmi/provider.h"
#include "provider/core_bindings.hpp"
#include "provider/provider_bindings.hpp"
#include "qdmi/core.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
#include <memory>
#include <ranges>
#include <span>
#include <spdlog/common.h>
#include <spdlog/fmt/bundled/ranges.h>
#include <spdlog/logger.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

namespace qdmi::provider {
namespace {
auto Log_callback(const QDMI_Log_Level level, const char *message,
                  void *user_data) -> void {
  static_cast<spdlog::logger *>(user_data)->log(
      static_cast<spdlog::level::level_enum>(level), message);
}
} // namespace
auto Provider::id() const -> const std::string & { return id_; }
auto Provider::name() const -> const std::string & { return name_; }
auto Provider::version() const -> size_t { return version_; }
auto Provider::authentication_options() const
    -> std::span<const QDMI_Authentication_Option> {
  return authentication_options_;
}
auto Provider::library() const -> const P_QDMI_Library & { return library_; }
auto Provider::core_interface() const -> const P_QDMI_Core_Interface & {
  return core_interface_;
}
auto Provider::provider_interface() const -> const P_QDMI_Provider_Interface & {
  return provider_interface_;
}
auto Provider::modules() -> std::span<Module *> { return module_ptrs_; }
auto Provider::module_by_id(const std::string &id) -> Module * {
  if (id == "provider") {
    return &provider_module_;
  }
  return nullptr;
}
auto Provider::devices() -> std::span<Device *> {
  return {device_ptrs_.data(), device_ptrs_.size()};
}
auto Provider::device_by_id(const std::string &id) const -> Device * {
  const auto it = device_by_id_.find(id);
  if (it == device_by_id_.end()) {
    return nullptr;
  }
  return it->second;
}
auto Provider::create_new_context(Logger logger) -> Context * {
  auto context = std::make_unique<Context>(logger);
  auto *context_ptr =
      contexts_.emplace(context.get(), std::move(context)).first->first;
  return context_ptr;
}
auto Provider::remove_context(Context *context) -> void {
  contexts_.erase(context);
}
auto Provider::create_new_session(Context *context, Logger logger)
    -> Session * {
  auto session = std::make_unique<Session>(context, logger);
  auto *session_ptr =
      sessions_.emplace(session.get(), std::move(session)).first->first;
  return session_ptr;
}
auto Provider::remove_session(Session *session) -> void {
  sessions_.erase(session);
}
Provider::Provider()
    : library_(Get_library()), core_interface_(Get_core_interface()),
      provider_interface_(Get_provider_interface()) {
  // NOLINTNEXTLINE(misc-include-cleaner)
  logger_->info("[" QDMI_DEVICE_ID "] Loading libraries: {}",
                fmt::join(
                    // NOLINTNEXTLINE(misc-include-cleaner)
                    QDMI_DYN_LIBS, ", "));
  std::ranges::transform(
      // NOLINTNEXTLINE(misc-include-cleaner)
      QDMI_DYN_LIBS, devices_.begin(), [this](const auto &path) -> Device {
        Device device{.library = LibraryWrapper{Dylib{path}, Log_callback,
                                                logger_.get()}};
        initialize_device_interface(device);
        return device;
      });
  std::ranges::transform(devices_, device_ptrs_.begin(),
                         [](auto &device) { return &device; });
  std::ranges::transform(
      devices_, std::inserter(device_by_id_, device_by_id_.end()),
      [](auto &device) -> std::pair<std::string, Device *> {
        if (const auto id{query_device_id(device)}; id.has_value()) {
          return {*id, &device};
        }
        throw std::runtime_error("Failed to query device ID");
      });
}
auto Provider::query_device_id(const Device &device)
    -> std::optional<std::string> {
  size_t size = 0;
  if (const auto ret = device.core_interface->context_query_id(
          device.library.context(), 0, nullptr, &size);
      ret != QDMI_SUCCESS) {
    return std::nullopt;
  }
  std::string id(size - 1, '\0');
  if (const auto ret = device.core_interface->context_query_id(
          device.library.context(), size, id.data(), nullptr);
      ret != QDMI_SUCCESS) {
    return std::nullopt;
  }
  return id;
}
auto Provider::initialize_device_interface(Device &device) -> void {
  if (const auto ret =
          device.library.get()->get_interface(&device.core_interface);
      ret != QDMI_SUCCESS) {
    throw std::runtime_error(
        std::format("Failed to get core interface: {}", ret));
  }
}
} // namespace qdmi::provider
