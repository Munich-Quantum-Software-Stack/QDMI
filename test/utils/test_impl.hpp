/*------------------------------------------------------------------------------
Part of the MQSS Project, under the Apache License v2.0 with LLVM Exceptions.
See https://llvm.org/LICENSE.txt for license information.
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
------------------------------------------------------------------------------*/

/** @file
 * @brief This file is for testing whether a device implements all the required
 * functions.
 * @details It calls all the functions in the device interface to ensure that
 * they are implemented. During linking, when a function is not implemented this
 * will raise an error. Additionally, when executed, the tests check that no
 * function returns QDMI_ERROR_NOT_IMPLEMENTED.
 */

#pragma once

#include "qdmi/common/types.h"

#include <gtest/gtest.h>
#include <string>

constexpr const char *Shared_library_file_extension() {
#if defined(_WIN32)
  return ".dll";
#elif defined(__APPLE__)
  return ".dylib";
#else
  return ".so";
#endif
}

class QDMIImplementationTest : public ::testing::TestWithParam<std::string> {
protected:
  void SetUp() override;

  void TearDown() override;

  QDMI_Session session = nullptr;
  QDMI_Device device = nullptr;
  std::string library_name;
  std::string config_file_name;
};
