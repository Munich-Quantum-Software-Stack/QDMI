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

#include "my_qdmi/device.h"

#include <cstddef>
#include <gtest/gtest.h>
#include <string>
#include <vector>

class QDMIImplementationTest : public ::testing::Test {
protected:
  MY_QDMI_Device_Session session = nullptr;

  void SetUp() override {
    ASSERT_EQ(MY_QDMI_device_initialize(), QDMI_SUCCESS)
        << "Failed to initialize the device";

    ASSERT_EQ(MY_QDMI_device_session_alloc(&session), QDMI_SUCCESS)
        << "Failed to allocate a session";

    ASSERT_EQ(MY_QDMI_device_session_init(session), QDMI_SUCCESS)
        << "Failed to initialize a session. Potential errors: Wrong or missing "
           "authentication information, device status is offline, or in "
           "maintenance. To provide credentials, take a look in " __FILE__
        << (__LINE__ - 4);
  }

  void TearDown() override { MY_QDMI_device_finalize(); }
};

TEST_F(QDMIImplementationTest, SessionSetParameterImplemented) {
  ASSERT_EQ(MY_QDMI_device_session_set_parameter(
                session, QDMI_DEVICE_SESSION_PARAMETER_MAX, 0, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
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

TEST_F(QDMIImplementationTest, ControlCreateJobImplemented) {
  MY_QDMI_Device_Job job = nullptr;
  ASSERT_NE(MY_QDMI_device_job_create(session, QDMI_PROGRAM_FORMAT_QASM2,
                                      Get_test_circuit().length() + 1,
                                      Get_test_circuit().c_str(), &job),
            QDMI_ERROR_NOTIMPLEMENTED);
  MY_QDMI_device_job_free(job);
}

TEST_F(QDMIImplementationTest, ControlSetParameterImplemented) {
  MY_QDMI_Device_Job job = nullptr;
  ASSERT_EQ(MY_QDMI_device_job_create(session, QDMI_PROGRAM_FORMAT_QASM2,
                                      Get_test_circuit().length() + 1,
                                      Get_test_circuit().c_str(), &job),
            QDMI_SUCCESS);
  ASSERT_EQ(MY_QDMI_device_job_set_parameter(job, QDMI_DEVICE_JOB_PARAMETER_MAX,
                                             0, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
  MY_QDMI_device_job_free(job);
}

TEST_F(QDMIImplementationTest, ControlSubmitJobImplemented) {
  MY_QDMI_Device_Job job = nullptr;
  ASSERT_EQ(MY_QDMI_device_job_create(session, QDMI_PROGRAM_FORMAT_QASM2,
                                      Get_test_circuit().length() + 1,
                                      Get_test_circuit().c_str(), &job),
            QDMI_SUCCESS);
  ASSERT_NE(MY_QDMI_device_job_submit(job), QDMI_ERROR_NOTIMPLEMENTED);
  MY_QDMI_device_job_free(job);
}

TEST_F(QDMIImplementationTest, ControlCancelImplemented) {
  MY_QDMI_Device_Job job = nullptr;
  ASSERT_EQ(MY_QDMI_device_job_create(session, QDMI_PROGRAM_FORMAT_QASM2,
                                      Get_test_circuit().length() + 1,
                                      Get_test_circuit().c_str(), &job),
            QDMI_SUCCESS);
  ASSERT_NE(MY_QDMI_device_job_cancel(job), QDMI_ERROR_NOTIMPLEMENTED);
  MY_QDMI_device_job_free(job);
}

TEST_F(QDMIImplementationTest, ControlCheckImplemented) {
  MY_QDMI_Device_Job job = nullptr;
  QDMI_Job_Status status = QDMI_JOB_STATUS_RUNNING;
  ASSERT_EQ(MY_QDMI_device_job_create(session, QDMI_PROGRAM_FORMAT_QASM2,
                                      Get_test_circuit().length() + 1,
                                      Get_test_circuit().c_str(), &job),
            QDMI_SUCCESS);
  ASSERT_NE(MY_QDMI_device_job_check(job, &status), QDMI_ERROR_NOTIMPLEMENTED);
  MY_QDMI_device_job_free(job);
}

TEST_F(QDMIImplementationTest, ControlWaitImplemented) {
  MY_QDMI_Device_Job job = nullptr;
  ASSERT_EQ(MY_QDMI_device_job_create(session, QDMI_PROGRAM_FORMAT_QASM2,
                                      Get_test_circuit().length() + 1,
                                      Get_test_circuit().c_str(), &job),
            QDMI_SUCCESS);
  ASSERT_NE(MY_QDMI_device_job_wait(job), QDMI_ERROR_NOTIMPLEMENTED);
  MY_QDMI_device_job_free(job);
}

TEST_F(QDMIImplementationTest, ControlGetDataImplemented) {
  MY_QDMI_Device_Job job = nullptr;
  ASSERT_EQ(MY_QDMI_device_job_create(session, QDMI_PROGRAM_FORMAT_QASM2,
                                      Get_test_circuit().length() + 1,
                                      Get_test_circuit().c_str(), &job),
            QDMI_SUCCESS);
  ASSERT_EQ(MY_QDMI_device_job_get_data(job, QDMI_JOB_RESULT_MAX, 0, nullptr,
                                        nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
  MY_QDMI_device_job_free(job);
}

TEST_F(QDMIImplementationTest, QueryDevicePropertyImplemented) {
  ASSERT_EQ(MY_QDMI_device_session_query_property(
                session, QDMI_DEVICE_PROPERTY_NAME, 0, nullptr, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
}

TEST_F(QDMIImplementationTest, QueryGetSitesImplemented) {
  ASSERT_EQ(MY_QDMI_device_session_get_sites(session, 0, nullptr, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
}

TEST_F(QDMIImplementationTest, QueryGetOperationsImplemented) {
  ASSERT_EQ(MY_QDMI_device_session_get_operations(session, 0, nullptr, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
}

TEST_F(QDMIImplementationTest, QuerySitePropertyImplemented) {
  ASSERT_EQ(MY_QDMI_device_site_query_property(
                nullptr, nullptr, QDMI_SITE_PROPERTY_MAX, 0, nullptr, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
}

TEST_F(QDMIImplementationTest, QueryOperationPropertyImplemented) {
  ASSERT_EQ(MY_QDMI_device_operation_query_property(
                nullptr, nullptr, 0, nullptr, QDMI_OPERATION_PROPERTY_MAX, 0,
                nullptr, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
}

TEST_F(QDMIImplementationTest, QueryDeviceNameImplemented) {
  size_t size = 0;
  ASSERT_EQ(MY_QDMI_device_session_query_property(
                session, QDMI_DEVICE_PROPERTY_NAME, 0, nullptr, &size),
            QDMI_SUCCESS)
      << "Devices must provide a name";
  std::string value(size - 1, '\0');
  ASSERT_EQ(MY_QDMI_device_session_query_property(session,
                                                  QDMI_DEVICE_PROPERTY_NAME,
                                                  size, value.data(), nullptr),
            QDMI_SUCCESS)
      << "Devices must provide a name";
  ASSERT_FALSE(value.empty()) << "Devices must provide a name";
}

TEST_F(QDMIImplementationTest, QueryDeviceVersionImplemented) {
  size_t size = 0;
  ASSERT_EQ(MY_QDMI_device_session_query_property(
                session, QDMI_DEVICE_PROPERTY_VERSION, 0, nullptr, &size),
            QDMI_SUCCESS)
      << "Devices must provide a version";
  std::string value(size - 1, '\0');
  ASSERT_EQ(MY_QDMI_device_session_query_property(session,
                                                  QDMI_DEVICE_PROPERTY_VERSION,
                                                  size, value.data(), nullptr),
            QDMI_SUCCESS)
      << "Devices must provide a version";
  ASSERT_FALSE(value.empty()) << "Devices must provide a version";
}

TEST_F(QDMIImplementationTest, QueryDeviceLibraryVersionImplemented) {
  size_t size = 0;
  ASSERT_EQ(
      MY_QDMI_device_session_query_property(
          session, QDMI_DEVICE_PROPERTY_LIBRARYVERSION, 0, nullptr, &size),
      QDMI_SUCCESS)
      << "Devices must provide a library version";
  std::string value(size - 1, '\0');
  ASSERT_EQ(MY_QDMI_device_session_query_property(
                session, QDMI_DEVICE_PROPERTY_LIBRARYVERSION, size,
                value.data(), nullptr),
            QDMI_SUCCESS)
      << "Devices must provide a library version";
  ASSERT_FALSE(value.empty()) << "Devices must provide a library version";
}

TEST_F(QDMIImplementationTest, QuerySiteIDImplemented) {
  size_t size = 0;
  ASSERT_EQ(MY_QDMI_device_session_get_sites(session, 0, nullptr, &size),
            QDMI_SUCCESS)
      << "Devices must provide a list of sites";
  std::vector<MY_QDMI_Site> sites(size);
  ASSERT_EQ(
      MY_QDMI_device_session_get_sites(session, size, sites.data(), nullptr),
      QDMI_SUCCESS)
      << "Devices must provide a list of sites";
  size_t id = 0;
  for (auto *site : sites) {
    ASSERT_EQ(MY_QDMI_device_site_query_property(session, site,
                                                 QDMI_SITE_PROPERTY_ID,
                                                 sizeof(size_t), &id, nullptr),
              QDMI_SUCCESS)
        << "Devices must provide a site id";
  }
}

TEST_F(QDMIImplementationTest, QubitNum) {
  size_t num_qubits = 0;
  EXPECT_EQ(MY_QDMI_device_session_query_property(
                session, QDMI_DEVICE_PROPERTY_QUBITSNUM, sizeof(size_t),
                &num_qubits, nullptr),
            QDMI_SUCCESS);
}
