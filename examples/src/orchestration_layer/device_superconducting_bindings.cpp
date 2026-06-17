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

#include "orchestration_layer/device_superconducting_bindings.hpp"

#include "od_qdmi/core.h"
#include "od_qdmi/superconducting.h"
#include "orchestration_layer/orchestration_layer.hpp"
#include "qdmi/superconducting.h"

#include <cstddef>
#include <cstdint>
#include <sstream>

// NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast,misc-include-cleaner)

namespace qdmi::orchestration_layer {
namespace {
/// @copydoc OD_QDMI_session_query_sc_duration_scale_factor
auto Session_query_sc_duration_scale_factor(DeviceSession *session,
                                            uint64_t *value) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[{}] Querying the duration scale factor",
                      session->context->device->id);
  if (session->status != DeviceSession::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Session is not initialized",
                        session->context->device->id);
    return QDMI_ERROR_BADSTATE;
  }
  if (const auto ret = session->context->device->sc_interface
                           ->session_query_sc_duration_scale_factor(
                               session->context->device->session, value);
      ret != QDMI_SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Failed to query duration scale factor: {}",
                        session->context->device->id, ret);
    return ret;
  }
  return QDMI_SUCCESS;
}

/// @copydoc OD_QDMI_session_query_sc_qubits
auto Session_query_sc_qubits(DeviceSession *session, const size_t size,
                             OD_QDMI_SCQubit *values, size_t *size_ret) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO, "[{}] Querying the number of qubits",
                      session->context->device->id);
  if (session->status != DeviceSession::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Session is not initialized",
                        session->context->device->id);
    return QDMI_ERROR_BADSTATE;
  }
  if (const auto ret =
          session->context->device->sc_interface->session_query_sc_qubits(
              session->context->device->session, size,
              reinterpret_cast<QDMI_SCQubit *>(values), size_ret);
      ret != QDMI_SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Failed to query qubits: {}",
                        session->context->device->id, ret);
    return ret;
  }
  return QDMI_SUCCESS;
}
/// @copydoc OD_QDMI_session_query_sc_qubit_by_index
auto Session_query_sc_qubit_by_index(DeviceSession *session, const size_t index,
                                     OD_QDMI_SCQubit *qubit) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO, "[{}] Querying a qubit by its index",
                      session->context->device->id);
  if (session->status != DeviceSession::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Session is not initialized",
                        session->context->device->id);
    return QDMI_ERROR_BADSTATE;
  }
  if (const auto ret = session->context->device->sc_interface
                           ->session_query_sc_qubit_by_index(
                               session->context->device->session, index,
                               reinterpret_cast<QDMI_SCQubit *>(qubit));
      ret != QDMI_SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Failed to query qubit by index: {}",
                        session->context->device->id, ret);
    return ret;
  }
  return QDMI_SUCCESS;
}
/// @copydoc OD_QDMI_session_query_sc_qubit_by_name
auto Session_query_sc_qubit_by_name(DeviceSession *session, const char *name,
                                    OD_QDMI_SCQubit *qubit) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO, "[{}] Querying a qubit by its name",
                      session->context->device->id);
  if (session->status != DeviceSession::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Session is not initialized",
                        session->context->device->id);
    return QDMI_ERROR_BADSTATE;
  }
  if (const auto ret = session->context->device->sc_interface
                           ->session_query_sc_qubit_by_name(
                               session->context->device->session, name,
                               reinterpret_cast<QDMI_SCQubit *>(qubit));
      ret != QDMI_SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Failed to query qubit by name: {}",
                        session->context->device->id, ret);
    return ret;
  }
  return QDMI_ERROR_NOTSUPPORTED;
}
/// @copydoc OD_QDMI_session_query_sc_qubit_index
auto Session_query_sc_qubit_index(DeviceSession *session, OD_QDMI_SCQubit qubit,
                                  size_t *value) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO, "[{}] Querying a qubit's index",
                      session->context->device->id);
  if (session->status != DeviceSession::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Session is not initialized",
                        session->context->device->id);
    return QDMI_ERROR_BADSTATE;
  }
  if (const auto ret =
          session->context->device->sc_interface->session_query_sc_qubit_index(
              session->context->device->session,
              reinterpret_cast<QDMI_SCQubit>(qubit), value);
      ret != QDMI_SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Failed to query qubit index: {}",
                        session->context->device->id, ret);
    return ret;
  }
  return QDMI_SUCCESS;
}
/// @copydoc OD_QDMI_session_query_sc_qubit_name
auto Session_query_sc_qubit_name(DeviceSession *session, OD_QDMI_SCQubit qubit,
                                 const size_t size, char *value,
                                 size_t *size_ret) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO, "[{}] Querying a qubit's name",
                      session->context->device->id);
  if (const auto ret =
          session->context->device->sc_interface->session_query_sc_qubit_name(
              session->context->device->session,
              reinterpret_cast<QDMI_SCQubit>(qubit), size, value, size_ret);
      ret != QDMI_SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Failed to query qubit name: {}",
                        session->context->device->id, ret);
    return ret;
  }
  return QDMI_SUCCESS;
}
/// @copydoc OD_QDMI_session_query_sc_qubit_coherence_t1
auto Session_query_sc_qubit_coherence_t1(DeviceSession *session,
                                         OD_QDMI_SCQubit qubit, uint64_t *value)
    -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[{}] Querying a qubit's coherence time",
                      session->context->device->id);
  if (session->status != DeviceSession::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Session is not initialized",
                        session->context->device->id);
    return QDMI_ERROR_BADSTATE;
  }
  if (const auto ret = session->context->device->sc_interface
                           ->session_query_sc_qubit_coherence_t1(
                               session->context->device->session,
                               reinterpret_cast<QDMI_SCQubit>(qubit), value);
      ret != QDMI_SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Failed to query qubit coherence time t1: {}",
                        session->context->device->id, ret);
    return ret;
  }
  return QDMI_SUCCESS;
}
/// @copydoc OD_QDMI_session_query_sc_qubit_coherence_t2
auto Session_query_sc_qubit_coherence_t2(DeviceSession *session,
                                         OD_QDMI_SCQubit qubit, uint64_t *value)
    -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[{}] Querying a qubit's coherence time",
                      session->context->device->id);
  if (session->status != DeviceSession::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Session is not initialized",
                        session->context->device->id);
    return QDMI_ERROR_BADSTATE;
  }
  if (const auto ret = session->context->device->sc_interface
                           ->session_query_sc_qubit_coherence_t2(
                               session->context->device->session,
                               reinterpret_cast<QDMI_SCQubit>(qubit), value);
      ret != QDMI_SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Failed to query qubit coherence time t2: {}",
                        session->context->device->id, ret);
    return ret;
  }
  return QDMI_SUCCESS;
}
/// @copydoc OD_QDMI_session_query_coupling_map
auto Session_query_coupling_map(DeviceSession *session, const size_t size,
                                OD_QDMI_SCQubit *values, size_t *size_ret)
    -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO, "[{}] Querying the coupling map",
                      session->context->device->id);
  if (session->status != DeviceSession::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Session is not initialized",
                        session->context->device->id);
    return QDMI_ERROR_BADSTATE;
  }
  if (const auto ret =
          session->context->device->sc_interface->session_query_coupling_map(
              session->context->device->session, size,
              reinterpret_cast<QDMI_SCQubit *>(values), size_ret);
      ret != QDMI_SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Failed to query couping map: {}",
                        session->context->device->id, ret);
    return ret;
  }
  return QDMI_SUCCESS;
}
/// @copydoc OD_QDMI_session_query_sc_operations
auto Session_query_sc_operations(DeviceSession *session, const size_t size,
                                 OD_QDMI_SCOperation *values, size_t *size_ret)
    -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[{}] Querying the number of operations",
                      session->context->device->id);
  if (session->status != DeviceSession::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Session is not initialized",
                        session->context->device->id);
    return QDMI_ERROR_BADSTATE;
  }
  if (const auto ret =
          session->context->device->sc_interface->session_query_sc_operations(
              session->context->device->session, size,
              reinterpret_cast<QDMI_SCOperation *>(values), size_ret);
      ret != QDMI_SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Failed to query operations: {}",
                        session->context->device->id, ret);
    return ret;
  }
  return QDMI_SUCCESS;
}
/// @copydoc OD_QDMI_session_query_sc_operation_by_id
auto Session_query_sc_operation_by_id(DeviceSession *session, const char *id,
                                      OD_QDMI_SCOperation *operation) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[{}] Querying an operation by its ID",
                      session->context->device->id);
  if (session->status != DeviceSession::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Session is not initialized",
                        session->context->device->id);
    return QDMI_ERROR_BADSTATE;
  }
  if (const auto ret = session->context->device->sc_interface
                           ->session_query_sc_operation_by_id(
                               session->context->device->session, id,
                               reinterpret_cast<QDMI_SCOperation *>(operation));
      ret != QDMI_SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Failed to query operation by ID: {}",
                        session->context->device->id, ret);
    return ret;
  }
  return QDMI_SUCCESS;
}

/// @copydoc OD_QDMI_session_query_sc_operation_id
auto Session_query_sc_operation_id(DeviceSession *session,
                                   OD_QDMI_SCOperation operation,
                                   const size_t size, char *value,
                                   size_t *size_ret) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO, "[{}] Querying an operation's ID",
                      session->context->device->id);
  if (session->status != DeviceSession::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Session is not initialized",
                        session->context->device->id);
    return QDMI_ERROR_BADSTATE;
  }
  if (const auto ret =
          session->context->device->sc_interface->session_query_sc_operation_id(
              session->context->device->session,
              reinterpret_cast<QDMI_SCOperation>(operation), size, value,
              size_ret);
      ret != QDMI_SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Failed to query operation ID: {}",
                        session->context->device->id, ret);
    return ret;
  }
  return QDMI_SUCCESS;
}
/// @copydoc OD_QDMI_session_query_sc_operation_name
auto Session_query_sc_operation_name(DeviceSession *session,
                                     OD_QDMI_SCOperation operation,
                                     const size_t size, char *value,
                                     size_t *size_ret) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO, "[{}] Querying an operation's name",
                      session->context->device->id);
  if (session->status != DeviceSession::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Session is not initialized",
                        session->context->device->id);
    return QDMI_ERROR_BADSTATE;
  }
  if (const auto ret = session->context->device->sc_interface
                           ->session_query_sc_operation_name(
                               session->context->device->session,
                               reinterpret_cast<QDMI_SCOperation>(operation),
                               size, value, size_ret);
      ret != QDMI_SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Failed to query operation name: {}",
                        session->context->device->id, ret);
    return ret;
  }
  return QDMI_SUCCESS;
}
/// @copydoc OD_QDMI_session_query_sc_operation_parameter_count
auto Session_query_sc_operation_parameter_count(DeviceSession *session,
                                                OD_QDMI_SCOperation operation,
                                                size_t *value) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[{}] Querying the number of parameters for operation",
                      session->context->device->id);
  if (session->status != DeviceSession::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Session is not initialized",
                        session->context->device->id);
    return QDMI_ERROR_BADSTATE;
  }
  if (const auto ret =
          session->context->device->sc_interface
              ->session_query_sc_operation_parameter_count(
                  session->context->device->session,
                  reinterpret_cast<QDMI_SCOperation>(operation), value);
      ret != QDMI_SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Failed to query operation parameter count: {}",
                        session->context->device->id, ret);
    return ret;
  }
  return QDMI_SUCCESS;
}
/// @copydoc OD_QDMI_session_query_sc_operation_qubit_count
auto Session_query_sc_operation_qubit_count(DeviceSession *session,
                                            OD_QDMI_SCOperation operation,
                                            size_t *value) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[{}] Querying the number of qubits for operation",
                      session->context->device->id);
  if (session->status != DeviceSession::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Session is not initialized",
                        session->context->device->id);
    return QDMI_ERROR_BADSTATE;
  }
  if (const auto ret =
          session->context->device->sc_interface
              ->session_query_sc_operation_qubit_count(
                  session->context->device->session,
                  reinterpret_cast<QDMI_SCOperation>(operation), value);
      ret != QDMI_SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Failed to query operation qubit count: {}",
                        session->context->device->id, ret);
    return ret;
  }
  return QDMI_SUCCESS;
}
/// @copydoc OD_QDMI_session_query_sc_operation_qubits
auto Session_query_sc_operation_qubits(DeviceSession *session,
                                       OD_QDMI_SCOperation operation,
                                       const size_t size,
                                       OD_QDMI_SCQubit *values,
                                       size_t *size_ret) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[{}] Querying the qubits for operation",
                      session->context->device->id);
  if (session->status != DeviceSession::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Session is not initialized",
                        session->context->device->id);
    return QDMI_ERROR_BADSTATE;
  }
  if (const auto ret =
          session->context->device->sc_interface
              ->session_query_sc_operation_qubits(
                  session->context->device->session,
                  reinterpret_cast<QDMI_SCOperation>(operation), size,
                  reinterpret_cast<QDMI_SCQubit *>(values), size_ret);
      ret != QDMI_SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Failed to query operation qubits: {}",
                        session->context->device->id, ret);
    return ret;
  }
  return QDMI_SUCCESS;
}
/// @copydoc OD_QDMI_session_query_sc_operation_duration
auto Session_query_sc_operation_duration(DeviceSession *session,
                                         OD_QDMI_SCOperation operation,
                                         const OD_QDMI_SCQubit *qubits,
                                         const double *params, uint64_t *value)
    -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[{}] Querying the duration for operation",
                      session->context->device->id);
  if (session->status != DeviceSession::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Session is not initialized",
                        session->context->device->id);
    return QDMI_ERROR_BADSTATE;
  }
  if (const auto ret = session->context->device->sc_interface
                           ->session_query_sc_operation_duration(
                               session->context->device->session,
                               reinterpret_cast<QDMI_SCOperation>(operation),
                               reinterpret_cast<const QDMI_SCQubit *>(qubits),
                               params, value);
      ret != QDMI_SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Failed to query operation duration: {}",
                        session->context->device->id, ret);
    return ret;
  }
  return QDMI_SUCCESS;
}
/// @copydoc OD_QDMI_session_query_sc_operation_fidelity
auto Session_query_sc_operation_fidelity(DeviceSession *session,
                                         OD_QDMI_SCOperation operation,
                                         const OD_QDMI_SCQubit *qubits,
                                         const double *params, double *value)
    -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO,
                      "[{}] Querying the fidelity for operation",
                      session->context->device->id);
  if (session->status != DeviceSession::STATUS::INITIALIZED) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Session is not initialized",
                        session->context->device->id);
    return QDMI_ERROR_BADSTATE;
  }
  if (const auto ret = session->context->device->sc_interface
                           ->session_query_sc_operation_fidelity(
                               session->context->device->session,
                               reinterpret_cast<QDMI_SCOperation>(operation),
                               reinterpret_cast<const QDMI_SCQubit *>(qubits),
                               params, value);
      ret != QDMI_SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Failed to query operation fidelity: {}",
                        session->context->device->id, ret);
    return ret;
  }
  return QDMI_SUCCESS;
}
} // namespace
auto Get_device_sc_interface() -> OD_QDMI_SC_Interface {
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
} // namespace qdmi::orchestration_layer

// NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast,misc-include-cleaner)
