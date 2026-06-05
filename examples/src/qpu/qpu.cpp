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

#include "qpu/qpu.hpp"

#include "q_qdmi/core.h"
#include "q_qdmi/qpu.h"
#include "q_qdmi/superconducting.h"
#include "qpu/core_bindings.hpp"
#include "qpu/qpu_bindings.hpp"
#include "qpu/superconducting_bindings.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <complex>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iterator>
#include <memory>
#include <numeric>
#include <random>
#include <span>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

namespace qdmi::qpu {
namespace {
template <class... Ts> struct overloaded : Ts... {
  using Ts::operator()...;
};
} // namespace
auto QPU::id() const -> const std::string & { return id_; }
auto QPU::name() const -> const std::string & { return name_; }
auto QPU::version() const -> size_t { return version_; }
auto QPU::authentication_options() const
    -> std::span<const QDMI_Authentication_Option> {
  return authentication_options_;
}
auto QPU::status() const -> QDMI_QPU_Status { return status_; }
auto QPU::duration_scale_factor() const -> uint64_t {
  return duration_scale_factor_;
}
auto QPU::library() const -> const Q_QDMI_Library & { return library_; }
auto QPU::core_interface() const -> const Q_QDMI_Core_Interface & {
  return core_interface_;
}
auto QPU::qpu_interface() const -> const Q_QDMI_QPU_Interface & {
  return qpu_interface_;
}
auto QPU::sc_interface() const -> const Q_QDMI_SC_Interface & {
  return sc_interface_;
}
auto QPU::qubit_count() const -> size_t { return qubits_.size(); }
auto QPU::modules() -> std::span<Module *> { return module_ptrs_; }
auto QPU::module_by_id(const std::string &id) -> Module * {
  if (id == "qpu") {
    return &qpu_module_;
  }
  if (id == "sc") {
    return &sc_module_;
  }
  return nullptr;
}
auto QPU::qubits() -> std::span<Qubit *> {
  return {qubit_ptrs_.data(), qubit_ptrs_.size()};
}
auto QPU::qubit_by_index(const size_t index) const -> Qubit * {
  if (index >= qubit_ptrs_.size()) {
    return nullptr;
  }
  return qubit_ptrs_.at(index);
}
auto QPU::coupling_map() const -> std::span<const std::array<Qubit *, 2>> {
  return {coupling_map_.data(), coupling_map_.size()};
}
auto QPU::operations() -> std::span<Operation *> {
  return {operation_ptrs_.data(), operation_ptrs_.size()};
}
auto QPU::operation_by_id(const std::string &id) -> Operation * {
  if (id == "rz") {
    return &rz_operation_;
  }
  if (id == "cx") {
    return &cx_operation_;
  }
  return nullptr;
}
auto QPU::formats() -> std::span<Format *> { return formats_; }
auto QPU::format_by_id_and_version(const std::string &id, size_t version)
    -> Format * {
  if (id == "qasm") {
    if (version == QDMI_MAKE_VERSION(2, 0, 0)) {
      return &qasm2_format_;
    }
    if (version == QDMI_MAKE_VERSION(3, 0, 0)) {
      return &qasm3_format_;
    }
  }
  if (id == "qir") {
    if (version == QDMI_MAKE_VERSION(1, 1, 0)) {
      return &qir_format_;
    }
  }
  return nullptr;
}
auto QPU::create_new_context(Logger logger) -> Context * {
  auto context = std::make_unique<Context>(logger);
  auto *context_ptr =
      contexts_.emplace(context.get(), std::move(context)).first->first;
  return context_ptr;
}
auto QPU::remove_context(Context *context) -> void { contexts_.erase(context); }
auto QPU::create_new_session(Context *context, Logger logger) -> Session * {
  auto session = std::make_unique<Session>(context, logger);
  auto *session_ptr =
      sessions_.emplace(session.get(), std::move(session)).first->first;
  return session_ptr;
}
auto QPU::remove_session(Session *session) -> void { sessions_.erase(session); }
auto QPU::create_new_job(Session *session, Logger logger) -> Job * {
  auto id = generate_pseudo_uuid_v4();
  auto job = std::make_unique<Job>(session, logger, std::move(id));
  auto *job_ptr = jobs_.emplace(job.get(), std::move(job)).first->first;
  jobs_by_id_.emplace(job_ptr->id, job_ptr);
  return job_ptr;
}
auto QPU::retrieve_job_by_id(const std::string &id, Session *session,
                             const Logger logger) -> Job * {
  const auto it = jobs_by_id_.find(id);
  if (it == jobs_by_id_.end()) {
    return nullptr;
  }
  auto &job = *it->second;
  job.session = session;
  job.logger = logger;
  return it->second;
}
auto QPU::submit_job(Job *job) -> void {
  job->status = QDMI_JOB_STATUS_SUBMITTED;
  job->status = QDMI_JOB_STATUS_QUEUED;
  job->status = QDMI_JOB_STATUS_RUNNING;
  auto &results = job->results;
  results.clear();
  auto &state_vecs = job->state_vecs;
  state_vecs.clear();
  std::visit(
      [this, &results, &state_vecs, shot_count = job->shot_count](
          [[maybe_unused]] const auto &payload) -> void {
        // We deliberately ignore the actual program for this example
        // implementation.
        auto [state_vec, result] = execute(shot_count);
        results.emplace_back(std::move(result));
        state_vecs.emplace_back(std::move(state_vec));
      },
      job->payload);
  job->status = QDMI_JOB_STATUS_DONE;
}
auto QPU::remove_job(Job *job) -> void {
  jobs_by_id_.erase(job->id);
  jobs_.erase(job);
}
QPU::QPU()
    : library_(Get_library()), core_interface_(Get_core_interface()),
      qpu_interface_{Get_qpu_interface()}, sc_interface_{Get_sc_interface()} {}
auto QPU::generate_bit() -> bool { return dis_bin_(gen_); }
auto QPU::generate_real() -> double { return dis_real_(gen_); }
auto QPU::generate_pseudo_uuid_v4() -> std::string {
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
auto QPU::execute(const size_t shot_count)
    -> std::pair<std::vector<std::complex<double>>, std::vector<std::string>> {
  status_ = QDMI_QPU_STATUS_BUSY;
  const std::pair result{generate_n_state_vector(shot_count),
                         generate_n_shots(shot_count)};
  status_ = QDMI_QPU_STATUS_IDLE;
  return result;
}
auto QPU::generate_n_shots(const size_t n) -> std::vector<std::string> {
  const auto qubit_count = qubits_.size();
  std::vector<std::string> shots;
  shots.reserve(n);
  std::ranges::generate_n(std::back_inserter(shots), static_cast<int64_t>(n),
                          [this, qubit_count]() -> std::string {
                            std::string shot(qubit_count, '0');
                            std::ranges::generate(shot, [this]() -> char {
                              return generate_bit() ? '1' : '0';
                            });
                            return shot;
                          });
  return shots;
}
auto QPU::generate_n_state_vector(const size_t n)
    -> std::vector<std::complex<double>> {
  const auto qubit_count = qubits_.size();
  std::vector<std::complex<double>> state_vec;
  state_vec.reserve(1U << qubit_count);
  std::generate_n(std::back_inserter(state_vec), static_cast<int64_t>(n),
                  [this]() -> std::complex<double> {
                    return {generate_real(), generate_real()};
                  });
  double norm = std::accumulate(
      state_vec.cbegin(), state_vec.cend(), 0.0,
      [](double acc, const auto &c) { return acc + std::norm(c); });
  // Normalize the vector
  norm = std::sqrt(norm);
  std::ranges::for_each(state_vec, [norm](auto &c) { c /= norm; });
  return state_vec;
}
auto Sort_qubit_tuples(QubitTuples &qubit_tuples) -> void {
  std::visit(overloaded{
                 [](std::vector<Qubit *> &qubits) -> void {
                   // Single-qubit: sort the flat list by pointer address
                   std::ranges::sort(qubits, std::less<Qubit *>{});
                 },
                 [](std::vector<std::array<Qubit *, 2>> &qubit_pairs) -> void {
                   // Two-qubit: normalize each pair (first < second)
                   // Use std::less for proper total order (pointer comparison
                   // with operator > invokes undefined behavior)
                   std::ranges::for_each(qubit_pairs, [](auto &p) {
                     if (std::less<Qubit *>{}(p.front(), p.back())) {
                       std::swap(p.front(), p.back());
                     }
                   });
                   std::ranges::sort(qubit_pairs);
                 },
             },
             qubit_tuples);
}
auto Is_one_of(const std::span<Qubit *const> &qubits,
               const QubitTuples &qubit_tuples) -> bool {
  return std::visit(
      overloaded{
          [&qubits](const std::vector<Qubit *> &single_qubits) -> bool {
            if (qubits.size() != 1) {
              throw std::invalid_argument("Expected exactly one qubit");
            }
            return std::ranges::binary_search(single_qubits, qubits.front(),
                                              std::less<Qubit *>{});
          },
          [&qubits](
              const std::vector<std::array<Qubit *, 2>> &qubit_pairs) -> bool {
            if (qubits.size() != 2) {
              throw std::invalid_argument("Expected exactly two qubits");
            }
            // NOLINTBEGIN(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
            const auto needle = std::less<Qubit *>{}(qubits[0], qubits[1])
                                    ? std::array{qubits[0], qubits[1]}
                                    : std::array{qubits[1], qubits[0]};
            // NOLINTEND(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
            return std::ranges::binary_search(qubit_pairs, needle);
          },
      },
      qubit_tuples);
}
auto Qubit_tuple_arity(const QubitTuples &qubit_tuples) -> size_t {
  return std::visit(
      overloaded{
          [](const std::vector<Qubit *> &) -> size_t { return 1; },
          [](const std::vector<std::array<Qubit *, 2>> &) -> size_t {
            return 2;
          },
      },
      qubit_tuples);
}
auto Qubit_tuple_size(const QubitTuples &qubit_tuples) -> size_t {
  return std::visit([](const auto &tuples) -> size_t { return tuples.size(); },
                    qubit_tuples);
}
auto Sizeof_qubit_tuple(const QubitTuples &qubit_tuples) -> size_t {
  return Qubit_tuple_arity(qubit_tuples) * Qubit_tuple_size(qubit_tuples);
}
} // namespace qdmi::qpu
