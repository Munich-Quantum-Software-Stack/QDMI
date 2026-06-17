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

#include "test_job.hpp"

#include "qdmi/core.h"

#include <cstddef>
#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

namespace qdmi::test {
auto QDMIJobTest::SetUp() -> void {
  QDMISessionTest::SetUp();
  if (!IsSkipped() && !HasFatalFailure()) {
    QDMI_Module mod{};
    if (const auto ret =
            qdmi_->context_query_module_by_id(context_, "ol", &mod);
        ret == QDMI_SUCCESS) {
      const QDMI_OrchestrationLayer_Interface *ol{};
      ASSERT_EQ(
          qdmi_->context_get_module_interface(
              // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
              context_, mod, reinterpret_cast<const void **>(&ol)),
          QDMI_SUCCESS)
          << "Failed to get orchestration layer interface.";
      job_interface_ = ol;
    } else {
      ASSERT_EQ(ret, QDMI_ERROR_NOTFOUND)
          << "Failed to find orchestration layer module.";
      ASSERT_EQ(qdmi_->context_query_module_by_id(context_, "qpu", &mod),
                QDMI_SUCCESS)
          << "Failed to get QPU or orchestration layer module";
      const QDMI_QPU_Interface *qpu{};
      ASSERT_EQ(
          qdmi_->context_get_module_interface(
              // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
              context_, mod, reinterpret_cast<const void **>(&qpu)),
          QDMI_SUCCESS)
          << "Failed to get orchestration layer interface.";
      job_interface_ = qpu;
    }
  }
  std::visit(
      [this](const auto *interface) -> void {
        if (interface != nullptr) {
          interface->session_create_job(session_, Log_callback,
                                        spdlog::default_logger().get(), &job_);
        }
      },
      job_interface_);
}
void QDMIJobTest::TearDown() {
  if (job_ != nullptr) {
    std::visit(
        [this](const auto *interface) -> void {
          if (interface != nullptr) {
            interface->job_free(job_);
          }
        },
        job_interface_);
  }
  QDMISessionTest::TearDown();
}
TEST_P(QDMIJobTest, QueryProgramFormats) {
  EXPECT_EQ(std::visit(
                [](const auto *interface) {
                  return interface->session_query_program_formats(
                      nullptr, 0, nullptr, nullptr);
                },
                job_interface_),
            QDMI_ERROR_INVALIDARGUMENT);
  EXPECT_EQ(
      std::visit(
          [](const auto *interface) {
            return interface->session_query_program_format_by_id_and_version(
                nullptr, "", 0, nullptr);
          },
          job_interface_),
      QDMI_ERROR_INVALIDARGUMENT);
  EXPECT_EQ(std::visit(
                [](const auto *interface) {
                  return interface->session_query_program_format_id(
                      nullptr, nullptr, 0, nullptr, nullptr);
                },
                job_interface_),
            QDMI_ERROR_INVALIDARGUMENT);
  EXPECT_EQ(std::visit(
                [this](const auto *interface) {
                  return interface->session_query_program_formats(
                      session_, 0, nullptr, nullptr);
                },
                job_interface_),
            QDMI_SUCCESS);
  size_t size = 0;
  ASSERT_EQ(std::visit(
                [this, &size](const auto *interface) {
                  return interface->session_query_program_formats(
                      session_, 0, nullptr, &size);
                },
                job_interface_),
            QDMI_SUCCESS);
  std::vector<QDMI_Program_Format> formats(size, nullptr);
  EXPECT_EQ(std::visit(
                [this, &formats](const auto *interface) {
                  return interface->session_query_program_formats(
                      session_, 0, formats.data(), nullptr);
                },
                job_interface_),
            QDMI_ERROR_INVALIDARGUMENT);
  ASSERT_EQ(std::visit(
                [this, size, &formats](const auto *interface) {
                  return interface->session_query_program_formats(
                      session_, size, formats.data(), nullptr);
                },
                job_interface_),
            QDMI_SUCCESS);
  EXPECT_FALSE(formats.empty()) << "Device must provide a format.";
  for (auto *expected_format : formats) {
    EXPECT_EQ(std::visit(
                  [expected_format](const auto *interface) {
                    return interface->session_query_program_format_id(
                        nullptr, expected_format, 0, nullptr, nullptr);
                  },
                  job_interface_),
              QDMI_ERROR_INVALIDARGUMENT);
    ASSERT_EQ(std::visit(
                  [this, expected_format](const auto *interface) {
                    return interface->session_query_program_format_id(
                        session_, expected_format, 0, nullptr, nullptr);
                  },
                  job_interface_),
              QDMI_SUCCESS);
    size_t id_size = 0;
    ASSERT_EQ(std::visit(
                  [this, expected_format, &id_size](const auto *interface) {
                    return interface->session_query_program_format_id(
                        session_, expected_format, 0, nullptr, &id_size);
                  },
                  job_interface_),
              QDMI_SUCCESS);
    std::string id(id_size - 1, '\0');
    EXPECT_EQ(std::visit(
                  [this, expected_format, &id](const auto *interface) {
                    return interface->session_query_program_format_id(
                        session_, expected_format, 0, id.data(), nullptr);
                  },
                  job_interface_),
              QDMI_ERROR_INVALIDARGUMENT);
    ASSERT_EQ(std::visit(
                  [this, expected_format, id_size, &id](const auto *interface) {
                    return interface->session_query_program_format_id(
                        session_, expected_format, id_size, id.data(), nullptr);
                  },
                  job_interface_),
              QDMI_SUCCESS);
    EXPECT_FALSE(id.empty()) << "Formats must provide an id.";
    EXPECT_EQ(std::visit(
                  [this, expected_format](const auto *interface) {
                    return interface->session_query_program_format_version(
                        session_, expected_format, nullptr);
                  },
                  job_interface_),
              QDMI_SUCCESS);
    EXPECT_EQ(std::visit(
                  [&expected_format](const auto *interface) {
                    return interface->session_query_program_format_version(
                        nullptr, expected_format, nullptr);
                  },
                  job_interface_),
              QDMI_ERROR_INVALIDARGUMENT);
    size_t version = 0;
    ASSERT_EQ(std::visit(
                  [this, &expected_format, &version](const auto *interface) {
                    return interface->session_query_program_format_version(
                        session_, expected_format, &version);
                  },
                  job_interface_),
              QDMI_SUCCESS);
    EXPECT_EQ(
        std::visit(
            [this, &id, version](const auto *interface) {
              return interface->session_query_program_format_by_id_and_version(
                  session_, id.c_str(), version, nullptr);
            },
            job_interface_),
        QDMI_SUCCESS);
    QDMI_Program_Format actual_format = nullptr;
    ASSERT_EQ(
        std::visit(
            [this, &id, version, &actual_format](const auto *interface) {
              return interface->session_query_program_format_by_id_and_version(
                  session_, id.c_str(), version, &actual_format);
            },
            job_interface_),
        QDMI_SUCCESS);
    EXPECT_EQ(actual_format, expected_format);
  }
}
TEST_P(QDMIJobTest, CreateJob) {
  EXPECT_EQ(std::visit(
                [this](const auto *interface) {
                  return interface->session_create_job(session_, nullptr,
                                                       nullptr, nullptr);
                },
                job_interface_),
            QDMI_ERROR_INVALIDARGUMENT);
  QDMI_Job job = nullptr;
  EXPECT_EQ(std::visit(
                [&job](const auto *interface) {
                  return interface->session_create_job(nullptr, nullptr,
                                                       nullptr, &job);
                },
                job_interface_),
            QDMI_ERROR_INVALIDARGUMENT);
}
TEST_P(QDMIJobTest, JobSubmit) {
  EXPECT_THAT(
      std::visit(
          [this](const auto *interface) { return interface->job_submit(job_); },
          job_interface_),
      ::testing::AnyOf(QDMI_SUCCESS, QDMI_ERROR_BADSTATE,
                       QDMI_ERROR_NOTSUPPORTED));
}
TEST_P(QDMIJobTest, JobCancel) {
  EXPECT_THAT(
      std::visit(
          [this](const auto *interface) { return interface->job_cancel(job_); },
          job_interface_),
      ::testing::AnyOf(QDMI_ERROR_BADSTATE, QDMI_ERROR_NOTSUPPORTED));
}
TEST_P(QDMIJobTest, JobCheck) {
  QDMI_Job_Status status = QDMI_JOB_STATUS_RUNNING;
  EXPECT_EQ(std::visit(
                [&status](const auto *interface) {
                  return interface->job_check(nullptr, &status);
                },
                job_interface_),
            QDMI_ERROR_INVALIDARGUMENT);
  EXPECT_EQ(std::visit(
                [this](const auto *interface) {
                  return interface->job_check(job_, nullptr);
                },
                job_interface_),
            QDMI_ERROR_INVALIDARGUMENT);
  int ret{};
  EXPECT_THAT(ret = std::visit(
                  [this, &status](const auto *interface) {
                    return interface->job_check(job_, &status);
                  },
                  job_interface_),
              ::testing::AnyOf(QDMI_SUCCESS, QDMI_ERROR_NOTSUPPORTED));
  if (ret == QDMI_SUCCESS) {
    EXPECT_EQ(status, QDMI_JOB_STATUS_CREATED);
  }
}
TEST_P(QDMIJobTest, JobWait) {
  EXPECT_EQ(
      std::visit(
          [](const auto *interface) { return interface->job_wait(nullptr, 0); },
          job_interface_),
      QDMI_ERROR_INVALIDARGUMENT);
  EXPECT_THAT(std::visit(
                  [this](const auto *interface) {
                    return interface->job_wait(job_, 0);
                  },
                  job_interface_),
              ::testing::AnyOf(QDMI_ERROR_BADSTATE, QDMI_ERROR_NOTSUPPORTED));
}
} // namespace qdmi::test
