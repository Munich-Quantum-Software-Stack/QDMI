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

#include "qpu/superconducting_bindings.hpp"

#include "q_qdmi/core.h"
#include "q_qdmi/superconducting.h"
#include "qpu/qpu.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <sstream>
#include <stdexcept>
#include <type_traits>
#include <variant>
#include <vector>

// NOLINTBEGIN(misc-include-cleaner)

namespace qdmi::qpu {
namespace {
template <class... Ts> struct overloaded : Ts... {
  using Ts::operator()...;
};
/// @copydoc Q_QDMI_session_query_sc_duration_scale_factor
auto Session_query_sc_duration_scale_factor(Session *session, uint64_t *value)
    -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID
                      "] Querying the duration scale factor");
  if (session->status != Session::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Session is not initialized");
    return QDMI_ERROR_BADSTATE;
  }
  if (value != nullptr) {
    *value = QPU::get().duration_scale_factor();
  }
  return QDMI_SUCCESS;
}

/// @copydoc Q_QDMI_session_query_sc_qubits
auto Session_query_sc_qubits(Session *session, const size_t size, Qubit **value,
                             size_t *size_ret) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID "] Querying the number of qubits");
  if (session->status != Session::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Session is not initialized");
    return QDMI_ERROR_BADSTATE;
  }
  if (value != nullptr) {
    if (size < QPU::get().qubits().size()) {
      session->logger.log(QDMI_LOG_LEVEL_ERROR,
                          "[" QDMI_DEVICE_ID
                          "] Provided buffer size is too small");
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    std::ranges::copy(QPU::get().qubits(), value);
  }
  if (size_ret != nullptr) {
    *size_ret = QPU::get().qubits().size();
  }
  return QDMI_SUCCESS;
}
/// @copydoc Q_QDMI_session_query_sc_qubit_by_index
auto Session_query_sc_qubit_by_index(Session *session, const size_t index,
                                     Qubit **qubit) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID "] Querying a qubit by its index");
  if (session->status != Session::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Session is not initialized");
    return QDMI_ERROR_BADSTATE;
  }
  if (*qubit = QPU::get().qubit_by_index(index); *qubit == nullptr) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Qubit with index {} not found.",
                        index);
    return QDMI_ERROR_NOTFOUND;
  }
  return QDMI_SUCCESS;
}
/// @copydoc Q_QDMI_session_query_sc_qubit_by_name
auto Session_query_sc_qubit_by_name(Session *session,
                                    [[maybe_unused]] const char *name,
                                    [[maybe_unused]] Qubit **qubit) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID "] Querying a qubit by its name");
  session->logger.log(QDMI_LOG_LEVEL_ERROR,
                      "[" QDMI_DEVICE_ID
                      "] Querying qubits by name is not supported");
  return QDMI_ERROR_NOTSUPPORTED;
}
/// @copydoc Q_QDMI_session_query_sc_qubit_index
auto Session_query_sc_qubit_index(Session *session, Qubit *qubit, size_t *value)
    -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID "] Querying a qubit's index");
  if (session->status != Session::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Session is not initialized");
    return QDMI_ERROR_BADSTATE;
  }
  if (qubit == nullptr) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Qubit is not specified");
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (value != nullptr) {
    *value = qubit->index;
  }
  return QDMI_SUCCESS;
}
/// @copydoc Q_QDMI_session_query_sc_qubit_name
auto Session_query_sc_qubit_name(Session *session,
                                 [[maybe_unused]] Qubit *qubit,
                                 [[maybe_unused]] const size_t size,
                                 [[maybe_unused]] char *value,
                                 [[maybe_unused]] size_t *size_ret) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID "] Querying a qubit's name");
  session->logger.log(QDMI_LOG_LEVEL_ERROR,
                      "[" QDMI_DEVICE_ID
                      "] Querying qubit names is not supported");
  return QDMI_ERROR_NOTSUPPORTED;
}
/// @copydoc Q_QDMI_session_query_sc_qubit_coherence_t1
auto Session_query_sc_qubit_coherence_t1(Session *session, Qubit *qubit,
                                         uint64_t *value) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID "] Querying a qubit's coherence time");
  if (session->status != Session::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Session is not initialized");
    return QDMI_ERROR_BADSTATE;
  }
  if (qubit == nullptr) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Qubit is not specified");
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (value != nullptr) {
    *value = qubit->coherence_t1;
  }
  return QDMI_SUCCESS;
}
/// @copydoc Q_QDMI_session_query_sc_qubit_coherence_t2
auto Session_query_sc_qubit_coherence_t2(Session *session, Qubit *qubit,
                                         uint64_t *value) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID "] Querying a qubit's coherence time");
  if (session->status != Session::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Session is not initialized");
    return QDMI_ERROR_BADSTATE;
  }
  if (qubit == nullptr) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Qubit is not specified");
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (value != nullptr) {
    *value = qubit->coherence_t2;
  }
  return QDMI_SUCCESS;
}
/// @copydoc Q_QDMI_session_query_coupling_map
auto Session_query_coupling_map(Session *session, const size_t size,
                                Qubit **value, size_t *size_ret) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID "] Querying the coupling map");
  if (session->status != Session::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Session is not initialized");
    return QDMI_ERROR_BADSTATE;
  }
  if (value != nullptr) {
    if (size < QPU::get().coupling_map().size()) {
      session->logger.log(QDMI_LOG_LEVEL_ERROR,
                          "[" QDMI_DEVICE_ID
                          "] Provided buffer size is too small");
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    std::ranges::copy(
        QPU::get().coupling_map(),
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        reinterpret_cast<std::array<Qubit *, 2> *>(value));
  }
  if (size_ret != nullptr) {
    *size_ret = QPU::get().coupling_map().size();
  }
  return QDMI_SUCCESS;
}
/// @copydoc Q_QDMI_session_query_sc_operations
auto Session_query_sc_operations(Session *session, const size_t size,
                                 Operation **value, size_t *size_ret) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID "] Querying the number of operations");
  if (session->status != Session::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Session is not initialized");
    return QDMI_ERROR_BADSTATE;
  }
  if (value != nullptr) {
    if (size < QPU::get().operations().size()) {
      session->logger.log(QDMI_LOG_LEVEL_ERROR,
                          "[" QDMI_DEVICE_ID
                          "] Provided buffer size is too small");
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    std::ranges::copy(QPU::get().operations(), value);
  }
  if (size_ret != nullptr) {
    *size_ret = QPU::get().operations().size();
  }
  return QDMI_SUCCESS;
}
/// @copydoc Q_QDMI_session_query_sc_operation_by_id
auto Session_query_sc_operation_by_id(Session *session, const char *id,
                                      Operation **operation) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID "] Querying an operation by its ID");
  if (session->status != Session::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Session is not initialized");
    return QDMI_ERROR_BADSTATE;
  }
  if (operation == nullptr || id == nullptr) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Operation or ID is null");
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (*operation = QPU::get().operation_by_id(id); *operation == nullptr) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Operation not found: {}", id);
  }
  return QDMI_SUCCESS;
}

/// @copydoc Q_QDMI_session_query_sc_operation_id
auto Session_query_sc_operation_id(Session *session, Operation *operation,
                                   const size_t size, char *value,
                                   size_t *size_ret) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID "] Querying an operation's ID");
  if (session->status != Session::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Session is not initialized");
    return QDMI_ERROR_BADSTATE;
  }
  if (operation == nullptr) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Operation is null");
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (value != nullptr) {
    if (size < operation->id.size() + 1) {
      session->logger.log(QDMI_LOG_LEVEL_ERROR,
                          "[" QDMI_DEVICE_ID
                          "] Provided buffer size is too small");
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    std::ranges::copy(operation->id, value);
    value[operation->id.size()] = '\0';
  }
  if (size_ret != nullptr) {
    *size_ret = operation->id.size() + 1;
  }
  return QDMI_SUCCESS;
}
/// @copydoc Q_QDMI_session_query_sc_operation_name
auto Session_query_sc_operation_name(Session *session, Operation *operation,
                                     const size_t size, char *value,
                                     size_t *size_ret) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID "] Querying an operation's name");
  if (session->status != Session::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Session is not initialized");
    return QDMI_ERROR_BADSTATE;
  }
  if (operation == nullptr) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Operation is null");
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (value != nullptr) {
    if (size < operation->name.size() + 1) {
      session->logger.log(QDMI_LOG_LEVEL_ERROR,
                          "[" QDMI_DEVICE_ID
                          "] Provided buffer size is too small");
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    std::ranges::copy(operation->name, value);
    value[operation->name.size()] = '\0';
  }
  if (size_ret != nullptr) {
    *size_ret = operation->name.size() + 1;
  }
  return QDMI_SUCCESS;
}
/// @copydoc Q_QDMI_session_query_sc_operation_parameter_count
auto Session_query_sc_operation_parameter_count(Session *session,
                                                Operation *operation,
                                                size_t *value) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID
                      "] Querying the number of parameters for operation");
  if (session->status != Session::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Session is not initialized");
    return QDMI_ERROR_BADSTATE;
  }
  if (value != nullptr) {
    *value = operation->parameter_count;
  }
  return QDMI_SUCCESS;
}
/// @copydoc Q_QDMI_session_query_sc_operation_qubit_count
auto Session_query_sc_operation_qubit_count(Session *session,
                                            Operation *operation, size_t *value)
    -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID
                      "] Querying the number of qubits for operation");
  if (session->status != Session::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Session is not initialized");
    return QDMI_ERROR_BADSTATE;
  }
  if (operation == nullptr) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Operation is null");
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (value != nullptr) {
    *value = Qubit_tuple_arity(operation->qubits);
  }
  return QDMI_SUCCESS;
}
/// @copydoc Q_QDMI_session_query_sc_operation_qubits
auto Session_query_sc_operation_qubits(Session *session, Operation *operation,
                                       const size_t size, Qubit **value,
                                       size_t *size_ret) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID "] Querying the qubits for operation");
  if (session->status != Session::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Session is not initialized");
    return QDMI_ERROR_BADSTATE;
  }
  if (operation == nullptr) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Operation is null");
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  const auto required_size = Sizeof_qubit_tuple(operation->qubits);
  if (value != nullptr) {
    if (size < required_size) {
      session->logger.log(QDMI_LOG_LEVEL_ERROR,
                          "[" QDMI_DEVICE_ID
                          "] Provided buffer size is too small");
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    std::visit(
        overloaded{
            [&value](const std::vector<Qubit *> &qubits) -> void {
              std::ranges::copy(qubits, value);
            },
            [&value](
                const std::vector<std::array<Qubit *, 2>> &qubits) -> void {
              // Ensure std::array<Qubit*, 2> has standard layout and expected
              // size
              static_assert(std::is_standard_layout_v<std::array<Qubit *, 2>>);
              static_assert(sizeof(std::array<Qubit *, 2>) ==
                            2 * sizeof(Qubit *));
              // Two-qubit: reinterpret as a flat array of sites using std::span
              // std::array has a standard layout, so the memory layout of
              // vector<array<Qubit*, 2>> is equivalent to Qubit*[2*N]
              const std::span flatView{
                  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
                  reinterpret_cast<Qubit *const *>(qubits.data()),
                  qubits.size() * 2};
              std::ranges::copy(flatView, value);
            }},
        operation->qubits);
  }
  if (size_ret != nullptr) {
    *size_ret = required_size;
  }
  return QDMI_SUCCESS;
}
/// @copydoc Q_QDMI_session_query_sc_operation_duration
auto Session_query_sc_operation_duration(Session *session, Operation *operation,
                                         Qubit *const *qubits,
                                         [[maybe_unused]] const double *params,
                                         uint64_t *value) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID
                      "] Querying the duration for operation");
  if (session->status != Session::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Session is not initialized");
    return QDMI_ERROR_BADSTATE;
  }
  if (operation == nullptr) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Operation is null");
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (const auto qubit_count{Qubit_tuple_arity(operation->qubits)};
      !Is_one_of(std::span{qubits, qubit_count}, operation->qubits)) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID
                        "] Operation cannot be applied to the given qubit(s).");
    return QDMI_ERROR_NOTSUPPORTED;
  }
  if (value != nullptr) {
    *value = operation->duration;
  }
  return QDMI_SUCCESS;
}
/// @copydoc Q_QDMI_session_query_sc_operation_fidelity
auto Session_query_sc_operation_fidelity(Session *session, Operation *operation,
                                         Qubit *const *qubits,
                                         [[maybe_unused]] const double *params,
                                         double *value) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[" QDMI_DEVICE_ID
                      "] Querying the fidelity for operation");
  if (session->status != Session::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Session is not initialized");
    return QDMI_ERROR_BADSTATE;
  }
  if (operation == nullptr) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID "] Operation is null");
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (const auto qubit_count{Qubit_tuple_arity(operation->qubits)};
      !Is_one_of(std::span{qubits, qubit_count}, operation->qubits)) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[" QDMI_DEVICE_ID
                        "] Operation cannot be applied to the given qubit(s).");
    return QDMI_ERROR_NOTSUPPORTED;
  }
  if (value != nullptr) {
    *value = operation->fidelity;
  }
  return QDMI_SUCCESS;
}
} // namespace

auto Get_sc_interface() -> Q_QDMI_SC_Interface {
  return {.session_query_sc_duration_scale_factor =
              &Session_query_sc_duration_scale_factor,
          .session_query_sc_qubits = &Session_query_sc_qubits,
          .session_query_sc_qubit_by_index = &Session_query_sc_qubit_by_index,
          .session_query_sc_qubit_by_name = &Session_query_sc_qubit_by_name,
          .session_query_sc_qubit_index = &Session_query_sc_qubit_index,
          .session_query_sc_qubit_name = &Session_query_sc_qubit_name,
          .session_query_sc_qubit_coherence_t1 =
              &Session_query_sc_qubit_coherence_t1,
          .session_query_sc_qubit_coherence_t2 =
              &Session_query_sc_qubit_coherence_t2,
          .session_query_coupling_map = &Session_query_coupling_map,
          .session_query_sc_operations = &Session_query_sc_operations,
          .session_query_sc_operation_by_id = &Session_query_sc_operation_by_id,
          .session_query_sc_operation_id = &Session_query_sc_operation_id,
          .session_query_sc_operation_name = &Session_query_sc_operation_name,
          .session_query_sc_operation_parameter_count =
              &Session_query_sc_operation_parameter_count,
          .session_query_sc_operation_qubit_count =
              &Session_query_sc_operation_qubit_count,
          .session_query_sc_operation_qubits =
              &Session_query_sc_operation_qubits,
          .session_query_sc_operation_duration =
              &Session_query_sc_operation_duration,
          .session_query_sc_operation_fidelity =
              &Session_query_sc_operation_fidelity};
}
} // namespace qdmi::qpu

// NOLINTEND(misc-include-cleaner)
