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

#include "qdmi/orchestration_layer.h"
#include "qdmi/qpu.h"
#include "utils/core/test_core.hpp"

namespace qdmi::test {
class QDMIJobTest : public QDMISessionTest {
protected:
  std::variant<const QDMI_QPU_Interface *,
               const QDMI_OrchestrationLayer_Interface *>
      job_interface_{};
  QDMI_Job job_ = nullptr;

  auto SetUp() -> void override;

  auto TearDown() -> void override;
};
} // namespace qdmi::test
