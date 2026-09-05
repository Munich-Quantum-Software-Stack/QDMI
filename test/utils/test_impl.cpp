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

#include "test_impl.hpp"

#include "qdmi/client.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>
#include <string>
#include <vector>

void QDMIImplementationTest::SetUp() {
  auto params = GetParam();
  const std::string &library_name = std::get<0>(params);
  const std::string &prefix = std::get<1>(params);
  const std::string &device_id = std::get<2>(params);
  mode = std::get<3>(params);

  // Get the current test info
  const ::testing::TestInfo *test_info =
      ::testing::UnitTest::GetInstance()->current_test_info();
  auto test_name =
      test_info->test_suite_name() + std::string("_") + test_info->name();
  // replace all `/` with `_` in the test name
  std::ranges::replace(test_name, '/', '_');

  config_file_name = "qdmi_" + test_name + ".conf";
  std::ofstream conf_file(config_file_name);
  conf_file << library_name << Shared_library_file_extension() << " " << prefix
            << " " << device_id << "\n";
  conf_file.close();

#ifdef _WIN32
  _putenv_s("QDMI_CONF", config_file_name.c_str());
#else
  // NOLINTNEXTLINE(misc-include-cleaner) already included from `<cstdlib>`
  setenv("QDMI_CONF", config_file_name.c_str(), 1);
#endif

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

  ASSERT_EQ(QDMI_session_query_session_property(
                session, QDMI_SESSION_PROPERTY_DEVICES, sizeof(QDMI_Device),
                static_cast<void *>(&device), nullptr),
            QDMI_SUCCESS)
      << "Failed to get device";
}

void QDMIImplementationTest::TearDown() {
  QDMI_session_free(session);
  std::filesystem::remove(config_file_name);
}

TEST_P(QDMIImplementationTest, SessionAllocImplemented) {
  ASSERT_EQ(QDMI_session_alloc(nullptr), QDMI_ERROR_INVALIDARGUMENT);
}

TEST_P(QDMIImplementationTest, SessionInitImplemented) {
  ASSERT_EQ(QDMI_session_init(nullptr), QDMI_ERROR_INVALIDARGUMENT);
}

TEST_P(QDMIImplementationTest, SessionFreeNULL) {
  ASSERT_NO_THROW(QDMI_session_free(nullptr));
}

TEST_P(QDMIImplementationTest, SessionSetParameterImplemented) {
  QDMI_Session uninitialized_session = nullptr;
  ASSERT_EQ(QDMI_session_alloc(&uninitialized_session), QDMI_SUCCESS);
  EXPECT_THAT(QDMI_session_set_parameter(uninitialized_session,
                                         QDMI_SESSION_PARAMETER_AUTHURL, 20,
                                         "https://example.com"),
              testing::AnyOf(QDMI_SUCCESS, QDMI_ERROR_NOTSUPPORTED,
                             QDMI_ERROR_INVALIDARGUMENT));
  QDMI_session_free(uninitialized_session);
  EXPECT_EQ(QDMI_session_set_parameter(session, QDMI_SESSION_PARAMETER_AUTHURL,
                                       20, "https://example.com"),
            QDMI_ERROR_BADSTATE);
  EXPECT_EQ(QDMI_session_set_parameter(session, QDMI_SESSION_PARAMETER_MAX, 0,
                                       nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
}

TEST_P(QDMIImplementationTest, JobCreateImplemented) {
  QDMI_Job job = nullptr;
  const auto expected_value = mode == TEST_SESSION_MODE::READWRITE
                                  ? QDMI_SUCCESS
                                  : QDMI_ERROR_PERMISSIONDENIED;
  EXPECT_EQ(QDMI_device_create_job(device, &job), expected_value);
  QDMI_job_free(job);
}

TEST_P(QDMIImplementationTest, JobSetParameterImplemented) {
  QDMI_Job job = nullptr;
  const auto expected_value = mode == TEST_SESSION_MODE::READWRITE
                                  ? QDMI_SUCCESS
                                  : QDMI_ERROR_PERMISSIONDENIED;
  EXPECT_EQ(QDMI_device_create_job(device, &job), expected_value);
  EXPECT_EQ(QDMI_job_set_parameter(job, QDMI_JOB_PARAMETER_MAX, 0, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
  QDMI_job_free(job);
}

TEST_P(QDMIImplementationTest, JobQueryJobPropertyImplemented) {
  QDMI_Job job = nullptr;
  const auto expected_value = mode == TEST_SESSION_MODE::READWRITE
                                  ? QDMI_SUCCESS
                                  : QDMI_ERROR_PERMISSIONDENIED;
  EXPECT_EQ(QDMI_device_create_job(device, &job), expected_value);
  EXPECT_EQ(
      QDMI_job_query_property(job, QDMI_JOB_PROPERTY_MAX, 0, nullptr, nullptr),
      QDMI_ERROR_INVALIDARGUMENT);
  QDMI_job_free(job);
}

TEST_P(QDMIImplementationTest, JobSubmitImplemented) {
  QDMI_Job job = nullptr;
  auto expected_value = mode == TEST_SESSION_MODE::READWRITE
                            ? QDMI_SUCCESS
                            : QDMI_ERROR_PERMISSIONDENIED;
  EXPECT_EQ(QDMI_device_create_job(device, &job), expected_value);
  expected_value = mode == TEST_SESSION_MODE::READWRITE
                       ? QDMI_SUCCESS
                       : QDMI_ERROR_INVALIDARGUMENT;
  EXPECT_EQ(QDMI_job_submit(job), expected_value);
  QDMI_job_free(job);
}

TEST_P(QDMIImplementationTest, JobCancelImplemented) {
  QDMI_Job job = nullptr;
  auto expected_value = mode == TEST_SESSION_MODE::READWRITE
                            ? QDMI_SUCCESS
                            : QDMI_ERROR_PERMISSIONDENIED;
  EXPECT_EQ(QDMI_device_create_job(device, &job), expected_value);
  expected_value = mode == TEST_SESSION_MODE::READWRITE
                       ? QDMI_SUCCESS
                       : QDMI_ERROR_INVALIDARGUMENT;
  EXPECT_EQ(QDMI_job_cancel(job), expected_value);
  QDMI_job_free(job);
}

TEST_P(QDMIImplementationTest, JobCheckImplemented) {
  QDMI_Job job = nullptr;
  QDMI_Job_Status status = QDMI_JOB_STATUS_RUNNING;
  auto expected_value = mode == TEST_SESSION_MODE::READWRITE
                            ? QDMI_SUCCESS
                            : QDMI_ERROR_PERMISSIONDENIED;
  EXPECT_EQ(QDMI_device_create_job(device, &job), expected_value);
  expected_value = mode == TEST_SESSION_MODE::READWRITE
                       ? QDMI_SUCCESS
                       : QDMI_ERROR_INVALIDARGUMENT;
  EXPECT_EQ(QDMI_job_check(job, &status), expected_value);
  QDMI_job_free(job);
}

TEST_P(QDMIImplementationTest, JobWaitImplemented) {
  QDMI_Job job = nullptr;
  auto expected_value = mode == TEST_SESSION_MODE::READWRITE
                            ? QDMI_SUCCESS
                            : QDMI_ERROR_PERMISSIONDENIED;
  EXPECT_EQ(QDMI_device_create_job(device, &job), expected_value);
  expected_value = mode == TEST_SESSION_MODE::READWRITE
                       ? QDMI_SUCCESS
                       : QDMI_ERROR_INVALIDARGUMENT;
  EXPECT_EQ(QDMI_job_wait(job, 0), expected_value);
  QDMI_job_free(job);
}

TEST_P(QDMIImplementationTest, JobGetResultsImplemented) {
  QDMI_Job job = nullptr;
  const auto expected_value = mode == TEST_SESSION_MODE::READWRITE
                                  ? QDMI_SUCCESS
                                  : QDMI_ERROR_PERMISSIONDENIED;
  EXPECT_EQ(QDMI_device_create_job(device, &job), expected_value);
  EXPECT_EQ(QDMI_job_get_results(job, QDMI_JOB_RESULT_MAX, 0, nullptr, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
  QDMI_job_free(job);
}

TEST_P(QDMIImplementationTest, QueryDevicePropertyImplemented) {
  ASSERT_EQ(QDMI_device_query_device_property(device, QDMI_DEVICE_PROPERTY_MAX,
                                              0, nullptr, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
}

TEST_P(QDMIImplementationTest, QuerySitePropertyImplemented) {
  ASSERT_EQ(QDMI_device_query_site_property(
                device, nullptr, QDMI_SITE_PROPERTY_MAX, 0, nullptr, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
}

TEST_P(QDMIImplementationTest, QueryOperationPropertyImplemented) {
  ASSERT_EQ(QDMI_device_query_operation_property(
                device, nullptr, 0, nullptr, 0, nullptr,
                QDMI_OPERATION_PROPERTY_MAX, 0, nullptr, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
}

TEST_P(QDMIImplementationTest, QueryDeviceNameImplemented) {
  size_t size = 0;
  ASSERT_EQ(QDMI_device_query_device_property(device, QDMI_DEVICE_PROPERTY_NAME,
                                              0, nullptr, &size),
            QDMI_SUCCESS)
      << "Devices must provide a name";
  std::string value(size - 1, '\0');
  ASSERT_EQ(QDMI_device_query_device_property(device, QDMI_DEVICE_PROPERTY_NAME,
                                              size, value.data(), nullptr),
            QDMI_SUCCESS)
      << "Devices must provide a name";
  ASSERT_FALSE(value.empty()) << "Devices must provide a name";
}

TEST_P(QDMIImplementationTest, QueryDeviceVersionImplemented) {
  size_t size = 0;
  ASSERT_EQ(QDMI_device_query_device_property(
                device, QDMI_DEVICE_PROPERTY_VERSION, 0, nullptr, &size),
            QDMI_SUCCESS)
      << "Devices must provide a version";
  std::string value(size - 1, '\0');
  ASSERT_EQ(QDMI_device_query_device_property(device,
                                              QDMI_DEVICE_PROPERTY_VERSION,
                                              size, value.data(), nullptr),
            QDMI_SUCCESS)
      << "Devices must provide a version";
  ASSERT_FALSE(value.empty()) << "Devices must provide a version";
}

TEST_P(QDMIImplementationTest, QueryDeviceLibraryVersionImplemented) {
  size_t size = 0;
  ASSERT_EQ(QDMI_device_query_device_property(
                device, QDMI_DEVICE_PROPERTY_LIBRARYVERSION, 0, nullptr, &size),
            QDMI_SUCCESS)
      << "Devices must provide a library version";
  std::string value(size - 1, '\0');
  ASSERT_EQ(QDMI_device_query_device_property(
                device, QDMI_DEVICE_PROPERTY_LIBRARYVERSION, size, value.data(),
                nullptr),
            QDMI_SUCCESS)
      << "Devices must provide a library version";
  ASSERT_FALSE(value.empty()) << "Devices must provide a library version";
}

TEST_P(QDMIImplementationTest, QuerySiteIndexImplemented) {
  size_t size = 0;
  ASSERT_EQ(QDMI_device_query_device_property(
                device, QDMI_DEVICE_PROPERTY_SITES, 0, nullptr, &size),
            QDMI_SUCCESS)
      << "Devices must provide a list of sites";
  std::vector<QDMI_Site> sites(size / sizeof(QDMI_Site));
  ASSERT_EQ(QDMI_device_query_device_property(
                device, QDMI_DEVICE_PROPERTY_SITES, size,
                static_cast<void *>(sites.data()), nullptr),
            QDMI_SUCCESS)
      << "Failed to get sites";
  uint64_t id = 0;
  for (auto *site : sites) {
    ASSERT_EQ(QDMI_device_query_site_property(device, site,
                                              QDMI_SITE_PROPERTY_INDEX,
                                              sizeof(uint64_t), &id, nullptr),
              QDMI_SUCCESS)
        << "Devices must provide a site id";
  }
}
