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

#include "common/dynamic_library/dynamic_library.hpp"

#include <cstddef>
#include <functional>
#include <string>

namespace qdmi::adapter {
struct V1SessionImpl;
using V1Session = V1SessionImpl *;
struct V1SiteImpl;
using V1Site = V1SiteImpl *;
struct V1OperationImpl;
using V1Operation = V1OperationImpl *;
struct V1JobImpl;
using V1Job = V1JobImpl *;
// NOLINTNEXTLINE(performance-enum-size,cppcoreguidelines-use-enum-class)
enum V1_STATUS : int {
  WARN_GENERAL = 1,
  SUCCESS = 0,
  ERROR_FATAL = -1,
  ERROR_OUTOFMEM = -2,
  ERROR_NOTIMPLEMENTED = -3,
  ERROR_LIBNOTFOUND = -4,
  ERROR_NOTFOUND = -5,
  ERROR_OUTOFRANGE = -6,
  ERROR_INVALIDARGUMENT = -7,
  ERROR_PERMISSIONDENIED = -8,
  ERROR_NOTSUPPORTED = -9,
  ERROR_BADSTATE = -10,
  ERROR_TIMEOUT = -11
};
// NOLINTNEXTLINE(performance-enum-size)
enum class V1_SESSION_PARAMETER : int {
  BASE_URL,
  TOKEN,
  AUTH_FILE,
  AUTH_URL,
  USERNAME,
  PASSWORD
};
// NOLINTNEXTLINE(performance-enum-size)
enum class V1_DEVICE_PROPERTY : int {
  NAME,
  VERSION,
  STATUS,
  LIBRARYVERSION,
  QUBITSNUM,
  SITES,
  OPERATIONS,
  COUPLINGMAP,
  NEEDSCALIBRATION,
  PULSESUPPORT,
  LENGTHUNIT,
  LENGTHSCALEFACTOR,
  DURATIONUNIT,
  DURATIONSCALEFACTOR,
  MINATOMDISTANCE,
  SUPPORTEDPROGRAMFORMATS
};
// NOLINTNEXTLINE(performance-enum-size)
enum class V1_SITE_PROPERTY : int {
  INDEX,
  T1,
  T2,
  NAME,
  XCOORDINATE,
  YCOORDINATE,
  ZCOORDINATE,
  ISZONE,
  XEXTENT,
  YEXTENT,
  ZEXTENT,
  MODULEINDEX,
  SUBMODULEINDEX
};
// NOLINTNEXTLINE(performance-enum-size)
enum class V1_OPERATION_PROPERTY : int {
  NAME,
  QUBITSNUM,
  PARAMETERSNUM,
  DURATION,
  FIDELITY,
  INTERACTIONRADIUS,
  BLOCKINGRADIUS,
  IDLINGFIDELITY,
  ISZONED,
  SITES,
  MEANSHUTTLINGSPEED
};
// NOLINTNEXTLINE(performance-enum-size)
enum class V1_JOB_PARAMETER : int { PROGRAMFORMAT, PROGRAM, SHOTSNUM };
// NOLINTNEXTLINE(performance-enum-size)
enum class V1_JOB_PROPERTY : int { ID, PROGRAMFORMAT, PROGRAM, SHOTSNUM };
// NOLINTNEXTLINE(performance-enum-size)
enum class V1_JOB_STATUS : int {
  CREATED,
  SUBMITTED,
  QUEUED,
  RUNNING,
  DONE,
  CANCELED,
  FAILED
};
// NOLINTNEXTLINE(performance-enum-size)
enum class V1_DEVICE_STATUS : int {
  OFFLINE,
  IDLE,
  BUSY,
  ERROR,
  MAINTENANCE,
  CALIBRATION
};
// NOLINTNEXTLINE(performance-enum-size)
enum class V1_PROGRAM_FORMAT : int {
  QASM2,
  QASM3,
  QIRBASESTRING,
  QIRBASEMODULE,
  QIRADAPTIVESTRING,
  QIRADAPTIVEMODULE,
  CALIBRATION,
  QPY,
  IQMJSON
};
// NOLINTNEXTLINE(performance-enum-size)
enum class V1_JOB_RESULT : int {
  SHOTS,
  HIST_KEYS,
  HIST_VALUES,
  STATEVECTOR_DENSE,
  PROBABILITIES_DENSE,
  STATEVECTOR_SPARSE_KEYS,
  STATEVECTOR_SPARSE_VALUES,
  PROBABILITIES_SPARSE_KEYS,
  PROBABILITIES_SPARSE_VALUES
};
class V1Library {
public:
  auto session_alloc(V1Session *session) const -> int;
  auto session_set_parameter(V1Session session, V1_SESSION_PARAMETER param,
                             size_t size, const void *value) const -> int;
  auto session_init(V1Session session) const -> int;
  auto session_free(V1Session session) const -> void;
  auto session_query_device_property(V1Session session, V1_DEVICE_PROPERTY prop,
                                     size_t size, void *value,
                                     size_t *size_ret) const -> int;
  auto session_query_site_property(V1Session session, V1Site site,
                                   V1_SITE_PROPERTY prop, size_t size,
                                   void *value, size_t *size_ret) const -> int;
  auto session_query_operation_property(V1Session session,
                                        V1Operation operation, size_t num_sites,
                                        const V1Site *sites, size_t num_params,
                                        const double *params,
                                        V1_OPERATION_PROPERTY prop, size_t size,
                                        void *value, size_t *size_ret) const
      -> int;
  auto session_create_job(V1Session session, V1Job *job) const -> int;
  auto job_set_parameter(V1Job job, V1_JOB_PARAMETER param, size_t size,
                         const void *value) const -> int;
  auto job_query_property(V1Job job, V1_JOB_PROPERTY prop, size_t size,
                          void *value, size_t *size_ret) const -> int;
  auto job_submit(V1Job job) const -> int;
  auto job_cancel(V1Job job) const -> int;
  auto job_check(V1Job job, V1_JOB_STATUS *status) const -> int;
  auto job_wait(V1Job job, size_t timeout) const -> int;
  auto job_get_results(V1Job job, V1_JOB_RESULT result, size_t size,
                       void *value, size_t *size_ret) const -> int;
  auto job_free(V1Job job) const -> void;

  V1Library() = default;
  V1Library(Dylib dylib, std::string prefix);
  V1Library(const V1Library &other);
  V1Library &operator=(const V1Library &other);
  V1Library(V1Library &&other) noexcept;
  V1Library &operator=(V1Library &&other) noexcept;
  virtual ~V1Library();

private:
  auto swap(V1Library &other) noexcept -> void;

  std::function<int()> initialize_{};
  std::function<int()> finalize_{};
  std::function<int(V1Session *)> session_alloc_{};
  std::function<int(V1Session, V1_SESSION_PARAMETER, size_t, const void *)>
      session_set_parameter_{};
  std::function<int(V1Session)> session_init_{};
  std::function<void(V1Session)> session_free_{};
  std::function<int(V1Session, V1_DEVICE_PROPERTY, size_t, void *, size_t *)>
      session_query_device_property_{};
  std::function<int(V1Session, V1Site, V1_SITE_PROPERTY, size_t, void *,
                    size_t *)>
      session_query_site_property_{};
  std::function<int(V1Session, V1Operation, size_t, const V1Site *, size_t,
                    const double *, V1_OPERATION_PROPERTY, size_t, void *,
                    size_t *)>
      session_query_operation_property_{};
  std::function<int(V1Session, V1Job *)> session_create_job_{};
  std::function<int(V1Job, V1_JOB_PARAMETER, size_t, const void *)>
      job_set_parameter_{};
  std::function<int(V1Job, V1_JOB_PROPERTY, size_t, void *, size_t *)>
      job_query_property_{};
  std::function<int(V1Job)> job_submit_{};
  std::function<int(V1Job)> job_cancel_{};
  std::function<int(V1Job, V1_JOB_STATUS *)> job_check_{};
  std::function<int(V1Job, size_t)> job_wait_{};
  std::function<int(V1Job, V1_JOB_RESULT, size_t, void *, size_t *)>
      job_get_results_{};
  std::function<void(V1Job)> job_free_{};

  Dylib dylib_{};
  std::string prefix_{};
};
} // namespace qdmi::adapter
