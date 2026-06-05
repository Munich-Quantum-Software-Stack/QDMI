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

#include "cxx_qdmi/device.h"

#include <cstddef>
#include <gtest/gtest.h>
#include <string>
#include <vector>

class QDMIImplementationTest : public ::testing::Test {
protected:
  CXX_QDMI_Device_Session session = nullptr;

  void SetUp() override {
    ASSERT_EQ(CXX_QDMI_device_initialize(), QDMI_SUCCESS)
        << "Failed to initialize the device";

    ASSERT_EQ(CXX_QDMI_device_session_alloc(&session), QDMI_SUCCESS)
        << "Failed to allocate a session";

    ASSERT_EQ(CXX_QDMI_device_session_init(session), QDMI_SUCCESS)
        << "Failed to initialize a session. Potential errors: Wrong or missing "
           "authentication information, device status is offline, or in "
           "maintenance. To provide credentials, take a look in " __FILE__
        << ":" << (__LINE__ - 4);
  }

  void TearDown() override { CXX_QDMI_device_finalize(); }
};

TEST_F(QDMIImplementationTest, SessionSetParameterImplemented) {
  ASSERT_EQ(CXX_QDMI_device_session_set_parameter(
                session, QDMI_DEVICE_SESSION_PARAMETER_MAX, 0, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
}

TEST_F(QDMIImplementationTest, JobCreateImplemented) {
  CXX_QDMI_Device_Job job = nullptr;
  ASSERT_NE(CXX_QDMI_device_session_create_device_job(session, &job),
            QDMI_ERROR_NOTIMPLEMENTED);
  CXX_QDMI_device_job_free(job);
}

TEST_F(QDMIImplementationTest, JobSetParameterImplemented) {
  CXX_QDMI_Device_Job job = nullptr;
  ASSERT_EQ(CXX_QDMI_device_session_create_device_job(session, &job),
            QDMI_SUCCESS);
  ASSERT_EQ(CXX_QDMI_device_job_set_parameter(
                job, QDMI_DEVICE_JOB_PARAMETER_MAX, 0, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
  CXX_QDMI_device_job_free(job);
}

TEST_F(QDMIImplementationTest, JobQueryPropertyImplemented) {
  CXX_QDMI_Device_Job job = nullptr;
  ASSERT_EQ(CXX_QDMI_device_session_create_device_job(session, &job),
            QDMI_SUCCESS);
  ASSERT_EQ(CXX_QDMI_device_job_query_property(
                job, QDMI_DEVICE_JOB_PROPERTY_MAX, 0, nullptr, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
  CXX_QDMI_device_job_free(job);
}

TEST_F(QDMIImplementationTest, JobSubmitImplemented) {
  CXX_QDMI_Device_Job job = nullptr;
  ASSERT_EQ(CXX_QDMI_device_session_create_device_job(session, &job),
            QDMI_SUCCESS);
  ASSERT_NE(CXX_QDMI_device_job_submit(job), QDMI_ERROR_NOTIMPLEMENTED);
  CXX_QDMI_device_job_free(job);
}

TEST_F(QDMIImplementationTest, JobCancelImplemented) {
  CXX_QDMI_Device_Job job = nullptr;
  ASSERT_EQ(CXX_QDMI_device_session_create_device_job(session, &job),
            QDMI_SUCCESS);
  ASSERT_NE(CXX_QDMI_device_job_cancel(job), QDMI_ERROR_NOTIMPLEMENTED);
  CXX_QDMI_device_job_free(job);
}

TEST_F(QDMIImplementationTest, JobCheckImplemented) {
  CXX_QDMI_Device_Job job = nullptr;
  QDMI_Job_Status status = QDMI_JOB_STATUS_RUNNING;
  ASSERT_EQ(CXX_QDMI_device_session_create_device_job(session, &job),
            QDMI_SUCCESS);
  ASSERT_NE(CXX_QDMI_device_job_check(job, &status), QDMI_ERROR_NOTIMPLEMENTED);
  CXX_QDMI_device_job_free(job);
}

TEST_F(QDMIImplementationTest, JobWaitImplemented) {
  CXX_QDMI_Device_Job job = nullptr;
  ASSERT_EQ(CXX_QDMI_device_session_create_device_job(session, &job),
            QDMI_SUCCESS);
  ASSERT_NE(CXX_QDMI_device_job_wait(job, 0), QDMI_ERROR_NOTIMPLEMENTED);
  CXX_QDMI_device_job_free(job);
}

TEST_F(QDMIImplementationTest, JobGetResultsImplemented) {
  CXX_QDMI_Device_Job job = nullptr;
  ASSERT_EQ(CXX_QDMI_device_session_create_device_job(session, &job),
            QDMI_SUCCESS);
  ASSERT_EQ(CXX_QDMI_device_job_get_results(job, QDMI_JOB_RESULT_MAX, 0,
                                            nullptr, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
  CXX_QDMI_device_job_free(job);
}

TEST_F(QDMIImplementationTest, QueryDevicePropertyImplemented) {
  ASSERT_EQ(CXX_QDMI_device_session_query_device_property(
                nullptr, QDMI_DEVICE_PROPERTY_NAME, 0, nullptr, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
}

TEST_F(QDMIImplementationTest, QuerySitePropertyImplemented) {
  ASSERT_EQ(CXX_QDMI_device_session_query_site_property(
                nullptr, nullptr, QDMI_SITE_PROPERTY_MAX, 0, nullptr, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
}

TEST_F(QDMIImplementationTest, QueryOperationPropertyImplemented) {
  ASSERT_EQ(CXX_QDMI_device_session_query_operation_property(
                nullptr, nullptr, 0, nullptr, 0, nullptr,
                QDMI_OPERATION_PROPERTY_MAX, 0, nullptr, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
}

TEST_F(QDMIImplementationTest, QueryDeviceNameImplemented) {
  size_t size = 0;
  ASSERT_EQ(CXX_QDMI_device_session_query_device_property(
                session, QDMI_DEVICE_PROPERTY_NAME, 0, nullptr, &size),
            QDMI_SUCCESS)
      << "Devices must provide a name";
  std::string value(size - 1, '\0');
  ASSERT_EQ(
      CXX_QDMI_device_session_query_device_property(
          session, QDMI_DEVICE_PROPERTY_NAME, size, value.data(), nullptr),
      QDMI_SUCCESS)
      << "Devices must provide a name";
  ASSERT_FALSE(value.empty()) << "Devices must provide a name";
}

TEST_F(QDMIImplementationTest, QueryDeviceVersionImplemented) {
  size_t size = 0;
  ASSERT_EQ(CXX_QDMI_device_session_query_device_property(
                session, QDMI_DEVICE_PROPERTY_VERSION, 0, nullptr, &size),
            QDMI_SUCCESS)
      << "Devices must provide a version";
  std::string value(size - 1, '\0');
  ASSERT_EQ(
      CXX_QDMI_device_session_query_device_property(
          session, QDMI_DEVICE_PROPERTY_VERSION, size, value.data(), nullptr),
      QDMI_SUCCESS)
      << "Devices must provide a version";
  ASSERT_FALSE(value.empty()) << "Devices must provide a version";
}

TEST_F(QDMIImplementationTest, QueryDeviceLibraryVersionImplemented) {
  size_t size = 0;
  ASSERT_EQ(
      CXX_QDMI_device_session_query_device_property(
          session, QDMI_DEVICE_PROPERTY_LIBRARYVERSION, 0, nullptr, &size),
      QDMI_SUCCESS)
      << "Devices must provide a library version";
  std::string value(size - 1, '\0');
  ASSERT_EQ(CXX_QDMI_device_session_query_device_property(
                session, QDMI_DEVICE_PROPERTY_LIBRARYVERSION, size,
                value.data(), nullptr),
            QDMI_SUCCESS)
      << "Devices must provide a library version";
  ASSERT_FALSE(value.empty()) << "Devices must provide a library version";
}

TEST_F(QDMIImplementationTest, QuerySiteIndexImplemented) {
  size_t size = 0;
  ASSERT_EQ(CXX_QDMI_device_session_query_device_property(
                session, QDMI_DEVICE_PROPERTY_SITES, 0, nullptr, &size),
            QDMI_SUCCESS)
      << "Devices must provide a list of sites";
  std::vector<CXX_QDMI_Site> sites(size / sizeof(CXX_QDMI_Site));
  ASSERT_EQ(CXX_QDMI_device_session_query_device_property(
                session, QDMI_DEVICE_PROPERTY_SITES, size,
                static_cast<void *>(sites.data()), nullptr),
            QDMI_SUCCESS)
      << "Devices must provide a list of sites";
  size_t id = 0;
  for (auto *site : sites) {
    ASSERT_EQ(CXX_QDMI_device_session_query_site_property(
                  session, site, QDMI_SITE_PROPERTY_INDEX, sizeof(size_t), &id,
                  nullptr),
              QDMI_SUCCESS)
        << "Devices must provide a site id";
  }
}

TEST_F(QDMIImplementationTest, QueryDeviceQubitNum) {
  size_t num_qubits = 0;
  EXPECT_EQ(CXX_QDMI_device_session_query_device_property(
                session, QDMI_DEVICE_PROPERTY_QUBITSNUM, sizeof(size_t),
                &num_qubits, nullptr),
            QDMI_SUCCESS);
}
