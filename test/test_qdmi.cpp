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

#include "qdmi/client.h"
#include "qdmi_example_driver.h"
#include "utils/test_impl.hpp"

#include <algorithm>
#include <array>
#include <complex>
#include <cstddef>
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
#include <unordered_set>
#include <utility>
#include <vector>

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
