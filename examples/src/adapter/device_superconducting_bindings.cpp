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

#include "adapter/device_superconducting_bindings.hpp"

#include "ad_qdmi/core.h"
#include "ad_qdmi/superconducting.h"
#include "adapter/adapter.hpp"

#include <cstddef>
#include <cstdint>

// NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast,misc-include-cleaner)

namespace qdmi::adapter {
namespace {
/// @copydoc AD_QDMI_session_query_sc_duration_scale_factor
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
  if (const auto ret =
          session->context->device->v1_library.session_query_device_property(
              session->v1_session, V1_DEVICE_PROPERTY::DURATIONSCALEFACTOR,
              sizeof(uint64_t), value, nullptr);
      ret != SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Failed to query duration scale factor: {}",
                        session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  return QDMI_SUCCESS;
}

/// @copydoc AD_QDMI_session_query_sc_qubits
auto Session_query_sc_qubits(DeviceSession *session, const size_t size,
                             AD_QDMI_SCQubit *values, size_t *size_ret) -> int {
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
          session->context->device->v1_library.session_query_device_property(
              session->v1_session, V1_DEVICE_PROPERTY::SITES, size, values,
              size_ret);
      ret != SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Failed to query qubits: {}",
                        session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_session_query_sc_qubit_by_index
auto Session_query_sc_qubit_by_index(DeviceSession *session, const size_t index,
                                     AD_QDMI_SCQubit *qubit) -> int {
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
  size_t size{};
  if (const auto ret =
          session->context->device->v1_library.session_query_device_property(
              session->v1_session, V1_DEVICE_PROPERTY::SITES, 0, nullptr,
              &size);
      ret != SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Failed to query site count: {}",
                        session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  if (index >= size) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Index {} is out of range",
                        session->context->device->id, index);
    return ERROR_OUTOFRANGE;
  }
  std::vector sites(size, V1Site{});
  if (const auto ret =
          session->context->device->v1_library.session_query_device_property(
              session->v1_session, V1_DEVICE_PROPERTY::SITES,
              size * sizeof(AD_QDMI_SCQubit), sites.data(), nullptr);
      ret != SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Failed to query sites: {}",
                        session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  if (qubit != nullptr) {
    *qubit = reinterpret_cast<AD_QDMI_SCQubit>(sites[index]);
  }
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_session_query_sc_qubit_by_name
auto Session_query_sc_qubit_by_name(DeviceSession *session, const char *name,
                                    AD_QDMI_SCQubit *qubit) -> int {
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
  size_t size{};
  if (const auto ret =
          session->context->device->v1_library.session_query_device_property(
              session->v1_session, V1_DEVICE_PROPERTY::SITES, 0, nullptr,
              &size);
      ret != SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Failed to query site count: {}",
                        session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  std::vector sites(size, V1Site{});
  if (const auto ret =
          session->context->device->v1_library.session_query_device_property(
              session->v1_session, V1_DEVICE_PROPERTY::SITES,
              size * sizeof(AD_QDMI_SCQubit), sites.data(), nullptr);
      ret != SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Failed to query sites: {}",
                        session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  for (const auto &site : sites) {
    if (const auto ret{
            session->context->device->v1_library.session_query_site_property(
                session->v1_session, site, V1_SITE_PROPERTY::NAME, 0, nullptr,
                &size)};
        ret != SUCCESS) {
      session->logger.log(QDMI_LOG_LEVEL_ERROR,
                          "[{}] Failed to query site name size: {}",
                          session->context->device->id, ret);
      return Transform_return_codes(ret);
    }
    std::string site_name(size - 1, '\0');
    if (const auto ret{
            session->context->device->v1_library.session_query_site_property(
                session->v1_session, site, V1_SITE_PROPERTY::NAME, size,
                site_name.data(), nullptr)};
        ret != SUCCESS) {
      session->logger.log(QDMI_LOG_LEVEL_ERROR,
                          "[{}] Failed to query site name: {}",
                          session->context->device->id, ret);
      return Transform_return_codes(ret);
    }
    if (site_name == name) {
      if (qubit != nullptr) {
        *qubit = reinterpret_cast<AD_QDMI_SCQubit>(site);
      }
      return QDMI_SUCCESS;
    }
  }
  session->logger.log(QDMI_LOG_LEVEL_ERROR, "[{}] Qubit not found",
                      session->context->device->id);
  return QDMI_ERROR_NOTFOUND;
}
/// @copydoc AD_QDMI_session_query_sc_qubit_index
auto Session_query_sc_qubit_index(DeviceSession *session, AD_QDMI_SCQubit qubit,
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
          session->context->device->v1_library.session_query_site_property(
              session->v1_session, reinterpret_cast<V1Site>(qubit),
              V1_SITE_PROPERTY::INDEX, sizeof(size_t), value, nullptr);
      ret != SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Failed to query qubit index: {}",
                        session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_session_query_sc_qubit_name
auto Session_query_sc_qubit_name(DeviceSession *session, AD_QDMI_SCQubit qubit,
                                 const size_t size, char *value,
                                 size_t *size_ret) -> int {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  session->logger.log(QDMI_LOG_LEVEL_INFO, "[{}] Querying a qubit's name",
                      session->context->device->id);
  if (const auto ret =
          session->context->device->v1_library.session_query_site_property(
              session->v1_session, reinterpret_cast<V1Site>(qubit),
              V1_SITE_PROPERTY::NAME, size, value, size_ret);
      ret != SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Failed to query qubit name: {}",
                        session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_session_query_sc_qubit_coherence_t1
auto Session_query_sc_qubit_coherence_t1(DeviceSession *session,
                                         AD_QDMI_SCQubit qubit, uint64_t *value)
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
  if (const auto ret =
          session->context->device->v1_library.session_query_site_property(
              session->v1_session, reinterpret_cast<V1Site>(qubit),
              V1_SITE_PROPERTY::T1, sizeof(uint64_t), value, nullptr);
      ret != SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Failed to query qubit coherence time t1: {}",
                        session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_session_query_sc_qubit_coherence_t2
auto Session_query_sc_qubit_coherence_t2(DeviceSession *session,
                                         AD_QDMI_SCQubit qubit, uint64_t *value)
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
  if (const auto ret =
          session->context->device->v1_library.session_query_site_property(
              session->v1_session, reinterpret_cast<V1Site>(qubit),
              V1_SITE_PROPERTY::T2, sizeof(uint64_t), value, nullptr);
      ret != SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Failed to query qubit coherence time t2: {}",
                        session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_session_query_coupling_map
auto Session_query_coupling_map(DeviceSession *session, const size_t size,
                                AD_QDMI_SCQubit *values, size_t *size_ret)
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
          session->context->device->v1_library.session_query_device_property(
              session->v1_session, V1_DEVICE_PROPERTY::COUPLINGMAP, size,
              values, size_ret);
      ret != SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Failed to query couping map: {}",
                        session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_session_query_sc_operations
auto Session_query_sc_operations(DeviceSession *session, const size_t size,
                                 AD_QDMI_SCOperation *values, size_t *size_ret)
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
          session->context->device->v1_library.session_query_device_property(
              session->v1_session, V1_DEVICE_PROPERTY::OPERATIONS, size, values,
              size_ret);
      ret != SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Failed to query operations: {}",
                        session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_session_query_sc_operation_by_id
auto Session_query_sc_operation_by_id(DeviceSession *session, const char *id,
                                      AD_QDMI_SCOperation *operation) -> int {
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
  size_t size{};
  if (const auto ret{
          session->context->device->v1_library.session_query_device_property(
              session->v1_session, V1_DEVICE_PROPERTY::OPERATIONS, 0, nullptr,
              &size)};
      ret != SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Failed to query operation count: {}",
                        session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  std::vector operations(size, V1Operation{});
  if (const auto ret{
          session->context->device->v1_library.session_query_device_property(
              session->v1_session, V1_DEVICE_PROPERTY::OPERATIONS,
              size * sizeof(AD_QDMI_SCOperation), operations.data(), nullptr)};
      ret != SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Failed to query operations: {}",
                        session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  for (const auto &v1_operation : operations) {
    if (const auto ret{session->context->device->v1_library
                           .session_query_operation_property(
                               session->v1_session, v1_operation, 0, nullptr, 0,
                               nullptr, V1_OPERATION_PROPERTY::NAME, 0, nullptr,
                               &size)};
        ret != SUCCESS) {
      session->logger.log(QDMI_LOG_LEVEL_ERROR,
                          "[{}] Failed to query operation ID size: {}",
                          session->context->device->id, ret);
      return Transform_return_codes(ret);
    }
    std::string operation_id(size - 1, '\0');
    if (const auto ret{session->context->device->v1_library
                           .session_query_operation_property(
                               session->v1_session, v1_operation, 0, nullptr, 0,
                               nullptr, V1_OPERATION_PROPERTY::NAME, size,
                               operation_id.data(), nullptr)};
        ret != SUCCESS) {
      session->logger.log(QDMI_LOG_LEVEL_ERROR,
                          "[{}] Failed to query operation ID: {}",
                          session->context->device->id, ret);
      return Transform_return_codes(ret);
    }
    if (operation_id == id) {
      if (v1_operation != nullptr) {
        *operation = reinterpret_cast<AD_QDMI_SCOperation>(v1_operation);
      }
      return QDMI_SUCCESS;
    }
  }
  session->logger.log(QDMI_LOG_LEVEL_ERROR,
                      "[{}] Operation ID \"{}\" not found",
                      session->context->device->id, id);
  return QDMI_ERROR_NOTFOUND;
}

/// @copydoc AD_QDMI_session_query_sc_operation_id
auto Session_query_sc_operation_id(DeviceSession *session,
                                   AD_QDMI_SCOperation operation,
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
  if (const auto ret =
          session->context->device->v1_library.session_query_operation_property(
              session->v1_session, reinterpret_cast<V1Operation>(operation), 0,
              nullptr, 0, nullptr, V1_OPERATION_PROPERTY::NAME, size, value,
              size_ret);
      ret != SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Failed to query operation name: {}",
                        session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_session_query_sc_operation_name
auto Session_query_sc_operation_name(DeviceSession *session,
                                     AD_QDMI_SCOperation operation,
                                     const size_t size, char *value,
                                     size_t *size_ret) -> int {
  return Session_query_sc_operation_id(session, operation, size, value,
                                       size_ret);
}
/// @copydoc AD_QDMI_session_query_sc_operation_parameter_count
auto Session_query_sc_operation_parameter_count(DeviceSession *session,
                                                AD_QDMI_SCOperation operation,
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
          session->context->device->v1_library.session_query_operation_property(
              session->v1_session, reinterpret_cast<V1Operation>(operation), 0,
              nullptr, 0, nullptr, V1_OPERATION_PROPERTY::PARAMETERSNUM,
              sizeof(size_t), value, nullptr);
      ret != SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Failed to query operation parameter count: {}",
                        session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_session_query_sc_operation_qubit_count
auto Session_query_sc_operation_qubit_count(DeviceSession *session,
                                            AD_QDMI_SCOperation operation,
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
          session->context->device->v1_library.session_query_operation_property(
              session->v1_session, reinterpret_cast<V1Operation>(operation), 0,
              nullptr, 0, nullptr, V1_OPERATION_PROPERTY::QUBITSNUM,
              sizeof(size_t), value, nullptr);
      ret != SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Failed to query operation qubit count: {}",
                        session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_session_query_sc_operation_qubits
auto Session_query_sc_operation_qubits(DeviceSession *session,
                                       AD_QDMI_SCOperation operation,
                                       const size_t size,
                                       AD_QDMI_SCQubit *values,
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
          session->context->device->v1_library.session_query_operation_property(
              session->v1_session, reinterpret_cast<V1Operation>(operation), 0,
              nullptr, 0, nullptr, V1_OPERATION_PROPERTY::SITES, size, values,
              size_ret);
      ret != SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Failed to query operation qubits: {}",
                        session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_session_query_sc_operation_duration
auto Session_query_sc_operation_duration(DeviceSession *session,
                                         AD_QDMI_SCOperation operation,
                                         const AD_QDMI_SCQubit *qubits,
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
  size_t qubits_num{};
  if (qubits != nullptr) {
    if (const auto ret =
            session->context->device->v1_library
                .session_query_operation_property(
                    session->v1_session,
                    reinterpret_cast<V1Operation>(operation), 0, nullptr, 0,
                    nullptr, V1_OPERATION_PROPERTY::QUBITSNUM, sizeof(size_t),
                    &qubits_num, nullptr);
        ret != SUCCESS) {
      session->logger.log(QDMI_LOG_LEVEL_ERROR,
                          "[{}] Failed to query operation qubits count: {}",
                          session->context->device->id, ret);
      return Transform_return_codes(ret);
    }
  }
  size_t params_num{};
  if (params != nullptr) {
    if (const auto ret =
            session->context->device->v1_library
                .session_query_operation_property(
                    session->v1_session,
                    reinterpret_cast<V1Operation>(operation), 0, nullptr, 0,
                    nullptr, V1_OPERATION_PROPERTY::PARAMETERSNUM,
                    sizeof(size_t), &params_num, nullptr);
        ret != SUCCESS) {
      session->logger.log(QDMI_LOG_LEVEL_ERROR,
                          "[{}] Failed to query operation parameters count: {}",
                          session->context->device->id, ret);
      return Transform_return_codes(ret);
    }
  }
  if (const auto ret =
          session->context->device->v1_library.session_query_operation_property(
              session->v1_session, reinterpret_cast<V1Operation>(operation),
              qubits_num, reinterpret_cast<const V1Site *>(qubits), params_num,
              params, V1_OPERATION_PROPERTY::DURATION, sizeof(uint64_t), value,
              nullptr);
      ret != SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Failed to query operation fidelity: {}",
                        session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  return QDMI_SUCCESS;
}
/// @copydoc AD_QDMI_session_query_sc_operation_fidelity
auto Session_query_sc_operation_fidelity(DeviceSession *session,
                                         AD_QDMI_SCOperation operation,
                                         const AD_QDMI_SCQubit *qubits,
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
  size_t qubits_num{};
  if (qubits != nullptr) {
    if (const auto ret =
            session->context->device->v1_library
                .session_query_operation_property(
                    session->v1_session,
                    reinterpret_cast<V1Operation>(operation), 0, nullptr, 0,
                    nullptr, V1_OPERATION_PROPERTY::QUBITSNUM, sizeof(size_t),
                    &qubits_num, nullptr);
        ret != SUCCESS) {
      session->logger.log(QDMI_LOG_LEVEL_ERROR,
                          "[{}] Failed to query operation qubits count: {}",
                          session->context->device->id, ret);
      return Transform_return_codes(ret);
    }
  }
  size_t params_num{};
  if (params != nullptr) {
    if (const auto ret =
            session->context->device->v1_library
                .session_query_operation_property(
                    session->v1_session,
                    reinterpret_cast<V1Operation>(operation), 0, nullptr, 0,
                    nullptr, V1_OPERATION_PROPERTY::PARAMETERSNUM,
                    sizeof(size_t), &params_num, nullptr);
        ret != SUCCESS) {
      session->logger.log(QDMI_LOG_LEVEL_ERROR,
                          "[{}] Failed to query operation parameters count: {}",
                          session->context->device->id, ret);
      return Transform_return_codes(ret);
    }
  }
  if (const auto ret =
          session->context->device->v1_library.session_query_operation_property(
              session->v1_session, reinterpret_cast<V1Operation>(operation),
              qubits_num, reinterpret_cast<const V1Site *>(qubits), params_num,
              params, V1_OPERATION_PROPERTY::FIDELITY, sizeof(double), value,
              nullptr);
      ret != SUCCESS) {
    session->logger.log(QDMI_LOG_LEVEL_ERROR,
                        "[{}] Failed to query operation fidelity: {}",
                        session->context->device->id, ret);
    return Transform_return_codes(ret);
  }
  return QDMI_SUCCESS;
}
} // namespace
auto Get_device_sc_interface() -> AD_QDMI_SC_Interface {
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
} // namespace qdmi::adapter

// NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast,misc-include-cleaner)
