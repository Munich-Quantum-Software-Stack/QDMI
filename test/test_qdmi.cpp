/*------------------------------------------------------------------------------
Part of the MQSS Project, under the Apache License v2.0 with LLVM Exceptions.
See https://llvm.org/LICENSE.txt for license information.
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
------------------------------------------------------------------------------*/

#include "example_fomac.hpp"
#include "example_tool.hpp"
#include "qdmi/client.h"
#include "utils/test_impl.hpp"

#include <array>
#include <cstdlib>
#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include <vector>

// Instantiate the test suite with different parameters
INSTANTIATE_TEST_SUITE_P(
    QDMIDevice,             // Custom instantiation name
    QDMIImplementationTest, // Test suite name
    // Parameters to test with
    ::testing::Values("../examples/device/c/libc_device",
                      "../examples/device/cxx/libcxx_device"),
    [](const testing::TestParamInfo<std::string> &inf) {
      // Extract the last part of the file path
      const size_t pos = inf.param.find_last_of("/\\");
      std::string filename =
          (pos == std::string::npos) ? inf.param : inf.param.substr(pos + 1);

      // Strip the 'lib' prefix if it exists
      const std::string prefix = "lib";
      if (filename.compare(0, prefix.size(), prefix) == 0) {
        filename = filename.substr(prefix.size());
      }

      return filename;
    });

TEST_P(QDMIImplementationTest, QueryNumQubits) {
  const auto fomac = FoMaC(device);
  ASSERT_GT(fomac.get_qubits_num(), 0);
}

TEST_P(QDMIImplementationTest, QueryGateSet) {
  const auto fomac = FoMaC(device);
  const auto gates = fomac.get_operation_map();
  ASSERT_GT(gates.size(), 0);
  for (const auto &[k, v] : gates) {
    ASSERT_FALSE(k.empty());
    std::string name(k.length(), '\0');
    ASSERT_EQ(QDMI_query_operation_property(
                  device, v, 0, nullptr, QDMI_OPERATION_PROPERTY_NAME,
                  static_cast<int>(name.length()) + 1, name.data(), nullptr),
              QDMI_SUCCESS);
    EXPECT_EQ(k, name);
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
    double duration = 0;
    double fidelity = 0;
    if (gate_num_qubits == 1) {
      for (const auto &site : sites) {
        auto site_arr = std::array{site};
        EXPECT_EQ(
            QDMI_query_operation_property(device, op, 1, site_arr.data(),
                                          QDMI_OPERATION_PROPERTY_DURATION,
                                          sizeof(double), &duration, nullptr),
            QDMI_SUCCESS)
            << "Failed to query duration for operation " << name;
        EXPECT_EQ(
            QDMI_query_operation_property(device, op, 1, site_arr.data(),
                                          QDMI_OPERATION_PROPERTY_FIDELITY,
                                          sizeof(double), &fidelity, nullptr),
            QDMI_SUCCESS)
            << "Failed to query fidelity for operation " << name;
      }
    }
    if (gate_num_qubits == 2) {
      for (const auto &[control, target] : coupling_map) {
        auto site_arr = std::array{control, target};
        EXPECT_EQ(QDMI_query_operation_property(
                      device, op, 2, site_arr.data(),
                      QDMI_OPERATION_PROPERTY_DURATION, sizeof(double) * 2,
                      &duration, nullptr),
                  QDMI_SUCCESS)
            << "Failed to query duration for gate " << op;
        EXPECT_EQ(QDMI_query_operation_property(
                      device, op, 2, site_arr.data(),
                      QDMI_OPERATION_PROPERTY_FIDELITY, sizeof(double) * 2,
                      &fidelity, nullptr),
                  QDMI_SUCCESS)
            << "Failed to query fidelity for gate " << op;
      }
    }
  }
}

TEST_P(QDMIImplementationTest, ControlJob) {
  QDMI_Job job{};
  const std::string input = "OPENQASM 2.0;\n"
                            "include \"qelib1.inc\";\n"
                            "qreg q[2];\n"
                            "h q[0];\n"
                            "cx q[0], q[1];\n";
  EXPECT_EQ(QDMI_control_create_job(device, QDMI_PROGRAM_FORMAT_QASM2, 1,
                                    nullptr, &job),
            QDMI_ERROR_INVALIDARGUMENT);
  ASSERT_EQ(QDMI_control_create_job(device, QDMI_PROGRAM_FORMAT_QASM2,
                                    static_cast<int>(input.length()) + 1,
                                    input.c_str(), &job),
            QDMI_SUCCESS);
  int shots = 5;
  EXPECT_EQ(QDMI_control_set_parameter(device, nullptr,
                                       QDMI_JOB_PARAMETER_SHOTS_NUM,
                                       sizeof(int), &shots),
            QDMI_ERROR_INVALIDARGUMENT);
  EXPECT_EQ(QDMI_control_set_parameter(
                device, job, QDMI_JOB_PARAMETER_SHOTS_NUM, 0, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
  EXPECT_EQ(QDMI_control_set_parameter(device, job, QDMI_JOB_PARAMETER_CUSTOM_5,
                                       sizeof(int), &shots),
            QDMI_ERROR_NOTSUPPORTED);
  ASSERT_EQ(QDMI_control_set_parameter(
                device, job, QDMI_JOB_PARAMETER_SHOTS_NUM, sizeof(int), &shots),
            QDMI_SUCCESS);
  ASSERT_EQ(QDMI_control_submit_job(device, job), QDMI_SUCCESS);
  EXPECT_EQ(QDMI_control_submit_job(device, job), QDMI_ERROR_INVALIDARGUMENT);
  QDMI_Job_Status status{};
  EXPECT_EQ(QDMI_control_check(device, job, &status), QDMI_SUCCESS);
  EXPECT_EQ(QDMI_control_wait(device, job), QDMI_SUCCESS);
  ASSERT_EQ(QDMI_control_check(device, job, &status), QDMI_SUCCESS);
  EXPECT_EQ(status, QDMI_JOB_STATUS_DONE);
  EXPECT_EQ(QDMI_control_set_parameter(
                device, job, QDMI_JOB_PARAMETER_SHOTS_NUM, sizeof(int), &shots),
            QDMI_ERROR_INVALIDARGUMENT);
  QDMI_control_free_job(device, job);
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

TEST_P(QDMIImplementationTest, ControlGetData) {
  const std::string test_circuit = R"(
OPENQASM 2.0;
include "qelib1.inc";
qreg q[2];
creg c[2];
h q[0];
cx q[0], q[1];
measure q -> c;
  )";
  QDMI_Job job = nullptr;
  ASSERT_EQ(QDMI_control_create_job(device, QDMI_PROGRAM_FORMAT_QASM2,
                                    static_cast<int>(test_circuit.length() + 1),
                                    test_circuit.c_str(), &job),
            QDMI_SUCCESS);
  int shots_num = 6;
  ASSERT_EQ(QDMI_control_set_parameter(device, job,
                                       QDMI_JOB_PARAMETER_SHOTS_NUM,
                                       sizeof(int), &shots_num),
            QDMI_SUCCESS);
  ASSERT_EQ(QDMI_control_submit_job(device, job), QDMI_SUCCESS);
  ASSERT_EQ(QDMI_control_wait(device, job), QDMI_SUCCESS);
  int size = 0;
  ASSERT_EQ(QDMI_control_get_data(device, job, QDMI_JOB_RESULT_HIST_KEYS, 0,
                                  nullptr, &size),
            QDMI_SUCCESS);
  std::string key_list(static_cast<std::size_t>(size), '\0');
  ASSERT_EQ(QDMI_control_get_data(device, job, QDMI_JOB_RESULT_HIST_KEYS, size,
                                  key_list.data(), nullptr),
            QDMI_SUCCESS);
  std::vector<std::string> key_vec;
  std::string token;
  std::stringstream ss(key_list);
  while (std::getline(ss, token, ',')) {
    key_vec.emplace_back(token);
  }
  size = static_cast<int>(key_vec.size());
  std::vector<int> val_vec(key_vec.size());
  ASSERT_EQ(QDMI_control_get_data(device, job, QDMI_JOB_RESULT_HIST_VALUES,
                                  static_cast<int>(sizeof(int)) * size,
                                  val_vec.data(), nullptr),
            QDMI_SUCCESS);
  std::map<std::string, int> results;
  for (size_t i = 0; i < key_vec.size(); ++i) {
    results[key_vec[i]] = val_vec[i];
  }
  ASSERT_EQ(results.size(), static_cast<std::size_t>(size));
  QDMI_control_free_job(device, job);
}
