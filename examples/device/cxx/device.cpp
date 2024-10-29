/*------------------------------------------------------------------------------
Part of the MQSS Project, under the Apache License v2.0 with LLVM Exceptions.
See https://llvm.org/LICENSE.txt for license information.
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
------------------------------------------------------------------------------*/

/** @file
 * @brief A simple example of a device implementation in C++.
 * @details This file can be used as a template for implementing a device in
 * C++. For more implemented functions, see also the \ref device5.c file.
 */

#include "qdmi/device.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <functional>
#include <iterator>
#include <limits>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

struct QDMI_Job_impl_d {
  int id = 0;
  QDMI_Job_Status status = QDMI_JOB_STATUS_SUBMITTED;
  int num_shots = 0;
  std::vector<std::string> results;
  std::bernoulli_distribution binary_dist{0.5};
};

struct QDMI_Site_impl_d {
  int id;
};

struct QDMI_Operation_impl_d {
  const char *name;
};

struct QDMI_Device_State {
  QDMI_Device_Status status = QDMI_DEVICE_STATUS_OFFLINE;
  std::random_device rd;
  std::mt19937 gen{rd()};
  std::uniform_int_distribution<> dis =
      std::uniform_int_distribution<>(0, std::numeric_limits<int>::max());
};

namespace {
// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
QDMI_Device_State device_state;

std::array<QDMI_Operation_impl_d, 4> device_operations = {
    QDMI_Operation_impl_d{"rx"}, QDMI_Operation_impl_d{"ry"},
    QDMI_Operation_impl_d{"rz"}, QDMI_Operation_impl_d{"cx"}};

std::array<QDMI_Site_impl_d, 7> device_sites = {
    QDMI_Site_impl_d{0}, QDMI_Site_impl_d{1}, QDMI_Site_impl_d{2},
    QDMI_Site_impl_d{3}, QDMI_Site_impl_d{4}};
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)
} // namespace

constexpr static std::array<const QDMI_Site_impl_d *const, 20>
    // clang-format off
    DEVICE_COUPLING_MAP = {
      device_sites.data(), &device_sites[1],
      &device_sites[1], device_sites.data(),
      &device_sites[1], &device_sites[2],
      &device_sites[2], &device_sites[1],
      &device_sites[2], &device_sites[3],
      &device_sites[3], &device_sites[2],
      &device_sites[3], &device_sites[4],
      &device_sites[4], &device_sites[3],
      &device_sites[4], device_sites.data(),
      device_sites.data(), &device_sites[4]};
// clang-format on

const static std::unordered_map<const QDMI_Operation_impl_d *, double>
    OPERATION_DURATIONS = {
        {device_operations.data(), 0.01},
        {&device_operations[1], 0.01},
        {&device_operations[2], 0.01},
        {&device_operations[3], 0.1},
};

struct Pair_hash {
  template <class T1, class T2>
  std::size_t operator()(const std::pair<T1, T2> &p) const {
    auto hash1 = std::hash<T1>{}(p.first);
    auto hash2 = std::hash<T2>{}(p.second);
    return hash1 ^ hash2;
  }
}; /// [DOXYGEN FUNCTION END]

const static std::unordered_map<
    const QDMI_Operation_impl_d *,
    std::unordered_map<
        std::pair<const QDMI_Site_impl_d *, const QDMI_Site_impl_d *>, double,
        Pair_hash>>
    OPERATION_FIDELITIES = {
        {&device_operations[3],
         {{{device_sites.data(), &device_sites[1]}, 0.99},
          {{&device_sites[1], device_sites.data()}, 0.99},
          {{&device_sites[1], &device_sites[2]}, 0.98},
          {{&device_sites[2], &device_sites[1]}, 0.98},
          {{&device_sites[2], &device_sites[3]}, 0.97},
          {{&device_sites[3], &device_sites[2]}, 0.97},
          {{&device_sites[3], &device_sites[4]}, 0.96},
          {{&device_sites[4], &device_sites[3]}, 0.96},
          {{&device_sites[4], device_sites.data()}, 0.95},
          {{device_sites.data(), &device_sites[4]}, 0.95}}},
        // No need to specify single-qubit fidelities here
};

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
        *(size_ret) = sizeof(prop_type);                                       \
      }                                                                        \
      return QDMI_SUCCESS;                                                     \
    }                                                                          \
  }

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
        *(size_ret) = static_cast<int>(strlen(prop_value)) + 1;                \
      }                                                                        \
      return QDMI_SUCCESS;                                                     \
    }                                                                          \
  }

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
        *(size_ret) = (int)((prop_values).size() * sizeof(prop_type));         \
      }                                                                        \
      return QDMI_SUCCESS;                                                     \
    }                                                                          \
  }
// NOLINTEND(bugprone-macro-parentheses)

int QDMI_query_get_sites_dev(const int num_entries, QDMI_Site *sites,
                             int *num_sites) {
  if ((sites != nullptr && num_entries <= 0) ||
      (sites == nullptr && num_sites == nullptr)) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (sites != nullptr) {
    for (int i = 0;
         i < std::min(num_entries, static_cast<int>(device_sites.size()));
         ++i) {
      sites[i] = &device_sites[i];
    }
  }
  if (num_sites != nullptr) {
    *num_sites = static_cast<int>(device_sites.size());
  }
  return QDMI_SUCCESS;
}

int QDMI_query_get_operations_dev(const int num_entries,
                                  QDMI_Operation *operations,
                                  int *num_operations) {
  if ((operations != nullptr && num_entries <= 0) ||
      (operations == nullptr && num_operations == nullptr)) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (operations != nullptr) {
    for (int i = 0;
         i < std::min(num_entries, static_cast<int>(device_operations.size()));
         ++i) {
      operations[i] = &device_operations[i];
    }
  }
  if (num_operations != nullptr) {
    *num_operations = static_cast<int>(device_operations.size());
  }
  return QDMI_SUCCESS;
}

int QDMI_query_device_property_dev(const QDMI_Device_Property prop,
                                   const int size, void *value, int *size_ret) {
  if (prop >= QDMI_DEVICE_PROPERTY_MAX ||
      (value == nullptr && size_ret == nullptr)) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  ADD_STRING_PROPERTY(QDMI_DEVICE_PROPERTY_NAME, "Device with 5 qubits", prop,
                      size, value, size_ret)
  ADD_STRING_PROPERTY(QDMI_DEVICE_PROPERTY_VERSION, "0.1.0", prop, size, value,
                      size_ret)
  ADD_STRING_PROPERTY(QDMI_DEVICE_PROPERTY_LIBRARYVERSION, "1.0.0b1", prop,
                      size, value, size_ret)
  ADD_SINGLE_VALUE_PROPERTY(QDMI_DEVICE_PROPERTY_QUBITSNUM, int, 5, prop, size,
                            value, size_ret)
  ADD_SINGLE_VALUE_PROPERTY(QDMI_DEVICE_PROPERTY_STATUS, QDMI_Device_Status,
                            device_state.status, prop, size, value, size_ret)
  ADD_LIST_PROPERTY(QDMI_DEVICE_PROPERTY_COUPLINGMAP, QDMI_Site,
                    DEVICE_COUPLING_MAP, prop, size, value, size_ret)
  return QDMI_ERROR_NOTSUPPORTED;
} /// [DOXYGEN FUNCTION END]

int QDMI_query_site_property_dev([[maybe_unused]] QDMI_Site site,
                                 QDMI_Site_Property prop, int size, void *value,
                                 int *size_ret) {
  if (prop >= QDMI_SITE_PROPERTY_MAX ||
      (value == nullptr && size_ret == nullptr)) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  ADD_SINGLE_VALUE_PROPERTY(QDMI_SITE_PROPERTY_TIME_T1, double, 1000.0, prop,
                            size, value, size_ret)
  ADD_SINGLE_VALUE_PROPERTY(QDMI_SITE_PROPERTY_TIME_T2, double, 100000.0, prop,
                            size, value, size_ret)
  return QDMI_ERROR_NOTSUPPORTED;
} /// [DOXYGEN FUNCTION END]

int QDMI_query_operation_property_dev(QDMI_Operation operation, int num_sites,
                                      const QDMI_Site *sites,
                                      QDMI_Operation_Property prop, int size,
                                      void *value, int *size_ret) {
  if (prop >= QDMI_OPERATION_PROPERTY_MAX || operation == nullptr ||
      (sites != nullptr && num_sites <= 0) ||
      (value == nullptr && size_ret == nullptr)) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  // General properties
  ADD_STRING_PROPERTY(QDMI_OPERATION_PROPERTY_NAME, operation->name, prop, size,
                      value, size_ret)
  if (strcmp(operation->name, "cx") == 0) {
    if (sites != nullptr && num_sites != 2) {
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    ADD_SINGLE_VALUE_PROPERTY(QDMI_OPERATION_PROPERTY_DURATION, double,
                              OPERATION_DURATIONS.at(operation), prop, size,
                              value, size_ret)
    if (sites == nullptr) {
      ADD_SINGLE_VALUE_PROPERTY(QDMI_OPERATION_PROPERTY_QUBITSNUM, int, 2, prop,
                                size, value, size_ret)
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
  } else if (strcmp(operation->name, "rx") == 0 ||
             strcmp(operation->name, "ry") == 0 ||
             strcmp(operation->name, "rz") == 0) {
    if (sites != nullptr && num_sites != 1) {
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    ADD_SINGLE_VALUE_PROPERTY(QDMI_OPERATION_PROPERTY_DURATION, double, 0.01,
                              prop, size, value, size_ret)
    ADD_SINGLE_VALUE_PROPERTY(QDMI_OPERATION_PROPERTY_QUBITSNUM, int, 1, prop,
                              size, value, size_ret)
    ADD_SINGLE_VALUE_PROPERTY(QDMI_OPERATION_PROPERTY_FIDELITY, double, 0.999,
                              prop, size, value, size_ret)
  }
  return QDMI_ERROR_NOTSUPPORTED;
} /// [DOXYGEN FUNCTION END]

int QDMI_control_create_job_dev(const QDMI_Program_Format format,
                                const int size, const void *prog,
                                QDMI_Job *job) {
  if (device_state.status != QDMI_DEVICE_STATUS_IDLE) {
    return QDMI_ERROR_FATAL;
  }
  if (size <= 0 || prog == nullptr || job == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (format != QDMI_PROGRAM_FORMAT_QASM2 &&
      format != QDMI_PROGRAM_FORMAT_QIRSTRING &&
      format != QDMI_PROGRAM_FORMAT_QIRMODULE) {
    return QDMI_ERROR_NOTSUPPORTED;
  }

  device_state.status = QDMI_DEVICE_STATUS_BUSY;
  *job = new QDMI_Job_impl_d;
  // set job id to current time for demonstration purposes
  (*job)->id = device_state.dis(device_state.gen);
  (*job)->status = QDMI_JOB_STATUS_CREATED;
  return QDMI_SUCCESS;
} /// [DOXYGEN FUNCTION END]

int QDMI_control_set_parameter_dev(QDMI_Job job, const QDMI_Job_Parameter param,
                                   const int size, const void *value) {
  if (job == nullptr || param >= QDMI_JOB_PARAMETER_MAX || size <= 0 ||
      job->status != QDMI_JOB_STATUS_CREATED) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (param == QDMI_JOB_PARAMETER_SHOTS_NUM) {
    job->num_shots = *static_cast<const int *>(value);
    return QDMI_SUCCESS;
  }
  return QDMI_ERROR_NOTSUPPORTED;
}

int QDMI_control_submit_job_dev(QDMI_Job job) {
  if (job == nullptr || job->status != QDMI_JOB_STATUS_CREATED) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  device_state.status = QDMI_DEVICE_STATUS_BUSY;
  job->status = QDMI_JOB_STATUS_SUBMITTED;
  // here, the actual submission of the problem to the device would happen
  // ...
  // set job status to running for demonstration purposes
  job->status = QDMI_JOB_STATUS_RUNNING;
  // generate random result data
  int num_qubits = 0;
  QDMI_query_device_property_dev(QDMI_DEVICE_PROPERTY_QUBITSNUM, sizeof(int),
                                 &num_qubits, nullptr);
  job->results.clear();
  job->results.reserve(job->num_shots);
  for (int i = 0; i < job->num_shots; i++) {
    // generate random 5-bit string
    std::string result(num_qubits, '0');
    std::generate(result.begin(), result.end(), [&]() {
      return device_state.dis(device_state.gen) % 2 ? '1' : '0';
    });
    job->results.emplace_back(std::move(result));
  }
  return QDMI_SUCCESS;
} /// [DOXYGEN FUNCTION END]

int QDMI_control_cancel_dev(QDMI_Job job) {
  if (job->status == QDMI_JOB_STATUS_DONE) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }

  job->status = QDMI_JOB_STATUS_CANCELLED;
  device_state.status = QDMI_DEVICE_STATUS_IDLE;
  return QDMI_SUCCESS;
} /// [DOXYGEN FUNCTION END]

int QDMI_control_check_dev(QDMI_Job job, QDMI_Job_Status *status) {
  // randomly decide whether job is done or not
  if (job->status == QDMI_JOB_STATUS_RUNNING &&
      job->binary_dist(device_state.gen)) {
    device_state.status = QDMI_DEVICE_STATUS_IDLE;
    job->status = QDMI_JOB_STATUS_DONE;
  }
  *status = job->status;
  return QDMI_SUCCESS;
} /// [DOXYGEN FUNCTION END]

int QDMI_control_wait_dev(QDMI_Job job) {
  job->status = QDMI_JOB_STATUS_DONE;
  device_state.status = QDMI_DEVICE_STATUS_IDLE;
  return QDMI_SUCCESS;
} /// [DOXYGEN FUNCTION END]

int QDMI_control_get_data_dev(QDMI_Job job, const QDMI_Job_Result result,
                              const int size, void *data, int *size_ret) {
  if (job->status != QDMI_JOB_STATUS_DONE) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (result == QDMI_JOB_RESULT_SHOTS) {
    // generate random measurement results
    int num_qubits = 0;
    QDMI_query_device_property_dev(QDMI_DEVICE_PROPERTY_QUBITSNUM, sizeof(int),
                                   &num_qubits, nullptr);
    if (data != nullptr) {
      if (size < job->num_shots * (num_qubits + 1)) {
        return QDMI_ERROR_INVALIDARGUMENT;
      }
      auto *data_ptr = static_cast<char *>(data);
      for (auto it = job->results.begin(); it != job->results.end(); ++it) {
        data_ptr = std::copy(it->begin(), it->end(), data_ptr);
        if (std::next(it) != job->results.end()) {
          *data_ptr++ = ','; // Add comma separator
        } else {
          *data_ptr++ = '\0'; // Add null terminator at the end
        }
      }
    }
    if ((size_ret) != nullptr) {
      *(size_ret) = job->num_shots * (num_qubits + 1);
    }
    return QDMI_SUCCESS;
  }
  if (result == QDMI_JOB_RESULT_HIST_KEYS) {
    int raw_size = 0;
    QDMI_control_get_data_dev(job, QDMI_JOB_RESULT_SHOTS, 0, nullptr,
                              &raw_size);
    std::string raw_data(raw_size, '\0');
    QDMI_control_get_data_dev(job, QDMI_JOB_RESULT_SHOTS, raw_size,
                              raw_data.data(), nullptr);
    // Count unique elements
    std::unordered_map<std::string, int> hist;
    std::stringstream ss(raw_data);
    std::string token;
    while (std::getline(ss, token, ',')) {
      hist[token]++;
    }
    int num_qubits = 0;
    QDMI_query_device_property_dev(QDMI_DEVICE_PROPERTY_QUBITSNUM, sizeof(int),
                                   &num_qubits, nullptr);
    if (data != nullptr) {
      if (size < hist.size() * (num_qubits + 1)) {
        return QDMI_ERROR_INVALIDARGUMENT;
      }
      char *data_ptr = static_cast<char *>(data);
      for (auto it = hist.begin(); it != hist.end(); ++it) {
        const auto &k = it->first;
        std::copy(k.begin(), k.end(), data_ptr);
        data_ptr += k.length();
        if (std::next(it) != hist.end()) {
          *data_ptr++ = ','; // Add comma separator
        } else {
          *data_ptr++ = '\0'; // Add null terminator at the end
        }
      }
    }
    if ((size_ret) != nullptr) {
      *(size_ret) = static_cast<int>(hist.size()) * (num_qubits + 1);
    }
    return QDMI_SUCCESS;
  }
  if (result == QDMI_JOB_RESULT_HIST_VALUES) {
    int raw_size = 0;
    QDMI_control_get_data_dev(job, QDMI_JOB_RESULT_SHOTS, 0, nullptr,
                              &raw_size);
    std::string raw_data(raw_size, '\0');
    QDMI_control_get_data_dev(job, QDMI_JOB_RESULT_SHOTS, raw_size,
                              raw_data.data(), nullptr);
    // Count unique elements
    std::unordered_map<std::string, int> hist;
    std::stringstream ss(raw_data);
    std::string token;
    while (std::getline(ss, token, ',')) {
      hist[token]++;
    }
    int num_qubits = 0;
    QDMI_query_device_property_dev(QDMI_DEVICE_PROPERTY_QUBITSNUM, sizeof(int),
                                   &num_qubits, nullptr);
    if (data != nullptr) {
      if (size < hist.size() * sizeof(int)) {
        return QDMI_ERROR_INVALIDARGUMENT;
      }
      int *int_data = static_cast<int *>(data);
      for (const auto &[k, v] : hist) {
        *int_data = v;
        int_data++;
      }
    }
    if ((size_ret) != nullptr) {
      *(size_ret) = static_cast<int>(hist.size() * sizeof(int));
    }
    return QDMI_SUCCESS;
  }
  return QDMI_ERROR_NOTSUPPORTED;
} /// [DOXYGEN FUNCTION END]

void QDMI_control_free_job_dev(QDMI_Job job) { delete job; }

int QDMI_control_initialize_dev() {
  device_state.status = QDMI_DEVICE_STATUS_IDLE;
  return QDMI_SUCCESS;
} /// [DOXYGEN FUNCTION END]

int QDMI_control_finalize_dev() {
  device_state.status = QDMI_DEVICE_STATUS_OFFLINE;
  return QDMI_SUCCESS;
} /// [DOXYGEN FUNCTION END]
