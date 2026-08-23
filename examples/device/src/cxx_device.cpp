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

/** @file
 * @brief A simple example of a device implementation in C++.
 * @details This file can be used as a template for implementing a device in
 * C++.
 */

#include "cxx_qdmi/device.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <complex>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <functional>
#include <iterator>
#include <limits>
#include <map>
#include <new>
#include <random>
#include <ranges>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

// NOLINTBEGIN(*-pro-bounds-avoid-unchecked-container-access,*-throwing-static-initialization)

/**
 * @brief Implementation of the CXX_QDMI_Device_Session structure.
 * @details This structure can, e.g., be used to store a token to access an API.
 */
enum class CXX_QDMI_DEVICE_SESSION_STATUS : uint8_t { ALLOCATED, INITIALIZED };
struct CXX_QDMI_Device_Session_impl_d {
  std::string token;
  CXX_QDMI_DEVICE_SESSION_STATUS status =
      CXX_QDMI_DEVICE_SESSION_STATUS::ALLOCATED;
};

/**
 * @brief Implementation of the CXX_QDMI_Device_Job structure.
 * @details This structure can, e.g., be used to store the job id.
 */
struct CXX_QDMI_Device_Job_impl_d {
  struct Result_data {
    std::vector<std::string> shots;
    std::vector<std::complex<double>> state_vec;
  };

  CXX_QDMI_Device_Session session = nullptr;
  int id = 0;
  QDMI_Program_Format format = QDMI_PROGRAM_FORMAT_MAX;
  std::vector<std::vector<char>> programs;
  QDMI_Job_Status status = QDMI_JOB_STATUS_SUBMITTED;
  size_t num_shots = 0;
  std::vector<Result_data> results;
};

struct CXX_QDMI_Device_State {
  QDMI_Device_Status status = QDMI_DEVICE_STATUS_OFFLINE;
  std::mt19937 gen{80333}; // Seeded with a constant for reproducibility
  std::uniform_int_distribution<> dis =
      std::uniform_int_distribution<>(0, std::numeric_limits<int>::max());
  std::bernoulli_distribution dis_bin{0.5};
  std::uniform_real_distribution<> dis_real =
      std::uniform_real_distribution<>(-1.0, 1.0);
};

/**
 * @brief Implementation of the CXX_QDMI_Site structure.
 * @details This structure can, e.g., be used to store the site id.
 */
struct CXX_QDMI_Site_impl_d {
  size_t id;
};

/**
 * @brief Implementation of the CXX_QDMI_Operation structure.
 * @details This structure can, e.g., be used to store the operation id.
 */
struct CXX_QDMI_Operation_impl_d {
  std::string name;
};

namespace {
/**
 * @brief Static function to maintain the device state.
 * @return a pointer to the device state.
 * @note This function is considered private and should not be used outside of
 * this file. Hence, it is not part of any header file.
 */
CXX_QDMI_Device_State *CXX_QDMI_get_device_state() {
  static CXX_QDMI_Device_State device_state;
  return &device_state;
}

/**
 * @brief Local function to read the device status.
 * @return the current device status.
 * @note This function is considered private and should not be used outside of
 * this file. Hence, it is not part of any header file.
 */
QDMI_Device_Status CXX_QDMI_get_device_status() {
  return CXX_QDMI_get_device_state()->status;
}

/**
 * @brief Local function to set the device status.
 * @param status the new device status.
 * @note This function is considered private and should not be used outside of
 * this file. Hence, it is not part of any header file.
 */
void CXX_QDMI_set_device_status(QDMI_Device_Status status) {
  CXX_QDMI_get_device_state()->status = status;
}

/**
 * @brief Generate a random job id.
 * @return a random job id.
 * @note This function is considered private and should not be used outside of
 * this file. Hence, it is not part of any header file.
 */
int CXX_QDMI_generate_job_id() {
  auto *state = CXX_QDMI_get_device_state();
  return state->dis(state->gen);
}

/**
 * @brief Generate a random bit.
 * @return a random bit.
 * @note This function is considered private and should not be used outside of
 * this file. Hence, it is not part of any header file.
 */
bool CXX_QDMI_generate_bit() {
  auto *state = CXX_QDMI_get_device_state();
  return state->dis_bin(state->gen);
}

/**
 * @brief Generate a random real number.
 * @return a random real number.
 * @note This function is considered private and should not be used outside of
 * this file. Hence, it is not part of any header file.
 */
double CXX_QDMI_generate_real() {
  auto *state = CXX_QDMI_get_device_state();
  return state->dis_real(state->gen);
}

const CXX_QDMI_Operation_impl_d RX{"rx"};
const CXX_QDMI_Operation_impl_d RY{"ry"};
const CXX_QDMI_Operation_impl_d RZ{"rz"};
const CXX_QDMI_Operation_impl_d CX{"cx"};

constexpr std::array<const CXX_QDMI_Operation_impl_d *, 4>
    CXX_DEVICE_OPERATIONS = {&RX, &RY, &RZ, &CX};

constexpr CXX_QDMI_Site_impl_d SITE0{0};
constexpr CXX_QDMI_Site_impl_d SITE1{1};
constexpr CXX_QDMI_Site_impl_d SITE2{2};
constexpr CXX_QDMI_Site_impl_d SITE3{3};
constexpr CXX_QDMI_Site_impl_d SITE4{4};

constexpr std::array<const CXX_QDMI_Site_impl_d *, 5> CXX_DEVICE_SITES = {
    &SITE0, &SITE1, &SITE2, &SITE3, &SITE4};

constexpr std::array<const CXX_QDMI_Site_impl_d *, 20>
    // clang-format off
    DEVICE_COUPLING_MAP = {
      CXX_DEVICE_SITES[0], CXX_DEVICE_SITES[1],
      CXX_DEVICE_SITES[1], CXX_DEVICE_SITES[0],
      CXX_DEVICE_SITES[1], CXX_DEVICE_SITES[2],
      CXX_DEVICE_SITES[2], CXX_DEVICE_SITES[1],
      CXX_DEVICE_SITES[2], CXX_DEVICE_SITES[3],
      CXX_DEVICE_SITES[3], CXX_DEVICE_SITES[2],
      CXX_DEVICE_SITES[3], CXX_DEVICE_SITES[4],
      CXX_DEVICE_SITES[4], CXX_DEVICE_SITES[3],
      CXX_DEVICE_SITES[4], CXX_DEVICE_SITES[0],
      CXX_DEVICE_SITES[0], CXX_DEVICE_SITES[4]};
// clang-format on

const std::unordered_map<const CXX_QDMI_Operation_impl_d *,
                         std::pair<std::string, uint64_t>>
    OPERATION_PROPERTIES = {
        {CXX_DEVICE_OPERATIONS[0], {"rx", 10}},
        {CXX_DEVICE_OPERATIONS[1], {"ry", 10}},
        {CXX_DEVICE_OPERATIONS[2], {"rz", 10}},
        {CXX_DEVICE_OPERATIONS[3], {"cx", 100}},
};

struct CXX_QDMI_Pair_hash {
  template <class T1, class T2>
  size_t operator()(const std::pair<T1, T2> &p) const {
    auto hash1 = std::hash<T1>{}(p.first);
    auto hash2 = std::hash<T2>{}(p.second);
    return hash1 ^ hash2;
  }
};

const std::unordered_map<
    const CXX_QDMI_Operation_impl_d *,
    std::unordered_map<
        std::pair<const CXX_QDMI_Site_impl_d *, const CXX_QDMI_Site_impl_d *>,
        double, CXX_QDMI_Pair_hash>>
    OPERATION_FIDELITIES = {
        {CXX_DEVICE_OPERATIONS[3],
         {{{CXX_DEVICE_SITES[0], CXX_DEVICE_SITES[1]}, 0.99},
          {{CXX_DEVICE_SITES[1], CXX_DEVICE_SITES[0]}, 0.99},
          {{CXX_DEVICE_SITES[1], CXX_DEVICE_SITES[2]}, 0.98},
          {{CXX_DEVICE_SITES[2], CXX_DEVICE_SITES[1]}, 0.98},
          {{CXX_DEVICE_SITES[2], CXX_DEVICE_SITES[3]}, 0.97},
          {{CXX_DEVICE_SITES[3], CXX_DEVICE_SITES[2]}, 0.97},
          {{CXX_DEVICE_SITES[3], CXX_DEVICE_SITES[4]}, 0.96},
          {{CXX_DEVICE_SITES[4], CXX_DEVICE_SITES[3]}, 0.96},
          {{CXX_DEVICE_SITES[4], CXX_DEVICE_SITES[0]}, 0.95},
          {{CXX_DEVICE_SITES[0], CXX_DEVICE_SITES[4]}, 0.95}}},
        // No need to specify single-qubit fidelities here
};

constexpr std::array SUPPORTED_PROGRAM_FORMATS = {
    QDMI_PROGRAM_FORMAT_QASM2, QDMI_PROGRAM_FORMAT_QIRBASESTRING,
    QDMI_PROGRAM_FORMAT_QIRBASEMODULE, QDMI_PROGRAM_FORMAT_CALIBRATION};
} // namespace

// NOLINTBEGIN(bugprone-macro-parentheses)
#define ADD_SINGLE_VALUE_PROPERTY(prop_name, prop_type, prop_value, prop,      \
                                  size, value, size_ret)                       \
  {                                                                            \
    if ((prop) == (prop_name)) {                                               \
      if ((value) != nullptr) {                                                \
        if ((size) < sizeof(prop_type)) {                                      \
          return QDMI_ERROR_INVALIDARGUMENT;                                   \
        }                                                                      \
        *static_cast<prop_type *>(value) = prop_value;                         \
      }                                                                        \
      if ((size_ret) != nullptr) {                                             \
        *size_ret = sizeof(prop_type);                                         \
      }                                                                        \
      return QDMI_SUCCESS;                                                     \
    }                                                                          \
  } /// [DOXYGEN MACRO END]

#define ADD_STRING_PROPERTY(prop_name, prop_value, prop, size, value,          \
                            size_ret)                                          \
  {                                                                            \
    if ((prop) == (prop_name)) {                                               \
      if ((value) != nullptr) {                                                \
        if ((size) < strlen(prop_value) + 1) {                                 \
          return QDMI_ERROR_INVALIDARGUMENT;                                   \
        }                                                                      \
        strncpy(static_cast<char *>(value), prop_value, size);                 \
        static_cast<char *>(value)[size - 1] = '\0';                           \
      }                                                                        \
      if ((size_ret) != nullptr) {                                             \
        *size_ret = strlen(prop_value) + 1;                                    \
      }                                                                        \
      return QDMI_SUCCESS;                                                     \
    }                                                                          \
  } /// [DOXYGEN MACRO END]

#define ADD_LIST_PROPERTY(prop_name, prop_type, prop_values, prop, size,       \
                          value, size_ret)                                     \
  {                                                                            \
    if ((prop) == (prop_name)) {                                               \
      if ((value) != nullptr) {                                                \
        if ((size) < (prop_values).size() * sizeof(prop_type)) {               \
          return QDMI_ERROR_INVALIDARGUMENT;                                   \
        }                                                                      \
        memcpy(static_cast<void *>(value),                                     \
               static_cast<const void *>((prop_values).data()),                \
               (prop_values).size() * sizeof(prop_type));                      \
      }                                                                        \
      if ((size_ret) != nullptr) {                                             \
        *size_ret = (prop_values).size() * sizeof(prop_type);                  \
      }                                                                        \
      return QDMI_SUCCESS;                                                     \
    }                                                                          \
  } /// [DOXYGEN MACRO END]
// NOLINTEND(bugprone-macro-parentheses)

int CXX_QDMI_device_initialize() {
  CXX_QDMI_set_device_status(QDMI_DEVICE_STATUS_IDLE);
  return QDMI_SUCCESS;
} /// [DOXYGEN FUNCTION END]

int CXX_QDMI_device_finalize() {
  CXX_QDMI_set_device_status(QDMI_DEVICE_STATUS_OFFLINE);
  return QDMI_SUCCESS;
} /// [DOXYGEN FUNCTION END]

int CXX_QDMI_device_session_alloc(CXX_QDMI_Device_Session *session) {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  *session = new CXX_QDMI_Device_Session_impl_d();
  return QDMI_SUCCESS;
} /// [DOXYGEN FUNCTION END]

int CXX_QDMI_device_session_init(CXX_QDMI_Device_Session session) {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  switch (CXX_QDMI_get_device_status()) {
  case QDMI_DEVICE_STATUS_ERROR:
  case QDMI_DEVICE_STATUS_OFFLINE:
  case QDMI_DEVICE_STATUS_MAINTENANCE:
    return QDMI_ERROR_FATAL;
  default:
    break;
  }
  if (session->token.empty()) {
    return QDMI_ERROR_PERMISSIONDENIED;
  }
  session->status = CXX_QDMI_DEVICE_SESSION_STATUS::INITIALIZED;
  return QDMI_SUCCESS;
} /// [DOXYGEN FUNCTION END]

void CXX_QDMI_device_session_free(CXX_QDMI_Device_Session session) {
  delete session;
} /// [DOXYGEN FUNCTION END]

int CXX_QDMI_device_session_set_parameter(CXX_QDMI_Device_Session session,
                                          QDMI_Device_Session_Parameter param,
                                          size_t size, const void *value) {
  if (session == nullptr || (value != nullptr && size == 0) ||
      (param >= QDMI_DEVICE_SESSION_PARAMETER_MAX &&
       param != QDMI_DEVICE_SESSION_PARAMETER_CUSTOM1 &&
       param != QDMI_DEVICE_SESSION_PARAMETER_CUSTOM2 &&
       param != QDMI_DEVICE_SESSION_PARAMETER_CUSTOM3 &&
       param != QDMI_DEVICE_SESSION_PARAMETER_CUSTOM4 &&
       param != QDMI_DEVICE_SESSION_PARAMETER_CUSTOM5)) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (session->status != CXX_QDMI_DEVICE_SESSION_STATUS::ALLOCATED) {
    return QDMI_ERROR_BADSTATE;
  }
  if (param != QDMI_DEVICE_SESSION_PARAMETER_TOKEN) {
    return QDMI_ERROR_NOTSUPPORTED;
  }
  if (value != nullptr) {
    session->token = std::string(static_cast<const char *>(value), size);
  }
  return QDMI_SUCCESS;
} /// [DOXYGEN FUNCTION END]

int CXX_QDMI_device_session_create_device_job(CXX_QDMI_Device_Session session,
                                              CXX_QDMI_Device_Job *job) {
  if (session == nullptr || job == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (session->status != CXX_QDMI_DEVICE_SESSION_STATUS::INITIALIZED) {
    return QDMI_ERROR_BADSTATE;
  }

  *job = new CXX_QDMI_Device_Job_impl_d;
  (*job)->session = session;
  // set job id to random number for demonstration purposes
  (*job)->id = CXX_QDMI_generate_job_id();
  (*job)->status = QDMI_JOB_STATUS_CREATED;
  return QDMI_SUCCESS;
} /// [DOXYGEN FUNCTION END]

int CXX_QDMI_device_session_retrieve_device_job_by_id(
    [[maybe_unused]] CXX_QDMI_Device_Session session,
    [[maybe_unused]] const char *job_id,
    [[maybe_unused]] CXX_QDMI_Device_Job *job) {
  return QDMI_ERROR_NOTSUPPORTED;
} /// [DOXYGEN FUNCTION END]

void CXX_QDMI_device_job_free(CXX_QDMI_Device_Job job) {
  delete job;
} /// [DOXYGEN FUNCTION END]

int CXX_QDMI_device_job_set_parameter(CXX_QDMI_Device_Job job,
                                      const QDMI_Device_Job_Parameter param,
                                      const size_t size, const void *value) {
  if (job == nullptr || (value != nullptr && size == 0) ||
      (param >= QDMI_DEVICE_JOB_PARAMETER_MAX &&
       param != QDMI_DEVICE_JOB_PARAMETER_CUSTOM1 &&
       param != QDMI_DEVICE_JOB_PARAMETER_CUSTOM2 &&
       param != QDMI_DEVICE_JOB_PARAMETER_CUSTOM3 &&
       param != QDMI_DEVICE_JOB_PARAMETER_CUSTOM4 &&
       param != QDMI_DEVICE_JOB_PARAMETER_CUSTOM5)) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (job->status != QDMI_JOB_STATUS_CREATED) {
    return QDMI_ERROR_BADSTATE;
  }
  switch (param) {
  case QDMI_DEVICE_JOB_PARAMETER_PROGRAMFORMAT:
    if (value != nullptr) {
      const auto format = *static_cast<const QDMI_Program_Format *>(value);
      if (format >= QDMI_PROGRAM_FORMAT_MAX &&
          format != QDMI_PROGRAM_FORMAT_CUSTOM1 &&
          format != QDMI_PROGRAM_FORMAT_CUSTOM2 &&
          format != QDMI_PROGRAM_FORMAT_CUSTOM3 &&
          format != QDMI_PROGRAM_FORMAT_CUSTOM4 &&
          format != QDMI_PROGRAM_FORMAT_CUSTOM5) {
        return QDMI_ERROR_INVALIDARGUMENT;
      }
      if (format != QDMI_PROGRAM_FORMAT_QASM2 &&
          format != QDMI_PROGRAM_FORMAT_QIRBASESTRING &&
          format != QDMI_PROGRAM_FORMAT_QIRBASEMODULE &&
          format != QDMI_PROGRAM_FORMAT_CALIBRATION) {
        return QDMI_ERROR_NOTSUPPORTED;
      }
      job->format = format;
    }
    return QDMI_SUCCESS;
  case QDMI_DEVICE_JOB_PARAMETER_PROGRAM:
    if (value != nullptr) {
      std::vector<char> program(size);
      memcpy(program.data(), value, size);
      job->programs = {std::move(program)};
    }
    return QDMI_SUCCESS;
  case QDMI_DEVICE_JOB_PARAMETER_SHOTSNUM:
    if (value != nullptr) {
      job->num_shots = *static_cast<const size_t *>(value);
    }
    return QDMI_SUCCESS;
  default:
    return QDMI_ERROR_NOTSUPPORTED;
  }
} /// [DOXYGEN FUNCTION END]

int CXX_QDMI_device_job_set_programs(CXX_QDMI_Device_Job job,
                                     const QDMI_Program_Format *format,
                                     const size_t count, const size_t *sizes,
                                     const void *const *programs) {
  if (job == nullptr || format == nullptr || !Valid_format(*format)) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (job->status != QDMI_JOB_STATUS_CREATED) {
    return QDMI_ERROR_BADSTATE;
  }
  if (!Supported_format(*format)) {
    return QDMI_ERROR_NOTSUPPORTED;
  }
  if (programs == nullptr) {
    return QDMI_SUCCESS;
  }
  if (count == 0 || sizes == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }

  std::vector<std::vector<char>> new_programs;
  try {
    new_programs.reserve(count);
    for (size_t i = 0; i < count; ++i) {
      if (sizes[i] == 0 || programs[i] == nullptr) {
        return QDMI_ERROR_INVALIDARGUMENT;
      }
      const auto *bytes = static_cast<const char *>(programs[i]);
      if (format->encoding == QDMI_PROGRAM_ENCODING_TEXT &&
          bytes[sizes[i] - 1] != '\0') {
        return QDMI_ERROR_INVALIDARGUMENT;
      }
      new_programs.emplace_back(bytes, bytes + sizes[i]);
    }
  } catch (const std::bad_alloc &) {
    return QDMI_ERROR_OUTOFMEM;
  } catch (const std::length_error &) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }

  job->format = *format;
  job->programs = std::move(new_programs);
  return QDMI_SUCCESS;
} /// [DOXYGEN FUNCTION END]

int CXX_QDMI_device_job_query_property(CXX_QDMI_Device_Job job,
                                       const QDMI_Device_Job_Property prop,
                                       const size_t size, void *value,
                                       size_t *size_ret) {
  if (job == nullptr || (value != nullptr && size == 0) ||
      (prop >= QDMI_DEVICE_JOB_PROPERTY_MAX &&
       prop != QDMI_DEVICE_JOB_PROPERTY_CUSTOM1 &&
       prop != QDMI_DEVICE_JOB_PROPERTY_CUSTOM2 &&
       prop != QDMI_DEVICE_JOB_PROPERTY_CUSTOM3 &&
       prop != QDMI_DEVICE_JOB_PROPERTY_CUSTOM4 &&
       prop != QDMI_DEVICE_JOB_PROPERTY_CUSTOM5)) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  const auto str = std::to_string(job->id);
  ADD_STRING_PROPERTY(QDMI_DEVICE_JOB_PROPERTY_ID, str.c_str(), prop, size,
                      value, size_ret)
  ADD_SINGLE_VALUE_PROPERTY(QDMI_DEVICE_JOB_PROPERTY_PROGRAMFORMAT,
                            QDMI_Program_Format, job->format, prop, size, value,
                            size_ret)
  ADD_SINGLE_VALUE_PROPERTY(QDMI_DEVICE_JOB_PROPERTY_SHOTSNUM, size_t,
                            job->num_shots, prop, size, value, size_ret)
  if (prop == QDMI_DEVICE_JOB_PROPERTY_PROGRAMSNUM && job->programs.empty()) {
    return QDMI_ERROR_BADSTATE;
  }
  ADD_SINGLE_VALUE_PROPERTY(QDMI_DEVICE_JOB_PROPERTY_PROGRAMSNUM, size_t,
                            job->programs.size(), prop, size, value, size_ret)
  return QDMI_ERROR_NOTSUPPORTED;
} /// [DOXYGEN FUNCTION END]

int CXX_QDMI_device_job_submit(CXX_QDMI_Device_Job job) {
  if (job == nullptr || job->status != QDMI_JOB_STATUS_CREATED) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (job->programs.empty() || !Valid_format(job->format)) {
    return QDMI_ERROR_BADSTATE;
  }

  // Calibration jobs complete immediately
  if (job->format == QDMI_PROGRAM_FORMAT_CALIBRATION) {
    job->status = QDMI_JOB_STATUS_DONE;
    return QDMI_SUCCESS;
  }

  CXX_QDMI_set_device_status(QDMI_DEVICE_STATUS_BUSY);
  job->status = QDMI_JOB_STATUS_SUBMITTED;
  // here, the actual submission of the problem to the device would happen
  // ...
  // set job status to running for demonstration purposes
  job->status = QDMI_JOB_STATUS_RUNNING;
  // generate random result data
  size_t num_qubits = 0;
  CXX_QDMI_device_session_query_device_property(
      job->session, QDMI_DEVICE_PROPERTY_QUBITSNUM, sizeof(size_t), &num_qubits,
      nullptr);
  constexpr std::array<std::string_view, 4> shot_outputs{FLAT_SHOT_OUTPUT, "10",
                                                         "11", "00"};
  job->results.clear();
  job->results.resize(job->programs.size());
  for (size_t program_index = 0; program_index < job->results.size();
       ++program_index) {
    auto &result = job->results[program_index];
    size_t output_index = 0;
    if (job->programs.size() > 1) {
      const auto &program = job->programs.at(program_index);
      const auto marker =
          program.at(program.size() -
                     (job->format.encoding == QDMI_PROGRAM_ENCODING_TEXT &&
                              program.size() > 1
                          ? 2
                          : 1));
      output_index = static_cast<unsigned char>(marker) % shot_outputs.size();
    }
    result.shots.assign(job->num_shots,
                        std::string{shot_outputs.at(output_index)});
    // Generate random complex numbers and calculate the norm
    result.state_vec.reserve(1U << num_qubits);
    double norm = 0.0;
    for (size_t i = 0; i < 1U << num_qubits; ++i) {
      const auto &c = result.state_vec.emplace_back(CXX_QDMI_generate_real(),
                                                    CXX_QDMI_generate_real());
      norm += std::norm(c);
    }
    // Normalize the vector
    norm = std::sqrt(norm);
    for (auto &c : result.state_vec) {
      c /= norm;
    }
  }
  return QDMI_SUCCESS;
} /// [DOXYGEN FUNCTION END]

int CXX_QDMI_device_job_cancel(CXX_QDMI_Device_Job job) {
  if (job == nullptr || job->status == QDMI_JOB_STATUS_DONE) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }

  job->status = QDMI_JOB_STATUS_CANCELED;
  CXX_QDMI_set_device_status(QDMI_DEVICE_STATUS_IDLE);
  return QDMI_SUCCESS;
} /// [DOXYGEN FUNCTION END]

int CXX_QDMI_device_job_check(CXX_QDMI_Device_Job job,
                              QDMI_Job_Status *status) {
  if (job == nullptr || status == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  // randomly decide whether job is done or not
  if (job->status == QDMI_JOB_STATUS_RUNNING && CXX_QDMI_generate_bit()) {
    CXX_QDMI_set_device_status(QDMI_DEVICE_STATUS_IDLE);
    job->status = QDMI_JOB_STATUS_DONE;
  }
  *status = job->status;
  return QDMI_SUCCESS;
} /// [DOXYGEN FUNCTION END]

int CXX_QDMI_device_job_wait(CXX_QDMI_Device_Job job,
                             [[maybe_unused]] const size_t timeout) {
  if (job == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  job->status = QDMI_JOB_STATUS_DONE;
  CXX_QDMI_set_device_status(QDMI_DEVICE_STATUS_IDLE);
  return QDMI_SUCCESS;
} /// [DOXYGEN FUNCTION END]

namespace {
int CXX_QDMI_device_job_get_results_shots(
    const CXX_QDMI_Device_Job_impl_d::Result_data &result, const size_t size,
    void *data, size_t *size_ret) {
  if (result.shots.empty()) {
    if (size_ret != nullptr) {
      *size_ret = 0;
    }
    return QDMI_SUCCESS;
  }
  const size_t req_size =
      result.shots.size() * (result.shots.front().length() + 1);
  if (data != nullptr) {
    if (size < req_size) {
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    auto *data_ptr = static_cast<char *>(data);
    for (auto it = result.shots.begin(); it != result.shots.end(); ++it) {
      data_ptr = std::ranges::copy(*it, data_ptr).out;
      if (std::next(it) != result.shots.end()) {
        *data_ptr++ = ','; // Add comma separator
      } else {
        *data_ptr++ = '\0'; // Add null terminator at the end
      }
    }
  }
  if (size_ret != nullptr) {
    *size_ret = req_size;
  }
  return QDMI_SUCCESS;
} /// [DOXYGEN FUNCTION END]

int CXX_QDMI_device_job_get_results_hist(
    const CXX_QDMI_Device_Job_impl_d::Result_data &job_result,
    const QDMI_Job_Result result, const size_t size, void *data,
    size_t *size_ret) {
  // Count unique elements
  std::map<std::string, size_t> hist;
  for (const auto &shot : job_result.shots) {
    hist[shot]++;
  }
  if (result == QDMI_JOB_RESULT_HIST_KEYS) {
    const size_t bitstring_size =
        job_result.shots.empty() ? 0 : job_result.shots.front().length();
    const size_t req_size = hist.size() * (bitstring_size + 1);
    if (size_ret != nullptr) {
      *size_ret = req_size;
    }
    if (data != nullptr && !hist.empty()) {
      if (size < req_size) {
        return QDMI_ERROR_INVALIDARGUMENT;
      }
      char *data_ptr = static_cast<char *>(data);
      for (const auto &bitstring : hist | std::views::keys) {
        std::ranges::copy(bitstring, data_ptr);
        data_ptr += bitstring.length();
        *data_ptr++ = ',';
      }
      *(data_ptr - 1) = '\0'; // Replace last comma with null terminator
    }
  } else {
    // case QDMI_JOB_RESULT_HIST_VALUES:
    const size_t req_size = hist.size() * sizeof(size_t);
    if (size_ret != nullptr) {
      *size_ret = req_size;
    }
    if (data != nullptr) {
      if (size < req_size) {
        return QDMI_ERROR_INVALIDARGUMENT;
      }
      auto *data_ptr = static_cast<size_t *>(data);
      for (const auto &count : hist | std::views::values) {
        *data_ptr++ = count;
      }
    }
  }
  return QDMI_SUCCESS;
} /// [DOXYGEN FUNCTION END]

int CXX_QDMI_device_job_get_results_statevector(
    const CXX_QDMI_Device_Job_impl_d::Result_data &result, const size_t size,
    void *data, size_t *size_ret) {
  const size_t req_size = result.state_vec.size() * 2 * sizeof(double);
  if (data != nullptr) {
    if (size < req_size) {
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    std::memcpy(data, result.state_vec.data(), req_size);
  }
  if (size_ret != nullptr) {
    *size_ret = req_size;
  }
  return QDMI_SUCCESS;
} /// [DOXYGEN FUNCTION END]

int CXX_QDMI_device_job_get_results_sparse(
    const CXX_QDMI_Device_Job_impl_d::Result_data &job_result,
    const QDMI_Job_Result result, const size_t size, void *data,
    size_t *size_ret) {
  // count non-zero elements
  size_t count = 0;
  for (const auto &c : job_result.state_vec) {
    if (c != 0.) {
      ++count;
    }
  }
  const auto num_qubits =
      static_cast<size_t>(std::log2(job_result.state_vec.size()));

  switch (result) {
  case QDMI_JOB_RESULT_STATEVECTOR_SPARSE_KEYS:
  case QDMI_JOB_RESULT_PROBABILITIES_SPARSE_KEYS: {
    const size_t req_size = count * (num_qubits + 1);
    if (data != nullptr) {
      if (size < req_size) {
        return QDMI_ERROR_INVALIDARGUMENT;
      }
      auto *data_ptr = static_cast<char *>(data);
      for (size_t i = 0; i < job_result.state_vec.size(); ++i) {
        if (job_result.state_vec[i] != 0.0) {
          for (size_t j = 0; j < num_qubits; ++j) {
            *data_ptr++ = ((i & (1 << (num_qubits - j - 1))) != 0U) ? '1' : '0';
          }
          *data_ptr++ = ',';
        }
      }
      *(data_ptr - 1) = '\0'; // Replace last comma with null terminator
    }
    if (size_ret != nullptr) {
      *size_ret = req_size;
    }
    return QDMI_SUCCESS;
  }

  case QDMI_JOB_RESULT_STATEVECTOR_SPARSE_VALUES: {
    const size_t req_size = count * 2 * sizeof(double);
    if (data != nullptr) {
      if (size < req_size) {
        return QDMI_ERROR_INVALIDARGUMENT;
      }
      auto *data_ptr = static_cast<double *>(data);
      for (auto c : job_result.state_vec) {
        if (c != 0.) {
          *data_ptr++ = c.real();
          *data_ptr++ = c.imag();
        }
      }
    }
    if (size_ret != nullptr) {
      *size_ret = req_size;
    }
    return QDMI_SUCCESS;
  }
  default: {
    // case QDMI_JOB_RESULT_PROBABILITIES_SPARSE_VALUES:
    const size_t req_size = count * sizeof(double);
    if (data != nullptr) {
      if (size < req_size) {
        return QDMI_ERROR_INVALIDARGUMENT;
      }
      auto *data_ptr = static_cast<double *>(data);
      for (const auto &c : job_result.state_vec) {
        *data_ptr++ = std::norm(c);
      }
    }
    if (size_ret != nullptr) {
      *size_ret = req_size;
    }
  }
  }
  return QDMI_SUCCESS;
} /// [DOXYGEN FUNCTION END]

int CXX_QDMI_device_job_get_results_probabilities(
    const CXX_QDMI_Device_Job_impl_d::Result_data &result, const size_t size,
    void *data, size_t *size_ret) {
  const size_t req_size = result.state_vec.size() * sizeof(double);
  if (data != nullptr) {
    if (size < req_size) {
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    auto *data_ptr = static_cast<double *>(data);
    for (const auto &c : result.state_vec) {
      *data_ptr++ = std::norm(c);
    }
  }
  if (size_ret != nullptr) {
    *size_ret = req_size;
  }
  return QDMI_SUCCESS;
} /// [DOXYGEN FUNCTION END]
} // namespace

int CXX_QDMI_device_job_get_results_for_program(CXX_QDMI_Device_Job job,
                                                const size_t program_index,
                                                const QDMI_Job_Result result,
                                                const size_t size, void *data,
                                                size_t *size_ret) {
  if (job == nullptr || job->status != QDMI_JOB_STATUS_DONE ||
      (data != nullptr && size == 0) ||
      (result >= QDMI_JOB_RESULT_MAX && result != QDMI_JOB_RESULT_CUSTOM1 &&
       result != QDMI_JOB_RESULT_CUSTOM2 && result != QDMI_JOB_RESULT_CUSTOM3 &&
       result != QDMI_JOB_RESULT_CUSTOM4 &&
       result != QDMI_JOB_RESULT_CUSTOM5)) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (program_index >= job->results.size()) {
    return QDMI_ERROR_OUTOFRANGE;
  }
  const auto &program_result = job->results[program_index];
  switch (result) {
  case QDMI_JOB_RESULT_SHOTS:
    return CXX_QDMI_device_job_get_results_shots(program_result, size, data,
                                                 size_ret);
  case QDMI_JOB_RESULT_HIST_KEYS:
  case QDMI_JOB_RESULT_HIST_VALUES:
    return CXX_QDMI_device_job_get_results_hist(program_result, result, size,
                                                data, size_ret);
  case QDMI_JOB_RESULT_STATEVECTOR_DENSE:
    return CXX_QDMI_device_job_get_results_statevector(program_result, size,
                                                       data, size_ret);
  case QDMI_JOB_RESULT_STATEVECTOR_SPARSE_KEYS:
  case QDMI_JOB_RESULT_STATEVECTOR_SPARSE_VALUES:
  case QDMI_JOB_RESULT_PROBABILITIES_SPARSE_KEYS:
  case QDMI_JOB_RESULT_PROBABILITIES_SPARSE_VALUES:
    return CXX_QDMI_device_job_get_results_sparse(program_result, result, size,
                                                  data, size_ret);
  case QDMI_JOB_RESULT_PROBABILITIES_DENSE:
    return CXX_QDMI_device_job_get_results_probabilities(program_result, size,
                                                         data, size_ret);
  case QDMI_JOB_RESULT_PROGRAMOUTPUT:
    return CXX_QDMI_device_job_get_program_output(job, size, data, size_ret);
  default:
    return QDMI_ERROR_NOTSUPPORTED;
  }
} /// [DOXYGEN FUNCTION END]

int CXX_QDMI_device_job_get_results(CXX_QDMI_Device_Job job,
                                    const QDMI_Job_Result result,
                                    const size_t size, void *data,
                                    size_t *size_ret) {
  if (job == nullptr || job->status != QDMI_JOB_STATUS_DONE ||
      (data != nullptr && size == 0) ||
      (result >= QDMI_JOB_RESULT_MAX && result != QDMI_JOB_RESULT_CUSTOM1 &&
       result != QDMI_JOB_RESULT_CUSTOM2 && result != QDMI_JOB_RESULT_CUSTOM3 &&
       result != QDMI_JOB_RESULT_CUSTOM4 &&
       result != QDMI_JOB_RESULT_CUSTOM5)) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (job->results.size() != 1) {
    return QDMI_ERROR_NOTSUPPORTED;
  }
  return CXX_QDMI_device_job_get_results_for_program(job, 0, result, size, data,
                                                     size_ret);
} /// [DOXYGEN FUNCTION END]

int CXX_QDMI_device_session_query_device_property(
    CXX_QDMI_Device_Session session, const QDMI_Device_Property prop,
    const size_t size, void *value, size_t *size_ret) {
  if (session == nullptr || (value != nullptr && size == 0) ||
      (prop >= QDMI_DEVICE_PROPERTY_MAX &&
       prop != QDMI_DEVICE_PROPERTY_CUSTOM1 &&
       prop != QDMI_DEVICE_PROPERTY_CUSTOM2 &&
       prop != QDMI_DEVICE_PROPERTY_CUSTOM3 &&
       prop != QDMI_DEVICE_PROPERTY_CUSTOM4 &&
       prop != QDMI_DEVICE_PROPERTY_CUSTOM5)) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (session->status != CXX_QDMI_DEVICE_SESSION_STATUS::INITIALIZED) {
    return QDMI_ERROR_BADSTATE;
  }
  ADD_STRING_PROPERTY(QDMI_DEVICE_PROPERTY_NAME, "C++ Device with 5 qubits",
                      prop, size, value, size_ret)
  // NOLINTNEXTLINE(misc-include-cleaner)
  ADD_STRING_PROPERTY(QDMI_DEVICE_PROPERTY_VERSION, CXX_QDMI_DEVICE_VERSION,
                      prop, size, value, size_ret)
  // NOLINTNEXTLINE(misc-include-cleaner)
  ADD_STRING_PROPERTY(QDMI_DEVICE_PROPERTY_LIBRARYVERSION, QDMI_VERSION, prop,
                      size, value, size_ret)
  ADD_SINGLE_VALUE_PROPERTY(QDMI_DEVICE_PROPERTY_STATUS, QDMI_Device_Status,
                            CXX_QDMI_get_device_status(), prop, size, value,
                            size_ret)
  ADD_SINGLE_VALUE_PROPERTY(QDMI_DEVICE_PROPERTY_QUBITSNUM, size_t, 5, prop,
                            size, value, size_ret)
  ADD_LIST_PROPERTY(QDMI_DEVICE_PROPERTY_SITES, CXX_QDMI_Site, CXX_DEVICE_SITES,
                    prop, size, value, size_ret)
  ADD_LIST_PROPERTY(QDMI_DEVICE_PROPERTY_OPERATIONS, CXX_QDMI_Operation,
                    CXX_DEVICE_OPERATIONS, prop, size, value, size_ret)
  ADD_LIST_PROPERTY(QDMI_DEVICE_PROPERTY_COUPLINGMAP, CXX_QDMI_Site,
                    DEVICE_COUPLING_MAP, prop, size, value, size_ret)

  // The example device never requires calibration
  ADD_SINGLE_VALUE_PROPERTY(QDMI_DEVICE_PROPERTY_NEEDSCALIBRATION, size_t, 0,
                            prop, size, value, size_ret)

  ADD_SINGLE_VALUE_PROPERTY(
      QDMI_DEVICE_PROPERTY_PULSESUPPORT, QDMI_Device_Pulse_Support_Level,
      QDMI_DEVICE_PULSE_SUPPORT_LEVEL_NONE, prop, size, value, size_ret)

  ADD_STRING_PROPERTY(QDMI_DEVICE_PROPERTY_LENGTHUNIT, "um", prop, size, value,
                      size_ret)
  ADD_SINGLE_VALUE_PROPERTY(QDMI_DEVICE_PROPERTY_LENGTHSCALEFACTOR, double, 1.0,
                            prop, size, value, size_ret)

  ADD_STRING_PROPERTY(QDMI_DEVICE_PROPERTY_DURATIONUNIT, "us", prop, size,
                      value, size_ret)
  ADD_SINGLE_VALUE_PROPERTY(QDMI_DEVICE_PROPERTY_DURATIONSCALEFACTOR, double,
                            0.001, prop, size, value, size_ret)

  ADD_LIST_PROPERTY(QDMI_DEVICE_PROPERTY_SUPPORTEDPROGRAMFORMATS,
                    QDMI_Program_Format, SUPPORTED_PROGRAM_FORMATS, prop, size,
                    value, size_ret)

  return QDMI_ERROR_NOTSUPPORTED;
} /// [DOXYGEN FUNCTION END]

int CXX_QDMI_device_session_query_site_property(CXX_QDMI_Device_Session session,
                                                CXX_QDMI_Site site,
                                                const QDMI_Site_Property prop,
                                                const size_t size, void *value,
                                                size_t *size_ret) {
  if (session == nullptr || site == nullptr ||
      (value != nullptr && size == 0) ||
      (prop >= QDMI_SITE_PROPERTY_MAX && prop != QDMI_SITE_PROPERTY_CUSTOM1 &&
       prop != QDMI_SITE_PROPERTY_CUSTOM2 &&
       prop != QDMI_SITE_PROPERTY_CUSTOM3 &&
       prop != QDMI_SITE_PROPERTY_CUSTOM4 &&
       prop != QDMI_SITE_PROPERTY_CUSTOM5)) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  ADD_SINGLE_VALUE_PROPERTY(QDMI_SITE_PROPERTY_INDEX, uint64_t, site->id, prop,
                            size, value, size_ret)
  ADD_SINGLE_VALUE_PROPERTY(QDMI_SITE_PROPERTY_MODULEINDEX, uint64_t, 0, prop,
                            size, value, size_ret)
  ADD_SINGLE_VALUE_PROPERTY(QDMI_SITE_PROPERTY_T1, uint64_t, 1000000U, prop,
                            size, value, size_ret)
  ADD_SINGLE_VALUE_PROPERTY(QDMI_SITE_PROPERTY_T2, uint64_t, 100000000U, prop,
                            size, value, size_ret)
  return QDMI_ERROR_NOTSUPPORTED;
} /// [DOXYGEN FUNCTION END]

int CXX_QDMI_device_session_query_operation_property(
    CXX_QDMI_Device_Session session, CXX_QDMI_Operation operation,
    const size_t num_sites, const CXX_QDMI_Site *sites, const size_t num_params,
    const double *params, QDMI_Operation_Property prop, const size_t size,
    void *value, size_t *size_ret) {
  if (session == nullptr || operation == nullptr ||
      (sites != nullptr && num_sites == 0) ||
      (params != nullptr && num_params == 0) ||
      (value != nullptr && size == 0) ||
      (prop >= QDMI_OPERATION_PROPERTY_MAX &&
       prop != QDMI_OPERATION_PROPERTY_CUSTOM1 &&
       prop != QDMI_OPERATION_PROPERTY_CUSTOM2 &&
       prop != QDMI_OPERATION_PROPERTY_CUSTOM3 &&
       prop != QDMI_OPERATION_PROPERTY_CUSTOM4 &&
       prop != QDMI_OPERATION_PROPERTY_CUSTOM5)) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  // General properties
  ADD_STRING_PROPERTY(QDMI_OPERATION_PROPERTY_NAME,
                      OPERATION_PROPERTIES.at(operation).first.c_str(), prop,
                      size, value, size_ret)
  ADD_SINGLE_VALUE_PROPERTY(QDMI_OPERATION_PROPERTY_ISZONED, bool, false, prop,
                            size, value, size_ret)
  if (operation == CXX_DEVICE_OPERATIONS[3]) {
    if (sites != nullptr && num_sites != 2) {
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    ADD_SINGLE_VALUE_PROPERTY(QDMI_OPERATION_PROPERTY_PARAMETERSNUM, size_t, 0,
                              prop, size, value, size_ret)
    ADD_SINGLE_VALUE_PROPERTY(QDMI_OPERATION_PROPERTY_DURATION, uint64_t,
                              OPERATION_PROPERTIES.at(operation).second, prop,
                              size, value, size_ret)
    ADD_LIST_PROPERTY(QDMI_OPERATION_PROPERTY_SITES, CXX_QDMI_Site,
                      DEVICE_COUPLING_MAP, prop, size, value, size_ret)
    if (sites == nullptr) {
      ADD_SINGLE_VALUE_PROPERTY(QDMI_OPERATION_PROPERTY_QUBITSNUM, size_t, 2,
                                prop, size, value, size_ret)
      return QDMI_ERROR_NOTSUPPORTED;
    }

    const std::pair site_pair = {sites[0], sites[1]};
    if (site_pair.first == site_pair.second) {
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    const auto it = OPERATION_FIDELITIES.find(operation);
    if (it == OPERATION_FIDELITIES.end()) {
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    const auto fit = it->second.find(site_pair);
    if (fit == it->second.end()) {
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    ADD_SINGLE_VALUE_PROPERTY(QDMI_OPERATION_PROPERTY_FIDELITY, double,
                              fit->second, prop, size, value, size_ret)
  } else if (operation == CXX_DEVICE_OPERATIONS[0] ||
             operation == CXX_DEVICE_OPERATIONS[1] ||
             operation == CXX_DEVICE_OPERATIONS[2]) {
    if ((sites != nullptr && num_sites != 1) ||
        (params != nullptr && num_params != 1)) {
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    ADD_SINGLE_VALUE_PROPERTY(QDMI_OPERATION_PROPERTY_DURATION, double, 0.01,
                              prop, size, value, size_ret)
    ADD_SINGLE_VALUE_PROPERTY(QDMI_OPERATION_PROPERTY_QUBITSNUM, size_t, 1,
                              prop, size, value, size_ret)
    ADD_SINGLE_VALUE_PROPERTY(QDMI_OPERATION_PROPERTY_PARAMETERSNUM, size_t, 1,
                              prop, size, value, size_ret)
    ADD_SINGLE_VALUE_PROPERTY(QDMI_OPERATION_PROPERTY_FIDELITY, double, 0.999,
                              prop, size, value, size_ret)
    ADD_LIST_PROPERTY(QDMI_OPERATION_PROPERTY_SITES, CXX_QDMI_Site,
                      CXX_DEVICE_SITES, prop, size, value, size_ret)
  }
  return QDMI_ERROR_NOTSUPPORTED;
} /// [DOXYGEN FUNCTION END]

// NOLINTEND(*-pro-bounds-avoid-unchecked-container-access,*-throwing-static-initialization)
