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

#include "example_fomac.hpp"
#include "example_tool.hpp"
#include "qdmi/client.h"
#include "utils/test_impl.hpp"

#include <array>
#include <complex>
#include <cstddef>
#include <ctime>
#include <gtest/gtest.h>
#include <random>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

// Instantiate the test suite with different parameters
INSTANTIATE_TEST_SUITE_P(
    QDMIDevice,
    // Custom instantiation name
    QDMIImplementationTest,
    // Test suite name
    // Parameters to test with
    ::testing::Values(std::tuple{"../examples/device/c/libc_device", "C",
                                 TEST_SESSION_MODE::READONLY},
                      std::tuple{"../examples/device/c/libc_device", "C",
                                 TEST_SESSION_MODE::READWRITE},
                      std::tuple{"../examples/device/cxx/libcxx_device", "CXX",
                                 TEST_SESSION_MODE::READONLY},
                      std::tuple{"../examples/device/cxx/libcxx_device", "CXX",
                                 TEST_SESSION_MODE::READWRITE}),
    [](const testing::TestParamInfo<
        std::tuple<std::string, std::string, TEST_SESSION_MODE>> &inf) {
      // Extract the last part of the file path
      const size_t pos = std::get<0>(inf.param).find_last_of("/\\");
      std::string filename = (pos == std::string::npos)
                                 ? std::get<0>(inf.param)
                                 : std::get<0>(inf.param).substr(pos + 1);

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

    double duration = 0;
    double fidelity = 0;
    if (gate_num_qubits == 1) {
      for (const auto &site : sites) {
        auto site_arr = std::array{site};
        EXPECT_EQ(QDMI_device_query_operation_property(
                      device, op, gate_num_qubits, site_arr.data(),
                      gate_num_params, params.data(),
                      QDMI_OPERATION_PROPERTY_DURATION, sizeof(double),
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
      for (const auto &[control, target] : coupling_map) {
        auto site_arr = std::array{control, target};
        EXPECT_EQ(QDMI_device_query_operation_property(
                      device, op, gate_num_qubits, site_arr.data(),
                      gate_num_params, params.data(),
                      QDMI_OPERATION_PROPERTY_DURATION, sizeof(double),
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
  constexpr std::array unsupported_formats = {
      QDMI_PROGRAM_FORMAT_QASM3,
      QDMI_PROGRAM_FORMAT_QIRADAPTIVESTRING,
      QDMI_PROGRAM_FORMAT_QIRADAPTIVEMODULE,
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
  EXPECT_EQ(QDMI_job_submit(job), QDMI_ERROR_INVALIDARGUMENT);
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

TEST_P(QDMIImplementationTest, QueryEveryEnvironmentProperties) {

  const auto fomac = FoMaC(device);

  auto environments = fomac.get_environment_variables();

  ASSERT_GT(environments.size(), 0);

  for (const auto &environment : environments) {

    auto environment_id = fomac.get_environment_id(environment);
    EXPECT_STRNE(environment_id.c_str(), "");

    auto environment_unit = fomac.get_environment_unit(environment);
    EXPECT_STRNE(environment_unit.c_str(), "");

    auto sampling_rate = fomac.get_environment_sampling_rate(environment);
    EXPECT_GT(sampling_rate, 0);
  }
}

TEST_P(QDMIImplementationTest, EnvironmentQuery) {
  if (mode == TEST_SESSION_MODE::READONLY) {
    GTEST_SKIP() << "Skipping test for read-only session";
  }

  const auto fomac = FoMaC(device);

  const std::vector<QDMI_EnvironmentSensor> environment_sensors =
      fomac.get_environment_variables();

  ASSERT_GT(environment_sensors.size(), 0);

  for (QDMI_EnvironmentSensor environment_sensor : environment_sensors) {
    QDMI_EnvironmentSensor_Query query = nullptr;
    QDMI_EnvironmentSensor_Query_Status status = {};
    time_t start_time = time(&start_time);
    time_t end_time = time(&end_time) + 600;

    EXPECT_EQ(QDMI_device_create_environmentsensor_query(device, &query),
              QDMI_SUCCESS);

    EXPECT_EQ(QDMI_environmentsensor_query_set_parameter(
                  query, QDMI_ENVIRONMENTSENSOR_QUERY_PARAMETER_ENVIRONMENT,
                  sizeof(QDMI_EnvironmentSensor), &*environment_sensor),
              QDMI_SUCCESS);

    EXPECT_EQ(QDMI_environmentsensor_query_set_parameter(
                  query, QDMI_ENVIRONMENTSENSOR_QUERY_PARAMETER_STARTTIME,
                  sizeof(time_t), &start_time),
              QDMI_SUCCESS);

    EXPECT_EQ(QDMI_environmentsensor_query_set_parameter(
                  query, QDMI_ENVIRONMENTSENSOR_QUERY_PARAMETER_ENDTIME,
                  sizeof(time_t), &end_time),
              QDMI_SUCCESS);

    EXPECT_EQ(QDMI_environmentsensor_query_submit(query), QDMI_SUCCESS);

    EXPECT_EQ(QDMI_environmentsensor_query_wait(query, 0), QDMI_SUCCESS);

    EXPECT_EQ(QDMI_environmentsensor_query_check_status(query, &status),
              QDMI_SUCCESS);

    size_t timestamps_size = 0;
    EXPECT_EQ(QDMI_environmentsensor_query_get_results(
                  query, QDMI_ENVIRONMENTSENSOR_QUERY_RESULT_TIMESTAMPS, 0,
                  nullptr, &timestamps_size),
              QDMI_SUCCESS);

    std::vector<time_t> timestamps;
    timestamps.reserve(timestamps_size);

    EXPECT_EQ(QDMI_environmentsensor_query_get_results(
                  query, QDMI_ENVIRONMENTSENSOR_QUERY_RESULT_TIMESTAMPS,
                  timestamps_size, timestamps.data(), nullptr),
              QDMI_SUCCESS);

    size_t size_values = 0;
    EXPECT_EQ(QDMI_environmentsensor_query_get_results(
                  query, QDMI_ENVIRONMENTSENSOR_QUERY_RESULT_VALUES, 0, nullptr,
                  &size_values),
              QDMI_SUCCESS);

    std::vector<float> values;
    values.reserve(size_values);

    EXPECT_EQ(QDMI_environmentsensor_query_get_results(
                  query, QDMI_ENVIRONMENTSENSOR_QUERY_RESULT_VALUES,
                  size_values, values.data(), nullptr),
              QDMI_SUCCESS);

    QDMI_environmentsensor_query_free(query);
  }
}
