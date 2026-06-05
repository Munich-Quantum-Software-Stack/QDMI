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

#pragma once

#include "ad_qdmi/qpu.h"
#include "ad_qdmi/superconducting.h"
#include "adapter/v1_library.hpp"
#include "common/logger/logger.hpp"
#include "common/singleton/singleton.hpp"
#include "v2_qdmi/provider.h"

#include <span>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <string>

namespace qdmi::adapter {
/// Instantiation of the logger with the concrete callback and log level types
/// used by the QPU implementation.
using Logger = Logger<QDMI_Log_Callback, QDMI_Log_Level>;
/// @copydoc V2_QDMI_Context_impl_d
using Context = V2_QDMI_Context_impl_d;
/// @copydoc V2_QDMI_Session_impl_d
using Session = V2_QDMI_Session_impl_d;
/// @copydoc V2_QDMI_Module_impl_d
using Module = V2_QDMI_Module_impl_d;
/// @copydoc V2_QDMI_Device_impl_d
using Device = V2_QDMI_Device_impl_d;
/// @copydoc AD_QDMI_Context_impl_d
using DeviceContext = AD_QDMI_Context_impl_d;
/// @copydoc AD_QDMI_Session_impl_d
using DeviceSession = AD_QDMI_Session_impl_d;
/// @copydoc AD_QDMI_Module_impl_d
using DeviceModule = AD_QDMI_Module_impl_d;
/// @copydoc AD_QDMI_Job_impl_d
using DeviceJob = AD_QDMI_Job_impl_d;
/// @copydoc AD_QDMI_SCQubit_impl_d
using DeviceQubit = AD_QDMI_SCQubit_impl_d;
/// @copydoc AD_QDMI_SCOperation_impl_d
using DeviceOperation = AD_QDMI_SCOperation_impl_d;
/// @copydoc AD_QDMI_Program_Format_impl_d
using DeviceFormat = AD_QDMI_Program_Format_impl_d;

using ModuleInterface = std::variant<const V2_QDMI_Provider_Interface *>;
using DeviceModuleInterface =
    std::variant<const AD_QDMI_QPU_Interface *, const AD_QDMI_SC_Interface *>;
/// A variant type representing either a vector of @ref Qubit pointers or a
/// vector of pairs of @ref Qubit pointers.
using QubitTuples = std::variant<std::vector<DeviceQubit *>,
                                 std::vector<std::array<DeviceQubit *, 2>>>;
/// A variant type representing either a vector of strings or a vector of byte
/// vectors, used to hold the payload of a @ref Job.
using Payload =
    std::variant<std::vector<std::string>, std::vector<std::vector<std::byte>>>;
} // namespace qdmi::adapter

struct V2_QDMI_Context_impl_d final {
  qdmi::adapter::Logger logger{};
};

struct V2_QDMI_Session_impl_d {
  const qdmi::adapter::Context *context{};
  qdmi::adapter::Logger logger{};
  enum class STATUS : uint8_t { ALLOCATED, INITIALIZED };
  STATUS status{STATUS::ALLOCATED};
  std::string token{};
};

struct V2_QDMI_Module_impl_d {
  std::string id{};
  qdmi::adapter::ModuleInterface interface{};
};

struct V2_QDMI_Device_impl_d {
  std::string id{};
  std::string name{}; // "Example Adapter of a QDMIv1 Device"
  // NOLINTNEXTLINE(misc-include-cleaner)
  qdmi::adapter::V1Library v1_library{};
  std::optional<std::unordered_map<
      std::string,
      std::unordered_map<size_t, std::unique_ptr<qdmi::adapter::DeviceFormat>>>>
      formats{};
  std::optional<std::unordered_map<qdmi::adapter::V1_PROGRAM_FORMAT,
                                   qdmi::adapter::DeviceFormat *>>
      formats_v1{};
  std::optional<std::vector<qdmi::adapter::DeviceFormat *>> format_ptrs{};
  std::unordered_map<qdmi::adapter::DeviceContext *,
                     std::unique_ptr<qdmi::adapter::DeviceContext>>
      contexts{};
  std::unordered_map<qdmi::adapter::DeviceSession *,
                     std::unique_ptr<qdmi::adapter::DeviceSession>>
      sessions{};
  std::unordered_map<qdmi::adapter::DeviceJob *,
                     std::unique_ptr<qdmi::adapter::DeviceJob>>
      jobs{};
  std::unordered_map<std::string, qdmi::adapter::DeviceJob *> jobs_by_id{};
};

struct AD_QDMI_Context_impl_d final {
  qdmi::adapter::Device *device{};
  qdmi::adapter::Logger logger{};
};

struct AD_QDMI_Session_impl_d {
  const qdmi::adapter::DeviceContext *context{};
  qdmi::adapter::Logger logger{};
  qdmi::adapter::V1Session v1_session{};
  enum class STATUS : uint8_t { ALLOCATED, INITIALIZED };
  STATUS status{STATUS::ALLOCATED};
};

struct AD_QDMI_Job_impl_d {
  const qdmi::adapter::DeviceSession *session{};
  qdmi::adapter::Logger logger{};
  qdmi::adapter::V1Job job_v1{};
  std::string id{};
};

struct AD_QDMI_Module_impl_d {
  std::string id{};
  qdmi::adapter::DeviceModuleInterface interface{};
};

struct AD_QDMI_Program_Format_impl_d {
  std::string id{};
  size_t version{};
  std::optional<qdmi::adapter::V1_PROGRAM_FORMAT> string_support{};
  std::optional<qdmi::adapter::V1_PROGRAM_FORMAT> binary_support{};
};

namespace qdmi::adapter {
class Adapter final : public Singleton<Adapter> {
  friend class Singleton;

public:
  /// @returns the unique identifier of this provider.
  [[nodiscard]] auto id() const -> const std::string &;
  /// @returns the human-readable name of this provider.
  [[nodiscard]] auto name() const -> const std::string &;
  /// @returns the implemented QDMI version in packed integer format.
  [[nodiscard]] auto version() const -> size_t;
  /// Returns the authentication options supported by this provider.
  ///
  /// @returns a span over the array of supported @ref
  /// QDMI_Authentication_Option values.
  [[nodiscard]] auto authentication_options() const
      -> std::span<const QDMI_Authentication_Option>;
  /// @returns a reference to the @ref Q_QDMI_Library holding the entry-point
  /// function pointers of this device.
  [[nodiscard]] auto library() const -> const V2_QDMI_Library &;
  /// @returns a reference to the @ref Q_QDMI_Core_Interface of this device.
  [[nodiscard]] auto core_interface() const -> const V2_QDMI_Core_Interface &;
  /// @returns a reference to the @ref V2_QDMI_Provider_Interface of this
  /// provider.
  [[nodiscard]] auto provider_interface() const
      -> const V2_QDMI_Provider_Interface &;
  /// @returns a span over all @ref Module pointers registered with this device.
  [[nodiscard]] auto modules() -> std::span<Module *>;
  /// Looks up a module by its string identifier.
  ///
  /// @param id is the identifier of the module to retrieve.
  ///
  /// @returns a pointer to the @ref Module with the given @p id. If no such
  /// module exists, returns @c NULL.
  [[nodiscard]] auto module_by_id(const std::string &id) -> Module *;
  /// @returns a span over all @ref Device pointers registered with this
  /// provider.
  [[nodiscard]] auto devices() -> std::span<Device *>;
  /// @returns a span over all @ref Format pointers registered with this device.
  /// Looks up a device by its string identifier.
  ///
  /// @param id is the identifier of the device to retrieve.
  ///
  /// @returns a pointer to the @ref Device with the given @p id. If no such
  /// device exists, returns @c NULL.
  [[nodiscard]] auto device_by_id(const std::string &id) const -> Device *;
  /// Allocates and registers a new @ref Context with this provider.
  ///
  /// @param logger is the logger instance to associate with the new context.
  ///
  /// @returns a raw pointer to the newly created @ref Context. Ownership is
  /// retained by the QPU; call @ref remove_context to release it.
  [[nodiscard]] auto create_new_context(Logger logger) -> Context *;
  /// Removes and destroys the given @ref Context.
  ///
  /// @param context is a pointer to the context to remove. Must have been
  /// created by @ref create_new_context on this provider instance.
  auto remove_context(Context *context) -> void;
  /// Allocates and registers a new @ref Session within the given context.
  ///
  /// @param context is the context in which to create the session.
  /// @param logger is the logger instance to associate with the new session.
  ///
  /// @returns a raw pointer to the newly created @ref Session. Ownership is
  /// retained by the provider; call @ref remove_session to release it.
  [[nodiscard]] auto create_new_session(Context *context, Logger logger)
      -> Session *;
  /// Removes and destroys the given @ref Session.
  ///
  /// @param session is a pointer to the session to remove. Must have been
  /// created by @ref create_new_session on this provider instance.
  auto remove_session(Session *session) -> void;

  [[nodiscard]] auto device_core_interface() const
      -> const AD_QDMI_Core_Interface &;
  [[nodiscard]] auto device_modules() -> std::span<DeviceModule *>;
  [[nodiscard]] auto device_module_by_id(const std::string &id)
      -> DeviceModule *;

private:
  Adapter();

  // NOLINTNEXTLINE(misc-include-cleaner)
  std::shared_ptr<spdlog::logger> logger_{
      spdlog::stdout_color_mt(QDMI_DEVICE_ID)};

  /// The provider's ID.
  // NOLINTNEXTLINE(misc-include-cleaner)
  std::string id_{QDMI_DEVICE_ID};
  /// The provider's human-readable name.
  std::string name_{"QDMI's Example Provider"};
  /// The implemented QDMI version.
  // NOLINTNEXTLINE(misc-include-cleaner)
  size_t version_{QDMI_VERSION};
  /// The supported authentication options.
  std::array<QDMI_Authentication_Option, 2> authentication_options_{
      QDMI_AUTHENTICATION_OPTION_NONE, QDMI_AUTHENTICATION_OPTION_TOKEN};

  /// The library containing the function pointers of the @ref Q_QDMI_Library.
  V2_QDMI_Library library_{};
  /// The core interface containing the function pointers of the @ref
  /// Q_QDMI_Core_Interface.
  V2_QDMI_Core_Interface core_interface_{};
  /// The provider interface containing the function pointers of the @ref
  /// V2_QDMI_Provider_Interface.
  V2_QDMI_Provider_Interface provider_interface_{};
  /// The provider module.
  Module provider_module_{.id = "provider", .interface = &provider_interface_};
  /// The modules as an array of raw pointers for fast access.
  std::array<Module *, 1> module_ptrs_{&provider_module_};
  /// The devices registered with this provider.
  // NOLINTNEXTLINE(misc-include-cleaner)
  std::array<Device, QDMI_DYN_LIBS.size()> devices_{};
  /// The devices as a vector of raw pointers for fast sequential access.
  std::array<Device *, QDMI_DYN_LIBS.size()> device_ptrs_{};
  /// A map from device IDs to the corresponding `Device` pointers.
  std::unordered_map<std::string, Device *> device_by_id_{};

  AD_QDMI_Core_Interface device_core_interface_{};
  /// The QPU interface containing the function pointers of the @ref
  /// AD_QDMI_QPU_Interface.
  AD_QDMI_QPU_Interface device_qpu_interface_{};
  /// The superconducting module interface, containing the function pointers of
  /// the @ref AD_QDMI_SC_Interface.
  AD_QDMI_SC_Interface device_sc_interface_{};
  /// The QPU module.
  DeviceModule device_qpu_module_{.id = "qpu",
                                  .interface = &device_qpu_interface_};
  /// The superconducting module.
  DeviceModule device_sc_module_{.id = "sc",
                                 .interface = &device_sc_interface_};
  /// The modules as an array of raw pointers for fast access.
  std::array<DeviceModule *, 2> device_module_ptrs_{&device_qpu_module_,
                                                    &device_sc_module_};

  /// A map from pointer addresses to the corresponding `unique_ptr` objects for
  /// contexts.
  ///
  /// Allows managing context instances while using their pointer address as a
  /// handle throughout the interface.
  std::unordered_map<Context *, std::unique_ptr<Context>> contexts_{};
  /// A map from pointer addresses to the corresponding `unique_ptr` objects for
  /// sessions.
  ///
  /// Allows managing session instances while using their pointer address as a
  /// handle throughout the interface.
  std::unordered_map<Session *, std::unique_ptr<Session>> sessions_{};
};

auto Ensure_formats_are_initialized(const DeviceSession &session) -> bool;
auto Init_formats(const DeviceSession &session) -> bool;
auto Init_format_ptrs(const DeviceSession &session) -> void;
[[nodiscard]] auto Query_formats_v1(const DeviceSession &session)
    -> std::optional<std::vector<V1_PROGRAM_FORMAT>>;
[[nodiscard]] auto Transform_return_codes(int code) -> QDMI_STATUS;
} // namespace qdmi::adapter
