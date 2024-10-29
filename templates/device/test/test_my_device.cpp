/*------------------------------------------------------------------------------
Part of the MQSS Project, under the Apache License v2.0 with LLVM Exceptions.
See https://llvm.org/LICENSE.txt for license information.
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
------------------------------------------------------------------------------*/

#include "qdmi/device.h"

#include <cstddef>
#include <gtest/gtest.h>
#include <string>

class QDMIImplementationTest : public ::testing::Test {
protected:
  void SetUp() override {
    ASSERT_EQ(QDMI_control_initialize_dev(), QDMI_SUCCESS)
        << "Failed to initialize the device";
  }

  void TearDown() override { QDMI_control_finalize_dev(); }
};

TEST_F(QDMIImplementationTest, QueryGetSitesImplemented) {
  ASSERT_EQ(QDMI_query_get_sites_dev(0, nullptr, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
}

TEST_F(QDMIImplementationTest, QueryGetOperationsImplemented) {
  ASSERT_EQ(QDMI_query_get_operations_dev(0, nullptr, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
}

TEST_F(QDMIImplementationTest, QueryDevicePropertyImplemented) {
  ASSERT_EQ(QDMI_query_device_property_dev(QDMI_DEVICE_PROPERTY_NAME, 0,
                                           nullptr, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
}

TEST_F(QDMIImplementationTest, QuerySitePropertyImplemented) {
  ASSERT_EQ(QDMI_query_site_property_dev(nullptr, QDMI_SITE_PROPERTY_MAX, 0,
                                         nullptr, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
}

TEST_F(QDMIImplementationTest, QueryOperationPropertyImplemented) {
  ASSERT_EQ(QDMI_query_operation_property_dev(nullptr, 0, nullptr,
                                              QDMI_OPERATION_PROPERTY_MAX, 0,
                                              nullptr, nullptr),
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
  QDMI_Job job = nullptr;
  ASSERT_NE(QDMI_control_create_job_dev(
                QDMI_PROGRAM_FORMAT_QASM2,
                static_cast<int>(Get_test_circuit().length() + 1),
                Get_test_circuit().c_str(), &job),
            QDMI_ERROR_NOTIMPLEMENTED);
  QDMI_control_free_job_dev(job);
}

TEST_F(QDMIImplementationTest, ControlSetParameterImplemented) {
  QDMI_Job job = nullptr;
  ASSERT_EQ(QDMI_control_create_job_dev(
                QDMI_PROGRAM_FORMAT_QASM2,
                static_cast<int>(Get_test_circuit().length() + 1),
                Get_test_circuit().c_str(), &job),
            QDMI_SUCCESS);
  ASSERT_EQ(
      QDMI_control_set_parameter_dev(job, QDMI_JOB_PARAMETER_MAX, 0, nullptr),
      QDMI_ERROR_INVALIDARGUMENT);
  QDMI_control_free_job_dev(job);
}

TEST_F(QDMIImplementationTest, ControlSubmitJobImplemented) {
  QDMI_Job job = nullptr;
  ASSERT_EQ(QDMI_control_create_job_dev(
                QDMI_PROGRAM_FORMAT_QASM2,
                static_cast<int>(Get_test_circuit().length() + 1),
                Get_test_circuit().c_str(), &job),
            QDMI_SUCCESS);
  ASSERT_NE(QDMI_control_submit_job_dev(job), QDMI_ERROR_NOTIMPLEMENTED);
  QDMI_control_free_job_dev(job);
}

TEST_F(QDMIImplementationTest, ControlCancelImplemented) {
  QDMI_Job job = nullptr;
  ASSERT_EQ(QDMI_control_create_job_dev(
                QDMI_PROGRAM_FORMAT_QASM2,
                static_cast<int>(Get_test_circuit().length() + 1),
                Get_test_circuit().c_str(), &job),
            QDMI_SUCCESS);
  ASSERT_NE(QDMI_control_cancel_dev(job), QDMI_ERROR_NOTIMPLEMENTED);
  QDMI_control_free_job_dev(job);
}

TEST_F(QDMIImplementationTest, ControlCheckImplemented) {
  QDMI_Job job = nullptr;
  QDMI_Job_Status status = QDMI_JOB_STATUS_RUNNING;
  ASSERT_EQ(QDMI_control_create_job_dev(
                QDMI_PROGRAM_FORMAT_QASM2,
                static_cast<int>(Get_test_circuit().length() + 1),
                Get_test_circuit().c_str(), &job),
            QDMI_SUCCESS);
  ASSERT_NE(QDMI_control_check_dev(job, &status), QDMI_ERROR_NOTIMPLEMENTED);
  QDMI_control_free_job_dev(job);
}

TEST_F(QDMIImplementationTest, ControlWaitImplemented) {
  QDMI_Job job = nullptr;
  ASSERT_EQ(QDMI_control_create_job_dev(
                QDMI_PROGRAM_FORMAT_QASM2,
                static_cast<int>(Get_test_circuit().length() + 1),
                Get_test_circuit().c_str(), &job),
            QDMI_SUCCESS);
  ASSERT_NE(QDMI_control_wait_dev(job), QDMI_ERROR_NOTIMPLEMENTED);
  QDMI_control_free_job_dev(job);
}

TEST_F(QDMIImplementationTest, ControlGetHistImplemented) {
  QDMI_Job job = nullptr;
  ASSERT_EQ(QDMI_control_create_job_dev(
                QDMI_PROGRAM_FORMAT_QASM2,
                static_cast<int>(Get_test_circuit().length() + 1),
                Get_test_circuit().c_str(), &job),
            QDMI_SUCCESS);
  ASSERT_EQ(
      QDMI_control_get_data_dev(job, QDMI_JOB_RESULT_MAX, 0, nullptr, nullptr),
      QDMI_ERROR_INVALIDARGUMENT);
  QDMI_control_free_job_dev(job);
}

TEST_F(QDMIImplementationTest, QueryDeviceNameImplemented) {
  int size = 0;
  ASSERT_EQ(QDMI_query_device_property_dev(QDMI_DEVICE_PROPERTY_NAME, 0,
                                           nullptr, &size),
            QDMI_SUCCESS)
      << "Devices must provide a name";
  std::string value(static_cast<size_t>(size), '\0');
  ASSERT_EQ(QDMI_query_device_property_dev(QDMI_DEVICE_PROPERTY_NAME, size,
                                           value.data(), nullptr),
            QDMI_SUCCESS)
      << "Devices must provide a name";
  ASSERT_FALSE(value.empty()) << "Devices must provide a name";
}

TEST_F(QDMIImplementationTest, QueryDeviceVersionImplemented) {
  int size = 0;
  ASSERT_EQ(QDMI_query_device_property_dev(QDMI_DEVICE_PROPERTY_VERSION, 0,
                                           nullptr, &size),
            QDMI_SUCCESS)
      << "Devices must provide a version";
  std::string value(static_cast<size_t>(size), '\0');
  ASSERT_EQ(QDMI_query_device_property_dev(QDMI_DEVICE_PROPERTY_VERSION, size,
                                           value.data(), nullptr),
            QDMI_SUCCESS)
      << "Devices must provide a version";
  ASSERT_FALSE(value.empty()) << "Devices must provide a version";
}

TEST_F(QDMIImplementationTest, QueryDeviceLibraryVersionImplemented) {
  int size = 0;
  ASSERT_EQ(QDMI_query_device_property_dev(QDMI_DEVICE_PROPERTY_LIBRARYVERSION,
                                           0, nullptr, &size),
            QDMI_SUCCESS)
      << "Devices must provide a library version";
  std::string value(static_cast<size_t>(size), '\0');
  ASSERT_EQ(QDMI_query_device_property_dev(QDMI_DEVICE_PROPERTY_LIBRARYVERSION,
                                           size, value.data(), nullptr),
            QDMI_SUCCESS)
      << "Devices must provide a library version";
  ASSERT_FALSE(value.empty()) << "Devices must provide a library version";
}

TEST_F(QDMIImplementationTest, QubitNum) {
  int num_qubits = 0;
  EXPECT_EQ(QDMI_query_device_property_dev(QDMI_DEVICE_PROPERTY_QUBITSNUM,
                                           sizeof(int), &num_qubits, nullptr),
            QDMI_SUCCESS);
}
