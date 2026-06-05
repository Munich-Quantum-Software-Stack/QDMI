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
/// Main header file for the orchestration layer implementation.

#pragma once

#include "common/library_wrapper/library_wrapper.hpp"
#include "common/logger/logger.hpp"
#include "common/session_wrapper/session_wrapper.hpp"
#include "common/singleton/singleton.hpp"
#include "o_qdmi/core.h"
#include "o_qdmi/orchestration_layer.h"
#include "od_qdmi/core.h"
#include "od_qdmi/superconducting.h"
#include "qdmi/core.h"
#include "qdmi/orchestration_layer.h"
#include "qdmi/qpu.h"
#include "qdmi/superconducting.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <random>
#include <span>
#include <spdlog/details/registry.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace qdmi::orchestration_layer {
/// Instantiation of the logger with the concrete callback and log level
/// types used by the orchestration layer implementation.
using Logger = Logger<QDMI_Log_Callback, QDMI_Log_Level>;
/// @copydoc O_QDMI_Context_impl_d
using Context = O_QDMI_Context_impl_d;
/// @copydoc O_QDMI_Session_impl_d
using Session = O_QDMI_Session_impl_d;
/// @copydoc O_QDMI_Job_impl_d
using Job = O_QDMI_Job_impl_d;
/// @copydoc O_QDMI_Module_impl_d
using Module = O_QDMI_Module_impl_d;
/// @copydoc O_QDMI_Device_impl_d
using Device = O_QDMI_Device_impl_d;
/// @copydoc O_QDMI_Program_Format_impl_d
using Format = O_QDMI_Program_Format_impl_d;
/// @copydoc OD_QDMI_Context_impl_d
using DeviceContext = OD_QDMI_Context_impl_d;
/// @copydoc OD_QDMI_Session_impl_d
using DeviceSession = OD_QDMI_Session_impl_d;
/// @copydoc OD_QDMI_Module_impl_d
using DeviceModule = OD_QDMI_Module_impl_d;

/// A variant type representing the orchestration layer module interface.
///
/// Currently only holds a pointer to @ref O_QDMI_OrchestrationLayer_Interface.
using ModuleInterface =
    std::variant<const O_QDMI_OrchestrationLayer_Interface *>;
/// A variant type representing a device module interface exposed to upstream
/// consumers.
///
/// Currently only holds a pointer to @ref OD_QDMI_SC_Interface.
using DeviceModuleInterface = std::variant<const OD_QDMI_SC_Interface *>;

/// A variant type representing either a vector of strings or a vector of byte
/// vectors, used to hold the payload of a @ref O_QDMI_Job_impl_d.
using Payload =
    std::variant<std::vector<std::string>, std::vector<std::vector<std::byte>>>;
} // namespace qdmi::orchestration_layer

struct O_QDMI_Context_impl_d final {
  qdmi::orchestration_layer::Logger logger{};
};

struct O_QDMI_Session_impl_d {
  const qdmi::orchestration_layer::Context *context{};
  qdmi::orchestration_layer::Logger logger{};
  enum class STATUS : uint8_t { ALLOCATED, INITIALIZED };
  STATUS status{STATUS::ALLOCATED};
  std::string token{};
};

struct O_QDMI_Job_impl_d {
  const qdmi::orchestration_layer::Session *session{};
  qdmi::orchestration_layer::Logger logger{};
  std::string id{};
  QDMI_Job_Status status{QDMI_JOB_STATUS_CREATED};
  O_QDMI_Program_Format format{};
  O_QDMI_Device device{};
  qdmi::orchestration_layer::Payload payload{};
  size_t shot_count{};
  std::vector<std::vector<std::string>> results{};
};

struct O_QDMI_Module_impl_d {
  std::string id{};
  qdmi::orchestration_layer::ModuleInterface interface{};
};

struct O_QDMI_Device_impl_d {
  QDMI_Context context{};
  QDMI_Session session{};
  const QDMI_Core_Interface *core_interface{};
  struct QPU {
    const QDMI_QPU_Interface *interface{};
  };
  struct OrchestrationLayer {
    QDMI_Session session{};
    QDMI_Device qpu{};
    const QDMI_OrchestrationLayer_Interface *interface{};
  };
  std::variant<QPU, OrchestrationLayer> job_target{};
  const QDMI_SC_Interface *sc_interface{};
  std::string id{};
  std::unordered_map<qdmi::orchestration_layer::DeviceContext *,
                     std::unique_ptr<qdmi::orchestration_layer::DeviceContext>>
      contexts{};
  std::unordered_map<qdmi::orchestration_layer::DeviceSession *,
                     std::unique_ptr<qdmi::orchestration_layer::DeviceSession>>
      sessions{};
};

struct O_QDMI_Program_Format_impl_d {
  std::string id{};
  size_t version{};
  bool string_support{};
  bool binary_support{};
};

struct OD_QDMI_Context_impl_d final {
  qdmi::orchestration_layer::Device *device{};
  qdmi::orchestration_layer::Logger logger{};
};

struct OD_QDMI_Session_impl_d {
  qdmi::orchestration_layer::DeviceContext *context{};
  qdmi::orchestration_layer::Logger logger{};
  enum class STATUS : uint8_t { ALLOCATED, INITIALIZED };
  STATUS status{STATUS::ALLOCATED};
};

struct OD_QDMI_Module_impl_d {
  std::string id{};
  qdmi::orchestration_layer::DeviceModuleInterface interface{};
};

struct OD_QDMI_Program_Format_impl_d {
  std::string id{};
  size_t version{};
  bool string_support{};
  bool binary_support{};
};

namespace qdmi::orchestration_layer {
/// The main class representing the QDMI orchestration layer.
///
/// The orchestration layer manages a set of QDMI devices loaded from dynamic
/// libraries. It discovers QPUs by recursively traversing providers and nested
/// orchestration layers and exposes them through a unified interface.
class OrchestrationLayer final : public Singleton<OrchestrationLayer> {
  friend class Singleton;

public:
  /// @returns the unique identifier of this orchestration layer.
  [[nodiscard]] auto id() const -> const std::string &;
  /// @returns the human-readable name of this orchestration layer.
  [[nodiscard]] auto name() const -> const std::string &;
  /// @returns the implemented QDMI version in packed integer format.
  [[nodiscard]] auto version() const -> size_t;
  /// Returns the authentication options supported by this orchestration layer.
  ///
  /// @returns a span over the array of supported @ref
  /// QDMI_Authentication_Option values.
  [[nodiscard]] auto authentication_options() const
      -> std::span<const QDMI_Authentication_Option>;
  /// @returns a reference to the @ref O_QDMI_Library holding the entry-point
  /// function pointers of this orchestration layer.
  [[nodiscard]] auto library() const -> const O_QDMI_Library &;
  /// @returns a reference to the @ref O_QDMI_Core_Interface of this
  /// orchestration layer.
  [[nodiscard]] auto core_interface() const -> const O_QDMI_Core_Interface &;
  /// @returns a reference to the @ref O_QDMI_OrchestrationLayer_Interface of
  /// this orchestration layer.
  [[nodiscard]] auto orchestration_layer_interface() const
      -> const O_QDMI_OrchestrationLayer_Interface &;
  /// @returns a span over all @ref O_QDMI_Module registered with
  /// this orchestration layer.
  [[nodiscard]] auto modules() -> std::span<Module *>;
  /// Looks up a module by its string identifier.
  ///
  /// @param id is the identifier of the module to retrieve.
  ///
  /// @returns a @ref O_QDMI_Module with the given @p id. If no such module
  /// exists, returns @c NULL.
  [[nodiscard]] auto module_by_id(const std::string &id) -> Module *;
  /// @returns a span over all @ref O_QDMI_Device registered with this
  /// orchestration layer.
  [[nodiscard]] auto devices() -> std::span<Device *>;
  /// Looks up a device by its string identifier.
  ///
  /// @param id is the identifier of the device to retrieve.
  ///
  /// @returns a @ref O_QDMI_Device with the given @p id. If no such device
  /// exists, returns @c NULL.
  [[nodiscard]] auto device_by_id(const std::string &id) const -> Device *;
  /// @returns a span over all @ref O_QDMI_Format registered with
  /// this orchestration layer.
  [[nodiscard]] auto formats() -> std::span<Format *>;
  /// Looks up a program format by its string identifier and version.
  ///
  /// @param id is the identifier of the format to retrieve (e.g., `"qasm"`).
  /// @param version is the packed version number of the format.
  ///
  /// @returns the matching @ref O_QDMI_Format. If no such format exists,
  /// returns
  /// @c NULL.
  [[nodiscard]] auto format_by_id_and_version(const std::string &id,
                                              size_t version) -> Format *;

  /// Allocates and registers a new @ref O_QDMI_Context with this orchestration
  /// layer.
  ///
  /// @param logger is the logger instance to associate with the new context.
  ///
  /// @returns a @ref O_QDMI_Context. Ownership is retained by the orchestration
  /// layer; call @ref remove_context to release it.
  [[nodiscard]] auto create_new_context(Logger logger) -> Context *;
  /// Removes and destroys the given @ref O_QDMI_Context.
  ///
  /// @param context is a reference to the context to remove. Must have been
  /// created by @ref create_new_context on this orchestration layer instance.
  auto remove_context(Context &context) -> void;
  /// Allocates and registers a new @ref O_QDMI_Session within the given
  /// context.
  ///
  /// @param context is the context in which to create the session.
  /// @param logger is the logger instance to associate with the new session.
  ///
  /// @returns the newly created @ref O_QDMI_Session. Ownership is retained by
  /// the orchestration layer; call @ref remove_session to release it.
  [[nodiscard]] auto create_new_session(const Context &context, Logger logger)
      -> Session *;
  /// Removes and destroys the given @ref O_QDMI_Session.
  ///
  /// @param session is a reference to the session to remove. Must have been
  /// created by @ref create_new_session on this orchestration layer instance.
  auto remove_session(Session &session) -> void;
  /// Allocates and registers a new @ref O_QDMI_Job within the given session,
  /// assigning it a freshly generated pseudo-UUID as its identifier.
  ///
  /// @param session is the session in which to create the job.
  /// @param logger is the logger instance to associate with the new job.
  ///
  /// @returns the newly created @ref O_QDMI_Job. Ownership is retained by the
  /// orchestration layer; call @ref remove_job to release it.
  [[nodiscard]] auto create_new_job(const Session &session, Logger logger)
      -> Job *;
  /// Retrieves an existing @ref O_QDMI_Job by its string identifier,
  /// reassigning its session and logger.
  ///
  /// @param id is the UUID string of the job to look up.
  /// @param session is the session to associate with the retrieved job.
  /// @param logger is the logger to associate with the retrieved job.
  ///
  /// @returns the matching @ref O_QDMI_Job. If no such job exists, returns @c
  /// nullptr.
  [[nodiscard]] auto retrieve_job_by_id(const std::string &id,
                                        const Session &session, Logger logger)
      -> Job *;
  /// Submits and executes the given @ref O_QDMI_Job synchronously.
  ///
  /// Validates that the job is fully specified (payload, format, shot count,
  /// and target device), then transitions the job through the status sequence
  /// `SUBMITTED → QUEUED → RUNNING → DONE`. Depending on the target device
  /// type, the job is forwarded to either the underlying QPU or a nested
  /// orchestration layer. On failure, the job status is set to `FAILED`.
  ///
  /// @param job is a reference to the job to execute. Must have been created by
  /// @ref create_new_job on this orchestration layer instance.
  ///
  /// @throws std::runtime_error if the job is not fully specified or if
  /// execution fails.
  auto submit_job(Job &job) -> void;
  /// Removes and destroys the given @ref O_QDMI_Job.
  ///
  /// @param job is a reference to the job to remove. Must have been created by
  /// @ref create_new_job on this orchestration layer instance.
  auto remove_job(Job &job) -> void;
  /// @returns a span over all @ref OD_QDMI_Module exposed by this orchestration
  /// layer for downstream device queries.
  [[nodiscard]] auto device_modules() -> std::span<DeviceModule *>;
  /// Looks up a device module by its string identifier.
  ///
  /// @param id is the identifier of the device module to retrieve.
  ///
  /// @returns the @ref OD_QDMI_Module with the given @p id. If no such module
  /// exists, returns @c NULL.
  [[nodiscard]] auto device_module_by_id(const std::string &id)
      -> DeviceModule *;
  /// @returns a reference to the @ref OD_QDMI_Core_Interface used for device
  /// queries.
  [[nodiscard]] auto device_core_interface() const
      -> const OD_QDMI_Core_Interface &;

private:
  /// Private constructor; use @ref Singleton::get to obtain the instance.
  OrchestrationLayer();

  /// A tree node representing an open QDMI session and its children.
  ///
  /// Used to track the hierarchy of sessions opened during device discovery
  /// through providers and nested orchestration layers.
  struct SessionNode {
    SessionWrapper session{};
    std::vector<SessionNode> children{};
  };

  /// Discovers and registers QPUs reachable through the given QDMI context.
  ///
  /// Inspects the context's available modules (QPU, provider, or orchestration
  /// layer) and recursively traverses nested layers to collect all QPU devices.
  ///
  /// @param context is the QDMI context to inspect.
  /// @param interface is the core interface obtained from the context.
  ///
  /// @returns a @ref SessionNode representing the session opened for this
  /// context and any child sessions.
  ///
  /// @throws std::runtime_error if no supported module interface is found.
  [[nodiscard]] auto add_qpus(QDMI_Context context,
                              const QDMI_Core_Interface &interface)
      -> SessionNode;
  /// Discovers and registers QPUs exposed by a provider.
  ///
  /// @param session is the QDMI session through which to query the provider.
  /// @param provider is the provider interface to query for devices.
  ///
  /// @returns a vector of @ref SessionNode objects for each device discovered.
  [[nodiscard]] auto
  add_qpus_from_provider(QDMI_Session session,
                         const QDMI_Provider_Interface &provider)
      -> std::vector<SessionNode>;
  /// Discovers and registers QPUs exposed by a nested orchestration layer.
  ///
  /// @param session is the QDMI session through which to query the
  /// orchestration layer.
  /// @param ol is the orchestration layer interface to query for devices.
  ///
  /// @returns a vector of @ref SessionNode objects for each device discovered.
  [[nodiscard]] auto
  add_qpus_from_orchestration_layer(QDMI_Session session,
                                    const QDMI_OrchestrationLayer_Interface &ol)
      -> std::vector<SessionNode>;

  /// Generates a random pseudo-UUID v4 string.
  ///
  /// Produces a string of the form `xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx`,
  /// where each `x` is a random lowercase hexadecimal digit. The result is not
  /// a standards-conformant UUID (variant and version bits are not set), hence
  /// the "pseudo" qualifier.
  ///
  /// @returns a randomly generated UUID-shaped string.
  auto generate_pseudo_uuid_v4() -> std::string;
  /// Initializes the ID of a @ref O_QDMI_Device by querying the device's core
  /// interface.
  ///
  /// @param device is the device whose ID should be initialized.
  ///
  /// @throws std::runtime_error if the ID query fails.
  static auto init_device_id(Device &device) -> void;
  /// Initializes the superconducting interface for the given device, if it
  /// exposes one.
  ///
  /// Queries the device for an `"sc"` module and, if found, retrieves the
  /// corresponding @ref QDMI_SC_Interface. Silently skips devices that do not
  /// provide the superconducting module.
  ///
  /// @param device is the device to inspect.
  ///
  /// @throws std::runtime_error if the module is found but its interface cannot
  /// be retrieved.
  auto init_sc_interface_if_present(Device &device) const -> void;
  /// Queries a context for a QPU module interface.
  ///
  /// @param context is the QDMI context to query.
  /// @param interface is the core interface of the context.
  ///
  /// @returns the @ref QDMI_QPU_Interface pointer if the module is present, or
  /// `std::nullopt` otherwise.
  ///
  /// @throws std::runtime_error if the module is found but its interface cannot
  /// be retrieved.
  [[nodiscard]] auto
  query_qpu_interface_if_present(QDMI_Context context,
                                 const QDMI_Core_Interface &interface) const
      -> std::optional<const QDMI_QPU_Interface *>;
  /// Queries a context for a provider module interface.
  ///
  /// @param context is the QDMI context to query.
  /// @param interface is the core interface of the context.
  ///
  /// @returns the @ref QDMI_Provider_Interface pointer if the module is
  /// present, or `std::nullopt` otherwise.
  ///
  /// @throws std::runtime_error if the module is found but its interface cannot
  /// be retrieved.
  [[nodiscard]] auto query_provider_interface_if_present(
      QDMI_Context context, const QDMI_Core_Interface &interface) const
      -> std::optional<const QDMI_Provider_Interface *>;
  /// Queries a context for an orchestration layer module interface.
  ///
  /// @param context is the QDMI context to query.
  /// @param interface is the core interface of the context.
  ///
  /// @returns the @ref QDMI_OrchestrationLayer_Interface pointer if the module
  /// is present, or `std::nullopt` otherwise.
  ///
  /// @throws std::runtime_error if the module is found but its interface cannot
  /// be retrieved.
  [[nodiscard]] auto
  query_ol_interface_if_present(QDMI_Context context,
                                const QDMI_Core_Interface &interface) const
      -> std::optional<const QDMI_OrchestrationLayer_Interface *>;
  /// Queries the list of devices exposed by a provider.
  ///
  /// @param session is the QDMI session for the provider.
  /// @param provider is the provider interface to query.
  ///
  /// @returns an optional vector of @ref QDMI_Device handles. If the provider
  /// query fails, returns `std::nullopt`.
  [[nodiscard]] static auto
  query_provider_devices(QDMI_Session session,
                         const QDMI_Provider_Interface &provider)
      -> std::optional<std::vector<QDMI_Device>>;
  /// Queries the QDMI context for a specific device from a provider.
  ///
  /// @param session is the QDMI session for the provider.
  /// @param provider is the provider interface to query.
  /// @param device is the device handle to query the context for.
  ///
  /// @returns the @ref QDMI_Context for the given device. If the query fails,
  /// returns @c NULL.
  [[nodiscard]] static auto
  query_provider_device_context(QDMI_Session session,
                                const QDMI_Provider_Interface &provider,
                                QDMI_Device device) -> QDMI_Context;
  /// Queries the core interface for a specific device from a provider.
  ///
  /// @param session is the QDMI session for the provider.
  /// @param provider is the provider interface to query.
  /// @param device is the device handle to query the interface for.
  ///
  /// @returns a reference to the @ref QDMI_Core_Interface for the given device.
  [[nodiscard]] static auto query_provider_device_interface(
      QDMI_Session session, const QDMI_Provider_Interface &provider,
      QDMI_Device device) -> const QDMI_Core_Interface &;
  /// Queries the list of devices exposed by a nested orchestration layer.
  ///
  /// @param session is the QDMI session for the orchestration layer.
  /// @param ol is the orchestration layer interface to query.
  ///
  /// @returns a vector of @ref QDMI_Device handles.
  ///
  /// @throws std::runtime_error if the query fails.
  [[nodiscard]] static auto
  query_orchestration_layer_qpus(QDMI_Session session,
                                 const QDMI_OrchestrationLayer_Interface &ol)
      -> std::vector<QDMI_Device>;
  /// Queries the QDMI context for a specific device from a nested orchestration
  /// layer.
  ///
  /// @param session is the QDMI session for the orchestration layer.
  /// @param ol is the orchestration layer interface to query.
  /// @param device is the device handle to query the context for.
  ///
  /// @returns the @ref QDMI_Context for the given device.
  [[nodiscard]] static auto query_orchestration_layer_qpu_context(
      QDMI_Session session, const QDMI_OrchestrationLayer_Interface &ol,
      QDMI_Device device) -> QDMI_Context;
  /// Queries the core interface for a specific device from a nested
  /// orchestration layer.
  ///
  /// @param session is the QDMI session for the orchestration layer.
  /// @param ol is the orchestration layer interface to query.
  /// @param device is the device handle to query the interface for.
  ///
  /// @returns a reference to the @ref QDMI_Core_Interface for the given device.
  [[nodiscard]] static auto query_orchestration_layer_qpu_interface(
      QDMI_Session session, const QDMI_OrchestrationLayer_Interface &ol,
      QDMI_Device device) -> const QDMI_Core_Interface &;
  /// Executes a job on a QPU device.
  ///
  /// Creates a QPU-level job, configures its shot count and payload, submits
  /// it, and retrieves the results.
  ///
  /// @param session is the QDMI session for the QPU.
  /// @param qpu is the QPU interface to execute the job on.
  /// @param job is the orchestration layer job containing the execution
  /// parameters.
  ///
  /// @returns the measurement results as a vector of string vectors (one per
  /// program).
  ///
  /// @throws std::runtime_error if any step of the execution fails.
  [[nodiscard]] auto execute_job_on_qpu(QDMI_Session session,
                                        const QDMI_QPU_Interface &qpu,
                                        const Job &job)
      -> std::vector<std::vector<std::string>>;
  /// Executes a job on a nested orchestration layer.
  ///
  /// Creates an orchestration-layer-level job, sets the target device,
  /// configures its shot count and payload, submits it, and retrieves the
  /// results.
  ///
  /// @param session is the QDMI session for the orchestration layer.
  /// @param ol is the orchestration layer interface to execute the job on.
  /// @param qpu is the target device handle within the orchestration layer.
  /// @param job is the orchestration layer job containing the execution
  /// parameters.
  ///
  /// @returns the measurement results as a vector of string vectors (one per
  /// program).
  ///
  /// @throws std::runtime_error if any step of the execution fails.
  [[nodiscard]] auto execute_job_on_orchestration_layer(
      QDMI_Session session, const QDMI_OrchestrationLayer_Interface &ol,
      QDMI_Device qpu, const Job &job) const
      -> std::vector<std::vector<std::string>>;
  /// Creates a new job on a QPU device.
  ///
  /// @param session is the QDMI session for the QPU.
  /// @param qpu is the QPU interface on which to create the job.
  ///
  /// @returns the newly created @ref QDMI_Job handle.
  ///
  /// @throws std::runtime_error if the job creation fails.
  [[nodiscard]] auto create_qpu_job(QDMI_Session session,
                                    const QDMI_QPU_Interface &qpu) -> QDMI_Job;
  /// Sets the shot count for a QPU job.
  ///
  /// @param job is the QPU job handle.
  /// @param qpu is the QPU interface.
  /// @param shot_count is the number of measurement shots.
  ///
  /// @throws std::runtime_error if setting the shot count fails.
  static auto set_shot_count_for_qpu_job(QDMI_Job job,
                                         const QDMI_QPU_Interface &qpu,
                                         size_t shot_count) -> void;
  /// Queries the first available program format from a QPU.
  ///
  /// @param session is the QDMI session for the QPU.
  /// @param qpu is the QPU interface to query.
  /// @param string_support if `true`, only consider formats that support
  /// string, if `false`, only consider formats that support binary.
  ///
  /// @returns the matching @ref QDMI_Program_Format handle.
  ///
  /// @throws std::runtime_error if the query fails.
  [[nodiscard]] static auto
  query_any_qpu_program_format(QDMI_Session session,
                               const QDMI_QPU_Interface &qpu,
                               bool string_support) -> QDMI_Program_Format;
  /// Sets the payload for a QPU job.
  ///
  /// @param job is the QPU job handle.
  /// @param qpu is the QPU interface.
  /// @param format is the program format for the payload.
  /// @param payload is the program payload (string or binary).
  ///
  /// @throws std::runtime_error if setting the payload fails.
  static auto set_payload_for_qpu_job(QDMI_Job job,
                                      const QDMI_QPU_Interface &qpu,
                                      QDMI_Program_Format format,
                                      const Payload &payload) -> void;
  /// Submits a QPU job for execution.
  ///
  /// @param job is the QPU job handle.
  /// @param qpu is the QPU interface.
  ///
  /// @throws std::runtime_error if submission fails.
  static auto submit_qpu_job(QDMI_Job job, const QDMI_QPU_Interface &qpu)
      -> void;
  /// Retrieves the measurement results from a completed QPU job.
  ///
  /// @param job is the QPU job handle.
  /// @param qpu is the QPU interface.
  /// @param program_count is the number of programs in the job payload.
  ///
  /// @returns the measurement results as a vector of string vectors (one per
  /// program).
  ///
  /// @throws std::runtime_error if waiting for or retrieving results fails.
  [[nodiscard]] static auto
  retrieve_qpu_job_result(QDMI_Job job, const QDMI_QPU_Interface &qpu,
                          size_t program_count)
      -> std::vector<std::vector<std::string>>;
  /// Creates a new job on a nested orchestration layer.
  ///
  /// @param session is the QDMI session for the orchestration layer.
  /// @param ol is the orchestration layer interface on which to create the job.
  ///
  /// @returns the newly created @ref QDMI_Job handle.
  ///
  /// @throws std::runtime_error if the job creation fails.
  [[nodiscard]] auto create_orchestration_layer_job(
      QDMI_Session session, const QDMI_OrchestrationLayer_Interface &ol) const
      -> QDMI_Job;
  /// Sets the shot count for an orchestration layer job.
  ///
  /// @param job is the orchestration layer job handle.
  /// @param ol is the orchestration layer interface.
  /// @param shot_count is the number of measurement shots.
  ///
  /// @throws std::runtime_error if setting the shot count fails.
  static auto set_shot_count_for_orchestration_layer_job(
      QDMI_Job job, const QDMI_OrchestrationLayer_Interface &ol,
      size_t shot_count) -> void;
  /// Sets the target device for an orchestration layer job.
  ///
  /// @param job is the orchestration layer job handle.
  /// @param ol is the orchestration layer interface.
  /// @param qpu is the target device handle.
  ///
  /// @throws std::runtime_error if setting the device fails.
  static auto set_qpu_for_orchestration_layer_job(
      QDMI_Job job, const QDMI_OrchestrationLayer_Interface &ol,
      QDMI_Device qpu) -> void;
  /// Queries the program format from a nested orchestration layer by identifier
  /// and version.
  ///
  /// @param session is the QDMI session for the orchestration layer.
  /// @param ol is the orchestration layer interface to query.
  /// @param id is the format identifier (e.g., `"qasm"`).
  /// @param version is the packed version number.
  ///
  /// @returns the matching @ref QDMI_Program_Format handle.
  ///
  /// @throws std::runtime_error if the query fails.
  [[nodiscard]] static auto query_orchestration_layer_program_format(
      QDMI_Session session, const QDMI_OrchestrationLayer_Interface &ol,
      const std::string &id, size_t version) -> QDMI_Program_Format;
  /// Sets the payload for an orchestration layer job.
  ///
  /// @param job is the orchestration layer job handle.
  /// @param ol is the orchestration layer interface.
  /// @param format is the program format for the payload.
  /// @param payload is the program payload (string or binary).
  ///
  /// @throws std::runtime_error if setting the payload fails.
  static auto set_payload_for_orchestration_layer_job(
      QDMI_Job job, const QDMI_OrchestrationLayer_Interface &ol,
      QDMI_Program_Format format, const Payload &payload) -> void;
  /// Submits an orchestration layer job for execution.
  ///
  /// @param job is the orchestration layer job handle.
  /// @param ol is the orchestration layer interface.
  ///
  /// @throws std::runtime_error if submission fails.
  static auto submit_orchestration_layer_job(
      QDMI_Job job, const QDMI_OrchestrationLayer_Interface &ol) -> void;
  /// Retrieves the measurement results from a completed orchestration layer
  /// job.
  ///
  /// @param job is the orchestration layer job handle.
  /// @param ol is the orchestration layer interface.
  /// @param program_count is the number of programs in the job payload.
  ///
  /// @returns the measurement results as a vector of string vectors (one per
  /// program).
  ///
  /// @throws std::runtime_error if waiting for or retrieving results fails.
  [[nodiscard]] static auto retrieve_orchestration_layer_job_result(
      QDMI_Job job, const QDMI_OrchestrationLayer_Interface &ol,
      size_t program_count) -> std::vector<std::vector<std::string>>;

  /// The logger instance for the orchestration layer.
  std::shared_ptr<spdlog::logger> logger_{
      // NOLINTNEXTLINE(misc-include-cleaner)
      spdlog::stdout_color_mt(QDMI_DEVICE_ID)};

  /// The orchestration layer's ID.
  // NOLINTNEXTLINE(misc-include-cleaner)
  const std::string id_{QDMI_DEVICE_ID};
  /// The orchestration layer's human-readable name.
  const std::string name_{"QDMI's Example Orchestration Layer"};
  /// The implemented QDMI version.
  // NOLINTNEXTLINE(misc-include-cleaner)
  const size_t version_{QDMI_VERSION};
  /// The supported authentication options.
  const std::array<QDMI_Authentication_Option, 2> authentication_options_{
      QDMI_AUTHENTICATION_OPTION_NONE, QDMI_AUTHENTICATION_OPTION_TOKEN};

  /// The library containing the function pointers of the @ref O_QDMI_Library.
  O_QDMI_Library library_{};
  /// The core interface containing the function pointers of the @ref
  /// O_QDMI_Core_Interface.
  O_QDMI_Core_Interface core_interface_{};
  /// The orchestration layer interface containing the function pointers of the
  /// @ref O_QDMI_OrchestrationLayer_Interface.
  O_QDMI_OrchestrationLayer_Interface orchestration_layer_interface_{};
  /// The device core interface containing the function pointers of the @ref
  /// OD_QDMI_Core_Interface, used for exposing device information upstream.
  OD_QDMI_Core_Interface qpu_core_interface_{};
  /// The device superconducting interface containing the function pointers of
  /// the @ref OD_QDMI_SC_Interface, used for exposing superconducting device
  /// properties upstream.
  OD_QDMI_SC_Interface qpu_sc_interface_{};

  /// The orchestration layer module.
  Module orchestration_layer_module_{
      .id = "ol", .interface = &orchestration_layer_interface_};
  /// The modules as an array of raw pointers for fast access.
  std::array<Module *, 1> module_ptrs_{&orchestration_layer_module_};
  /// The superconducting device module.
  DeviceModule qpu_sc_module_{.id = "sc", .interface = &qpu_sc_interface_};
  /// The device modules as an array of raw pointers for fast access.
  std::array<DeviceModule *, 1> qpu_module_ptrs_{&qpu_sc_module_};

  /// The dynamically loaded libraries.
  // NOLINTNEXTLINE(misc-include-cleaner)
  std::array<LibraryWrapper, QDMI_DYN_LIBS.size()> open_libraries_{};
  /// The tree of open sessions created during device discovery.
  ///
  /// Each node holds a session and its children, mirroring the hierarchy of
  /// providers and orchestration layers traversed during initialization.
  std::vector<SessionNode> open_sessions_{};
  /// The discovered QPU devices.
  // NOLINTNEXTLINE(misc-include-cleaner)
  std::vector<Device> qpus_{};
  /// The QPU devices as a vector of raw pointers for fast sequential access.
  std::vector<Device *> qpu_ptrs_{};
  /// A map from device IDs to the corresponding @ref O_QDMI_Device_impl_d
  /// pointers.
  std::unordered_map<std::string, Device *> qpu_by_id_{};

  /// The QASMv2 program format.
  Format qasm2_format_{.id = "qasm",
                       .version = QDMI_MAKE_VERSION(2, 0, 0),
                       .string_support = true,
                       .binary_support = false};
  /// The QASMv3 program format.
  Format qasm3_format_{.id = "qasm",
                       .version = QDMI_MAKE_VERSION(3, 0, 0),
                       .string_support = true,
                       .binary_support = false};
  /// The QIRv1.1 program format.
  Format qir_format_{.id = "qir",
                     .version = QDMI_MAKE_VERSION(1, 1, 0),
                     .string_support = true,
                     .binary_support = true};
  /// The formats as an array of raw pointers for fast access.
  std::array<Format *, 3> formats_{&qasm2_format_, &qasm3_format_,
                                   &qir_format_};

  /// The random number generator used for generating pseudo-UUIDs, seeded with
  /// a constant for reproducibility.
  std::mt19937_64 gen_{4096}; // Seeded with a constant for reproducibility
  /// A uniform integer distribution for generating random hexadecimal digits
  /// (0-15).
  std::uniform_int_distribution<size_t> dis_hex_ =
      std::uniform_int_distribution<size_t>{0, 15};

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
  /// A map from pointer addresses to the corresponding `unique_ptr` objects for
  /// jobs.
  ///
  /// Allows managing job instances while using their pointer address as a
  /// handle throughout the interface.
  std::unordered_map<Job *, std::unique_ptr<Job>> jobs_{};
  /// A map from job IDs to raw pointers for fast retrieval of jobs by their ID.
  std::unordered_map<std::string, Job *> jobs_by_id_{};
};
} // namespace qdmi::orchestration_layer
