/*------------------------------------------------------------------------------
Copyright 2024 Munich Quantum Software Stack Project

Licensed under the Apache License, Version 2.0 with LLVM Exceptions (the
"License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at

https://github.com/Munich-Quantum-Software-Stack/QDMI/blob/develop/LICENSE

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
License for the specific language governing permissions and limitations under
the License.

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
------------------------------------------------------------------------------*/

/** @file
 * @brief A simple example of a driver implementation in C.
 * @details This file can be used as a template for implementing a driver in C.
 */

#include "qdmi_example_driver.h"

#include "qdmi/client.h"
#include "qdmi/device.h"

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <dlfcn.h>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

/**
 * @brief Enum of the modes a device can be opened in.
 * @note The values of this enum are meant to be used as bitflags. Hence, their
 * values must be powers of 2.
 */
enum QDMI_DEVICE_MODE : uint8_t {
  QDMI_DEVICE_MODE_READONLY = 0,   ///< To open the device in read-only mode.
  QDMI_SESSION_MODE_READWRITE = 1, ///< To open the device in read-write mode.
};

enum class QDMI_SESSION_STATUS : uint8_t { ALLOCATED, INITIALIZED };

/**
 * @brief Definition of the QDMI Library.
 */
struct QDMI_Driver_Library_impl_d {
  void *lib_handle = nullptr;

  /// Function pointer to @ref QDMI_device_initialize.
  decltype(QDMI_device_initialize) *device_initialize{};
  /// Function pointer to @ref QDMI_device_finalize.
  decltype(QDMI_device_finalize) *device_finalize{};
  /// Function pointer to @ref QDMI_device_session_alloc.
  decltype(QDMI_device_session_alloc) *device_session_alloc{};
  /// Function pointer to @ref QDMI_device_session_init.
  decltype(QDMI_device_session_init) *device_session_init{};
  /// Function pointer to @ref QDMI_device_session_free.
  decltype(QDMI_device_session_free) *device_session_free{};
  /// Function pointer to @ref QDMI_device_session_set_parameter.
  decltype(QDMI_device_session_set_parameter) *device_session_set_parameter{};
  /// Function pointer to @ref QDMI_device_job_create.
  decltype(QDMI_device_job_create) *device_job_create{};
  /// Function pointer to @ref QDMI_device_job_free.
  decltype(QDMI_device_job_free) *device_job_free{};
  /// Function pointer to @ref QDMI_device_job_set_parameter.
  decltype(QDMI_device_job_set_parameter) *device_job_set_parameter{};
  /// Function pointer to @ref QDMI_device_job_submit.
  decltype(QDMI_device_job_submit) *device_job_submit{};
  /// Function pointer to @ref QDMI_device_job_cancel.
  decltype(QDMI_device_job_cancel) *device_job_cancel{};
  /// Function pointer to @ref QDMI_device_job_check.
  decltype(QDMI_device_job_check) *device_job_check{};
  /// Function pointer to @ref QDMI_device_job_wait.
  decltype(QDMI_device_job_wait) *device_job_wait{};
  /// Function pointer to @ref QDMI_device_job_get_data.
  decltype(QDMI_device_job_get_data) *device_job_get_data{};
  /// Function pointer to @ref QDMI_device_session_query_property.
  decltype(QDMI_device_session_query_property) *device_session_query_property{};
  /// Function pointer to @ref QDMI_device_session_get_sites.
  decltype(QDMI_device_session_get_sites) *device_session_get_sites{};
  /// Function pointer to @ref QDMI_device_session_get_operations.
  decltype(QDMI_device_session_get_operations) *device_session_get_operations{};
  /// Function pointer to @ref QDMI_device_site_query_property.
  decltype(QDMI_device_site_query_property) *device_site_query_property{};
  /// Function pointer to @ref QDMI_device_operation_query_property.
  decltype(QDMI_device_operation_query_property)
      *device_operation_query_property{};

  // default constructor
  QDMI_Driver_Library_impl_d() = default;

  // delete copy constructor, copy assignment, move constructor, move assignment
  // to allow only one instance and proper destruction of the dynamic library
  QDMI_Driver_Library_impl_d(const QDMI_Device_impl_d &) = delete;

  QDMI_Driver_Library_impl_d &operator=(const QDMI_Device_impl_d &) = delete;

  QDMI_Driver_Library_impl_d(QDMI_Device_impl_d &&) = delete;

  QDMI_Driver_Library_impl_d &operator=(QDMI_Device_impl_d &&) = delete;

  // destructor
  ~QDMI_Driver_Library_impl_d() {
    // Check if QDMI_control_finalize is not NULL before calling it
    if (device_finalize != nullptr) {
      device_finalize();
    }
    // close the dynamic library
    if (lib_handle != nullptr) {
      dlclose(lib_handle);
    }
  }
};

using QDMI_Driver_Library = QDMI_Driver_Library_impl_d *;

/**
 * @brief Definition of the QDMI Device.
 */
struct QDMI_Device_impl_d {
  QDMI_Driver_Library library = nullptr;
  QDMI_Session session = nullptr;
  QDMI_Device_Session device_session = nullptr;
  std::unordered_map<QDMI_Device_Site, QDMI_Site_impl_d> sites;
  std::unordered_map<QDMI_Device_Operation, QDMI_Operation_impl_d> operations;
};

/**
 * @brief Definition of the QDMI Session.
 */
struct QDMI_Session_impl_d {
  QDMI_SESSION_STATUS status = QDMI_SESSION_STATUS::ALLOCATED;
  std::vector<std::unique_ptr<QDMI_Device_impl_d>> device_list;
  std::string token;
  QDMI_DEVICE_MODE mode = QDMI_DEVICE_MODE::QDMI_DEVICE_MODE_READONLY;
};

/**
 * @brief Definition of the QDMI Job.
 */
struct QDMI_Job_impl_d {
  QDMI_Device device = nullptr;
  QDMI_Device_Job device_job = nullptr;
};

/**
 * @brief Definition of the QDMI Site.
 */
struct QDMI_Site_impl_d {
  QDMI_Device device = nullptr;
  QDMI_Device_Site device_site = nullptr;
};

/**
 * @brief Definition of the QDMI Operation.
 */
struct QDMI_Operation_impl_d {
  QDMI_Device device = nullptr;
  QDMI_Device_Operation operation = nullptr;
};

namespace {
/**
 * @brief Global list of devices managed by the driver.
 */
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
std::unordered_map<void *, std::unique_ptr<QDMI_Driver_Library_impl_d>>
    libraries;
std::unordered_set<QDMI_Session> sessions;

#define LOAD_SYMBOL(device, prefix, symbol)                                    \
  {                                                                            \
    const std::string symbol_name = std::string(prefix) + "_QDMI_" + #symbol;  \
    (device).symbol = reinterpret_cast<decltype((device).symbol)>(             \
        dlsym((device).lib_handle, symbol_name.c_str()));                      \
    if ((device).symbol == nullptr) {                                          \
      throw std::runtime_error("Failed to load symbol: " + symbol_name);       \
    }                                                                          \
  }

void QDMI_library_load(const std::string &lib_name, const std::string &prefix) {
  auto *lib_handle = dlopen(lib_name.c_str(), RTLD_NOW | RTLD_LOCAL);
  if (lib_handle == nullptr) {
    throw std::runtime_error("Failed to open device library: " + lib_name);
  }
  if (const auto it = libraries.find(lib_handle); it != libraries.end()) {
    // dlopen employs reference counting so we need to decrement the reference
    // count that was increased by dlopen
    dlclose(lib_handle);
    return;
  }
  auto it =
      libraries
          .emplace(lib_handle, std::make_unique<QDMI_Driver_Library_impl_d>())
          .first;
  auto &library = *it->second;
  library.lib_handle = lib_handle;

  try {
    // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)

    // load the function symbols from the dynamic library
    LOAD_SYMBOL(library, prefix, device_initialize)
    LOAD_SYMBOL(library, prefix, device_finalize)
    LOAD_SYMBOL(library, prefix, device_session_alloc)
    LOAD_SYMBOL(library, prefix, device_session_init)
    LOAD_SYMBOL(library, prefix, device_session_free)
    LOAD_SYMBOL(library, prefix, device_session_set_parameter)
    LOAD_SYMBOL(library, prefix, device_job_create)
    LOAD_SYMBOL(library, prefix, device_job_free)
    LOAD_SYMBOL(library, prefix, device_job_set_parameter)
    LOAD_SYMBOL(library, prefix, device_job_submit)
    LOAD_SYMBOL(library, prefix, device_job_cancel)
    LOAD_SYMBOL(library, prefix, device_job_check)
    LOAD_SYMBOL(library, prefix, device_job_wait)
    LOAD_SYMBOL(library, prefix, device_job_get_data)
    LOAD_SYMBOL(library, prefix, device_session_query_property)
    LOAD_SYMBOL(library, prefix, device_session_get_sites)
    LOAD_SYMBOL(library, prefix, device_session_get_operations)
    LOAD_SYMBOL(library, prefix, device_site_query_property)
    LOAD_SYMBOL(library, prefix, device_operation_query_property)

    // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
  } catch (const std::exception &) {
    dlclose(library.lib_handle);
    throw;
  }
  // initialize the device
  library.device_initialize();
}

bool Is_path_allowed(const std::filesystem::path &path) {
  // Define the whitelist of allowed directories
  const std::vector<std::filesystem::path> whitelist = {
      std::filesystem::current_path(),
      std::filesystem::path(std::getenv("HOME"))};

  // Resolve the provided path to its absolute form
  std::filesystem::path resolved_path = std::filesystem::absolute(path);

  // Check if the resolved path starts with any of the whitelisted directories
  return std::any_of(
      whitelist.begin(), whitelist.end(), [&](const auto &allowed_path) {
        return resolved_path.string().rfind(allowed_path.string(), 0) == 0;
      });
}
} // namespace

int QDMI_driver_init() {
  const char *config_file = std::getenv("QDMI_CONF");
  if (config_file == nullptr) {
    config_file = "qdmi.conf";
  }

  // Validate the configuration file path
  if (!Is_path_allowed(config_file)) {
    std::cerr << "Config file path is not allowed: " << config_file << "\n";
    return QDMI_ERROR_FATAL;
  }

  std::ifstream file(config_file);
  if (!file.is_open()) {
    std::cerr << "Failed to open configuration file: " << config_file << "\n";
    return QDMI_ERROR_FATAL;
  }

  std::string line;
  while (std::getline(file, line)) {
    if (line.empty() || line[0] == '#') {
      continue; // Skip empty lines and comments
    }

    std::istringstream iss(line);
    std::string lib_name;
    std::string prefix;
    if (!(iss >> lib_name >> prefix)) {
      std::cerr << "Invalid configuration line: " << line << "\n";
      continue;
    }

    try {
      QDMI_library_load(lib_name, prefix);
    } catch (const std::exception &e) {
      std::cerr << "Failed to open device: " << e.what() << "\n";
      return QDMI_ERROR_FATAL;
    }
  }

  file.close();
  return QDMI_SUCCESS;
}

int QDMI_session_alloc(QDMI_Session *session) {
  *session = new QDMI_Session_impl_d();
  sessions.emplace(*session);
  return QDMI_SUCCESS;
}

int QDMI_session_init(QDMI_Session session) {
  if (session == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  // in this simple implementation, each session has access to all devices
  // for every library the driver creates a device for the calling client
  for (const auto &[_, lib] : libraries) {
    auto &device = session->device_list.emplace_back(
        std::make_unique<QDMI_Device_impl_d>());
    device->library = lib.get();
    device->session = session;
    device->library->device_session_alloc(&device->device_session);
    device->library->device_session_set_parameter(
        device->device_session, QDMI_DEVICE_SESSION_PARAMETER_TOKEN,
        session->token.size() + 1, session->token.c_str());
    device->library->device_session_init(device->device_session);
  }
  session->status = QDMI_SESSION_STATUS::INITIALIZED;
  return QDMI_SUCCESS;
}

void QDMI_session_free(QDMI_Session session) {
  for (auto &device : session->device_list) {
    device->library->device_session_free(device->device_session);
  }
  sessions.erase(session);
  delete session;
}

int QDMI_session_set_parameter(QDMI_Session session,
                               QDMI_Session_Parameter param, const size_t size,
                               const void *value) {
  if (session == nullptr || param >= QDMI_SESSION_PARAMETER_MAX ||
      value == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if (session->status != QDMI_SESSION_STATUS::ALLOCATED) {
    return QDMI_ERROR_BADSTATE;
  }
  switch (param) {
  case QDMI_SESSION_PARAMETER_TOKEN:
    session->token = std::string(static_cast<const char *>(value), size - 1);
    session->mode = session->token.empty() ? QDMI_DEVICE_MODE_READONLY
                                           : QDMI_SESSION_MODE_READWRITE;
    return QDMI_SUCCESS;
  default:
    break;
  }
  return QDMI_ERROR_NOTSUPPORTED;
}

int QDMI_session_get_devices(QDMI_Session session, const size_t num_entries,
                             QDMI_Device *devices, size_t *num_devices) {
  if ((num_entries == 0 && devices != nullptr) ||
      (devices == nullptr && num_devices == nullptr) || session == nullptr ||
      session->status != QDMI_SESSION_STATUS::INITIALIZED) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }

  const auto num_devices_in_session = session->device_list.size();
  if (devices == nullptr) {
    *num_devices = num_devices_in_session;
    return QDMI_SUCCESS;
  }

  const auto num_devices_to_copy =
      std::min(num_entries, num_devices_in_session);
  for (size_t i = 0; i < num_devices_to_copy; ++i) {
    devices[i] = session->device_list[i].get();
  }
  if (num_devices != nullptr) {
    *num_devices = num_devices_to_copy;
  }
  return QDMI_SUCCESS;
}

int QDMI_driver_shutdown() {
  // close all open sessions
  for (const auto &session : sessions) {
    QDMI_session_free(session);
  }
  // Close all libraries
  libraries.clear();
  return QDMI_SUCCESS;
}

int QDMI_job_create(QDMI_Device dev, QDMI_Program_Format format,
                    const size_t size, const void *prog, QDMI_Job *job) {
  if (((prog != nullptr || job != nullptr) &&
       (prog == nullptr || job == nullptr || size == 0)) ||
      (format >= QDMI_PROGRAM_FORMAT_MAX &&
       format != QDMI_PROGRAM_FORMAT_CUSTOM1 &&
       format != QDMI_PROGRAM_FORMAT_CUSTOM2 &&
       format != QDMI_PROGRAM_FORMAT_CUSTOM3 &&
       format != QDMI_PROGRAM_FORMAT_CUSTOM4 &&
       format != QDMI_PROGRAM_FORMAT_CUSTOM5) ||
      dev == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if ((dev->session->mode & QDMI_SESSION_MODE_READWRITE) != 0) {
    if (job == nullptr) {
      return dev->library->device_job_create(dev->device_session, format, 0,
                                             nullptr, nullptr);
    }
    *job = new QDMI_Job_impl_d();
    (*job)->device = dev;
    return dev->library->device_job_create(dev->device_session, format, size,
                                           prog, &(*job)->device_job);
  }
  return QDMI_ERROR_PERMISSIONDENIED;
}

void QDMI_job_free(QDMI_Job job) {
  if (job != nullptr) {
    job->device->library->device_job_free(job->device_job);
    delete job;
  }
}

int QDMI_job_set_parameter(QDMI_Job job, QDMI_Job_Parameter param,
                           const size_t size, const void *value) {
  if (job == nullptr ||
      (param >= QDMI_JOB_PARAMETER_MAX && param != QDMI_JOB_PARAMETER_CUSTOM1 &&
       param != QDMI_JOB_PARAMETER_CUSTOM2 &&
       param != QDMI_JOB_PARAMETER_CUSTOM3 &&
       param != QDMI_JOB_PARAMETER_CUSTOM4 &&
       param != QDMI_JOB_PARAMETER_CUSTOM5) ||
      size == 0 || value == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if ((job->device->session->mode & QDMI_SESSION_MODE_READWRITE) != 0) {
    QDMI_Device_Job_Parameter device_param = QDMI_DEVICE_JOB_PARAMETER_MAX;
    switch (param) {
    case QDMI_JOB_PARAMETER_SHOTS_NUM:
      device_param = QDMI_DEVICE_JOB_PARAMETER_SHOTS_NUM;
      break;
    case QDMI_JOB_PARAMETER_MAX:
      device_param = QDMI_DEVICE_JOB_PARAMETER_MAX;
      break;
    case QDMI_JOB_PARAMETER_CUSTOM1:
      device_param = QDMI_DEVICE_JOB_PARAMETER_CUSTOM1;
      break;
    case QDMI_JOB_PARAMETER_CUSTOM2:
      device_param = QDMI_DEVICE_JOB_PARAMETER_CUSTOM2;
      break;
    case QDMI_JOB_PARAMETER_CUSTOM3:
      device_param = QDMI_DEVICE_JOB_PARAMETER_CUSTOM3;
      break;
    case QDMI_JOB_PARAMETER_CUSTOM4:
      device_param = QDMI_DEVICE_JOB_PARAMETER_CUSTOM4;
      break;
    case QDMI_JOB_PARAMETER_CUSTOM5:
      device_param = QDMI_DEVICE_JOB_PARAMETER_CUSTOM5;
      break;
    }
    return job->device->library->device_job_set_parameter(
        job->device_job, device_param, size, value);
  }
  return QDMI_ERROR_PERMISSIONDENIED;
}

int QDMI_job_submit(QDMI_Job job) {
  if ((job->device->session->mode & QDMI_SESSION_MODE_READWRITE) != 0) {
    return job->device->library->device_job_submit(job->device_job);
  }
  return QDMI_ERROR_PERMISSIONDENIED;
}

int QDMI_job_cancel(QDMI_Job job) {
  if ((job->device->session->mode & QDMI_SESSION_MODE_READWRITE) != 0) {
    return job->device->library->device_job_cancel(job->device_job);
  }
  return QDMI_ERROR_PERMISSIONDENIED;
}

int QDMI_job_check(QDMI_Job job, QDMI_Job_Status *status) {
  if ((job->device->session->mode & QDMI_SESSION_MODE_READWRITE) != 0) {
    return job->device->library->device_job_check(job->device_job, status);
  }
  return QDMI_ERROR_PERMISSIONDENIED;
}

int QDMI_job_wait(QDMI_Job job) {
  if ((job->device->session->mode & QDMI_SESSION_MODE_READWRITE) != 0) {
    return job->device->library->device_job_wait(job->device_job);
  }
  return QDMI_ERROR_PERMISSIONDENIED;
}

int QDMI_job_get_data(QDMI_Job job, QDMI_Job_Result result, const size_t size,
                      void *data, size_t *size_ret) {
  if ((job->device->session->mode & QDMI_SESSION_MODE_READWRITE) != 0) {
    return job->device->library->device_job_get_data(job->device_job, result,
                                                     size, data, size_ret);
  }
  return QDMI_ERROR_PERMISSIONDENIED;
}

int QDMI_device_query_property(QDMI_Device device, QDMI_Device_Property prop,
                               const size_t size, void *value,
                               size_t *size_ret) {
  if (prop >= QDMI_DEVICE_PROPERTY_MAX &&
      prop != QDMI_DEVICE_PROPERTY_CUSTOM1 &&
      prop != QDMI_DEVICE_PROPERTY_CUSTOM2 &&
      prop != QDMI_DEVICE_PROPERTY_CUSTOM3 &&
      prop != QDMI_DEVICE_PROPERTY_CUSTOM4 &&
      prop != QDMI_DEVICE_PROPERTY_CUSTOM5) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  switch (prop) {
  case QDMI_DEVICE_PROPERTY_NAME:
  case QDMI_DEVICE_PROPERTY_VERSION:
  case QDMI_DEVICE_PROPERTY_STATUS:
  case QDMI_DEVICE_PROPERTY_LIBRARYVERSION:
  case QDMI_DEVICE_PROPERTY_QUBITSNUM:
  case QDMI_DEVICE_PROPERTY_CUSTOM1:
  case QDMI_DEVICE_PROPERTY_CUSTOM2:
  case QDMI_DEVICE_PROPERTY_CUSTOM3:
  case QDMI_DEVICE_PROPERTY_CUSTOM4:
  case QDMI_DEVICE_PROPERTY_CUSTOM5:
    return device->library->device_session_query_property(
        device->device_session, prop, size, value, size_ret);
  case QDMI_DEVICE_PROPERTY_COUPLINGMAP: {
    if ((value == nullptr && size_ret == nullptr) || device == nullptr) {
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    size_t num_sites = 0;
    int result = QDMI_device_get_sites(device, 0, nullptr, &num_sites);
    if (result != QDMI_SUCCESS) {
      return QDMI_ERROR_FATAL;
    }
    size_t buff_size = 0;
    result = device->library->device_session_query_property(
        device->device_session, prop, 0, nullptr, &buff_size);
    if (result != QDMI_SUCCESS) {
      return result;
    }
    if (size_ret != nullptr) {
      *size_ret = buff_size / sizeof(QDMI_Device_Site) * sizeof(QDMI_Site);
    }
    if (value != nullptr) {
      if (size < buff_size / sizeof(QDMI_Device_Site) * sizeof(QDMI_Site)) {
        return QDMI_ERROR_INVALIDARGUMENT;
      }
      std::vector<QDMI_Site> sites(num_sites);
      // call the following function, to populate the sites map in the device
      // struct; the vector in the line above is actually not needed.
      result = QDMI_device_get_sites(device, num_sites, sites.data(), nullptr);
      if (result != QDMI_SUCCESS) {
        return QDMI_ERROR_FATAL;
      }
      std::vector<std::pair<QDMI_Device_Site, QDMI_Device_Site>>
          device_coupling_map(buff_size / sizeof(QDMI_Device_Site) / 2);
      result = device->library->device_session_query_property(
          device->device_session, prop, buff_size,
          static_cast<void *>(device_coupling_map.data()), nullptr);
      if (result != QDMI_SUCCESS) {
        return result;
      }
      std::vector<std::pair<QDMI_Site, QDMI_Site>> coupling_map(
          device_coupling_map.size());
      for (size_t i = 0; i < device_coupling_map.size(); ++i) {
        coupling_map[i].first = &device->sites[device_coupling_map[i].first];
        coupling_map[i].second = &device->sites[device_coupling_map[i].second];
      }
      memcpy(value, static_cast<void *>(coupling_map.data()),
             2 * sizeof(QDMI_Site) * coupling_map.size());
    }
    return QDMI_SUCCESS;
  }
  default:
    return QDMI_ERROR_NOTSUPPORTED;
  }
}

int QDMI_device_get_sites(QDMI_Device device, const size_t num_entries,
                          QDMI_Site *sites, size_t *num_sites) {
  if ((sites != nullptr && num_entries == 0) ||
      (sites == nullptr && num_sites == nullptr) || device == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  size_t num_device_sites = 0;
  int result = device->library->device_session_get_sites(
      device->device_session, 0, nullptr, &num_device_sites);
  if (result != QDMI_SUCCESS) {
    return result;
  }
  if (num_sites != nullptr) {
    *num_sites = num_device_sites;
  }
  if (sites != nullptr) {
    const size_t device_entries = std::min(num_entries, num_device_sites);
    std::vector<QDMI_Device_Site> device_sites(device_entries);
    result = device->library->device_session_get_sites(
        device->device_session, device_entries, device_sites.data(), nullptr);
    if (result != QDMI_SUCCESS) {
      return result;
    }
    for (size_t i = 0; i < device_entries; ++i) {
      auto it = device->sites.find(device_sites[i]);
      if (it == device->sites.end()) {
        it = device->sites
                 .emplace(device_sites[i],
                          QDMI_Site_impl_d{device, device_sites[i]})
                 .first;
      }
      sites[i] = &it->second;
    }
  }
  return QDMI_SUCCESS;
}

int QDMI_device_get_operations(QDMI_Device device, const size_t num_entries,
                               QDMI_Operation *operations,
                               size_t *num_operations) {
  if ((operations != nullptr && num_entries == 0) ||
      (operations == nullptr && num_operations == nullptr) ||
      device == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  size_t num_device_operations = 0;
  int result = device->library->device_session_get_operations(
      device->device_session, 0, nullptr, &num_device_operations);
  if (result != QDMI_SUCCESS) {
    return result;
  }
  if (num_operations != nullptr) {
    *num_operations = num_device_operations;
  }
  if (operations != nullptr) {
    const size_t device_entries = std::min(num_entries, num_device_operations);
    std::vector<QDMI_Device_Operation> device_operations(device_entries);
    result = device->library->device_session_get_operations(
        device->device_session, device_entries, device_operations.data(),
        nullptr);
    if (result != QDMI_SUCCESS) {
      return result;
    }
    for (size_t i = 0; i < device_entries; ++i) {
      auto it = device->operations.find(device_operations[i]);
      if (it == device->operations.end()) {
        it = device->operations
                 .emplace(device_operations[i],
                          QDMI_Operation_impl_d{device, device_operations[i]})
                 .first;
      }
      operations[i] = &it->second;
    }
  }
  return QDMI_SUCCESS;
}

int QDMI_site_query_property(QDMI_Site site, QDMI_Site_Property prop,
                             const size_t size, void *value, size_t *size_ret) {
  if (prop >= QDMI_SITE_PROPERTY_MAX && prop != QDMI_SITE_PROPERTY_CUSTOM1 &&
      prop != QDMI_SITE_PROPERTY_CUSTOM2 &&
      prop != QDMI_SITE_PROPERTY_CUSTOM3 &&
      prop != QDMI_SITE_PROPERTY_CUSTOM4 &&
      prop != QDMI_SITE_PROPERTY_CUSTOM5) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  return site->device->library->device_site_query_property(
      site->device_site, prop, size, value, size_ret);
}

int QDMI_operation_query_property(QDMI_Operation operation,
                                  const size_t num_sites,
                                  const QDMI_Site *sites,
                                  QDMI_Operation_Property prop,
                                  const size_t size, void *value,
                                  size_t *size_ret) {
  if (prop >= QDMI_OPERATION_PROPERTY_MAX &&
      prop != QDMI_OPERATION_PROPERTY_CUSTOM1 &&
      prop != QDMI_OPERATION_PROPERTY_CUSTOM2 &&
      prop != QDMI_OPERATION_PROPERTY_CUSTOM3 &&
      prop != QDMI_OPERATION_PROPERTY_CUSTOM4 &&
      prop != QDMI_OPERATION_PROPERTY_CUSTOM5) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  std::vector<QDMI_Device_Site> device_sites(num_sites);
  for (size_t i = 0; i < num_sites; ++i) {
    device_sites[i] = sites[i]->device_site;
  }
  return operation->device->library->device_operation_query_property(
      operation->operation, num_sites, device_sites.data(), prop, size, value,
      size_ret);
}
