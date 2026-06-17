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

/// @file
/// Main header file for the provider implementation.

#pragma once

#include "common/library_wrapper/library_wrapper.hpp"
#include "common/logger/logger.hpp"
#include "common/singleton/singleton.hpp"
#include "p_qdmi/core.h"
#include "p_qdmi/provider.h"
#include "qdmi/core.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>
#include <spdlog/details/registry.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <string>
#include <unordered_map>
#include <variant>

namespace qdmi::provider {
/// Instantiation of the logger with the concrete callback and log level types
/// used by the provider implementation.
using Logger = Logger<QDMI_Log_Callback, QDMI_Log_Level>;
/// @copydoc P_QDMI_Context_impl_d
using Context = P_QDMI_Context_impl_d;
/// @copydoc P_QDMI_Session_impl_d
using Session = P_QDMI_Session_impl_d;
/// @copydoc P_QDMI_Module_impl_d
using Module = P_QDMI_Module_impl_d;
/// @copydoc P_QDMI_Device_impl_d
using Device = P_QDMI_Device_impl_d;

/// A variant type representing the provider module interface.
/// Currently only holds a pointer to @ref P_QDMI_Provider_Interface.
using ModuleInterface = std::variant<const P_QDMI_Provider_Interface *>;
} // namespace qdmi::provider

struct P_QDMI_Context_impl_d final {
  qdmi::provider::Logger logger{};
};

struct P_QDMI_Session_impl_d {
  const qdmi::provider::Context *context{};
  qdmi::provider::Logger logger{};
  enum class STATUS : uint8_t { ALLOCATED, INITIALIZED };
  STATUS status{STATUS::ALLOCATED};
  std::string token{};
  std::vector<qdmi::LibraryWrapper> open_libraries{};
};

struct P_QDMI_Module_impl_d {
  std::string id{};
  qdmi::provider::ModuleInterface interface{};
};

struct P_QDMI_Device_impl_d {
  qdmi::LibraryWrapper library{};
  const QDMI_Core_Interface *core_interface{};
};

namespace qdmi::provider {
/// The main class representing the QDMI provider.
///
/// The provider manages a set of QDMI devices loaded from dynamic libraries.
class Provider final : public Singleton<Provider> {
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
  /// @returns a reference to the @ref P_QDMI_Library holding the entry-point
  /// function pointers of this provider.
  [[nodiscard]] auto library() const -> const P_QDMI_Library &;
  /// @returns a reference to the @ref P_QDMI_Core_Interface of this provider.
  [[nodiscard]] auto core_interface() const -> const P_QDMI_Core_Interface &;
  /// @returns a reference to the @ref P_QDMI_Provider_Interface of this
  /// provider.
  [[nodiscard]] auto provider_interface() const
      -> const P_QDMI_Provider_Interface &;
  /// @returns a span over all @ref Module pointers registered with this
  /// provider.
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
  /// retained by the provider; call @ref remove_context to release it.
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

private:
  /// Private constructor; use @ref Singleton::get to obtain the instance.
  Provider();

  /// Helper function to query the device ID from a given @ref Device instance.
  ///
  /// @param device is the @ref Device instance from which to query the ID.
  ///
  /// @returns the device ID as a string if the query is successful, or @c
  /// std::nullopt if it fails.
  [[nodiscard]] static auto query_device_id(const Device &device)
      -> std::optional<std::string>;

  /// Helper function to initialize the provider module interface for a given
  /// @ref Device instance.
  ///
  /// @param device is the @ref Device instance for which to initialize the
  /// interface.
  static auto initialize_device_interface(Device &device) -> void;

  /// The logger instance for the provider.
  // NOLINTNEXTLINE(misc-include-cleaner)
  std::shared_ptr<spdlog::logger> logger_{
      spdlog::stdout_color_mt(QDMI_DEVICE_ID)};

  /// The provider's ID.
  // NOLINTNEXTLINE(misc-include-cleaner)
  const std::string id_{QDMI_DEVICE_ID};
  /// The provider's human-readable name.
  const std::string name_{"QDMI's Example Provider"};
  /// The implemented QDMI version.
  // NOLINTNEXTLINE(misc-include-cleaner)
  const size_t version_{QDMI_VERSION};
  /// The supported authentication options.
  const std::array<QDMI_Authentication_Option, 2> authentication_options_{
      QDMI_AUTHENTICATION_OPTION_NONE, QDMI_AUTHENTICATION_OPTION_TOKEN};

  /// The library containing the function pointers of the @ref P_QDMI_Library.
  P_QDMI_Library library_{};
  /// The core interface containing the function pointers of the @ref
  /// P_QDMI_Core_Interface.
  P_QDMI_Core_Interface core_interface_{};
  /// The provider interface containing the function pointers of the @ref
  /// P_QDMI_Provider_Interface.
  P_QDMI_Provider_Interface provider_interface_{};

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
} // namespace qdmi::provider
