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

#include "common/library_wrapper/library_wrapper.hpp"
#include "common/session_wrapper/session_wrapper.hpp"
#include "qdmi/core.h"

#include <gtest/gtest.h>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

namespace qdmi::test {
struct TestParam {
  std::string lib_path{};
  std::string lib_prefix{};
  std::string dev_id{};
  std::string mod_id{};

  // For pretty printing test names
  explicit operator std::string() const;
  friend auto operator<<(std::ostream &os, const TestParam &param)
      -> std::ostream &;
};

auto Log_callback(QDMI_Log_Level level, const char *message, void *user_data)
    -> void;

class QDMITest : public ::testing::TestWithParam<TestParam> {
protected:
  LibraryWrapper library_{};
  QDMI_Context context_{};
  const QDMI_Core_Interface *qdmi_{};

  QDMITest();

  auto SetUp() -> void override;

public:
  [[nodiscard]] static auto generate_test_params(
      const std::vector<std::pair<std::string, std::string>> &libs)
      -> std::vector<TestParam>;
};

class QDMISessionTest : public QDMITest {
protected:
  std::optional<SessionWrapper> parent_session_wrapper_{};
  SessionWrapper session_wrapper_{};
  QDMI_Session session_{};

  auto SetUp() -> void override;

public:
  [[nodiscard]] static auto generate_test_params(
      const std::vector<std::pair<std::string, std::string>> &libs,
      const std::vector<std::string> &dev_ids = {}) -> std::vector<TestParam>;
};

class QDMIModuleTest : public QDMITest {
protected:
  std::optional<SessionWrapper> session_wrapper_{};
  QDMI_Module module_{};

  auto SetUp() -> void override;

public:
  [[nodiscard]] static auto generate_test_params(
      const std::vector<std::pair<std::string, std::string>> &libs,
      const std::vector<std::string> &mod_ids,
      const std::vector<std::string> &dev_ids = {}) -> std::vector<TestParam>;
};
} // namespace qdmi::test
