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

#include "test_impl.hpp"

#include "qdmi/client.h"
#include "qdmi_example_driver.h"

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <string>

void QDMIImplementationTest::SetUp() {
  auto params = GetParam();
  const std::string &library_name = std::get<0>(params);
  const std::string &prefix = std::get<1>(params);
  mode = std::get<2>(params);

  // Get the current test info
  const ::testing::TestInfo *test_info =
      ::testing::UnitTest::GetInstance()->current_test_info();
  auto test_name =
      test_info->test_suite_name() + std::string("_") + test_info->name();
  // replace all `/` with `_` in the test name
  std::replace(test_name.begin(), test_name.end(), '/', '_');

  config_file_name = "qdmi_" + test_name + ".conf";
  std::ofstream conf_file(config_file_name);
  conf_file << library_name << Shared_library_file_extension() << " " << prefix
            << "\n";
  conf_file.close();

#ifdef _WIN32
  _putenv_s("QDMI_CONF", config_file_name.c_str());
#else
  // NOLINTNEXTLINE(misc-include-cleaner) already included from `<cstdlib>`
  setenv("QDMI_CONF", config_file_name.c_str(), 1);
#endif

  ASSERT_EQ(QDMI_driver_init(), QDMI_SUCCESS)
      << "Failed to initialize the driver";

  ASSERT_EQ(QDMI_session_alloc(&session), QDMI_SUCCESS)
      << "Failed to allocate session";

  if (mode == TEST_SESSION_MODE::READWRITE) {
    const char *token = "token";
    ASSERT_EQ(QDMI_session_set_parameter(session, QDMI_SESSION_PARAMETER_TOKEN,
                                         6, token),
              QDMI_SUCCESS)
        << "Failed to set session parameter";
  } else if (mode == TEST_SESSION_MODE::READONLY) {
    const char *token = "";
    ASSERT_EQ(QDMI_session_set_parameter(session, QDMI_SESSION_PARAMETER_TOKEN,
                                         1, token),
              QDMI_SUCCESS)
        << "Failed to set session parameter";
  } else {
    FAIL() << "Invalid mode";
  }

  ASSERT_EQ(QDMI_session_init(session), QDMI_SUCCESS)
      << "Failed to initialize session. Potential errors: Wrong or missing "
         "authentication information, device status is offline, or in "
         "maintenance. To provide credentials, take a look in " __FILE__
      << (__LINE__ - 4);

  ASSERT_EQ(QDMI_session_get_devices(session, 1, &device, nullptr),
            QDMI_SUCCESS)
      << "Failed to get device";
}

void QDMIImplementationTest::TearDown() {
  QDMI_session_free(session);
  QDMI_driver_shutdown();
  std::filesystem::remove(config_file_name);
}

namespace {
std::string Get_test_circuit() {
  return "OPENQASM 2.0;\n"
         "include \"qelib1.inc\";\n"
         "qreg q[2];\n"
         "creg c[2];\n"
         "h q[0];\n"
         "cx q[0], q[1];\n"
         "measure q -> c;\n";
}
} // namespace

TEST_P(QDMIImplementationTest, SetSessionParameterImplemented) {
  ASSERT_EQ(QDMI_session_set_parameter(session, QDMI_SESSION_PARAMETER_MAX, 0,
                                       nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
}

TEST_P(QDMIImplementationTest, CreateQASM2JobImplemented) {
  QDMI_Job job = nullptr;
  const auto expected_value = mode == TEST_SESSION_MODE::READWRITE
                                  ? QDMI_SUCCESS
                                  : QDMI_ERROR_PERMISSIONDENIED;
  EXPECT_EQ(QDMI_job_create(device, QDMI_PROGRAM_FORMAT_QASM2,
                            Get_test_circuit().length() + 1,
                            Get_test_circuit().c_str(), &job),
            expected_value);
  QDMI_job_free(job);
}

TEST_P(QDMIImplementationTest, SetJobParameterImplemented) {
  QDMI_Job job = nullptr;
  const auto expected_value = mode == TEST_SESSION_MODE::READWRITE
                                  ? QDMI_SUCCESS
                                  : QDMI_ERROR_PERMISSIONDENIED;
  ASSERT_EQ(QDMI_job_create(device, QDMI_PROGRAM_FORMAT_QASM2,
                            Get_test_circuit().length() + 1,
                            Get_test_circuit().c_str(), &job),
            expected_value);
  EXPECT_EQ(QDMI_job_set_parameter(job, QDMI_JOB_PARAMETER_MAX, 0, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
  QDMI_job_free(job);
}

TEST_P(QDMIImplementationTest, SubmitJobImplemented) {
  QDMI_Job job = nullptr;
  auto expected_value = mode == TEST_SESSION_MODE::READWRITE
                            ? QDMI_SUCCESS
                            : QDMI_ERROR_PERMISSIONDENIED;
  ASSERT_EQ(QDMI_job_create(device, QDMI_PROGRAM_FORMAT_QASM2,
                            Get_test_circuit().length() + 1,
                            Get_test_circuit().c_str(), &job),
            expected_value);
  expected_value = mode == TEST_SESSION_MODE::READWRITE
                       ? QDMI_SUCCESS
                       : QDMI_ERROR_INVALIDARGUMENT;
  EXPECT_EQ(QDMI_job_submit(job), expected_value);
  QDMI_job_free(job);
}

TEST_P(QDMIImplementationTest, CancelJobImplemented) {
  QDMI_Job job = nullptr;
  auto expected_value = mode == TEST_SESSION_MODE::READWRITE
                            ? QDMI_SUCCESS
                            : QDMI_ERROR_PERMISSIONDENIED;
  ASSERT_EQ(QDMI_job_create(device, QDMI_PROGRAM_FORMAT_QASM2,
                            Get_test_circuit().length() + 1,
                            Get_test_circuit().c_str(), &job),
            expected_value);
  expected_value = mode == TEST_SESSION_MODE::READWRITE
                       ? QDMI_SUCCESS
                       : QDMI_ERROR_INVALIDARGUMENT;
  EXPECT_EQ(QDMI_job_cancel(job), expected_value);
  QDMI_job_free(job);
}

TEST_P(QDMIImplementationTest, CheckJobStatusImplemented) {
  QDMI_Job job = nullptr;
  QDMI_Job_Status status = QDMI_JOB_STATUS_RUNNING;
  auto expected_value = mode == TEST_SESSION_MODE::READWRITE
                            ? QDMI_SUCCESS
                            : QDMI_ERROR_PERMISSIONDENIED;
  ASSERT_EQ(QDMI_job_create(device, QDMI_PROGRAM_FORMAT_QASM2,
                            Get_test_circuit().length() + 1,
                            Get_test_circuit().c_str(), &job),
            expected_value);
  expected_value = mode == TEST_SESSION_MODE::READWRITE
                       ? QDMI_SUCCESS
                       : QDMI_ERROR_INVALIDARGUMENT;
  EXPECT_EQ(QDMI_job_check(job, &status), expected_value);
  QDMI_job_free(job);
}

TEST_P(QDMIImplementationTest, WaitOnJobImplemented) {
  QDMI_Job job = nullptr;
  auto expected_value = mode == TEST_SESSION_MODE::READWRITE
                            ? QDMI_SUCCESS
                            : QDMI_ERROR_PERMISSIONDENIED;
  ASSERT_EQ(QDMI_job_create(device, QDMI_PROGRAM_FORMAT_QASM2,
                            Get_test_circuit().length() + 1,
                            Get_test_circuit().c_str(), &job),
            expected_value);
  expected_value = mode == TEST_SESSION_MODE::READWRITE
                       ? QDMI_SUCCESS
                       : QDMI_ERROR_INVALIDARGUMENT;
  EXPECT_EQ(QDMI_job_wait(job), expected_value);
  QDMI_job_free(job);
}

TEST_P(QDMIImplementationTest, GetJobDataImplemented) {
  QDMI_Job job = nullptr;
  const auto expected_value = mode == TEST_SESSION_MODE::READWRITE
                                  ? QDMI_SUCCESS
                                  : QDMI_ERROR_PERMISSIONDENIED;
  ASSERT_EQ(QDMI_job_create(device, QDMI_PROGRAM_FORMAT_QASM2,
                            Get_test_circuit().length() + 1,
                            Get_test_circuit().c_str(), &job),
            expected_value);
  EXPECT_EQ(QDMI_job_get_data(job, QDMI_JOB_RESULT_MAX, 0, nullptr, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
  QDMI_job_free(job);
}

TEST_P(QDMIImplementationTest, QueryDevicePropertyImplemented) {
  ASSERT_EQ(QDMI_device_query_property(device, QDMI_DEVICE_PROPERTY_NAME, 0,
                                       nullptr, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
}

TEST_P(QDMIImplementationTest, GetSitesImplemented) {
  ASSERT_EQ(QDMI_device_get_sites(device, 0, nullptr, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
}

TEST_P(QDMIImplementationTest, GetOperationsImplemented) {
  ASSERT_EQ(QDMI_device_get_operations(device, 0, nullptr, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
}

TEST_P(QDMIImplementationTest, QuerySitePropertyImplemented) {
  ASSERT_EQ(QDMI_site_query_property(device, nullptr, QDMI_SITE_PROPERTY_MAX, 0,
                                     nullptr, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
}

TEST_P(QDMIImplementationTest, QueryOperationPropertyImplemented) {
  ASSERT_EQ(QDMI_operation_query_property(device, nullptr, 0, nullptr,
                                          QDMI_OPERATION_PROPERTY_MAX, 0,
                                          nullptr, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
}

TEST_P(QDMIImplementationTest, QueryDeviceNameImplemented) {
  size_t size = 0;
  ASSERT_EQ(QDMI_device_query_property(device, QDMI_DEVICE_PROPERTY_NAME, 0,
                                       nullptr, &size),
            QDMI_SUCCESS)
      << "Devices must provide a name";
  std::string value(size - 1, '\0');
  ASSERT_EQ(QDMI_device_query_property(device, QDMI_DEVICE_PROPERTY_NAME, size,
                                       value.data(), nullptr),
            QDMI_SUCCESS)
      << "Devices must provide a name";
  ASSERT_FALSE(value.empty()) << "Devices must provide a name";
}

TEST_P(QDMIImplementationTest, QueryDeviceVersionImplemented) {
  size_t size = 0;
  ASSERT_EQ(QDMI_device_query_property(device, QDMI_DEVICE_PROPERTY_VERSION, 0,
                                       nullptr, &size),
            QDMI_SUCCESS)
      << "Devices must provide a version";
  std::string value(size - 1, '\0');
  ASSERT_EQ(QDMI_device_query_property(device, QDMI_DEVICE_PROPERTY_VERSION,
                                       size, value.data(), nullptr),
            QDMI_SUCCESS)
      << "Devices must provide a version";
  ASSERT_FALSE(value.empty()) << "Devices must provide a version";
}

TEST_P(QDMIImplementationTest, QueryDeviceLibraryVersionImplemented) {
  size_t size = 0;
  ASSERT_EQ(QDMI_device_query_property(
                device, QDMI_DEVICE_PROPERTY_LIBRARYVERSION, 0, nullptr, &size),
            QDMI_SUCCESS)
      << "Devices must provide a library version";
  std::string value(size - 1, '\0');
  ASSERT_EQ(QDMI_device_query_property(device,
                                       QDMI_DEVICE_PROPERTY_LIBRARYVERSION,
                                       size, value.data(), nullptr),
            QDMI_SUCCESS)
      << "Devices must provide a library version";
  ASSERT_FALSE(value.empty()) << "Devices must provide a library version";
}
