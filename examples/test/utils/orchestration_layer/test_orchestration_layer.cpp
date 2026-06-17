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

#include "test_orchestration_layer.hpp"

#include <spdlog/spdlog.h>

namespace qdmi::test {
void QDMIOrchestrationLayerTest::SetUp() {
  QDMIProviderTest::SetUp();
  if (!IsSkipped() && !HasFatalFailure()) {
    ASSERT_NO_THROW(
        ol_ = std::get<const QDMI_OrchestrationLayer_Interface *>(provider_));
  }
}
TEST_P(QDMIOrchestrationLayerTest, JobSetDevice) {
  QDMI_Job job{};
  ASSERT_EQ(ol_->session_create_job(session_, Log_callback,
                                    spdlog::default_logger().get(), &job),
            QDMI_SUCCESS)
      << "Failed to create job.";
  EXPECT_EQ(ol_->job_set_device(job, nullptr), QDMI_ERROR_INVALIDARGUMENT);
  auto *device = all_devices().front();
  EXPECT_EQ(ol_->job_set_device(nullptr, device), QDMI_ERROR_INVALIDARGUMENT);
  EXPECT_EQ(ol_->job_set_device(job, device), QDMI_SUCCESS);
}
} // namespace qdmi::test
