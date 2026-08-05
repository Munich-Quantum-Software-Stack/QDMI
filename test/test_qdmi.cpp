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

#include "example_fomac.hpp"
#include "example_tool.hpp"
#include "qdmi/client.h"
#include "qdmi_example_driver.h"
#include "utils/test_impl.hpp"

#include <algorithm>
#include <array>
#include <complex>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <functional>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <random>
#include <sstream>
extern "C" {
#include <stdlib.h>
}
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace {
/// Hash function for a pair
struct Pair_hash {
  template <class T, class U>
  auto operator()(const std::pair<T, U> &p) const noexcept -> std::size_t {
    // Use the hash of the first and second element of the pair
    return std::hash<T>{}(p.first) ^ std::hash<U>{}(p.second);
  }
};
} // namespace

// Instantiate the test suite with different parameters
INSTANTIATE_TEST_SUITE_P(
    QDMIDevice,
    // Custom instantiation name
    QDMIImplementationTest,
    // Test suite name
    // Parameters to test with
    ::testing::Values(std::tuple{"../examples/device/src/libcxx-qdmi-device",
                                 "CXX", TEST_SESSION_MODE::READONLY},
                      std::tuple{"../examples/device/src/libcxx-qdmi-device",
                                 "CXX", TEST_SESSION_MODE::READWRITE}),
    [](const testing::TestParamInfo<
        std::tuple<std::string, std::string, TEST_SESSION_MODE>> &inf) {
      // Extract the last part of the file path
      const size_t pos = std::get<0>(inf.param).find_last_of("/\\");
      std::string filename = (pos == std::string::npos)
                                 ? std::get<0>(inf.param)
                                 : std::get<0>(inf.param).substr(pos + 1);

      // Replace '-' by '_' for valid test names
      std::ranges::replace(filename, '-', '_');

      // Strip the 'lib' prefix if it exists
      const std::string prefix = "lib";
      if (filename.compare(0, prefix.size(), prefix) == 0) {
        filename = filename.substr(prefix.size());
      }

      // return name for the test
      switch (std::get<2>(inf.param)) {
      case TEST_SESSION_MODE::READONLY:
        return filename + "__readonly";
      case TEST_SESSION_MODE::READWRITE:
        return filename + "__readwrite";
      default:
        return filename;
      }
    });

TEST(QDMIOptionalDeviceFunctionTest, MissingOpenSymbolReturnsNotSupported) {
  const std::filesystem::path config_file =
      "qdmi_optional_device_function.conf";
  {
    std::ofstream config(config_file);
    config << QDMI_TEMPLATE_DEVICE_LIBRARY << " MY\n";
  }
#ifdef _WIN32
  ASSERT_EQ(_putenv_s("QDMI_CONF", config_file.string().c_str()), 0);
#else
  ASSERT_EQ(setenv("QDMI_CONF", config_file.string().c_str(), 1), 0);
#endif

  ASSERT_EQ(QDMI_driver_init(), QDMI_SUCCESS);
  QDMI_Session test_session = nullptr;
  ASSERT_EQ(QDMI_session_alloc(&test_session), QDMI_SUCCESS);
  ASSERT_EQ(QDMI_session_set_parameter(
                test_session, QDMI_SESSION_PARAMETER_TOKEN, 6, "token"),
            QDMI_SUCCESS);
  ASSERT_EQ(QDMI_session_init(test_session), QDMI_SUCCESS);

  size_t devices_size = 0;
  ASSERT_EQ(QDMI_session_query_session_property(test_session,
                                                QDMI_SESSION_PROPERTY_DEVICES,
                                                0, nullptr, &devices_size),
            QDMI_SUCCESS);
  ASSERT_GE(devices_size, sizeof(QDMI_Device));
  std::vector<QDMI_Device> devices(devices_size / sizeof(QDMI_Device));
  ASSERT_EQ(QDMI_session_query_session_property(
                test_session, QDMI_SESSION_PROPERTY_DEVICES, devices_size,
                static_cast<void *>(devices.data()), nullptr),
            QDMI_SUCCESS);
  const auto test_device = devices.front();
  QDMI_Job job = nullptr;
  EXPECT_EQ(QDMI_device_open_job(test_device, "job-id", &job),
            QDMI_ERROR_NOTSUPPORTED);

  QDMI_session_free(test_session);
  EXPECT_EQ(QDMI_driver_shutdown(), QDMI_SUCCESS);
  std::filesystem::remove(config_file);
}

TEST_P(QDMIImplementationTest, QueryNumQubits) {
  const auto fomac = FoMaC(device);
  ASSERT_GT(fomac.get_qubits_num(), 0);
}

TEST_P(QDMIImplementationTest, QueryOperationSet) {
  const auto fomac = FoMaC(device);
  const auto gates = fomac.get_operation_map();
  ASSERT_GT(gates.size(), 0);
  for (const auto &[op_name, op] : gates) {
    ASSERT_FALSE(op_name.empty());
    std::string name(op_name.length(), '\0');
    ASSERT_EQ(QDMI_device_query_operation_property(
                  device, op, 0, nullptr, 0, nullptr,
                  QDMI_OPERATION_PROPERTY_NAME, name.length() + 1, name.data(),
                  nullptr),
              QDMI_SUCCESS);
    EXPECT_EQ(op_name, name);
  }
}

TEST_P(QDMIImplementationTest, QueryCouplingMap) {
  const auto fomac = FoMaC(device);
  const auto coupling_map = fomac.get_coupling_map();
  const auto num_qubits = fomac.get_qubits_num();
  if (num_qubits == 1) {
    ASSERT_TRUE(coupling_map.empty());
  } else {
    ASSERT_GT(coupling_map.size(), 0);
  }
}

TEST_P(QDMIImplementationTest, QueryGatePropertiesForEachGate) {
  // for every gate in the gate set, query the duration of the gate
  const auto fomac = FoMaC(device);
  const auto ops = fomac.get_operation_map();
  const auto sites = fomac.get_sites();
  const auto coupling_map = fomac.get_coupling_map();

  for (const auto &[name, op] : ops) {
    const auto gate_num_qubits = fomac.get_operands_num(op);
    const auto gate_num_params = fomac.get_parameters_num(op);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(0.0, 1.0);
    std::vector<double> params(gate_num_params);
    for (auto &param : params) {
      param = dis(gen);
    }

    uint64_t duration = 0;
    double fidelity = 0;
    if (gate_num_qubits == 1) {
      const std::unordered_set set_of_all_sites(sites.cbegin(), sites.cend());
      size_t size_of_supported_sites = 0;
      ASSERT_EQ(QDMI_device_query_operation_property(
                    device, op, 0, nullptr, 0, nullptr,
                    QDMI_OPERATION_PROPERTY_SITES, 0, nullptr,
                    &size_of_supported_sites),
                QDMI_SUCCESS);
      ASSERT_EQ(size_of_supported_sites % sizeof(QDMI_Site), 0)
          << "size_of_supported_sites (" << size_of_supported_sites
          << ") is not a multiple of sizeof(QDMI_Site) (" << sizeof(QDMI_Site)
          << ")";
      std::vector<QDMI_Site> supported_sites(size_of_supported_sites /
                                             sizeof(QDMI_Site));
      ASSERT_EQ(QDMI_device_query_operation_property(
                    device, op, 0, nullptr, 0, nullptr,
                    QDMI_OPERATION_PROPERTY_SITES, size_of_supported_sites,
                    static_cast<void *>(supported_sites.data()), nullptr),
                QDMI_SUCCESS);
      for (const auto &site : supported_sites) {
        EXPECT_NE(set_of_all_sites.find(site), set_of_all_sites.end())
            << "Supported sites must be a subset of all sites.";
        auto site_arr = std::array{site};
        EXPECT_EQ(QDMI_device_query_operation_property(
                      device, op, gate_num_qubits, site_arr.data(),
                      gate_num_params, params.data(),
                      QDMI_OPERATION_PROPERTY_DURATION, sizeof(uint64_t),
                      &duration, nullptr),
                  QDMI_SUCCESS)
            << "Failed to query duration for operation " << name;
        EXPECT_EQ(QDMI_device_query_operation_property(
                      device, op, gate_num_qubits, site_arr.data(),
                      gate_num_params, params.data(),
                      QDMI_OPERATION_PROPERTY_FIDELITY, sizeof(double),
                      &fidelity, nullptr),
                  QDMI_SUCCESS)
            << "Failed to query fidelity for operation " << name;
      }
    }
    if (gate_num_qubits == 2) {
      const std::unordered_set<std::pair<QDMI_Site, QDMI_Site>, Pair_hash>
          set_of_all_site_pairs(coupling_map.cbegin(), coupling_map.cend());
      size_t size_of_supported_site_pairs = 0;
      ASSERT_EQ(QDMI_device_query_operation_property(
                    device, op, 0, nullptr, 0, nullptr,
                    QDMI_OPERATION_PROPERTY_SITES, 0, nullptr,
                    &size_of_supported_site_pairs),
                QDMI_SUCCESS);
      ASSERT_EQ(size_of_supported_site_pairs %
                    sizeof(std::pair<QDMI_Site, QDMI_Site>),
                0)
          << "size_of_supported_site_pairs is not a multiple of "
             "sizeof(std::pair<QDMI_Site, QDMI_Site>)";
      std::vector<std::pair<QDMI_Site, QDMI_Site>> supported_site_pairs(
          size_of_supported_site_pairs /
          sizeof(std::pair<QDMI_Site, QDMI_Site>));
      ASSERT_EQ(QDMI_device_query_operation_property(
                    device, op, 0, nullptr, 0, nullptr,
                    QDMI_OPERATION_PROPERTY_SITES, size_of_supported_site_pairs,
                    static_cast<void *>(supported_site_pairs.data()), nullptr),
                QDMI_SUCCESS);
      for (const auto &pair : supported_site_pairs) {
        EXPECT_NE(set_of_all_site_pairs.find(pair), set_of_all_site_pairs.end())
            << "Supported site pairs must be a subset of edges in the coupling "
               "map.";
        const auto &[control, target] = pair;
        auto site_arr = std::array{control, target};
        EXPECT_EQ(QDMI_device_query_operation_property(
                      device, op, gate_num_qubits, site_arr.data(),
                      gate_num_params, params.data(),
                      QDMI_OPERATION_PROPERTY_DURATION, sizeof(uint64_t),
                      &duration, nullptr),
                  QDMI_SUCCESS)
            << "Failed to query duration for gate " << op;
        EXPECT_EQ(QDMI_device_query_operation_property(
                      device, op, gate_num_qubits, site_arr.data(),
                      gate_num_params, params.data(),
                      QDMI_OPERATION_PROPERTY_FIDELITY, sizeof(double),
                      &fidelity, nullptr),
                  QDMI_SUCCESS)
            << "Failed to query fidelity for gate " << op;
      }
    }

    bool is_global = true;
    EXPECT_EQ(
        QDMI_device_query_operation_property(device, op, 0, nullptr, 0, nullptr,
                                             QDMI_OPERATION_PROPERTY_ISZONED,
                                             sizeof(bool), &is_global, nullptr),
        QDMI_SUCCESS);
    EXPECT_FALSE(is_global);

    // The example device does not support neutral atom-specific properties
    EXPECT_EQ(QDMI_device_query_operation_property(
                  device, op, 0, nullptr, 0, nullptr,
                  QDMI_OPERATION_PROPERTY_INTERACTIONRADIUS, 0, nullptr,
                  nullptr),
              QDMI_ERROR_NOTSUPPORTED);
    EXPECT_EQ(QDMI_device_query_operation_property(
                  device, op, 0, nullptr, 0, nullptr,
                  QDMI_OPERATION_PROPERTY_BLOCKINGRADIUS, 0, nullptr, nullptr),
              QDMI_ERROR_NOTSUPPORTED);
    EXPECT_EQ(QDMI_device_query_operation_property(
                  device, op, 0, nullptr, 0, nullptr,
                  QDMI_OPERATION_PROPERTY_IDLINGFIDELITY, 0, nullptr, nullptr),
              QDMI_ERROR_NOTSUPPORTED);
    EXPECT_EQ(QDMI_device_query_operation_property(
                  device, op, 0, nullptr, 0, nullptr,
                  QDMI_OPERATION_PROPERTY_MEANSHUTTLINGSPEED, 0, nullptr,
                  nullptr),
              QDMI_ERROR_NOTSUPPORTED);

    // The MAX property is not a valid value for any device
    EXPECT_EQ(QDMI_device_query_operation_property(
                  device, op, 0, nullptr, 0, nullptr,
                  QDMI_OPERATION_PROPERTY_MAX, 0, nullptr, nullptr),
              QDMI_ERROR_INVALIDARGUMENT);

    // The example devices do not support custom properties
    EXPECT_EQ(QDMI_device_query_operation_property(
                  device, op, 0, nullptr, 0, nullptr,
                  QDMI_OPERATION_PROPERTY_CUSTOM1, 0, nullptr, nullptr),
              QDMI_ERROR_NOTSUPPORTED);
    EXPECT_EQ(QDMI_device_query_operation_property(
                  device, op, 0, nullptr, 0, nullptr,
                  QDMI_OPERATION_PROPERTY_CUSTOM2, 0, nullptr, nullptr),
              QDMI_ERROR_NOTSUPPORTED);
    EXPECT_EQ(QDMI_device_query_operation_property(
                  device, op, 0, nullptr, 0, nullptr,
                  QDMI_OPERATION_PROPERTY_CUSTOM3, 0, nullptr, nullptr),
              QDMI_ERROR_NOTSUPPORTED);
    EXPECT_EQ(QDMI_device_query_operation_property(
                  device, op, 0, nullptr, 0, nullptr,
                  QDMI_OPERATION_PROPERTY_CUSTOM4, 0, nullptr, nullptr),
              QDMI_ERROR_NOTSUPPORTED);
    EXPECT_EQ(QDMI_device_query_operation_property(
                  device, op, 0, nullptr, 0, nullptr,
                  QDMI_OPERATION_PROPERTY_CUSTOM5, 0, nullptr, nullptr),
              QDMI_ERROR_NOTSUPPORTED);
  }
}

TEST_P(QDMIImplementationTest, QuerySiteProperties) {
  // Check whether there are equally many sites as reported qubits
  const auto fomac = FoMaC(device);
  const auto sites = fomac.get_sites();
  const auto qubits_num = fomac.get_qubits_num();
  EXPECT_EQ(sites.size(), qubits_num);
  // For every site check that the site ID is less than the number of qubits.
  // Note that this assumption is only true for the provided example devices.
  for (const auto &site : sites) {
    const auto site_id = fomac.get_site_id(site);
    EXPECT_LT(site_id, qubits_num);
    const auto t1 = fomac.get_site_t1(site);
    EXPECT_GT(t1, 0);
    const auto t2 = fomac.get_site_t2(site);
    EXPECT_GT(t2, 0);

    // the example device only offers regular sites
    EXPECT_EQ(QDMI_device_query_site_property(
                  device, site, QDMI_SITE_PROPERTY_ISZONE, 0, nullptr, nullptr),
              QDMI_ERROR_NOTSUPPORTED);
    uint64_t module_id = 1;
    EXPECT_EQ(QDMI_device_query_site_property(
                  device, site, QDMI_SITE_PROPERTY_MODULEINDEX,
                  sizeof(uint64_t), &module_id, nullptr),
              QDMI_SUCCESS);
    // Example device always returns 0 for module index
    EXPECT_EQ(module_id, 0);
    EXPECT_EQ(QDMI_device_query_site_property(device, site,
                                              QDMI_SITE_PROPERTY_SUBMODULEINDEX,
                                              0, nullptr, nullptr),
              QDMI_ERROR_NOTSUPPORTED);
    // The example devices do not support neutral atom-specific properties
    EXPECT_EQ(QDMI_device_query_site_property(device, site,
                                              QDMI_SITE_PROPERTY_XCOORDINATE, 0,
                                              nullptr, nullptr),
              QDMI_ERROR_NOTSUPPORTED);
    EXPECT_EQ(QDMI_device_query_site_property(device, site,
                                              QDMI_SITE_PROPERTY_YCOORDINATE, 0,
                                              nullptr, nullptr),
              QDMI_ERROR_NOTSUPPORTED);
    EXPECT_EQ(QDMI_device_query_site_property(device, site,
                                              QDMI_SITE_PROPERTY_ZCOORDINATE, 0,
                                              nullptr, nullptr),
              QDMI_ERROR_NOTSUPPORTED);
    EXPECT_EQ(QDMI_device_query_site_property(device, site,
                                              QDMI_SITE_PROPERTY_XEXTENT, 0,
                                              nullptr, nullptr),
              QDMI_ERROR_NOTSUPPORTED);
    EXPECT_EQ(QDMI_device_query_site_property(device, site,
                                              QDMI_SITE_PROPERTY_YEXTENT, 0,
                                              nullptr, nullptr),
              QDMI_ERROR_NOTSUPPORTED);
    EXPECT_EQ(QDMI_device_query_site_property(device, site,
                                              QDMI_SITE_PROPERTY_ZEXTENT, 0,
                                              nullptr, nullptr),
              QDMI_ERROR_NOTSUPPORTED);

    // The MAX property is not a valid value for any device.
    EXPECT_EQ(QDMI_device_query_site_property(
                  device, site, QDMI_SITE_PROPERTY_MAX, 0, nullptr, nullptr),
              QDMI_ERROR_INVALIDARGUMENT);

    // The example devices do not support custom properties
    EXPECT_EQ(QDMI_device_query_site_property(device, site,
                                              QDMI_SITE_PROPERTY_CUSTOM1, 0,
                                              nullptr, nullptr),
              QDMI_ERROR_NOTSUPPORTED);
    EXPECT_EQ(QDMI_device_query_site_property(device, site,
                                              QDMI_SITE_PROPERTY_CUSTOM2, 0,
                                              nullptr, nullptr),
              QDMI_ERROR_NOTSUPPORTED);
    EXPECT_EQ(QDMI_device_query_site_property(device, site,
                                              QDMI_SITE_PROPERTY_CUSTOM3, 0,
                                              nullptr, nullptr),
              QDMI_ERROR_NOTSUPPORTED);
    EXPECT_EQ(QDMI_device_query_site_property(device, site,
                                              QDMI_SITE_PROPERTY_CUSTOM4, 0,
                                              nullptr, nullptr),
              QDMI_ERROR_NOTSUPPORTED);
    EXPECT_EQ(QDMI_device_query_site_property(device, site,
                                              QDMI_SITE_PROPERTY_CUSTOM5, 0,
                                              nullptr, nullptr),
              QDMI_ERROR_NOTSUPPORTED);
  }
}

TEST_P(QDMIImplementationTest, QueryDeviceProperties) {
  // Query the name of the device
  size_t size = 0;
  EXPECT_EQ(QDMI_device_query_device_property(device, QDMI_DEVICE_PROPERTY_NAME,
                                              0, nullptr, &size),
            QDMI_SUCCESS);
  std::string name(size - 1, '\0');
  EXPECT_EQ(QDMI_device_query_device_property(device, QDMI_DEVICE_PROPERTY_NAME,
                                              name.size() + 1, name.data(),
                                              nullptr),
            QDMI_SUCCESS);

  // Query the length unit of the device
  size = 0;
  EXPECT_EQ(QDMI_device_query_device_property(
                device, QDMI_DEVICE_PROPERTY_LENGTHUNIT, 0, nullptr, &size),
            QDMI_SUCCESS);
  std::string length_unit(size - 1, '\0');
  EXPECT_EQ(QDMI_device_query_device_property(
                device, QDMI_DEVICE_PROPERTY_LENGTHUNIT, length_unit.size() + 1,
                length_unit.data(), nullptr),
            QDMI_SUCCESS);
  EXPECT_THAT(length_unit, testing::AnyOf("mm", "um", "nm"));
  double scale_factor = 0.0;
  EXPECT_EQ(QDMI_device_query_device_property(
                device, QDMI_DEVICE_PROPERTY_LENGTHSCALEFACTOR, sizeof(double),
                &scale_factor, nullptr),
            QDMI_SUCCESS);
  EXPECT_GE(scale_factor, 0.0);

  // The example device does not support neutral atom-specific properties
  EXPECT_EQ(
      QDMI_device_query_device_property(
          device, QDMI_DEVICE_PROPERTY_MINATOMDISTANCE, 0, nullptr, nullptr),
      QDMI_ERROR_NOTSUPPORTED);

  // The MAX property is not a valid value for any device.
  EXPECT_EQ(QDMI_device_query_device_property(device, QDMI_DEVICE_PROPERTY_MAX,
                                              0, nullptr, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);

  // The example devices do not support custom properties
  EXPECT_EQ(QDMI_device_query_device_property(
                device, QDMI_DEVICE_PROPERTY_CUSTOM1, 0, nullptr, nullptr),
            QDMI_ERROR_NOTSUPPORTED);
  EXPECT_EQ(QDMI_device_query_device_property(
                device, QDMI_DEVICE_PROPERTY_CUSTOM2, 0, nullptr, nullptr),
            QDMI_ERROR_NOTSUPPORTED);
  EXPECT_EQ(QDMI_device_query_device_property(
                device, QDMI_DEVICE_PROPERTY_CUSTOM3, 0, nullptr, nullptr),
            QDMI_ERROR_NOTSUPPORTED);
  EXPECT_EQ(QDMI_device_query_device_property(
                device, QDMI_DEVICE_PROPERTY_CUSTOM4, 0, nullptr, nullptr),
            QDMI_ERROR_NOTSUPPORTED);
  EXPECT_EQ(QDMI_device_query_device_property(
                device, QDMI_DEVICE_PROPERTY_CUSTOM5, 0, nullptr, nullptr),
            QDMI_ERROR_NOTSUPPORTED);
}

TEST_P(QDMIImplementationTest, JobLifecycle) {
  if (mode == TEST_SESSION_MODE::READONLY) {
    GTEST_SKIP() << "Skipping test for read-only session";
  }
  QDMI_Job job{};
  EXPECT_EQ(QDMI_device_create_job(device, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
  EXPECT_EQ(QDMI_device_create_job(nullptr, &job), QDMI_ERROR_INVALIDARGUMENT);
  ASSERT_EQ(QDMI_device_create_job(device, &job), QDMI_SUCCESS);

  // Test format support
  EXPECT_EQ(
      QDMI_job_set_parameter(job, QDMI_JOB_PARAMETER_PROGRAMFORMAT, 0, nullptr),
      QDMI_SUCCESS);
  QDMI_Program_Format format = QDMI_PROGRAM_FORMAT_MAX;
  EXPECT_EQ(QDMI_job_set_parameter(job, QDMI_JOB_PARAMETER_PROGRAMFORMAT,
                                   sizeof(QDMI_Program_Format), &format),
            QDMI_ERROR_INVALIDARGUMENT);

  constexpr std::array supported_formats = {
      QDMI_PROGRAM_FORMAT_QASM2, QDMI_PROGRAM_FORMAT_QIRBASESTRING,
      QDMI_PROGRAM_FORMAT_QIRBASEMODULE, QDMI_PROGRAM_FORMAT_CALIBRATION};

  for (const auto &supported_format : supported_formats) {
    ASSERT_EQ(QDMI_job_set_parameter(job, QDMI_JOB_PARAMETER_PROGRAMFORMAT,
                                     sizeof(QDMI_Program_Format),
                                     &supported_format),
              QDMI_SUCCESS);
  }

  const auto fomac = FoMaC(device);
  const auto formats = fomac.get_supported_program_formats();
  for (const auto &program_format : formats) {
    ASSERT_EQ(QDMI_job_set_parameter(job, QDMI_JOB_PARAMETER_PROGRAMFORMAT,
                                     sizeof(QDMI_Program_Format),
                                     &program_format),
              QDMI_SUCCESS);
  }

  constexpr std::array unsupported_formats = {
      QDMI_PROGRAM_FORMAT_QASM3,
      QDMI_PROGRAM_FORMAT_QIRADAPTIVESTRING,
      QDMI_PROGRAM_FORMAT_QIRADAPTIVEMODULE,
      QDMI_PROGRAM_FORMAT_QPY,
      QDMI_PROGRAM_FORMAT_IQMJSON,
      QDMI_PROGRAM_FORMAT_CUSTOM1,
      QDMI_PROGRAM_FORMAT_CUSTOM2,
      QDMI_PROGRAM_FORMAT_CUSTOM3,
      QDMI_PROGRAM_FORMAT_CUSTOM4,
      QDMI_PROGRAM_FORMAT_CUSTOM5};

  for (const auto &unsupported_format : unsupported_formats) {
    EXPECT_EQ(QDMI_job_set_parameter(job, QDMI_JOB_PARAMETER_PROGRAMFORMAT,
                                     sizeof(QDMI_Program_Format),
                                     &unsupported_format),
              QDMI_ERROR_NOTSUPPORTED);
  }

  // The MAX parameter is not a valid value for any device
  EXPECT_EQ(QDMI_job_set_parameter(job, QDMI_JOB_PARAMETER_MAX, 0, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);

  // The example devices do not support custom parameters
  EXPECT_EQ(QDMI_job_set_parameter(job, QDMI_JOB_PARAMETER_CUSTOM1, 0, nullptr),
            QDMI_ERROR_NOTSUPPORTED);
  EXPECT_EQ(QDMI_job_set_parameter(job, QDMI_JOB_PARAMETER_CUSTOM2, 0, nullptr),
            QDMI_ERROR_NOTSUPPORTED);
  EXPECT_EQ(QDMI_job_set_parameter(job, QDMI_JOB_PARAMETER_CUSTOM3, 0, nullptr),
            QDMI_ERROR_NOTSUPPORTED);
  EXPECT_EQ(QDMI_job_set_parameter(job, QDMI_JOB_PARAMETER_CUSTOM4, 0, nullptr),
            QDMI_ERROR_NOTSUPPORTED);
  EXPECT_EQ(QDMI_job_set_parameter(job, QDMI_JOB_PARAMETER_CUSTOM5, 0, nullptr),
            QDMI_ERROR_NOTSUPPORTED);

  format = QDMI_PROGRAM_FORMAT_QASM2;
  EXPECT_EQ(QDMI_job_set_parameter(job, QDMI_JOB_PARAMETER_PROGRAMFORMAT,
                                   sizeof(QDMI_Program_Format), &format),
            QDMI_SUCCESS);
  // The set parameter value must coincide with the value returned for the
  // respective property
  size_t size = 0;
  EXPECT_EQ(QDMI_job_query_property(job, QDMI_JOB_PROPERTY_PROGRAMFORMAT,
                                    sizeof(QDMI_Program_Format), &format,
                                    &size),
            QDMI_SUCCESS);
  EXPECT_EQ(size, sizeof(QDMI_Program_Format));
  EXPECT_EQ(format, QDMI_PROGRAM_FORMAT_QASM2);

  size_t shots = 5;
  EXPECT_EQ(QDMI_job_set_parameter(nullptr, QDMI_JOB_PARAMETER_SHOTSNUM,
                                   sizeof(size_t), &shots),
            QDMI_ERROR_INVALIDARGUMENT);
  EXPECT_EQ(
      QDMI_job_set_parameter(job, QDMI_JOB_PARAMETER_SHOTSNUM, 0, nullptr),
      QDMI_SUCCESS);
  EXPECT_EQ(QDMI_job_set_parameter(job, QDMI_JOB_PARAMETER_MAX, sizeof(size_t),
                                   &shots),
            QDMI_ERROR_INVALIDARGUMENT);
  ASSERT_EQ(QDMI_job_set_parameter(job, QDMI_JOB_PARAMETER_SHOTSNUM,
                                   sizeof(size_t), &shots),
            QDMI_SUCCESS);
  // The set parameter value must coincide with the value returned for the
  // respective property
  EXPECT_EQ(QDMI_job_query_property(job, QDMI_JOB_PROPERTY_SHOTSNUM,
                                    sizeof(size_t), &shots, nullptr),
            QDMI_SUCCESS);
  EXPECT_EQ(shots, 5);
  ASSERT_EQ(QDMI_job_submit(job), QDMI_SUCCESS);
  EXPECT_EQ(QDMI_job_submit(job), QDMI_ERROR_BADSTATE);
  EXPECT_EQ(QDMI_job_submit(nullptr), QDMI_ERROR_INVALIDARGUMENT);
  // Cannot get results from a job that is not done yet.
  EXPECT_EQ(
      QDMI_job_get_results(job, QDMI_JOB_RESULT_SHOTS, 0, nullptr, nullptr),
      QDMI_ERROR_INVALIDARGUMENT);
  EXPECT_EQ(QDMI_job_check(job, nullptr), QDMI_ERROR_INVALIDARGUMENT);
  QDMI_Job_Status status{};
  EXPECT_EQ(QDMI_job_check(nullptr, &status), QDMI_ERROR_INVALIDARGUMENT);
  EXPECT_EQ(QDMI_job_check(job, &status), QDMI_SUCCESS);
  EXPECT_EQ(QDMI_job_wait(job, 0), QDMI_SUCCESS);
  EXPECT_EQ(QDMI_job_wait(nullptr, 0), QDMI_ERROR_INVALIDARGUMENT);
  ASSERT_EQ(QDMI_job_check(job, &status), QDMI_SUCCESS);
  EXPECT_EQ(status, QDMI_JOB_STATUS_DONE);
  EXPECT_EQ(QDMI_job_cancel(job), QDMI_ERROR_INVALIDARGUMENT);
  EXPECT_EQ(QDMI_job_cancel(nullptr), QDMI_ERROR_INVALIDARGUMENT);
  EXPECT_EQ(QDMI_job_set_parameter(job, QDMI_JOB_PARAMETER_SHOTSNUM,
                                   sizeof(size_t), &shots),
            QDMI_ERROR_BADSTATE);
  QDMI_job_free(job);
}

TEST_P(QDMIImplementationTest, ToolCompile) {
  Tool tool(device);
  const auto fomac = FoMaC(device);
  const auto num_qubits = fomac.get_qubits_num();

  const std::string input = "OPENQASM 2.0;\n"
                            "include \"qelib1.inc\";\n"
                            "qreg q[2];\n"
                            "h q[0];\n"
                            "cx q[0], q[1];\n";
  const std::string expected = "OPENQASM 2.0;\n"
                               "include \"qelib1.inc\";\n"
                               "qreg q[" +
                               std::to_string(num_qubits) +
                               "];\n"
                               "h q[0];\n"
                               "cx q[0], q[1];\n";
  const std::string actual = tool.compile(input);
  ASSERT_EQ(actual, expected);
}

namespace {
QDMI_Job Submit_test_job(QDMI_Device dev, const size_t num_shots = 0) {
  static const std::string TEST_CIRCUIT = R"(
OPENQASM 2.0;
include "qelib1.inc";
qreg q[2];
creg c[2];
h q[0];
cx q[0], q[1];
measure q -> c;
  )";
  QDMI_Job job = nullptr;
  EXPECT_EQ(QDMI_device_create_job(dev, &job), QDMI_SUCCESS);
  const auto format = QDMI_PROGRAM_FORMAT_QASM2;
  EXPECT_EQ(QDMI_job_set_parameter(job, QDMI_JOB_PARAMETER_PROGRAMFORMAT,
                                   sizeof(QDMI_Program_Format), &format),
            QDMI_SUCCESS);
  EXPECT_EQ(QDMI_job_set_parameter(job, QDMI_JOB_PARAMETER_PROGRAM,
                                   TEST_CIRCUIT.size() + 1,
                                   TEST_CIRCUIT.c_str()),
            QDMI_SUCCESS);
  if (num_shots > 0) {
    EXPECT_EQ(QDMI_job_set_parameter(job, QDMI_JOB_PARAMETER_SHOTSNUM,
                                     sizeof(size_t), &num_shots),
              QDMI_SUCCESS);
  }
  EXPECT_EQ(QDMI_job_submit(job), QDMI_SUCCESS);
  EXPECT_EQ(QDMI_job_wait(job, 0), QDMI_SUCCESS);
  return job;
}
} // namespace

TEST_P(QDMIImplementationTest, GetResultsCornerCases) {
  if (mode == TEST_SESSION_MODE::READONLY) {
    GTEST_SKIP() << "Skipping test for read-only session";
  }
  QDMI_Job job = Submit_test_job(device);

  // The MAX parameter is not a valid value for any device
  EXPECT_EQ(QDMI_job_get_results(job, QDMI_JOB_RESULT_MAX, 0, nullptr, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);

  // The example devices do not support custom results
  EXPECT_EQ(
      QDMI_job_get_results(job, QDMI_JOB_RESULT_CUSTOM1, 0, nullptr, nullptr),
      QDMI_ERROR_NOTSUPPORTED);
  EXPECT_EQ(
      QDMI_job_get_results(job, QDMI_JOB_RESULT_CUSTOM2, 0, nullptr, nullptr),
      QDMI_ERROR_NOTSUPPORTED);
  EXPECT_EQ(
      QDMI_job_get_results(job, QDMI_JOB_RESULT_CUSTOM3, 0, nullptr, nullptr),
      QDMI_ERROR_NOTSUPPORTED);
  EXPECT_EQ(
      QDMI_job_get_results(job, QDMI_JOB_RESULT_CUSTOM4, 0, nullptr, nullptr),
      QDMI_ERROR_NOTSUPPORTED);
  EXPECT_EQ(
      QDMI_job_get_results(job, QDMI_JOB_RESULT_CUSTOM5, 0, nullptr, nullptr),
      QDMI_ERROR_NOTSUPPORTED);
}

TEST_P(QDMIImplementationTest, GetShots) {
  if (mode == TEST_SESSION_MODE::READONLY) {
    GTEST_SKIP() << "Skipping test for read-only session";
  }
  const auto fomac = FoMaC(device);
  const size_t shots_num = 64;
  QDMI_Job job = Submit_test_job(device, shots_num);
  size_t size = 0;
  ASSERT_EQ(QDMI_job_get_results(job, QDMI_JOB_RESULT_SHOTS, 0, nullptr, &size),
            QDMI_SUCCESS);
  std::string shots(static_cast<std::size_t>(size - 1), '\0');
  ASSERT_EQ(QDMI_job_get_results(job, QDMI_JOB_RESULT_SHOTS, size, shots.data(),
                                 nullptr),
            QDMI_SUCCESS);
  std::vector<std::string> shots_vec;
  std::string token;
  std::stringstream ss(shots);
  while (std::getline(ss, token, ',')) {
    shots_vec.emplace_back(token);
    ASSERT_EQ(token.size(), fomac.get_qubits_num());
  }
  ASSERT_EQ(shots_vec.size(), shots_num);
  QDMI_job_free(job);
}

TEST_P(QDMIImplementationTest, GetHistogram) {
  if (mode == TEST_SESSION_MODE::READONLY) {
    GTEST_SKIP() << "Skipping test for read-only session";
  }
  const auto fomac = FoMaC(device);
  const size_t shots_num = 64;
  QDMI_Job job = Submit_test_job(device, shots_num);

  size_t size = 0;
  ASSERT_EQ(
      QDMI_job_get_results(job, QDMI_JOB_RESULT_HIST_KEYS, 0, nullptr, &size),
      QDMI_SUCCESS);
  std::string key_list(size - 1, '\0');
  ASSERT_EQ(QDMI_job_get_results(job, QDMI_JOB_RESULT_HIST_KEYS, size,
                                 key_list.data(), nullptr),
            QDMI_SUCCESS);
  std::vector<std::string> key_vec;
  std::string token;
  std::stringstream ss(key_list);
  while (std::getline(ss, token, ',')) {
    ASSERT_EQ(token.size(), fomac.get_qubits_num());
    key_vec.emplace_back(token);
  }

  // keys should be sorted
  for (size_t i = 1; i < key_vec.size(); ++i) {
    ASSERT_LT(key_vec[i - 1], key_vec[i]);
  }

  size_t val_size = 0;
  ASSERT_EQ(QDMI_job_get_results(job, QDMI_JOB_RESULT_HIST_VALUES, 0, nullptr,
                                 &val_size),
            QDMI_SUCCESS);
  ASSERT_EQ(val_size / sizeof(size_t), key_vec.size());

  std::vector<size_t> val_vec(key_vec.size());
  ASSERT_EQ(QDMI_job_get_results(job, QDMI_JOB_RESULT_HIST_VALUES, val_size,
                                 val_vec.data(), nullptr),
            QDMI_SUCCESS);

  size_t sum = 0;
  for (const auto &val : val_vec) {
    sum += val;
  }
  ASSERT_EQ(sum, shots_num);

  std::unordered_map<std::string, size_t> results;
  for (size_t i = 0; i < key_vec.size(); ++i) {
    results[key_vec[i]] = val_vec[i];
  }
  ASSERT_EQ(results.size(), key_vec.size());

  QDMI_job_free(job);
}

TEST_P(QDMIImplementationTest, GetStateDense) {
  if (mode == TEST_SESSION_MODE::READONLY) {
    GTEST_SKIP() << "Skipping test for read-only session";
  }
  QDMI_Job job = Submit_test_job(device);

  size_t state_size = 0;
  ASSERT_EQ(QDMI_job_get_results(job, QDMI_JOB_RESULT_STATEVECTOR_DENSE, 0,
                                 nullptr, &state_size),
            QDMI_SUCCESS);
  const size_t vec_length = state_size / sizeof(double);
  ASSERT_EQ(vec_length % 2, 0) << "State vector must contain pairs of values";

  std::vector<double> state_vector(vec_length);
  ASSERT_EQ(QDMI_job_get_results(job, QDMI_JOB_RESULT_STATEVECTOR_DENSE,
                                 state_size, state_vector.data(), nullptr),
            QDMI_SUCCESS);

  std::vector<std::complex<double>> complex_state_vector;
  complex_state_vector.reserve(vec_length / 2);
  for (size_t i = 0; i < state_vector.size(); i += 2) {
    complex_state_vector.emplace_back(state_vector[i], state_vector[i + 1]);
  }

  // assert that the complex vector is normalized up to a certain tolerance
  double norm = 0;
  for (const auto &val : complex_state_vector) {
    norm += std::norm(val);
  }
  ASSERT_NEAR(norm, 1, 1e-6);

  QDMI_job_free(job);
}

TEST_P(QDMIImplementationTest, GetStateSparse) {
  if (mode == TEST_SESSION_MODE::READONLY) {
    GTEST_SKIP() << "Skipping test for read-only session";
  }
  const auto fomac = FoMaC(device);
  QDMI_Job job = Submit_test_job(device);
  size_t size = 0;
  ASSERT_EQ(QDMI_job_get_results(job, QDMI_JOB_RESULT_STATEVECTOR_SPARSE_KEYS,
                                 0, nullptr, &size),
            QDMI_SUCCESS);
  std::string key_list(size - 1, '\0');
  ASSERT_EQ(QDMI_job_get_results(job, QDMI_JOB_RESULT_STATEVECTOR_SPARSE_KEYS,
                                 size, key_list.data(), nullptr),
            QDMI_SUCCESS);
  std::vector<std::string> key_vec;
  std::string token;
  std::stringstream ss(key_list);
  while (std::getline(ss, token, ',')) {
    ASSERT_EQ(token.size(), fomac.get_qubits_num());
    key_vec.emplace_back(token);
  }

  // keys should be sorted
  for (size_t i = 1; i < key_vec.size(); ++i) {
    ASSERT_LT(key_vec[i - 1], key_vec[i]);
  }

  size_t val_size = 0;
  ASSERT_EQ(QDMI_job_get_results(job, QDMI_JOB_RESULT_STATEVECTOR_SPARSE_VALUES,
                                 0, nullptr, &val_size),
            QDMI_SUCCESS);
  ASSERT_EQ(val_size / 2 / sizeof(double), key_vec.size());

  std::vector<std::complex<double>> val_vec(key_vec.size());
  ASSERT_EQ(QDMI_job_get_results(job, QDMI_JOB_RESULT_STATEVECTOR_SPARSE_VALUES,
                                 val_size, val_vec.data(), nullptr),
            QDMI_SUCCESS);

  double norm = 0;
  for (const auto &val : val_vec) {
    norm += std::norm(val);
  }
  ASSERT_NEAR(norm, 1, 1e-6);

  QDMI_job_free(job);
}

TEST_P(QDMIImplementationTest, GetProbsDense) {
  if (mode == TEST_SESSION_MODE::READONLY) {
    GTEST_SKIP() << "Skipping test for read-only session";
  }
  const auto fomac = FoMaC(device);
  QDMI_Job job = Submit_test_job(device);

  std::vector<double> prob_vector(1ULL << fomac.get_qubits_num());
  ASSERT_EQ(QDMI_job_get_results(job, QDMI_JOB_RESULT_PROBABILITIES_DENSE,
                                 sizeof(double) * prob_vector.size(),
                                 prob_vector.data(), nullptr),
            QDMI_SUCCESS);

  double sum = 0;
  for (const auto &prob : prob_vector) {
    sum += prob;
  }
  ASSERT_NEAR(sum, 1.0, 1e-6);

  QDMI_job_free(job);
}

TEST_P(QDMIImplementationTest, GetProbsSparse) {
  if (mode == TEST_SESSION_MODE::READONLY) {
    GTEST_SKIP() << "Skipping test for read-only session";
  }
  const auto fomac = FoMaC(device);
  QDMI_Job job = Submit_test_job(device);

  size_t size = 0;
  ASSERT_EQ(QDMI_job_get_results(job, QDMI_JOB_RESULT_PROBABILITIES_SPARSE_KEYS,
                                 0, nullptr, &size),
            QDMI_SUCCESS);
  std::string key_list(size - 1, '\0');
  ASSERT_EQ(QDMI_job_get_results(job, QDMI_JOB_RESULT_PROBABILITIES_SPARSE_KEYS,
                                 size, key_list.data(), nullptr),
            QDMI_SUCCESS);
  std::vector<std::string> key_vec;
  std::string token;
  std::stringstream ss(key_list);
  while (std::getline(ss, token, ',')) {
    ASSERT_EQ(token.size(), fomac.get_qubits_num());
    key_vec.emplace_back(token);
  }

  // keys should be sorted
  for (size_t i = 1; i < key_vec.size(); ++i) {
    ASSERT_LT(key_vec[i - 1], key_vec[i]);
  }

  size_t val_size = 0;
  ASSERT_EQ(QDMI_job_get_results(job,
                                 QDMI_JOB_RESULT_PROBABILITIES_SPARSE_VALUES, 0,
                                 nullptr, &val_size),
            QDMI_SUCCESS);
  ASSERT_EQ(val_size / sizeof(double), key_vec.size());

  std::vector<double> val_vec(key_vec.size());
  ASSERT_EQ(QDMI_job_get_results(job,
                                 QDMI_JOB_RESULT_PROBABILITIES_SPARSE_VALUES,
                                 val_size, val_vec.data(), nullptr),
            QDMI_SUCCESS);

  double sum = 0;
  for (const auto &val : val_vec) {
    sum += val;
  }
  ASSERT_NEAR(sum, 1.0, 1e-6);

  QDMI_job_free(job);
}

TEST_P(QDMIImplementationTest, GetShotsBufferTooSmall) {
  if (mode == TEST_SESSION_MODE::READONLY) {
    GTEST_SKIP() << "Skipping test for read-only session";
  }
  QDMI_Job job = Submit_test_job(device, 64);
  size_t size = 0;
  ASSERT_EQ(QDMI_job_get_results(job, QDMI_JOB_RESULT_SHOTS, 0, nullptr, &size),
            QDMI_SUCCESS);
  std::vector<char> buffer(size - 1); // Buffer too small
  EXPECT_EQ(QDMI_job_get_results(job, QDMI_JOB_RESULT_SHOTS, buffer.size(),
                                 buffer.data(), nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
  QDMI_job_free(job);
}

TEST_P(QDMIImplementationTest, GetHistogramKeysBufferTooSmall) {
  if (mode == TEST_SESSION_MODE::READONLY) {
    GTEST_SKIP() << "Skipping test for read-only session";
  }
  QDMI_Job job = Submit_test_job(device, 64);
  size_t size = 0;
  ASSERT_EQ(
      QDMI_job_get_results(job, QDMI_JOB_RESULT_HIST_KEYS, 0, nullptr, &size),
      QDMI_SUCCESS);
  std::vector<char> buffer(size - 1); // Buffer too small
  EXPECT_EQ(QDMI_job_get_results(job, QDMI_JOB_RESULT_HIST_KEYS, buffer.size(),
                                 buffer.data(), nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
  QDMI_job_free(job);
}

TEST_P(QDMIImplementationTest, GetHistogramValuesBufferTooSmall) {
  if (mode == TEST_SESSION_MODE::READONLY) {
    GTEST_SKIP() << "Skipping test for read-only session";
  }
  QDMI_Job job = Submit_test_job(device, 64);
  size_t size = 0;
  ASSERT_EQ(
      QDMI_job_get_results(job, QDMI_JOB_RESULT_HIST_VALUES, 0, nullptr, &size),
      QDMI_SUCCESS);
  std::vector<char> buffer(size - 1); // Buffer too small
  EXPECT_EQ(QDMI_job_get_results(job, QDMI_JOB_RESULT_HIST_VALUES,
                                 buffer.size(), buffer.data(), nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
  QDMI_job_free(job);
}

TEST_P(QDMIImplementationTest, GetStateDenseBufferTooSmall) {
  if (mode == TEST_SESSION_MODE::READONLY) {
    GTEST_SKIP() << "Skipping test for read-only session";
  }
  QDMI_Job job = Submit_test_job(device);
  size_t size = 0;
  ASSERT_EQ(QDMI_job_get_results(job, QDMI_JOB_RESULT_STATEVECTOR_DENSE, 0,
                                 nullptr, &size),
            QDMI_SUCCESS);
  std::vector<char> buffer(size - 1); // Buffer too small
  EXPECT_EQ(QDMI_job_get_results(job, QDMI_JOB_RESULT_STATEVECTOR_DENSE,
                                 buffer.size(), buffer.data(), nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
  QDMI_job_free(job);
}

TEST_P(QDMIImplementationTest, GetStateSparseKeysBufferTooSmall) {
  if (mode == TEST_SESSION_MODE::READONLY) {
    GTEST_SKIP() << "Skipping test for read-only session";
  }
  QDMI_Job job = Submit_test_job(device);
  size_t size = 0;
  ASSERT_EQ(QDMI_job_get_results(job, QDMI_JOB_RESULT_STATEVECTOR_SPARSE_KEYS,
                                 0, nullptr, &size),
            QDMI_SUCCESS);
  std::vector<char> buffer(size - 1); // Buffer too small
  EXPECT_EQ(QDMI_job_get_results(job, QDMI_JOB_RESULT_STATEVECTOR_SPARSE_KEYS,
                                 buffer.size(), buffer.data(), nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
  QDMI_job_free(job);
}

TEST_P(QDMIImplementationTest, GetStateSparseValuesBufferTooSmall) {
  if (mode == TEST_SESSION_MODE::READONLY) {
    GTEST_SKIP() << "Skipping test for read-only session";
  }
  QDMI_Job job = Submit_test_job(device);
  size_t size = 0;
  ASSERT_EQ(QDMI_job_get_results(job, QDMI_JOB_RESULT_STATEVECTOR_SPARSE_VALUES,
                                 0, nullptr, &size),
            QDMI_SUCCESS);
  std::vector<char> buffer(size - 1); // Buffer too small
  EXPECT_EQ(QDMI_job_get_results(job, QDMI_JOB_RESULT_STATEVECTOR_SPARSE_VALUES,
                                 buffer.size(), buffer.data(), nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
  QDMI_job_free(job);
}

TEST_P(QDMIImplementationTest, GetProbsDenseBufferTooSmall) {
  if (mode == TEST_SESSION_MODE::READONLY) {
    GTEST_SKIP() << "Skipping test for read-only session";
  }
  QDMI_Job job = Submit_test_job(device);
  size_t size = 0;
  ASSERT_EQ(QDMI_job_get_results(job, QDMI_JOB_RESULT_PROBABILITIES_DENSE, 0,
                                 nullptr, &size),
            QDMI_SUCCESS);
  std::vector<char> buffer(size - 1); // Buffer too small
  EXPECT_EQ(QDMI_job_get_results(job, QDMI_JOB_RESULT_PROBABILITIES_DENSE,
                                 buffer.size(), buffer.data(), nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
  QDMI_job_free(job);
}

TEST_P(QDMIImplementationTest, GetProbsSparseKeysBufferTooSmall) {
  if (mode == TEST_SESSION_MODE::READONLY) {
    GTEST_SKIP() << "Skipping test for read-only session";
  }
  QDMI_Job job = Submit_test_job(device);
  size_t size = 0;
  ASSERT_EQ(QDMI_job_get_results(job, QDMI_JOB_RESULT_PROBABILITIES_SPARSE_KEYS,
                                 0, nullptr, &size),
            QDMI_SUCCESS);
  std::vector<char> buffer(size - 1); // Buffer too small
  EXPECT_EQ(QDMI_job_get_results(job, QDMI_JOB_RESULT_PROBABILITIES_SPARSE_KEYS,
                                 buffer.size(), buffer.data(), nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
  QDMI_job_free(job);
}

TEST_P(QDMIImplementationTest, GetProbsSparseValuesBufferTooSmall) {
  if (mode == TEST_SESSION_MODE::READONLY) {
    GTEST_SKIP() << "Skipping test for read-only session";
  }
  QDMI_Job job = Submit_test_job(device);
  size_t size = 0;
  ASSERT_EQ(QDMI_job_get_results(job,
                                 QDMI_JOB_RESULT_PROBABILITIES_SPARSE_VALUES, 0,
                                 nullptr, &size),
            QDMI_SUCCESS);
  std::vector<char> buffer(size - 1); // Buffer too small
  EXPECT_EQ(QDMI_job_get_results(job,
                                 QDMI_JOB_RESULT_PROBABILITIES_SPARSE_VALUES,
                                 buffer.size(), buffer.data(), nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
  QDMI_job_free(job);
}

TEST_P(QDMIImplementationTest, SessionSetParameter) {
  const std::string test_token = "test_token";
  EXPECT_EQ(QDMI_session_set_parameter(session, QDMI_SESSION_PARAMETER_TOKEN,
                                       test_token.length() + 1,
                                       test_token.c_str()),
            QDMI_ERROR_BADSTATE);

  QDMI_Session session2 = nullptr;
  ASSERT_EQ(QDMI_session_alloc(&session2), QDMI_SUCCESS);
  EXPECT_EQ(QDMI_session_set_parameter(session2,
                                       QDMI_SESSION_PARAMETER_USERNAME, 1, ""),
            QDMI_ERROR_NOTSUPPORTED);
  QDMI_session_free(session2);
}

TEST_P(QDMIImplementationTest, SessionInit) {
  // `session == nullptr` is not a valid argument
  EXPECT_EQ(QDMI_session_init(nullptr), QDMI_ERROR_INVALIDARGUMENT);

  // Initializing a session again is not allowed
  ASSERT_EQ(QDMI_session_init(session), QDMI_ERROR_BADSTATE);

  // Driver requires token for initialization
  QDMI_Session session2 = nullptr;
  ASSERT_EQ(QDMI_session_alloc(&session2), QDMI_SUCCESS);
  EXPECT_EQ(QDMI_session_init(session2), QDMI_ERROR_PERMISSIONDENIED);

  const std::string test_token = "test_token";
  EXPECT_EQ(QDMI_session_set_parameter(session2, QDMI_SESSION_PARAMETER_TOKEN,
                                       test_token.length() + 1,
                                       test_token.c_str()),
            QDMI_SUCCESS);
  EXPECT_EQ(QDMI_session_init(session2), QDMI_SUCCESS);
}

TEST_P(QDMIImplementationTest, SessionQuerySessionProperty) {
  // `session == nullptr` is not a valid argument
  EXPECT_EQ(QDMI_session_query_session_property(
                nullptr, QDMI_SESSION_PROPERTY_DEVICES, 0, nullptr, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);

  // `prop >= QDMI_SESSION_PROPERTY_MAX` is not a valid argument
  EXPECT_EQ(QDMI_session_query_session_property(
                session, QDMI_SESSION_PROPERTY_MAX, 0, nullptr, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);

  // The example driver does not support custom properties
  EXPECT_EQ(QDMI_session_query_session_property(
                session, QDMI_SESSION_PROPERTY_CUSTOM1, 0, nullptr, nullptr),
            QDMI_ERROR_NOTSUPPORTED);
  EXPECT_EQ(QDMI_session_query_session_property(
                session, QDMI_SESSION_PROPERTY_CUSTOM2, 0, nullptr, nullptr),
            QDMI_ERROR_NOTSUPPORTED);
  EXPECT_EQ(QDMI_session_query_session_property(
                session, QDMI_SESSION_PROPERTY_CUSTOM3, 0, nullptr, nullptr),
            QDMI_ERROR_NOTSUPPORTED);
  EXPECT_EQ(QDMI_session_query_session_property(
                session, QDMI_SESSION_PROPERTY_CUSTOM4, 0, nullptr, nullptr),
            QDMI_ERROR_NOTSUPPORTED);
  EXPECT_EQ(QDMI_session_query_session_property(
                session, QDMI_SESSION_PROPERTY_CUSTOM5, 0, nullptr, nullptr),
            QDMI_ERROR_NOTSUPPORTED);

  // Must not query on an uninitialized session
  QDMI_Session session2 = nullptr;
  ASSERT_EQ(QDMI_session_alloc(&session2), QDMI_SUCCESS);
  EXPECT_EQ(QDMI_session_query_session_property(
                session2, QDMI_SESSION_PROPERTY_DEVICES, 0, nullptr, nullptr),
            QDMI_ERROR_BADSTATE);

  // Buffer too small
  constexpr size_t size = sizeof(QDMI_Device) - 1;
  std::array<char, size> devices{};
  EXPECT_EQ(QDMI_session_query_session_property(
                session, QDMI_SESSION_PROPERTY_DEVICES, size,
                static_cast<void *>(devices.data()), nullptr),
            QDMI_ERROR_INVALIDARGUMENT);

  // Successful query
  size_t devices_size = 0;
  EXPECT_EQ(QDMI_session_query_session_property(session,
                                                QDMI_SESSION_PROPERTY_DEVICES,
                                                0, nullptr, &devices_size),
            QDMI_SUCCESS);
  std::vector<QDMI_Device> devices_vec(devices_size / sizeof(QDMI_Device));
  EXPECT_EQ(QDMI_session_query_session_property(
                session, QDMI_SESSION_PROPERTY_DEVICES, devices_size,
                static_cast<void *>(devices_vec.data()), nullptr),
            QDMI_SUCCESS);
}

TEST_P(QDMIImplementationTest, OpenJob) {
  QDMI_Job opened_job = nullptr;
  EXPECT_EQ(QDMI_device_open_job(nullptr, "job-id", &opened_job),
            QDMI_ERROR_INVALIDARGUMENT);
  EXPECT_EQ(QDMI_device_open_job(device, nullptr, &opened_job),
            QDMI_ERROR_INVALIDARGUMENT);
  EXPECT_EQ(QDMI_device_open_job(device, "", &opened_job),
            QDMI_ERROR_INVALIDARGUMENT);
  EXPECT_EQ(QDMI_device_open_job(device, "job-id", nullptr),
            QDMI_ERROR_INVALIDARGUMENT);

  if (mode == TEST_SESSION_MODE::READONLY) {
    EXPECT_EQ(QDMI_device_open_job(device, "job-id", &opened_job),
              QDMI_ERROR_PERMISSIONDENIED);
    return;
  }

  EXPECT_EQ(QDMI_device_open_job(device, "unknown", &opened_job),
            QDMI_ERROR_NOTFOUND);

  QDMI_Job job = nullptr;
  ASSERT_EQ(QDMI_device_create_job(device, &job), QDMI_SUCCESS);
  const QDMI_Program_Format format = QDMI_PROGRAM_FORMAT_QASM2;
  ASSERT_EQ(QDMI_job_set_parameter(job, QDMI_JOB_PARAMETER_PROGRAMFORMAT,
                                   sizeof(format), &format),
            QDMI_SUCCESS);
  const size_t shots = 2;
  ASSERT_EQ(QDMI_job_set_parameter(job, QDMI_JOB_PARAMETER_SHOTSNUM,
                                   sizeof(shots), &shots),
            QDMI_SUCCESS);
  ASSERT_EQ(QDMI_job_submit(job), QDMI_SUCCESS);

  size_t id_size = 0;
  ASSERT_EQ(
      QDMI_job_query_property(job, QDMI_JOB_PROPERTY_ID, 0, nullptr, &id_size),
      QDMI_SUCCESS);
  ASSERT_GT(id_size, 1);
  std::string id(id_size, '\0');
  ASSERT_EQ(QDMI_job_query_property(job, QDMI_JOB_PROPERTY_ID, id.size(),
                                    id.data(), nullptr),
            QDMI_SUCCESS);
  QDMI_job_free(job);

  ASSERT_EQ(QDMI_device_open_job(device, id.c_str(), &opened_job),
            QDMI_SUCCESS);
  EXPECT_EQ(QDMI_job_set_parameter(opened_job, QDMI_JOB_PARAMETER_SHOTSNUM,
                                   sizeof(shots), &shots),
            QDMI_ERROR_BADSTATE);
  EXPECT_EQ(QDMI_job_submit(opened_job), QDMI_ERROR_BADSTATE);
  ASSERT_EQ(QDMI_job_wait(opened_job, 0), QDMI_SUCCESS);

  QDMI_Job_Status status = QDMI_JOB_STATUS_RUNNING;
  ASSERT_EQ(QDMI_job_check(opened_job, &status), QDMI_SUCCESS);
  EXPECT_EQ(status, QDMI_JOB_STATUS_DONE);

  size_t result_size = 0;
  EXPECT_EQ(QDMI_job_get_results(opened_job, QDMI_JOB_RESULT_SHOTS, 0, nullptr,
                                 &result_size),
            QDMI_SUCCESS);
  EXPECT_GT(result_size, 0);
  QDMI_job_free(opened_job);
}

TEST_P(QDMIImplementationTest, SupportsCalibration) {
  if (mode == TEST_SESSION_MODE::READONLY) {
    GTEST_SKIP() << "Skipping test for read-only session";
  }
  QDMI_Job job = nullptr;
  QDMI_Program_Format format = QDMI_PROGRAM_FORMAT_CALIBRATION;
  QDMI_device_create_job(device, &job);
  const auto ret = QDMI_job_set_parameter(job, QDMI_JOB_PARAMETER_PROGRAMFORMAT,
                                          sizeof(QDMI_Program_Format), &format);
  EXPECT_EQ(ret, QDMI_SUCCESS);
  EXPECT_EQ(QDMI_job_submit(job), QDMI_SUCCESS);
}

TEST_P(QDMIImplementationTest, NeedsCalibration) {
  size_t needs_calibration = 0;
  const auto ret = QDMI_device_query_device_property(
      device, QDMI_DEVICE_PROPERTY_NEEDSCALIBRATION, sizeof(size_t),
      &needs_calibration, nullptr);
  EXPECT_EQ(ret, QDMI_SUCCESS);
  EXPECT_EQ(needs_calibration, 0);
}

TEST_P(QDMIImplementationTest, QueryPulseSupportLevel) {
  QDMI_Device_Pulse_Support_Level pulse_support_level =
      QDMI_DEVICE_PULSE_SUPPORT_LEVEL_NONE;
  const auto ret = QDMI_device_query_device_property(
      device, QDMI_DEVICE_PROPERTY_PULSESUPPORT,
      sizeof(QDMI_Device_Pulse_Support_Level), &pulse_support_level, nullptr);
  EXPECT_EQ(ret, QDMI_SUCCESS);
  EXPECT_EQ(pulse_support_level, QDMI_DEVICE_PULSE_SUPPORT_LEVEL_NONE);
}

// Standalone tests for driver library loading corner cases
TEST(QDMIDriverLoadingTest, LoadConfigWithNonExistentFile) {
  // Save the original QDMI_CONF environment variable
  const char *original_conf = std::getenv("QDMI_CONF");
  const std::string saved_conf =
      (original_conf != nullptr) ? original_conf : "";

#ifdef _WIN32
  _putenv_s("QDMI_CONF", "/nonexistent/path/to/qdmi.conf");
#else
  setenv("QDMI_CONF", "/nonexistent/path/to/qdmi.conf", 1);
#endif

  // Driver initialization should fail because the config file doesn't exist
  const auto init_result = QDMI_driver_init();

  // Clean up before assertions
  QDMI_driver_shutdown();

  // Restore the original QDMI_CONF environment variable
#ifdef _WIN32
  _putenv_s("QDMI_CONF", saved_conf.c_str());
#else
  setenv("QDMI_CONF", saved_conf.c_str(), 1);
#endif

  // Now perform the assertion after cleanup
  EXPECT_NE(init_result, QDMI_SUCCESS)
      << "Driver should fail to initialize with non-existent config file";
}

TEST(QDMIDriverLoadingTest, LoadLibraryWithNonExistentPath) {
  // Save the original QDMI_CONF environment variable
  const char *original_conf = std::getenv("QDMI_CONF");
  const std::string saved_conf =
      (original_conf != nullptr) ? original_conf : "";

  // Create a config file pointing to a non-existent library path
  const std::string config_file_name = "qdmi_nonexistent_library.conf";
  std::ofstream conf_file(config_file_name);
  conf_file << "/nonexistent/path/to/library" << Shared_library_file_extension()
            << " CXX\n";
  conf_file.close();

#ifdef _WIN32
  _putenv_s("QDMI_CONF", config_file_name.c_str());
#else
  setenv("QDMI_CONF", config_file_name.c_str(), 1);
#endif

  // Driver initialization should fail because the library path doesn't exist
  // The Is_path_allowed function should return false when the path cannot be
  // canonicalized
  const auto init_result = QDMI_driver_init();

  // Clean up before assertions
  QDMI_driver_shutdown();
  std::filesystem::remove(config_file_name);

  // Restore the original QDMI_CONF environment variable
#ifdef _WIN32
  if (!saved_conf.empty()) {
    _putenv_s("QDMI_CONF", saved_conf.c_str());
  } else {
    _putenv_s("QDMI_CONF", "");
  }
#else
  if (!saved_conf.empty()) {
    setenv("QDMI_CONF", saved_conf.c_str(), 1);
  } else {
    unsetenv("QDMI_CONF");
  }
#endif

  // Now perform the assertion after cleanup
  EXPECT_NE(init_result, QDMI_SUCCESS)
      << "Driver should fail to initialize with non-existent library path";
}

TEST(QDMIDriverLoadingTest, LoadLibraryWithInvalidHomeEnv) {
  // Save the original HOME environment variable
  const char *original_home = std::getenv("HOME");
  const std::string saved_home =
      (original_home != nullptr) ? original_home : "";

  // Set HOME to a non-existent path
#ifdef _WIN32
  _putenv_s("HOME", "/nonexistent/home/directory");
#else
  setenv("HOME", "/nonexistent/home/directory", 1);
#endif

  // Create a valid config file pointing to the example device in the current
  // directory (test runs from build directory, library is in examples/device/)
  const std::string config_file_name = "qdmi_invalid_home.conf";
  std::ofstream conf_file(config_file_name);
  conf_file << "../examples/device/src/libcxx-qdmi-device"
            << Shared_library_file_extension() << " CXX\n";
  conf_file.close();

#ifdef _WIN32
  _putenv_s("QDMI_CONF", config_file_name.c_str());
#else
  setenv("QDMI_CONF", config_file_name.c_str(), 1);
#endif

  // Driver initialization should succeed because the library is in an allowed
  // path (current directory) even though HOME is invalid. The Is_path_allowed
  // function should catch the exception when canonicalizing HOME and skip it.
  const auto init_result = QDMI_driver_init();

  // Clean up and restore the environment BEFORE any assertions
  QDMI_driver_shutdown();
  std::filesystem::remove(config_file_name);

  // Restore the original HOME environment variable
#ifdef _WIN32
  if (!saved_home.empty()) {
    _putenv_s("HOME", saved_home.c_str());
  }
#else
  if (!saved_home.empty()) {
    setenv("HOME", saved_home.c_str(), 1);
  } else {
    unsetenv("HOME");
  }
#endif

  // Now perform the assertion after cleanup
  EXPECT_EQ(init_result, QDMI_SUCCESS)
      << "Driver should initialize successfully even with invalid HOME "
         "environment variable";
}
