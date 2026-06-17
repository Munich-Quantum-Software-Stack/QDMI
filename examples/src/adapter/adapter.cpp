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

#include "adapter/adapter.hpp"

#include "adapter/core_bindings.hpp"
#include "adapter/device_core_bindings.hpp"
#include "adapter/device_qpu_bindings.hpp"
#include "adapter/device_superconducting_bindings.hpp"
#include "adapter/provider_bindings.hpp"

#include <spdlog/common.h>
#include <spdlog/fmt/bundled/ranges.h>
#include <spdlog/logger.h>
#include <unordered_set>
#include <utility>

namespace qdmi::adapter {
auto Adapter::id() const -> const std::string & { return id_; }
auto Adapter::name() const -> const std::string & { return name_; }
auto Adapter::version() const -> size_t { return version_; }
auto Adapter::authentication_options() const
    -> std::span<const QDMI_Authentication_Option> {
  return authentication_options_;
}
auto Adapter::library() const -> const V2_QDMI_Library & { return library_; }
auto Adapter::core_interface() const -> const V2_QDMI_Core_Interface & {
  return core_interface_;
}
auto Adapter::provider_interface() const -> const V2_QDMI_Provider_Interface & {
  return provider_interface_;
}
auto Adapter::modules() -> std::span<Module *> { return module_ptrs_; }
auto Adapter::module_by_id(const std::string &id) -> Module * {
  if (id == "provider") {
    return &provider_module_;
  }
  return nullptr;
}
auto Adapter::devices() -> std::span<Device *> { return device_ptrs_; }
auto Adapter::device_by_id(const std::string &id) const -> Device * {
  if (const auto it{device_by_id_.find(id)}; it != device_by_id_.end()) {
    return it->second;
  }
  return nullptr;
}
auto Adapter::create_new_context(Logger logger) -> Context * {
  auto context = std::make_unique<Context>(logger);
  auto *context_ptr =
      contexts_.emplace(context.get(), std::move(context)).first->first;
  return context_ptr;
}
auto Adapter::remove_context(Context *context) -> void {
  contexts_.erase(context);
}
auto Adapter::create_new_session(Context *context, Logger logger) -> Session * {
  auto session = std::make_unique<Session>(context, logger);
  auto *session_ptr =
      sessions_.emplace(session.get(), std::move(session)).first->first;
  return session_ptr;
}
auto Adapter::remove_session(Session *session) -> void {
  sessions_.erase(session);
}
auto Adapter::device_core_interface() const -> const AD_QDMI_Core_Interface & {
  return device_core_interface_;
}
auto Adapter::device_modules() -> std::span<DeviceModule *> {
  return device_module_ptrs_;
}
auto Adapter::device_module_by_id(const std::string &id) -> DeviceModule * {
  if (id == "qpu") {
    return &device_qpu_module_;
  }
  if (id == "sc") {
    return &device_sc_module_;
  }
  return nullptr;
}
Adapter::Adapter()
    : library_{Get_library()}, core_interface_{Get_core_interface()},
      provider_interface_{Get_provider_interface()},
      device_core_interface_{Get_device_core_interface()},
      device_qpu_interface_{Get_device_qpu_interface()},
      device_sc_interface_{Get_device_sc_interface()} {
  // NOLINTNEXTLINE(misc-include-cleaner)
  logger_->info("[" QDMI_DEVICE_ID "] Loading libraries: {}",
                fmt::join(
                    // NOLINTNEXTLINE(misc-include-cleaner)
                    QDMI_DYN_LIBS, ", "));
  std::ranges::transform(
      // NOLINTNEXTLINE(misc-include-cleaner)
      QDMI_DYN_LIBS, devices_.begin(),
      [](const auto &path_and_prefix) -> Device {
        const auto &[path, prefix] = path_and_prefix;
        // todo: the ID cannot be retrieved without a session, this might
        //  require credentials
        return Device{prefix, "Adapted QDMIv1 Device",
                      V1Library{Dylib{path}, prefix}};
      });
  std::ranges::transform(devices_, device_ptrs_.begin(),
                         [](auto &device) { return &device; });
  std::ranges::transform(devices_,
                         std::inserter(device_by_id_, device_by_id_.end()),
                         [](auto &device) -> std::pair<std::string, Device *> {
                           return {device.id, &device};
                         });
}
auto Ensure_formats_are_initialized(const DeviceSession &session) -> bool {
  auto &format_ptrs{session.context->device->formats_v1};
  if (!format_ptrs.has_value()) {
    if (!Init_formats(session)) {
      return false;
    }
    Init_format_ptrs(session);
  }
  return true;
}
auto Init_formats(const DeviceSession &session) -> bool {
  const auto &formats_v1_opt = Query_formats_v1(session);
  if (!formats_v1_opt.has_value()) {
    return false;
  }
  const std::unordered_set<V1_PROGRAM_FORMAT> formats_v1_set{
      formats_v1_opt->begin(), formats_v1_opt->end()};
  auto &formats{session.context->device->formats};
  auto &formats_v1{session.context->device->formats_v1};
  formats = std::unordered_map<
      std::string, std::unordered_map<size_t, std::unique_ptr<DeviceFormat>>>{};
  formats_v1 = std::unordered_map<V1_PROGRAM_FORMAT, DeviceFormat *>{};
  if (formats_v1_set.contains(V1_PROGRAM_FORMAT::QASM2)) {
    auto format =
        std::make_unique<DeviceFormat>("qasm", QDMI_MAKE_VERSION(2, 0, 0),
                                       V1_PROGRAM_FORMAT::QASM2, std::nullopt);
    formats_v1->emplace(V1_PROGRAM_FORMAT::QASM2, format.get());
    formats->try_emplace(format->id)
        .first->second.try_emplace(format->version, std::move(format));
  }
  if (formats_v1_set.contains(V1_PROGRAM_FORMAT::QASM3)) {
    auto format =
        std::make_unique<DeviceFormat>("qasm", QDMI_MAKE_VERSION(2, 0, 0),
                                       V1_PROGRAM_FORMAT::QASM3, std::nullopt);
    formats_v1->emplace(V1_PROGRAM_FORMAT::QASM3, format.get());
    formats->try_emplace(format->id)
        .first->second.try_emplace(format->version, std::move(format));
  }
  if (formats_v1_set.contains(V1_PROGRAM_FORMAT::IQMJSON)) {
    auto format = std::make_unique<DeviceFormat>(
        "iqmjson", QDMI_MAKE_VERSION(1, 0, 0), V1_PROGRAM_FORMAT::IQMJSON,
        std::nullopt);
    formats_v1->emplace(V1_PROGRAM_FORMAT::IQMJSON, format.get());
    formats->try_emplace(format->id)
        .first->second.try_emplace(format->version, std::move(format));
  }
  if (formats_v1_set.contains(V1_PROGRAM_FORMAT::QPY)) {
    auto format =
        std::make_unique<DeviceFormat>("qpy", QDMI_MAKE_VERSION(1, 0, 0),
                                       std::nullopt, V1_PROGRAM_FORMAT::QPY);
    formats_v1->emplace(V1_PROGRAM_FORMAT::QPY, format.get());
    formats->try_emplace(format->id)
        .first->second.try_emplace(format->version, std::move(format));
  }
  std::optional<V1_PROGRAM_FORMAT> qir_string{};
  if (formats_v1_set.contains(V1_PROGRAM_FORMAT::QIRBASESTRING)) {
    qir_string = V1_PROGRAM_FORMAT::QIRBASESTRING;
  }
  if (formats_v1_set.contains(V1_PROGRAM_FORMAT::QIRADAPTIVESTRING)) {
    qir_string = V1_PROGRAM_FORMAT::QIRADAPTIVESTRING;
  }
  std::optional<V1_PROGRAM_FORMAT> qir_module{};
  if (formats_v1_set.contains(V1_PROGRAM_FORMAT::QIRBASEMODULE)) {
    qir_module = V1_PROGRAM_FORMAT::QIRBASEMODULE;
  }
  if (formats_v1_set.contains(V1_PROGRAM_FORMAT::QIRADAPTIVEMODULE)) {
    qir_module = V1_PROGRAM_FORMAT::QIRADAPTIVEMODULE;
  }
  if (qir_string || qir_module) {
    auto format = std::make_unique<DeviceFormat>(
        "qir", QDMI_MAKE_VERSION(1, 1, 0), qir_string, qir_module);
    formats->try_emplace(format->id)
        .first->second.try_emplace(format->version, std::move(format));
    if (formats_v1_set.contains(V1_PROGRAM_FORMAT::QIRBASESTRING)) {
      formats_v1->emplace(V1_PROGRAM_FORMAT::QIRBASESTRING, format.get());
    }
    if (formats_v1_set.contains(V1_PROGRAM_FORMAT::QIRADAPTIVESTRING)) {
      formats_v1->emplace(V1_PROGRAM_FORMAT::QIRADAPTIVESTRING, format.get());
    }
    if (formats_v1_set.contains(V1_PROGRAM_FORMAT::QIRBASEMODULE)) {
      formats_v1->emplace(V1_PROGRAM_FORMAT::QIRBASEMODULE, format.get());
    }
    if (formats_v1_set.contains(V1_PROGRAM_FORMAT::QIRADAPTIVEMODULE)) {
      formats_v1->emplace(V1_PROGRAM_FORMAT::QIRADAPTIVEMODULE, format.get());
    }
  }
  return true;
}
auto Init_format_ptrs(const DeviceSession &session) -> void {
  std::ranges::for_each(
      *session.context->device->formats, [&session](auto &formats) -> void {
        std::ranges::for_each(formats.second, [&session](auto &format) -> void {
          session.context->device->format_ptrs->emplace_back(
              format.second.get());
        });
      });
}
auto Query_formats_v1(const DeviceSession &session)
    -> std::optional<std::vector<V1_PROGRAM_FORMAT>> {
  size_t size{};
  if (session.context->device->v1_library.session_query_device_property(
          session.v1_session, V1_DEVICE_PROPERTY::SUPPORTEDPROGRAMFORMATS, 0,
          nullptr, &size) != SUCCESS) {
    return std::nullopt;
  }
  std::vector<V1_PROGRAM_FORMAT> formats_v1(size / sizeof(V1_PROGRAM_FORMAT));
  if (session.context->device->v1_library.session_query_device_property(
          session.v1_session, V1_DEVICE_PROPERTY::SUPPORTEDPROGRAMFORMATS, size,
          formats_v1.data(), nullptr) != SUCCESS) {
    return std::nullopt;
  }
  return formats_v1;
}
auto Transform_return_codes(const int code) -> QDMI_STATUS {
  switch (code) {
  case WARN_GENERAL:
    return QDMI_WARN_GENERAL;
  case SUCCESS:
    return QDMI_SUCCESS;
  case ERROR_OUTOFMEM:
    return QDMI_ERROR_OUTOFMEM;
  case ERROR_NOTIMPLEMENTED:
    return QDMI_ERROR_NOTIMPLEMENTED;
  case ERROR_LIBNOTFOUND:
  case ERROR_NOTFOUND:
    return QDMI_ERROR_NOTFOUND;
  case ERROR_OUTOFRANGE:
    return QDMI_ERROR_OUTOFRANGE;
  case ERROR_INVALIDARGUMENT:
    return QDMI_ERROR_INVALIDARGUMENT;
  case ERROR_PERMISSIONDENIED:
    return QDMI_ERROR_PERMISSIONDENIED;
  case ERROR_NOTSUPPORTED:
    return QDMI_ERROR_NOTSUPPORTED;
  case ERROR_BADSTATE:
    return QDMI_ERROR_BADSTATE;
  case ERROR_TIMEOUT:
    return QDMI_ERROR_TIMEOUT;
  case ERROR_FATAL:
  default:
    return QDMI_ERROR_FATAL;
  }
}
} // namespace qdmi::adapter
