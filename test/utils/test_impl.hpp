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
 * @brief This file is for testing whether a device implements all the required
 * functions.
 * @details It calls all the functions in the device interface to ensure that
 * they are implemented. During linking, when a function is not implemented this
 * will raise an error. Additionally, when executed, the tests check that no
 * function returns QDMI_ERROR_NOTIMPLEMENTED.
 */

#pragma once

#include "qdmi/client.h"

#include <cstdint>
#include <gtest/gtest.h>
#include <string>
#include <tuple>

constexpr const char *Shared_library_file_extension() {
#ifdef _WIN32
  return ".dll";
#endif
#ifdef __APPLE__
  return ".dylib";
#else
  return ".so";
#endif
}

enum class TEST_SESSION_MODE : uint8_t { READONLY, READWRITE };

class QDMIImplementationTest
    : public ::testing::TestWithParam<
          std::tuple<std::string, std::string, TEST_SESSION_MODE>> {
protected:
  QDMI_Session session = nullptr;
  QDMI_Device device = nullptr;
  TEST_SESSION_MODE mode = TEST_SESSION_MODE::READONLY;
  std::string config_file_name;

  void SetUp() override;

  void TearDown() override;
};
