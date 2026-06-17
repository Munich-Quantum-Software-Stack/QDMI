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

#include "test_qpu.hpp"

#include "qdmi/core.h"

#include <cstddef>
#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include <unordered_set>

namespace qdmi::test {
auto QDMIQpuTest::SetUp() -> void {
  QDMISessionTest::SetUp();
  if (!IsSkipped() && !HasFatalFailure()) {
    QDMI_Module mod{};
    ASSERT_EQ(qdmi_->context_query_module_by_id(context_, "qpu", &mod),
              QDMI_SUCCESS)
        << "Failed to get QPU module.";
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    ASSERT_EQ(qdmi_->context_get_module_interface(
                  context_, mod, reinterpret_cast<const void **>(&qpu_)),
              QDMI_SUCCESS)
        << "Failed to get QPU interface.";
  }
}
TEST_P(QDMIQpuTest, QueryQubitCount) {
  EXPECT_EQ(qpu_->session_query_qubit_count(nullptr, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
  EXPECT_EQ(qpu_->session_query_qubit_count(session_, nullptr), QDMI_SUCCESS);
  size_t count = 0;
  EXPECT_EQ(qpu_->session_query_qubit_count(session_, &count), QDMI_SUCCESS);
}
TEST_P(QDMIQpuTest, QueryQpuStatus) {
  EXPECT_EQ(qpu_->session_query_qpu_status(nullptr, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
  EXPECT_EQ(qpu_->session_query_qpu_status(session_, nullptr), QDMI_SUCCESS);
  QDMI_QPU_Status status = QDMI_QPU_STATUS_OFFLINE;
  EXPECT_EQ(qpu_->session_query_qpu_status(session_, &status), QDMI_SUCCESS);
}
} // namespace qdmi::test
