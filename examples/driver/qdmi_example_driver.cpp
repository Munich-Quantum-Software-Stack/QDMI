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
#include "qdmi/types.h"

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
 * @brief Definition of the QDMI Device.
 */
struct QDMI_Device_impl_d {
  QDMI_Library library = nullptr;
  QDMI_Session session = nullptr;
  QDMI_Device_Session device_session = nullptr;
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

namespace {
/**
 * @brief Global list of devices managed by the driver.
 */
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
std::unordered_map<void *, QDMI_Library> libraries;
std::unordered_set<QDMI_Session> sessions;

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
  QDMI_Library library = nullptr;
  try {
    // load the function symbols from the dynamic library
    const std::string symbol_name = std::string(prefix) + "_QDMI_LIBRARY";
    library = static_cast<QDMI_Library>(dlsym(lib_handle, symbol_name.c_str()));
    if (library == nullptr) {
      throw std::runtime_error("Failed to load symbol: " + symbol_name);
    }
  } catch (const std::exception &) {
    dlclose(lib_handle);
    throw;
  }
  libraries.emplace(lib_handle, library);
  // initialize the device
  library->device_initialize();
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
    device->library = lib;
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
  if (job == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if ((job->device->session->mode & QDMI_SESSION_MODE_READWRITE) != 0) {
    return job->device->library->device_job_submit(job->device_job);
  }
  return QDMI_ERROR_PERMISSIONDENIED;
}

int QDMI_job_cancel(QDMI_Job job) {
  if (job == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if ((job->device->session->mode & QDMI_SESSION_MODE_READWRITE) != 0) {
    return job->device->library->device_job_cancel(job->device_job);
  }
  return QDMI_ERROR_PERMISSIONDENIED;
}

int QDMI_job_check(QDMI_Job job, QDMI_Job_Status *status) {
  if (job == nullptr || status == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if ((job->device->session->mode & QDMI_SESSION_MODE_READWRITE) != 0) {
    return job->device->library->device_job_check(job->device_job, status);
  }
  return QDMI_ERROR_PERMISSIONDENIED;
}

int QDMI_job_wait(QDMI_Job job) {
  if (job == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  if ((job->device->session->mode & QDMI_SESSION_MODE_READWRITE) != 0) {
    return job->device->library->device_job_wait(job->device_job);
  }
  return QDMI_ERROR_PERMISSIONDENIED;
}

int QDMI_job_get_data(QDMI_Job job, QDMI_Job_Result result, const size_t size,
                      void *data, size_t *size_ret) {
  if (job == nullptr ||
      (result >= QDMI_JOB_RESULT_MAX && result != QDMI_JOB_RESULT_CUSTOM1 &&
       result != QDMI_JOB_RESULT_CUSTOM2 && result != QDMI_JOB_RESULT_CUSTOM3 &&
       result != QDMI_JOB_RESULT_CUSTOM4 &&
       result != QDMI_JOB_RESULT_CUSTOM5) ||
      ((size == 0 || data == nullptr) && size_ret == nullptr)) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
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
  return device->library->device_session_query_property(
      device->device_session, prop, size, value, size_ret);
}

int QDMI_device_get_sites(QDMI_Device device, const size_t num_entries,
                          QDMI_Site *sites, size_t *num_sites) {
  if ((sites != nullptr && num_entries == 0) ||
      (sites == nullptr && num_sites == nullptr) || device == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  return device->library->device_session_get_sites(
      device->device_session, num_entries, sites, num_sites);
}

int QDMI_device_get_operations(QDMI_Device device, const size_t num_entries,
                               QDMI_Operation *operations,
                               size_t *num_operations) {
  if ((operations != nullptr && num_entries == 0) ||
      (operations == nullptr && num_operations == nullptr) ||
      device == nullptr) {
    return QDMI_ERROR_INVALIDARGUMENT;
  }
  return device->library->device_session_get_operations(
      device->device_session, num_entries, operations, num_operations);
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
  return site->library->device_site_query_property(site, prop, size, value,
                                                   size_ret);
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
  return operation->library->device_operation_query_property(
      operation, num_sites, sites, prop, size, value, size_ret);
}
