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

#include "control_qdmi/device.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>

struct CONTROL_QDMI_Device_Session_impl_d {};
struct CONTROL_QDMI_Site_impl_d {
  size_t index;
};
struct CONTROL_QDMI_Operation_impl_d {
  size_t index;
};

namespace {
// The fake C ABI needs process-wide state and stable, mutable opaque handles.
// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
bool initialized = false;
std::array<CONTROL_QDMI_Site_impl_d, 3> sites{{{17}, {42}, {99}}};
std::array<CONTROL_QDMI_Operation_impl_d, 2> operations{{{0}, {1}}};
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

auto Copy(size_t bytes, const void *source, size_t size, void *value,
          size_t *size_ret) -> int {
  if (value != nullptr && size < bytes) {
    // Error returns may overwrite the advertised buffer bytes.
    std::memset(value, 0, size);
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (size_ret != nullptr) {
    *size_ret = bytes;
  }
  if (value != nullptr && bytes != 0) {
    std::memcpy(value, source, bytes);
  }
  return QDMI_SUCCESS;
}

template <class T>
auto Scalar(const T &source, size_t size, void *value, size_t *size_ret)
    -> int {
  return Copy(sizeof(T), &source, size, value, size_ret);
}
} // namespace

int CONTROL_QDMI_device_initialize() {
  if (initialized) {
    return QDMI_ERROR_BADSTATE;
  }
  initialized = true;
  return QDMI_SUCCESS;
}
int CONTROL_QDMI_device_finalize() {
  if (!initialized) {
    return QDMI_ERROR_BADSTATE;
  }
  initialized = false;
  return QDMI_SUCCESS;
}
int CONTROL_QDMI_device_session_alloc(CONTROL_QDMI_Device_Session *session) {
  *session = new CONTROL_QDMI_Device_Session_impl_d;
  return QDMI_SUCCESS;
}
int CONTROL_QDMI_device_session_init(CONTROL_QDMI_Device_Session /*session*/) {
  return initialized ? QDMI_SUCCESS : QDMI_ERROR_BADSTATE;
}
void CONTROL_QDMI_device_session_free(CONTROL_QDMI_Device_Session session) {
  delete session;
}
int CONTROL_QDMI_device_session_query_device_property(
    CONTROL_QDMI_Device_Session session, QDMI_Device_Property prop, size_t size,
    void *value, size_t *size_ret) {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  switch (prop) {
  case QDMI_DEVICE_PROPERTY_QUBITSNUM:
    return Scalar(size_t{0x11223344}, size, value, size_ret);
  case QDMI_DEVICE_PROPERTY_STATUS:
    return Scalar(QDMI_DEVICE_STATUS_IDLE, size, value, size_ret);
  case QDMI_DEVICE_PROPERTY_SITES: {
    const std::array handles{sites.data(), &sites[1], &sites[2]};
    return Copy(sizeof(handles), static_cast<const void *>(handles.data()),
                size, value, size_ret);
  }
  case QDMI_DEVICE_PROPERTY_OPERATIONS: {
    const std::array handles{operations.data(), &operations[1]};
    return Copy(sizeof(handles), static_cast<const void *>(handles.data()),
                size, value, size_ret);
  }
  case QDMI_DEVICE_PROPERTY_COUPLINGMAP:
    return Copy(0, nullptr, size, value, size_ret);
  case QDMI_DEVICE_PROPERTY_LENGTHUNIT:
    return Copy(sizeof(""), "", size, value, size_ret);
  case QDMI_DEVICE_PROPERTY_DURATIONUNIT:
    return Copy(sizeof("picosecond"), "picosecond", size, value, size_ret);
  case QDMI_DEVICE_PROPERTY_QUEUELENGTH:
  case QDMI_DEVICE_PROPERTY_SUPPORTEDPROGRAMFORMATS:
    // Both a size_t scalar and an enum array need more than one byte.
    return Scalar(char{0}, size, value, size_ret);
  case QDMI_DEVICE_PROPERTY_MAX:
    return QDMI_SUCCESS;
  default:
    return QDMI_ERROR_NOTSUPPORTED;
  }
}
int CONTROL_QDMI_device_session_query_site_property(
    CONTROL_QDMI_Device_Session session, CONTROL_QDMI_Site site,
    QDMI_Site_Property prop, size_t size, void *value, size_t *size_ret) {
  if (session == nullptr || site == nullptr || prop == QDMI_SITE_PROPERTY_MAX) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (prop == QDMI_SITE_PROPERTY_XCOORDINATE) {
    return Scalar(int64_t{17}, size, value, size_ret);
  }
  if (prop == QDMI_SITE_PROPERTY_ISZONE) {
    return Scalar(true, size, value, size_ret);
  }
  if (prop == QDMI_SITE_PROPERTY_INDEX && site != &sites[2]) {
    return Scalar(site->index, size, value, size_ret);
  }
  if (prop == QDMI_SITE_PROPERTY_T1 && site == &sites[1]) {
    return Scalar(uint64_t{100}, size, value, size_ret);
  }
  return QDMI_ERROR_NOTSUPPORTED;
}
int CONTROL_QDMI_device_session_query_operation_property(
    CONTROL_QDMI_Device_Session session, CONTROL_QDMI_Operation operation,
    size_t /*num_sites*/, const CONTROL_QDMI_Site * /*sites*/,
    size_t /*num_params*/, const double * /*params*/,
    QDMI_Operation_Property prop, size_t size, void *value, size_t *size_ret) {
  if (session == nullptr || operation == nullptr ||
      prop == QDMI_OPERATION_PROPERTY_MAX) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (prop == QDMI_OPERATION_PROPERTY_NAME && operation == &operations[1]) {
    return Copy(1, "", size, value, size_ret);
  }
  if (prop == QDMI_OPERATION_PROPERTY_SITES) {
    return Copy(0, nullptr, size, value, size_ret);
  }
  return QDMI_ERROR_NOTSUPPORTED;
}
