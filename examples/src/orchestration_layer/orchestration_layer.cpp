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

#include "orchestration_layer/orchestration_layer.hpp"

#include "common/dynamic_library/dynamic_library.hpp"
#include "common/library_wrapper/library_wrapper.hpp"
#include "common/session_wrapper/session_wrapper.hpp"
#include "o_qdmi/core.h"
#include "o_qdmi/orchestration_layer.h"
#include "od_qdmi/core/types.h"
#include "orchestration_layer/core_bindings.hpp"
#include "orchestration_layer/device_core_bindings.hpp"
#include "orchestration_layer/device_superconducting_bindings.hpp"
#include "orchestration_layer/orchestration_layer_bindings.hpp"
#include "qdmi/core.h"
#include "qdmi/orchestration_layer.h"
#include "qdmi/provider.h"
#include "qdmi/qpu.h"
#include "qdmi/superconducting.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <memory>
#include <optional>
#include <ranges>
#include <span>
#include <spdlog/common.h>
#include <spdlog/fmt/bundled/ranges.h>
#include <spdlog/logger.h>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

// NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)

namespace qdmi::orchestration_layer {
namespace {
template <class... Ts> struct overloaded : Ts... {
  using Ts::operator()...;
};
auto Log_callback(const QDMI_Log_Level level, const char *message,
                  void *user_data) -> void {
  static_cast<spdlog::logger *>(user_data)->log(
      static_cast<spdlog::level::level_enum>(level), message);
}
} // namespace
auto OrchestrationLayer::id() const -> const std::string & { return id_; }
auto OrchestrationLayer::name() const -> const std::string & { return name_; }
auto OrchestrationLayer::version() const -> size_t { return version_; }
auto OrchestrationLayer::authentication_options() const
    -> std::span<const QDMI_Authentication_Option> {
  return authentication_options_;
}
auto OrchestrationLayer::library() const -> const O_QDMI_Library & {
  return library_;
}
auto OrchestrationLayer::core_interface() const
    -> const O_QDMI_Core_Interface & {
  return core_interface_;
}
auto OrchestrationLayer::orchestration_layer_interface() const
    -> const O_QDMI_OrchestrationLayer_Interface & {
  return orchestration_layer_interface_;
}
auto OrchestrationLayer::modules() -> std::span<Module *> {
  return module_ptrs_;
}
auto OrchestrationLayer::module_by_id(const std::string &id) -> Module * {
  if (id == "ol") {
    return &orchestration_layer_module_;
  }
  return nullptr;
}
auto OrchestrationLayer::devices() -> std::span<Device *> {
  return {qpu_ptrs_.data(), qpu_ptrs_.size()};
}
auto OrchestrationLayer::device_by_id(const std::string &id) const -> Device * {
  const auto it = qpu_by_id_.find(id);
  if (it == qpu_by_id_.end()) {
    return nullptr;
  }
  return it->second;
}
auto OrchestrationLayer::formats() -> std::span<Format *> {
  return {formats_.data(), formats_.size()};
}
auto OrchestrationLayer::format_by_id_and_version(const std::string &id,
                                                  size_t version) -> Format * {
  if (id == "qasm") {
    if (version == QDMI_MAKE_VERSION(2, 0, 0)) {
      return &qasm2_format_;
    }
    if (version == QDMI_MAKE_VERSION(3, 0, 0)) {
      return &qasm3_format_;
    }
  } else if (id == "qir") {
    if (version == QDMI_MAKE_VERSION(1, 1, 0)) {
      return &qir_format_;
    }
  }
  return nullptr;
}
auto OrchestrationLayer::create_new_context(Logger logger) -> Context * {
  auto context = std::make_unique<Context>(logger);
  auto *context_ptr =
      contexts_.emplace(context.get(), std::move(context)).first->first;
  return context_ptr;
}
auto OrchestrationLayer::remove_context(Context &context) -> void {
  contexts_.erase(&context);
}
auto OrchestrationLayer::create_new_session(const Context &context,
                                            Logger logger) -> Session * {
  auto session = std::make_unique<Session>(&context, logger);
  auto *session_ptr =
      sessions_.emplace(session.get(), std::move(session)).first->first;
  return session_ptr;
}
auto OrchestrationLayer::remove_session(Session &session) -> void {
  sessions_.erase(&session);
}
auto OrchestrationLayer::create_new_job(const Session &session, Logger logger)
    -> Job * {
  auto id = generate_pseudo_uuid_v4();
  auto job = std::make_unique<Job>(&session, logger, std::move(id));
  auto *job_ptr = jobs_.emplace(job.get(), std::move(job)).first->first;
  jobs_by_id_.emplace(job_ptr->id, job_ptr);
  return job_ptr;
}
auto OrchestrationLayer::retrieve_job_by_id(const std::string &id,
                                            const Session &session,
                                            Logger logger) -> Job * {
  const auto it = jobs_by_id_.find(id);
  if (it == jobs_by_id_.end()) {
    return nullptr;
  }
  auto &job = *it->second;
  job.session = &session;
  job.logger = logger;
  return it->second;
}
auto OrchestrationLayer::submit_job(Job &job) -> void {
  job.status = QDMI_JOB_STATUS_SUBMITTED;
  job.status = QDMI_JOB_STATUS_QUEUED;
  job.status = QDMI_JOB_STATUS_RUNNING;
  auto &device = *job.device;
  try {
    job.results = std::visit(
        overloaded{
            [this, session = device.session, &job](const Device::QPU &qpu)
                -> std::vector<std::vector<std::string>> {
              return execute_job_on_qpu(session, *qpu.interface, job);
            },
            [this, &job](const Device::OrchestrationLayer &ol)
                -> std::vector<std::vector<std::string>> {
              return execute_job_on_orchestration_layer(
                  ol.session, *ol.interface, ol.qpu, job);
            }},
        device.job_target);
  } catch (const std::runtime_error &e) {
    job.status = QDMI_JOB_STATUS_FAILED;
    throw std::runtime_error(std::string("Failed to execute job: ") + e.what());
  }
  job.status = QDMI_JOB_STATUS_DONE;
}
auto OrchestrationLayer::execute_job_on_qpu(QDMI_Session session,
                                            const QDMI_QPU_Interface &qpu,
                                            const Job &job)
    -> std::vector<std::vector<std::string>> {
  auto *qpu_job{create_qpu_job(session, qpu)};
  set_shot_count_for_qpu_job(qpu_job, qpu, job.shot_count);
  const auto payload_is_string{
      std::holds_alternative<std::vector<std::string>>(job.payload)};
  const auto &format{
      query_any_qpu_program_format(session, qpu, payload_is_string)};
  set_payload_for_qpu_job(qpu_job, qpu, format, job.payload);
  submit_qpu_job(qpu_job, qpu);
  return retrieve_qpu_job_result(
      qpu_job, qpu,
      std::visit([](const auto &programs) { return programs.size(); },
                 job.payload));
}
auto OrchestrationLayer::execute_job_on_orchestration_layer(
    QDMI_Session session, const QDMI_OrchestrationLayer_Interface &ol,
    QDMI_Device qpu, const Job &job) const
    -> std::vector<std::vector<std::string>> {
  auto *orchestration_layer_job{create_orchestration_layer_job(session, ol)};
  set_qpu_for_orchestration_layer_job(orchestration_layer_job, ol, qpu);
  set_shot_count_for_orchestration_layer_job(orchestration_layer_job, ol,
                                             job.shot_count);
  const auto &format{query_orchestration_layer_program_format(
      session, ol, job.format->id, job.format->version)};
  set_payload_for_orchestration_layer_job(orchestration_layer_job, ol, format,
                                          job.payload);
  submit_orchestration_layer_job(orchestration_layer_job, ol);
  return retrieve_orchestration_layer_job_result(
      orchestration_layer_job, ol,
      std::visit([](const auto &programs) { return programs.size(); },
                 job.payload));
}
auto OrchestrationLayer::remove_job(Job &job) -> void {
  jobs_by_id_.erase(job.id);
  jobs_.erase(&job);
}
auto OrchestrationLayer::device_modules() -> std::span<DeviceModule *> {
  return {qpu_module_ptrs_.data(), qpu_module_ptrs_.size()};
}
auto OrchestrationLayer::device_module_by_id(const std::string &id)
    -> DeviceModule * {
  if (id == "sc") {
    return &qpu_sc_module_;
  }
  return nullptr;
}
auto OrchestrationLayer::device_core_interface() const
    -> const OD_QDMI_Core_Interface & {
  return qpu_core_interface_;
}
OrchestrationLayer::OrchestrationLayer()
    : library_{Get_library()}, core_interface_{Get_core_interface()},
      orchestration_layer_interface_{Get_orchestration_layer_interface()},
      qpu_core_interface_{Get_device_core_interface()},
      qpu_sc_interface_{Get_device_sc_interface()} {
  // NOLINTNEXTLINE(misc-include-cleaner)
  logger_->info("[" QDMI_DEVICE_ID "] Loading libraries: {}",
                fmt::join(
                    // NOLINTNEXTLINE(misc-include-cleaner)
                    QDMI_DYN_LIBS, ", "));
  std::ranges::transform(
      // NOLINTNEXTLINE(misc-include-cleaner)
      QDMI_DYN_LIBS, open_libraries_.begin(),
      [this](const auto &path) -> LibraryWrapper {
        return LibraryWrapper{Dylib{path}, Log_callback, logger_.get()};
      });
  std::ranges::transform(
      open_libraries_, std::back_inserter(open_sessions_),
      [this](const LibraryWrapper &library) -> SessionNode {
        const QDMI_Core_Interface *core_interface{};
        if (const auto ret = library.get()->get_interface(&core_interface);
            ret != QDMI_SUCCESS) {
          throw std::runtime_error(
              std::format("Failed to get core interface: {}", ret));
        }
        return add_qpus(library.context(), *core_interface);
      });

  std::ranges::transform(qpus_, std::back_inserter(qpu_ptrs_),
                         [](auto &device) { return &device; });
  std::ranges::transform(qpus_, std::inserter(qpu_by_id_, qpu_by_id_.end()),
                         [](auto &device) -> std::pair<std::string, Device *> {
                           return {device.id, &device};
                         });
}
auto OrchestrationLayer::add_qpus(QDMI_Context context,
                                  const QDMI_Core_Interface &interface)
    -> SessionNode {
  if (const auto qpu_interface{
          query_qpu_interface_if_present(context, interface)}) {
    SessionWrapper session{context, &interface, Log_callback, logger_.get()};
    auto &qpu{qpus_.emplace_back(context, session.get(), &interface,
                                 Device::QPU{*qpu_interface})};
    init_device_id(qpu);
    init_sc_interface_if_present(qpu);
    return {.session = std::move(session), .children = {}};
  }
  if (const auto provider_interface{
          query_provider_interface_if_present(context, interface)}) {
    SessionWrapper session{context, &interface, Log_callback, logger_.get()};
    auto children = add_qpus_from_provider(session.get(), **provider_interface);
    return {.session = std::move(session), .children = std::move(children)};
  }
  if (const auto ol_interface{
          query_ol_interface_if_present(context, interface)}) {
    SessionWrapper session{context, &interface, Log_callback, logger_.get()};
    auto children =
        add_qpus_from_orchestration_layer(session.get(), **ol_interface);
    return {.session = std::move(session), .children = std::move(children)};
  }
  throw std::runtime_error(
      "No supported module interface found in the context");
}
auto OrchestrationLayer::add_qpus_from_provider(
    QDMI_Session session, const QDMI_Provider_Interface &provider)
    -> std::vector<SessionNode> {
  std::vector<SessionNode> result;
  const auto &devices{query_provider_devices(session, provider)};
  if (!devices.has_value()) {
    throw std::runtime_error("Failed to query provider devices.");
  }
  std::ranges::transform(
      *devices, std::back_inserter(result),
      [this, session, &provider](const auto device) -> SessionNode {
        return add_qpus(
            query_provider_device_context(session, provider, device),
            query_provider_device_interface(session, provider, device));
      });
  return result;
}
auto OrchestrationLayer::add_qpus_from_orchestration_layer(
    QDMI_Session session, const QDMI_OrchestrationLayer_Interface &ol)
    -> std::vector<SessionNode> {
  std::vector<SessionNode> result;
  std::ranges::transform(
      query_orchestration_layer_qpus(session, ol), std::back_inserter(result),
      [this, session, &ol](const auto qpu) -> SessionNode {
        auto context{query_orchestration_layer_qpu_context(session, ol, qpu)};
        const auto interface{
            query_orchestration_layer_qpu_interface(session, ol, qpu)};
        SessionWrapper qpu_session{context, &interface, Log_callback,
                                   logger_.get()};
        auto &qpu_device{
            qpus_.emplace_back(context, qpu_session.get(), &interface,
                               Device::OrchestrationLayer{session, qpu, &ol})};
        init_device_id(qpu_device);
        init_sc_interface_if_present(qpu_device);
        return {.session = std::move(qpu_session), .children = {}};
      });
  return result;
}
auto OrchestrationLayer::generate_pseudo_uuid_v4() -> std::string {
  constexpr std::array hex_digits{'0', '1', '2', '3', '4', '5', '6', '7',
                                  '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
  std::string uuid{"xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"};
  std::ranges::for_each(uuid, [this, &hex_digits](auto &c) -> void {
    if (c != '-') {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
      c = hex_digits[dis_hex_(gen_)];
    }
  });
  return uuid;
}
auto OrchestrationLayer::init_device_id(Device &device) -> void {
  size_t size = 0;
  if (const auto ret = device.core_interface->context_query_id(
          device.context, 0, nullptr, &size);
      ret != QDMI_SUCCESS) {
    throw std::runtime_error(
        std::format("Failed to query context ID size: {}", ret));
  }
  device.id.resize(size - 1, '\0');
  if (const auto ret = device.core_interface->context_query_id(
          device.context, size, device.id.data(), nullptr);
      ret != QDMI_SUCCESS) {
    throw std::runtime_error(
        std::format("Failed to query context ID: {}", ret));
  }
}
auto OrchestrationLayer::init_sc_interface_if_present(Device &device) const
    -> void {
  QDMI_Module sc_module{};
  // Temporarily disable logging because the following query may fail for
  // devices that do not implement the superconducting module, and we want to
  // avoid logging an error in this case.
  const auto log_level{logger_->level()};
  logger_->set_level(spdlog::level::off);
  if (const auto ret = device.core_interface->context_query_module_by_id(
          device.context, "sc", &sc_module);
      ret == QDMI_ERROR_NOTFOUND) {
    logger_->set_level(log_level);
    return; // skip
    // NOLINTNEXTLINE(readability-else-after-return)
  } else if (ret != QDMI_SUCCESS) {
    logger_->set_level(log_level);
    throw std::runtime_error(std::format("Failed to query sc module: {}", ret));
  }
  logger_->set_level(log_level);
  if (const auto ret = device.core_interface->context_get_module_interface(
          device.context, sc_module,
          reinterpret_cast<const void **>(&device.sc_interface));
      ret != QDMI_SUCCESS) {
    throw std::runtime_error(
        std::format("Failed to get interface for sc module: {}", ret));
  }
}
auto OrchestrationLayer::query_qpu_interface_if_present(
    QDMI_Context context, const QDMI_Core_Interface &interface) const
    -> std::optional<const QDMI_QPU_Interface *> {
  QDMI_Module qpu_module{};
  // Temporarily disable logging because the following query may fail for
  // contexts that do not implement the qpu module, and we want to avoid logging
  // an error in this case.
  const auto log_level{logger_->level()};
  logger_->set_level(spdlog::level::off);
  if (const auto ret =
          interface.context_query_module_by_id(context, "qpu", &qpu_module);
      ret == QDMI_ERROR_NOTFOUND) {
    logger_->set_level(log_level);
    return std::nullopt;
    // NOLINTNEXTLINE(readability-else-after-return)
  } else if (ret != QDMI_SUCCESS) {
    logger_->set_level(log_level);
    throw std::runtime_error(
        std::format("Failed to query qpu module: {}", ret));
  }
  logger_->set_level(log_level);
  const QDMI_QPU_Interface *qpu_interface{};
  if (const auto ret = interface.context_get_module_interface(
          context, qpu_module, reinterpret_cast<const void **>(&qpu_interface));
      ret != QDMI_SUCCESS) {
    throw std::runtime_error(
        std::format("Failed to get interface for qpu module: {}", ret));
  }
  return qpu_interface;
}
auto OrchestrationLayer::query_provider_interface_if_present(
    QDMI_Context context, const QDMI_Core_Interface &interface) const
    -> std::optional<const QDMI_Provider_Interface *> {
  QDMI_Module provider_module{};
  // Temporarily disable logging because the following query may fail for
  // contexts that do not implement the provider module, and we want to avoid
  // logging an error in this case.
  const auto log_level{logger_->level()};
  logger_->set_level(spdlog::level::off);
  if (const auto ret = interface.context_query_module_by_id(context, "provider",
                                                            &provider_module);
      ret == QDMI_ERROR_NOTFOUND) {
    logger_->set_level(log_level);
    return std::nullopt;
    // NOLINTNEXTLINE(readability-else-after-return)
  } else if (ret != QDMI_SUCCESS) {
    logger_->set_level(log_level);
    throw std::runtime_error(
        std::format("Failed to query provider module: {}", ret));
  }
  logger_->set_level(log_level);
  const QDMI_Provider_Interface *provider_interface{};
  if (const auto ret = interface.context_get_module_interface(
          context, provider_module,
          reinterpret_cast<const void **>(&provider_interface));
      ret != QDMI_SUCCESS) {
    throw std::runtime_error(
        std::format("Failed to get interface for provider module: {}", ret));
  }
  return provider_interface;
}
auto OrchestrationLayer::query_ol_interface_if_present(
    QDMI_Context context, const QDMI_Core_Interface &interface) const
    -> std::optional<const QDMI_OrchestrationLayer_Interface *> {
  QDMI_Module ol_module{};
  // Temporarily disable logging because the following query may fail for
  // contexts that do not implement the orchestration layer module, and we want
  // to avoid logging an error in this case.
  const auto log_level{logger_->level()};
  logger_->set_level(spdlog::level::off);
  if (const auto ret =
          interface.context_query_module_by_id(context, "ol", &ol_module);
      ret == QDMI_ERROR_NOTFOUND) {
    logger_->set_level(log_level);
    return std::nullopt;
    // NOLINTNEXTLINE(readability-else-after-return)
  } else if (ret != QDMI_SUCCESS) {
    logger_->set_level(log_level);
    throw std::runtime_error(std::format("Failed to query ol module: {}", ret));
  }
  logger_->set_level(log_level);
  const QDMI_OrchestrationLayer_Interface *ol_interface{};
  if (const auto ret = interface.context_get_module_interface(
          context, ol_module, reinterpret_cast<const void **>(&ol_interface));
      ret != QDMI_SUCCESS) {
    throw std::runtime_error(
        std::format("Failed to get interface for ol module: {}", ret));
  }
  return std::make_optional(ol_interface);
}
auto OrchestrationLayer::query_provider_devices(
    QDMI_Session session, const QDMI_Provider_Interface &provider)
    -> std::optional<std::vector<QDMI_Device>> {
  size_t device_count{};
  if (const auto ret =
          provider.session_query_devices(session, 0, nullptr, &device_count);
      ret != QDMI_SUCCESS) {
    return std::nullopt;
  }
  std::vector<QDMI_Device> devices(device_count);
  if (const auto ret = provider.session_query_devices(session, device_count,
                                                      devices.data(), nullptr);
      ret != QDMI_SUCCESS) {
    return std::nullopt;
  }
  return devices;
}
auto OrchestrationLayer::query_provider_device_context(
    QDMI_Session session, const QDMI_Provider_Interface &provider,
    QDMI_Device device) -> QDMI_Context {
  QDMI_Context context{};
  if (const auto ret =
          provider.session_query_device_context(session, device, &context);
      ret != QDMI_SUCCESS) {
    return nullptr;
  }
  return context;
}
auto OrchestrationLayer::query_provider_device_interface(
    QDMI_Session session, const QDMI_Provider_Interface &provider,
    QDMI_Device device) -> const QDMI_Core_Interface & {
  const QDMI_Core_Interface *core_interface{};
  if (const auto ret = provider.session_get_device_interface(session, device,
                                                             &core_interface);
      ret != QDMI_SUCCESS) {
    throw std::runtime_error(std::format(
        "Failed to get the core interface for the device: {}", ret));
  }
  return *core_interface;
}
auto OrchestrationLayer::query_orchestration_layer_qpus(
    QDMI_Session session, const QDMI_OrchestrationLayer_Interface &ol)
    -> std::vector<QDMI_Device> {
  size_t device_count{};
  if (const auto ret =
          ol.session_query_devices(session, 0, nullptr, &device_count);
      ret != QDMI_SUCCESS) {
    throw std::runtime_error(std::format(
        "Failed to query the number of devices from the orchestration "
        "layer: {}",
        ret));
  }
  std::vector<QDMI_Device> devices(device_count);
  if (const auto ret = ol.session_query_devices(session, device_count,
                                                devices.data(), nullptr);
      ret != QDMI_SUCCESS) {
    throw std::runtime_error(std::format(
        "Failed to query the devices from the orchestration layer: {}", ret));
  }
  return devices;
}
auto OrchestrationLayer::query_orchestration_layer_qpu_context(
    QDMI_Session session, const QDMI_OrchestrationLayer_Interface &ol,
    QDMI_Device device) -> QDMI_Context {
  QDMI_Context context{};
  if (const auto ret =
          ol.session_query_device_context(session, device, &context);
      ret != QDMI_SUCCESS) {
    throw std::runtime_error(
        std::format("Failed to create a context for the device: {}", ret));
  }
  return context;
}
auto OrchestrationLayer::query_orchestration_layer_qpu_interface(
    QDMI_Session session, const QDMI_OrchestrationLayer_Interface &ol,
    QDMI_Device device) -> const QDMI_Core_Interface & {
  const QDMI_Core_Interface *core_interface{};
  if (const auto ret =
          ol.session_get_device_interface(session, device, &core_interface);
      ret != QDMI_SUCCESS) {
    throw std::runtime_error(std::format(
        "Failed to get the core interface for the device: {}", ret));
  }
  return *core_interface;
}
auto OrchestrationLayer::create_qpu_job(QDMI_Session session,
                                        const QDMI_QPU_Interface &qpu)
    -> QDMI_Job {
  QDMI_Job device_job{};
  if (const auto ret = qpu.session_create_job(session, Log_callback,
                                              logger_.get(), &device_job);
      ret != QDMI_SUCCESS) {
    throw std::runtime_error(
        std::format("Failed to create a job on the QPU: {}", ret));
  }
  return device_job;
}
auto OrchestrationLayer::set_shot_count_for_qpu_job(
    QDMI_Job job, const QDMI_QPU_Interface &qpu, const size_t shot_count)
    -> void {
  if (const auto ret = qpu.job_set_shot_count(job, shot_count);
      ret != QDMI_SUCCESS) {
    throw std::runtime_error(
        std::format("Failed to set the shot count on the QPU job: {}", ret));
  }
}
auto OrchestrationLayer::query_any_qpu_program_format(
    QDMI_Session session, const QDMI_QPU_Interface &qpu,
    const bool string_support) -> QDMI_Program_Format {
  size_t size{};
  if (const auto ret =
          qpu.session_query_program_formats(session, 0, nullptr, &size);
      ret != QDMI_SUCCESS) {
    throw std::runtime_error(
        std::format("Failed to query the number of supported program formats "
                    "from the QPU: {}",
                    ret));
  }
  std::vector<QDMI_Program_Format> formats(size);
  if (const auto ret = qpu.session_query_program_formats(
          session, size, formats.data(), nullptr);
      ret != QDMI_SUCCESS) {
    throw std::runtime_error(std::format(
        "Failed to query the supported program formats from the QPU: {}", ret));
  }
  auto format_it = std::ranges::find_if(
      formats, [&qpu, session, string_support](const auto format) -> bool {
        if (string_support) {
          return qpu.session_query_program_format_string_support(
                     session, format) == QDMI_SUCCESS;
        }
        return qpu.session_query_program_format_binary_support(
                   session, format) == QDMI_SUCCESS;
      });
  if (format_it == formats.end()) {
    throw std::runtime_error("No supported program format found for the QPU");
  }
  return *format_it;
}
auto OrchestrationLayer::set_payload_for_qpu_job(QDMI_Job job,
                                                 const QDMI_QPU_Interface &qpu,
                                                 QDMI_Program_Format format,
                                                 const Payload &payload)
    -> void {
  if (const auto ret = std::visit(
          overloaded{
              [&qpu, job,
               format](const std::vector<std::string> &payload) -> int {
                std::vector<const char *> programs{};
                programs.reserve(payload.size());
                std::ranges::transform(
                    payload, std::back_inserter(programs),
                    [](const std::string &p) { return p.c_str(); });
                return qpu.job_set_payload_string(job, format, payload.size(),
                                                  programs.data());
              },
              [&qpu, job, format](
                  const std::vector<std::vector<std::byte>> &payload) -> int {
                std::vector<const void *> programs{};
                programs.reserve(payload.size());
                std::ranges::transform(
                    payload, std::back_inserter(programs),
                    [](const std::vector<std::byte> &p) { return p.data(); });
                std::vector<size_t> sizes{};
                sizes.reserve(payload.size());
                std::ranges::transform(
                    payload, std::back_inserter(sizes),
                    [](const std::vector<std::byte> &p) { return p.size(); });
                return qpu.job_set_payload_binary(
                    job, format, payload.size(), sizes.data(), programs.data());
              }},
          payload);
      ret != QDMI_SUCCESS) {
    throw std::runtime_error(
        std::format("Failed to set the payload on the QPU job: {}", ret));
  }
}
auto OrchestrationLayer::submit_qpu_job(QDMI_Job job,
                                        const QDMI_QPU_Interface &qpu) -> void {
  if (const auto ret = qpu.job_submit(job); ret != QDMI_SUCCESS) {
    throw std::runtime_error(
        std::format("Failed to submit the job to the QPU: {}", ret));
  }
}
auto OrchestrationLayer::retrieve_qpu_job_result(QDMI_Job job,
                                                 const QDMI_QPU_Interface &qpu,
                                                 const size_t program_count)
    -> std::vector<std::vector<std::string>> {
  if (const auto ret = qpu.job_wait(job, 0); ret != QDMI_SUCCESS) {
    throw std::runtime_error(std::format(
        "Failed to wait for the job to complete on the QPU: {}", ret));
  }
  std::vector<std::vector<std::string>> results(program_count);
  for (size_t i = 0; i < program_count; ++i) {
    size_t size{};
    if (const auto ret = qpu.job_get_shots(job, i, 0, nullptr, &size);
        ret != QDMI_SUCCESS) {
      throw std::runtime_error(std::format(
          "Failed to query the size of the shot results from the QPU: {}",
          ret));
    }
    std::string bitstring(size - 1, '\0');
    if (const auto ret =
            qpu.job_get_shots(job, i, size, bitstring.data(), nullptr);
        ret != QDMI_SUCCESS) {
      throw std::runtime_error(std::format(
          "Failed to query the shot results from the QPU: {}", ret));
    }
    auto &result = results.at(i);
    std::ranges::for_each(bitstring | std::views::split(','),
                          [&result](const auto &part) {
                            result.emplace_back(part.begin(), part.end());
                          });
  }
  return results;
}
auto OrchestrationLayer::create_orchestration_layer_job(
    QDMI_Session session, const QDMI_OrchestrationLayer_Interface &ol) const
    -> QDMI_Job {
  QDMI_Job device_job{};
  if (const auto ret = ol.session_create_job(session, Log_callback,
                                             logger_.get(), &device_job);
      ret != QDMI_SUCCESS) {
    throw std::runtime_error(std::format(
        "Failed to create a job on the orchestration layer: {}", ret));
  }
  return device_job;
}
auto OrchestrationLayer::set_shot_count_for_orchestration_layer_job(
    QDMI_Job job, const QDMI_OrchestrationLayer_Interface &ol,
    size_t shot_count) -> void {
  if (const auto ret = ol.job_set_shot_count(job, shot_count);
      ret != QDMI_SUCCESS) {
    throw std::runtime_error(std::format(
        "Failed to set the shot count on the orchestration layer job: {}",
        ret));
  }
}
auto OrchestrationLayer::set_qpu_for_orchestration_layer_job(
    QDMI_Job job, const QDMI_OrchestrationLayer_Interface &ol, QDMI_Device qpu)
    -> void {
  if (const auto ret = ol.job_set_device(job, qpu); ret != QDMI_SUCCESS) {
    throw std::runtime_error(std::format(
        "Failed to set the target device on the orchestration layer job: {}",
        ret));
  }
}
auto OrchestrationLayer::query_orchestration_layer_program_format(
    QDMI_Session session, const QDMI_OrchestrationLayer_Interface &ol,
    const std::string &id, size_t version) -> QDMI_Program_Format {
  QDMI_Program_Format format{};
  if (const auto ret = ol.session_query_program_format_by_id_and_version(
          session, id.c_str(), version, &format);
      ret != QDMI_SUCCESS) {
    throw std::runtime_error(std::format(
        "Failed to query the payload program format of the orchestration "
        "layer job: {}",
        ret));
  }
  return format;
}
auto OrchestrationLayer::set_payload_for_orchestration_layer_job(
    QDMI_Job job, const QDMI_OrchestrationLayer_Interface &ol,
    QDMI_Program_Format format, const Payload &payload) -> void {
  if (const auto ret = std::visit(
          overloaded{
              [&ol, job,
               format](const std::vector<std::string> &payload) -> int {
                std::vector<const char *> programs{};
                programs.reserve(payload.size());
                std::ranges::transform(
                    payload, std::back_inserter(programs),
                    [](const std::string &p) { return p.c_str(); });
                return ol.job_set_payload_string(job, format, payload.size(),
                                                 programs.data());
              },
              [&ol, job, format](
                  const std::vector<std::vector<std::byte>> &payload) -> int {
                std::vector<const void *> programs{};
                programs.reserve(payload.size());
                std::ranges::transform(
                    payload, std::back_inserter(programs),
                    [](const std::vector<std::byte> &p) { return p.data(); });
                std::vector<size_t> sizes{};
                sizes.reserve(payload.size());
                std::ranges::transform(
                    payload, std::back_inserter(sizes),
                    [](const std::vector<std::byte> &p) { return p.size(); });
                return ol.job_set_payload_binary(job, format, payload.size(),
                                                 sizes.data(), programs.data());
              }},
          payload);
      ret != QDMI_SUCCESS) {
    throw std::runtime_error(std::format(
        "Failed to set the payload on the orchestration layer job: {}", ret));
  }
}
auto OrchestrationLayer::submit_orchestration_layer_job(
    QDMI_Job job, const QDMI_OrchestrationLayer_Interface &ol) -> void {
  if (const auto ret = ol.job_submit(job); ret != QDMI_SUCCESS) {
    throw std::runtime_error(std::format(
        "Failed to submit the job to the orchestration layer: {}", ret));
  }
}
auto OrchestrationLayer::retrieve_orchestration_layer_job_result(
    QDMI_Job job, const QDMI_OrchestrationLayer_Interface &ol,
    size_t program_count) -> std::vector<std::vector<std::string>> {
  if (const auto ret = ol.job_wait(job, 0); ret != QDMI_SUCCESS) {
    throw std::runtime_error(std::format(
        "Failed to wait for the job to complete on the orchestration layer: {}",
        ret));
  }
  std::vector<std::vector<std::string>> results(program_count);
  for (size_t i = 0; i < program_count; ++i) {
    size_t size{};
    if (const auto ret = ol.job_get_shots(job, i, 0, nullptr, &size);
        ret != QDMI_SUCCESS) {
      throw std::runtime_error(
          std::format("Failed to query the size of the shot results from the "
                      "orchestration layer: {}",
                      ret));
    }
    std::string bitstring(size - 1, '\0');
    if (const auto ret =
            ol.job_get_shots(job, i, size, bitstring.data(), nullptr);
        ret != QDMI_SUCCESS) {
      throw std::runtime_error(std::format(
          "Failed to query the shot results from the orchestration layer: {}",
          ret));
    }
    auto &result = results.at(i);
    std::ranges::for_each(bitstring | std::views::split(','),
                          [&result](const auto &part) {
                            result.emplace_back(part.begin(), part.end());
                          });
  }
  return results;
}
} // namespace qdmi::orchestration_layer

// NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
