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
/// Main header file for the quantum processing unit implementation.

#pragma once

#include "common/logger/logger.hpp"
#include "common/singleton/singleton.hpp"
#include "q_qdmi/core.h"
#include "q_qdmi/qpu.h"
#include "q_qdmi/superconducting.h"

#include <array>
#include <complex>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <random>
#include <span>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

namespace qdmi::qpu {
/// Instantiation of the logger with the concrete callback and log level types
/// used by the QPU implementation.
using Logger = Logger<QDMI_Log_Callback, QDMI_Log_Level>;
/// @copydoc Q_QDMI_Context_impl_d
using Context = Q_QDMI_Context_impl_d;
/// @copydoc Q_QDMI_Session_impl_d
using Session = Q_QDMI_Session_impl_d;
/// @copydoc Q_QDMI_Job_impl_d
using Job = Q_QDMI_Job_impl_d;
/// @copydoc Q_QDMI_Module_impl_d
using Module = Q_QDMI_Module_impl_d;
/// @copydoc Q_QDMI_SCQubit_impl_d
using Qubit = Q_QDMI_SCQubit_impl_d;
/// @copydoc Q_QDMI_SCOperation_impl_d
using Operation = Q_QDMI_SCOperation_impl_d;
/// @copydoc Q_QDMI_Program_Format_impl_d
using Format = Q_QDMI_Program_Format_impl_d;
/// A variant type representing either a @ref Q_QDMI_QPU_Interface "QPU
/// interface" or a @ref Q_QDMI_SC_Interface "superconducting interface".
using ModuleInterface =
    std::variant<const Q_QDMI_QPU_Interface *, const Q_QDMI_SC_Interface *>;
/// A variant type representing either a vector of @ref Qubit pointers or a
/// vector of pairs of @ref Qubit pointers.
using QubitTuples =
    std::variant<std::vector<Qubit *>, std::vector<std::array<Qubit *, 2>>>;
/// A variant type representing either a vector of strings or a vector of byte
/// vectors, used to hold the payload of a @ref Job.
using Payload =
    std::variant<std::vector<std::string>, std::vector<std::vector<std::byte>>>;
} // namespace qdmi::qpu

struct Q_QDMI_Context_impl_d final {
  qdmi::qpu::Logger logger{};
};

struct Q_QDMI_Session_impl_d {
  const qdmi::qpu::Context *context{};
  qdmi::qpu::Logger logger{};
  enum class STATUS : uint8_t { ALLOCATED, INITIALIZED };
  STATUS status{STATUS::ALLOCATED};
  std::string token{};
};

struct Q_QDMI_Job_impl_d {
  const qdmi::qpu::Session *session{};
  qdmi::qpu::Logger logger{};
  std::string id{};
  QDMI_Job_Status status{QDMI_JOB_STATUS_CREATED};
  Q_QDMI_Program_Format format{};
  qdmi::qpu::Payload payload{};
  size_t shot_count{};
  std::vector<std::vector<std::string>> results{};
  std::vector<std::vector<std::complex<double>>> state_vecs{};
};
struct Q_QDMI_Module_impl_d {
  std::string id{};
  qdmi::qpu::ModuleInterface interface{};
};

struct Q_QDMI_SCQubit_impl_d {
  uint64_t index{};
  uint64_t coherence_t1{};
  uint64_t coherence_t2{};
};

struct Q_QDMI_SCOperation_impl_d {
  std::string id{};
  std::string name{};
  size_t parameter_count{};
  qdmi::qpu::QubitTuples qubits{};
  uint64_t duration{};
  double fidelity{};
};

struct Q_QDMI_Program_Format_impl_d {
  std::string id{};
  size_t version{};
  bool string_support{};
  bool binary_support{};
};

namespace qdmi::qpu {
/// @brief The main class representing the quantum processing unit (QPU) device.
class QPU final : public Singleton<QPU> {
  friend class Singleton;

public:
  /// @returns the unique identifier of this QPU device.
  [[nodiscard]] auto id() const -> const std::string &;
  /// @returns the human-readable name of this QPU device.
  [[nodiscard]] auto name() const -> const std::string &;
  /// @returns the implemented QDMI version in packed integer format.
  [[nodiscard]] auto version() const -> size_t;
  /// Returns the authentication options supported by this device.
  ///
  /// @returns a span over the array of supported @ref
  /// QDMI_Authentication_Option values.
  [[nodiscard]] auto authentication_options() const
      -> std::span<const QDMI_Authentication_Option>;
  /// @returns the current status of the QPU.
  [[nodiscard]] auto status() const -> QDMI_QPU_Status;
  /// Returns the duration scale factor of the device.
  ///
  /// Multiply any duration value returned by the QPU by this factor to obtain
  /// the actual duration in nanoseconds.
  ///
  /// @returns the duration scale factor.
  [[nodiscard]] auto duration_scale_factor() const -> uint64_t;
  /// @returns a reference to the @ref Q_QDMI_Library holding the entry-point
  /// function pointers of this device.
  [[nodiscard]] auto library() const -> const Q_QDMI_Library &;
  /// @returns a reference to the @ref Q_QDMI_Core_Interface of this device.
  [[nodiscard]] auto core_interface() const -> const Q_QDMI_Core_Interface &;
  /// @returns a reference to the @ref Q_QDMI_QPU_Interface of this device.
  [[nodiscard]] auto qpu_interface() const -> const Q_QDMI_QPU_Interface &;
  /// @returns a reference to the @ref Q_QDMI_SC_Interface of this device.
  [[nodiscard]] auto sc_interface() const -> const Q_QDMI_SC_Interface &;
  /// @returns the number of qubits available on the device.
  [[nodiscard]] auto qubit_count() const -> size_t;
  /// @returns a span over all @ref Module pointers registered with this device.
  [[nodiscard]] auto modules() -> std::span<Module *>;
  /// Looks up a module by its string identifier.
  ///
  /// @param id is the identifier of the module to retrieve.
  ///
  /// @returns a pointer to the @ref Module with the given @p id. If no such
  /// module exists, returns @c NULL.
  [[nodiscard]] auto module_by_id(const std::string &id) -> Module *;
  /// @returns a span over all @ref Qubit pointers registered with this device.
  [[nodiscard]] auto qubits() -> std::span<Qubit *>;
  /// Looks up a qubit by its index.
  ///
  /// @param index is the zero-based index of the qubit to retrieve.
  ///
  /// @returns a pointer to the @ref Qubit with the given @p index. If the index
  /// is out of range, returns @c NULL.
  [[nodiscard]] auto qubit_by_index(size_t index) const -> Qubit *;
  /// Returns the coupling map of the device.
  ///
  /// Each element of the returned span is a pair of @ref Qubit pointers
  /// representing a directed edge between two qubits that can directly
  /// interact.
  ///
  /// @returns a span over the coupling map.
  [[nodiscard]] auto coupling_map() const
      -> std::span<const std::array<Qubit *, 2>>;
  /// @returns a span over all @ref Operation pointers registered with this
  /// device.
  [[nodiscard]] auto operations() -> std::span<Operation *>;
  /// Looks up an operation by its string identifier.
  ///
  /// @param id is the identifier of the operation to retrieve.
  ///
  /// @returns a pointer to the @ref Operation with the given @p id. If no such
  /// operation exists, returns @c NULL.
  [[nodiscard]] auto operation_by_id(const std::string &id) -> Operation *;
  /// @returns a span over all @ref Format pointers registered with this device.
  [[nodiscard]] auto formats() -> std::span<Format *>;
  /// Looks up a program format by its string identifier and version.
  ///
  /// @param id is the identifier of the format to retrieve (e.g., `"qasm"`).
  /// @param version is the packed version number of the format.
  ///
  /// @returns a pointer to the matching @ref Format. If no such format exists,
  /// returns @c NULL.
  [[nodiscard]] auto format_by_id_and_version(const std::string &id,
                                              size_t version) -> Format *;

  /// Allocates and registers a new @ref Context with this QPU.
  ///
  /// @param logger is the logger instance to associate with the new context.
  ///
  /// @returns a raw pointer to the newly created @ref Context. Ownership is
  /// retained by the QPU; call @ref remove_context to release it.
  [[nodiscard]] auto create_new_context(Logger logger) -> Context *;
  /// Removes and destroys the given @ref Context.
  ///
  /// @param context is a pointer to the context to remove. Must have been
  /// created by @ref create_new_context on this QPU instance.
  auto remove_context(Context *context) -> void;
  /// Allocates and registers a new @ref Session within the given context.
  ///
  /// @param context is the context in which to create the session.
  /// @param logger is the logger instance to associate with the new session.
  ///
  /// @returns a raw pointer to the newly created @ref Session. Ownership is
  /// retained by the QPU; call @ref remove_session to release it.
  [[nodiscard]] auto create_new_session(Context *context, Logger logger)
      -> Session *;
  /// Removes and destroys the given @ref Session.
  ///
  /// @param session is a pointer to the session to remove. Must have been
  /// created by @ref create_new_session on this QPU instance.
  auto remove_session(Session *session) -> void;
  /// Allocates and registers a new @ref Job within the given session, assigning
  /// it a freshly generated pseudo-UUID as its identifier.
  ///
  /// @param session is the session in which to create the job.
  /// @param logger is the logger instance to associate with the new job.
  ///
  /// @returns a raw pointer to the newly created @ref Job. Ownership is
  /// retained by the QPU; call @ref remove_job to release it.
  [[nodiscard]] auto create_new_job(Session *session, Logger logger) -> Job *;
  /// Retrieves an existing @ref Job by its string identifier, reassigning its
  /// session and logger.
  ///
  /// @param id is the UUID string of the job to look up.
  /// @param session is the session to associate with the retrieved job.
  /// @param logger is the logger to associate with the retrieved job.
  ///
  /// @returns a raw pointer to the matching @ref Job. If no such job exists,
  /// returns @c NULL.
  [[nodiscard]] auto retrieve_job_by_id(const std::string &id, Session *session,
                                        Logger logger) -> Job *;
  /// Executes the given @ref Job synchronously.
  ///
  /// Transitions the job through the status sequence `SUBMITTED → QUEUED →
  /// RUNNING → DONE`, runs the simulated execution (ignoring the actual program
  /// payload), and stores the generated results and state vectors in the job.
  ///
  /// @param job is a pointer to the job to execute. Must have been created by
  /// @ref create_new_job on this QPU instance.
  auto submit_job(Job *job) -> void;
  /// Removes and destroys the given @ref Job.
  ///
  /// @param job is a pointer to the job to remove. Must have been created by
  /// @ref create_new_job on this QPU instance.
  auto remove_job(Job *job) -> void;

private:
  QPU();

  /// @returns a single random bit, drawn from a Bernoulli distribution with
  /// equal probability.
  auto generate_bit() -> bool;
  /// @returns a single random real number drawn from a uniform distribution
  /// over [-1.0, 1.0].
  auto generate_real() -> double;
  /// Generates a random pseudo-UUID v4 string.
  ///
  /// Produces a string of the form `xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx`,
  /// where each `x` is a random lowercase hexadecimal digit. The result is not
  /// a standards-conformant UUID (variant and version bits are not set), hence
  /// the "pseudo" qualifier.
  ///
  /// @returns a randomly generated UUID-shaped string.
  auto generate_pseudo_uuid_v4() -> std::string;

  /// Simulates execution of a quantum circuit for the given number of shots.
  ///
  /// The actual program payload is ignored. Instead, random bitstrings (one per
  /// shot) and a single random normalized complex state vector are generated.
  /// Sets @ref status_ to `BUSY` during execution and back to `IDLE` on
  /// completion.
  ///
  /// @param shot_count is the number of measurement shots to simulate.
  ///
  /// @returns a pair of (state vector, list of bitstring results).
  auto execute(size_t shot_count)
      -> std::pair<std::vector<std::complex<double>>, std::vector<std::string>>;

  /// Generates @p n random measurement bitstrings, one per shot.
  ///
  /// @param n is the number of shots to generate.
  ///
  /// @returns a vector of @p n bitstrings, each of length equal to the qubit
  /// count of this device.
  [[nodiscard]] auto generate_n_shots(size_t n) -> std::vector<std::string>;
  /// Generates a random normalized complex state vector of dimension 2^(qubit
  /// count).
  ///
  /// @param n is the number of amplitudes to generate (i.e., 2^(qubit count)).
  ///
  /// @returns a normalized vector of @p n complex amplitudes.
  [[nodiscard]] auto generate_n_state_vector(size_t n)
      -> std::vector<std::complex<double>>;

  /// The QPU's ID
  // NOLINTNEXTLINE(misc-include-cleaner)
  const std::string id_{QDMI_DEVICE_ID};
  /// The QPU's human-readable name
  const std::string name_{"QDMI's Example Superconducting QPU"};
  /// The implemented QDMI version
  // NOLINTNEXTLINE(misc-include-cleaner)
  const size_t version_{QDMI_VERSION};
  /// The supported authentication options
  const std::array<QDMI_Authentication_Option, 2> authentication_options_{
      QDMI_AUTHENTICATION_OPTION_NONE, QDMI_AUTHENTICATION_OPTION_TOKEN};
  /// The current status of the QPU
  QDMI_QPU_Status status_ = QDMI_QPU_STATUS_IDLE;
  /// The factor by which all durations returned by the QPU should be scaled to
  /// obtain the actual duration in nanoseconds.
  ///
  /// @see Q_QDMI_session_query_sc_duration_scale_factor
  uint64_t duration_scale_factor_ = 10;

  /// The library containing the function pointers of the @ref Q_QDMI_Library.
  Q_QDMI_Library library_{};
  /// The core interface containing the function pointers of the @ref
  /// Q_QDMI_Core_Interface.
  Q_QDMI_Core_Interface core_interface_{};
  /// The QPU interface containing the function pointers of the @ref
  /// Q_QDMI_QPU_Interface.
  Q_QDMI_QPU_Interface qpu_interface_{};
  /// The superconducting module interface, containing the function pointers of
  /// the @ref Q_QDMI_SC_Interface.
  Q_QDMI_SC_Interface sc_interface_{};

  /// The QPU module.
  Module qpu_module_{.id = "qpu", .interface = &qpu_interface_};
  /// The superconducting module.
  Module sc_module_{.id = "sc", .interface = &sc_interface_};
  /// The modules as an array of raw pointers for fast access.
  std::array<Module *, 2> module_ptrs_{&qpu_module_, &sc_module_};
  /// The QPU's qubits.
  std::array<Qubit, 5> qubits_{
      Qubit{.index = 0, .coherence_t1 = 10000, .coherence_t2 = 100000},
      Qubit{.index = 1, .coherence_t1 = 10000, .coherence_t2 = 100000},
      Qubit{.index = 2, .coherence_t1 = 10000, .coherence_t2 = 100000},
      Qubit{.index = 3, .coherence_t1 = 10000, .coherence_t2 = 100000},
      Qubit{.index = 4, .coherence_t1 = 10000, .coherence_t2 = 100000},
  };
  /// The qubits as an array of raw pointers for fast access.
  std::array<Qubit *, 5> qubit_ptrs_{&qubits_.at(0), &qubits_.at(1),
                                     &qubits_.at(2), &qubits_.at(3),
                                     &qubits_.at(4)};
  /// The coupling map of the device, represented as an array of pairs of qubits
  /// that can directly interact with each other.
  ///
  /// The device features the following coupling map
  /// ```
  ///            ┌─────┐
  ///            │  1  │
  ///            └──┬──┘
  /// ┌─────┐    ┌──┴──┐    ┌─────┐
  /// │  2  ├────┤  0  ├────┤  3  │
  /// └─────┘    └──┬──┘    └─────┘
  ///            ┌──┴──┐
  ///            │  4  │
  ///            └─────┘
  /// ```
  std::array<std::array<Qubit *, 2>, 4> coupling_map_{
      std::array{&qubits_.at(0), &qubits_.at(1)},
      std::array{&qubits_.at(0), &qubits_.at(2)},
      std::array{&qubits_.at(0), &qubits_.at(3)},
      std::array{&qubits_.at(0), &qubits_.at(4)},
  };
  /// The QPU's RZ operation.
  Operation rz_operation_{
      .id = "rz",
      .name = "Z-Rotation",
      .parameter_count = 1,
      .qubits = std::vector(qubit_ptrs_.cbegin(), qubit_ptrs_.cend()),
      .duration = 300,
      .fidelity = 0.9999};
  /// The QPU's CX operation.
  Operation cx_operation_{
      .id = "cx",
      .name = "Controlled X-Gate",
      .parameter_count = 2,
      .qubits = std::vector(coupling_map_.cbegin(), coupling_map_.cend()),
      .duration = 100,
      .fidelity = 0.999};
  /// The operations as an array of raw pointers for fast access.
  std::array<Operation *, 2> operation_ptrs_{&rz_operation_, &cx_operation_};
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

  /// The random number generator used for simulating job execution results,
  /// seeded with a constant for reproducibility.
  // NOLINTNEXTLINE(cert-msc51-cpp) Seeded with a constant for reproducibility
  std::mt19937_64 gen_{4096};
  /// A uniform integer distribution for generating random hexadecimal digits
  /// (0-15).
  std::uniform_int_distribution<size_t> dis_hex_ =
      std::uniform_int_distribution<size_t>(0, 15);
  /// A Bernoulli distribution for generating random bits (0 or 1) with equal
  /// probability.
  std::bernoulli_distribution dis_bin_{0.5};
  /// A uniform real distribution for generating random real numbers in the
  /// range [-1.0, 1.0].
  std::uniform_real_distribution<> dis_real_ =
      std::uniform_real_distribution<>(-1.0, 1.0);

  /// A map from pointer addresses to the corresponding `unique_ptr` object for
  /// contexts.
  ///
  /// This map allows for managing instances of contexts while using their
  /// pointer address as a handle throughout the interface.
  std::unordered_map<Context *, std::unique_ptr<Context>> contexts_{};
  /// A map from pointer addresses to the corresponding `unique_ptr` object for
  /// sessions.
  ///
  /// This map allows for managing instances of sessions while using their
  /// pointer address as a handle throughout the interface.
  std::unordered_map<Session *, std::unique_ptr<Session>> sessions_{};
  /// A map from pointer addresses to the corresponding `unique_ptr` object for
  /// jobs.
  ///
  /// This map allows for managing instances of jobs while using their pointer
  /// address as a handle throughout the interface.
  std::unordered_map<Job *, std::unique_ptr<Job>> jobs_{};
  /// A map from job IDs to raw pointers for fast retrieval of jobs by their ID.
  std::unordered_map<std::string, Job *> jobs_by_id_{};
};

/// Sorts the qubit tuples in-place to ensure a canonical order for fast binary
/// search afterward.
///
/// @param qubit_tuples The qubit tuples to sort.
///
/// @see Is_one_of
auto Sort_qubit_tuples(QubitTuples &qubit_tuples) -> void;

/// Checks if the given qubits are contained in the provided qubit tuples.
///
/// @param qubits The qubits to check for membership in the qubit tuples.
/// @param qubit_tuples The qubit tuples to check against. Must be sorted using
/// @ref Sort_qubit_tuples before calling this function.
///
/// @returns `true` if the qubits are contained in the qubit tuples, `false`
/// otherwise.
///
/// @attention The qubit tuples must be sorted using @ref Sort_qubit_tuples
/// before calling this function, otherwise the behavior is undefined.
[[nodiscard]] auto Is_one_of(const std::span<Qubit *const> &qubits,
                             const QubitTuples &qubit_tuples) -> bool;

/// Computes the arity of the qubit tuples, i.e., the number of qubits in each
/// tuple.
///
/// @param qubit_tuples The qubit tuples for which to compute the arity.
///
/// @returns The arity of the qubit tuples.
[[nodiscard]] auto Qubit_tuple_arity(const QubitTuples &qubit_tuples) -> size_t;

/// Computes the total number of qubits contained in the qubit tuples.
///
/// @param qubit_tuples The qubit tuples for which to compute the total number
/// of qubits.
///
/// @returns The total number of qubits contained in the qubit tuples.
[[nodiscard]] auto Qubit_tuple_size(const QubitTuples &qubit_tuples) -> size_t;

/// Computes the total number of qubits contained in the qubit tuples.
///
/// For single-qubit tuples, this is simply the size of the vector. For
/// two-qubit tuples, this is twice the size of the vector (since each tuple
/// contains two qubits).
///
/// @param qubit_tuples The qubit tuples for which to compute the total number
/// of qubits.
///
/// @returns The total number of qubits contained in the qubit tuples.
[[nodiscard]] auto Sizeof_qubit_tuple(const QubitTuples &qubit_tuples)
    -> size_t;

} // namespace qdmi::qpu
