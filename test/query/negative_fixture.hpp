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

#pragma once

#include "control_qdmi/device.h"

#include <gtest/gtest.h>

class QDMIQueryFixture : public ::testing::Test {
protected:
  CONTROL_QDMI_Device_Session session = nullptr;
  static void SetUpTestSuite() {
    ASSERT_EQ(CONTROL_QDMI_device_initialize(), QDMI_SUCCESS);
  }
  static void TearDownTestSuite() {
    EXPECT_EQ(CONTROL_QDMI_device_finalize(), QDMI_SUCCESS);
  }
  void SetUp() override {
    ASSERT_EQ(CONTROL_QDMI_device_session_alloc(&session), QDMI_SUCCESS);
    ASSERT_EQ(CONTROL_QDMI_device_session_init(session), QDMI_SUCCESS);
  }
  void TearDown() override { CONTROL_QDMI_device_session_free(session); }
};
