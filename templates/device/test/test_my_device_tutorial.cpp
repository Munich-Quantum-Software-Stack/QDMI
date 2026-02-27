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

#include "my_qdmi/device.h"

#include <gtest/gtest.h>

class QDMIBaseTest : public ::testing::Test {
protected:
  void SetUp() override {
    ASSERT_EQ(MY_QDMI_device_initialize(), QDMI_SUCCESS)
        << "Checkpoint Init Failed: Basic device initialization returned an "
           "error.";
  }
  void TearDown() override { MY_QDMI_device_finalize(); }
};

class QDMISessionTest : public QDMIBaseTest {
protected:
  MY_QDMI_Device_Session session = nullptr;
  void SetUp() override {
    QDMIBaseTest::SetUp();
    ASSERT_EQ(MY_QDMI_device_session_alloc(&session), QDMI_SUCCESS)
        << "Checkpoint 1 Failed: Could not allocate a session handle.";
  }
  void TearDown() override {
    if (session)
      MY_QDMI_device_session_free(session);
    QDMIBaseTest::TearDown();
  }
};

class QDMIInitializedSessionTest : public QDMISessionTest {
protected:
  void SetUp() override {
    QDMISessionTest::SetUp();
    const std::string token = "MY_TOKEN";
    ASSERT_EQ(MY_QDMI_device_session_set_parameter(
                  session, QDMI_DEVICE_SESSION_PARAMETER_TOKEN, token.size(),
                  token.c_str()),
              QDMI_SUCCESS);
    ASSERT_EQ(MY_QDMI_device_session_init(session), QDMI_SUCCESS)
        << "Checkpoint 2 Failed: Session initialization failed.";
  }
};

TEST_F(QDMIBaseTest, Initialization) {
  // Checkpoint Init: Verified by SetUp/TearDown
}

TEST_F(QDMISessionTest, Allocation) {
  // Checkpoint 1: Verified by SetUp/TearDown
}

TEST_F(QDMIInitializedSessionTest, Initialization) {
  // Checkpoint 2: Verified by SetUp/TearDown
}

TEST_F(QDMISessionTest, QueryBeforeInit) {
  // Querying properties on an uninitialised session must return BADSTATE
  size_t size = 0;
  EXPECT_EQ(MY_QDMI_device_session_query_device_property(
                session, QDMI_DEVICE_PROPERTY_NAME, 0, nullptr, &size),
            QDMI_ERROR_BADSTATE);
}

TEST_F(QDMIInitializedSessionTest, QueryProperties) {
  size_t size = 0;

  // Null session must return INVALIDARGUMENT
  EXPECT_EQ(MY_QDMI_device_session_query_device_property(
                nullptr, QDMI_DEVICE_PROPERTY_NAME, 0, nullptr, &size),
            QDMI_ERROR_INVALIDARGUMENT);

  // First call: retrieve the required buffer size
  ASSERT_EQ(MY_QDMI_device_session_query_device_property(
                session, QDMI_DEVICE_PROPERTY_NAME, 0, nullptr, &size),
            QDMI_SUCCESS)
      << "Checkpoint 3 Failed: Device failed to report name size.";

  // Buffer too small must return INVALIDARGUMENT
  std::string small_buffer(size > 1 ? size - 1 : 0, '\0');
  if (size > 1) {
    EXPECT_EQ(MY_QDMI_device_session_query_device_property(
                  session, QDMI_DEVICE_PROPERTY_NAME, small_buffer.size(),
                  small_buffer.data(), nullptr),
              QDMI_ERROR_INVALIDARGUMENT);
  }

  // Second call: retrieve the actual name
  std::string value(size, '\0');
  ASSERT_EQ(
      MY_QDMI_device_session_query_device_property(
          session, QDMI_DEVICE_PROPERTY_NAME, size, value.data(), nullptr),
      QDMI_SUCCESS);
  EXPECT_GT(value.size(), 0)
      << "Checkpoint 3 Failed: Name should not be empty.";

  // Unsupported property must return NOTSUPPORTED
  EXPECT_EQ(MY_QDMI_device_session_query_device_property(
                session, QDMI_DEVICE_PROPERTY_MAX, 0, nullptr, &size),
            QDMI_ERROR_NOTSUPPORTED);
}

TEST_F(QDMIInitializedSessionTest, SubmitAndSimulateJob) {
  MY_QDMI_Device_Job job = nullptr;

  // Null session must return INVALIDARGUMENT
  EXPECT_EQ(MY_QDMI_device_session_create_device_job(nullptr, &job),
            QDMI_ERROR_INVALIDARGUMENT);
  // Null job-pointer must return INVALIDARGUMENT
  EXPECT_EQ(MY_QDMI_device_session_create_device_job(session, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);

  ASSERT_EQ(MY_QDMI_device_session_create_device_job(session, &job),
            QDMI_SUCCESS)
      << "Checkpoint 4 Failed: Could not create a device job.";

  const std::string qasm = "OPENQASM 2.0; qreg q[1]; h q[0];";

  // Null job must return INVALIDARGUMENT
  EXPECT_EQ(MY_QDMI_device_job_set_parameter(nullptr,
                                             QDMI_DEVICE_JOB_PARAMETER_PROGRAM,
                                             qasm.size(), qasm.c_str()),
            QDMI_ERROR_INVALIDARGUMENT);
  // Null value with size==0 must return INVALIDARGUMENT
  EXPECT_EQ(MY_QDMI_device_job_set_parameter(
                job, QDMI_DEVICE_JOB_PARAMETER_PROGRAM, 0, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
  // Unsupported parameter must return NOTSUPPORTED
  EXPECT_EQ(MY_QDMI_device_job_set_parameter(job, QDMI_DEVICE_JOB_PARAMETER_MAX,
                                             qasm.size(), qasm.c_str()),
            QDMI_ERROR_NOTSUPPORTED);

  // Submitting without a program set must return BADSTATE
  EXPECT_EQ(MY_QDMI_device_job_submit(job), QDMI_ERROR_BADSTATE);

  ASSERT_EQ(MY_QDMI_device_job_set_parameter(job,
                                             QDMI_DEVICE_JOB_PARAMETER_PROGRAM,
                                             qasm.size(), qasm.c_str()),
            QDMI_SUCCESS);

  // Null job must return INVALIDARGUMENT
  EXPECT_EQ(MY_QDMI_device_job_submit(nullptr), QDMI_ERROR_INVALIDARGUMENT);

  ASSERT_EQ(MY_QDMI_device_job_submit(job), QDMI_SUCCESS)
      << "Checkpoint 4 Failed: Job submission failed.";

  // Re-submission after completion must return BADSTATE
  EXPECT_EQ(MY_QDMI_device_job_submit(job), QDMI_ERROR_BADSTATE);

  QDMI_Job_Status status;
  // Null job must return INVALIDARGUMENT
  EXPECT_EQ(MY_QDMI_device_job_check(nullptr, &status),
            QDMI_ERROR_INVALIDARGUMENT);
  // Null status pointer must return INVALIDARGUMENT
  EXPECT_EQ(MY_QDMI_device_job_check(job, nullptr), QDMI_ERROR_INVALIDARGUMENT);
  ASSERT_EQ(MY_QDMI_device_job_check(job, &status), QDMI_SUCCESS);
  EXPECT_EQ(status, QDMI_JOB_STATUS_DONE);

  double probs[2];
  // Null job must return INVALIDARGUMENT
  EXPECT_EQ(MY_QDMI_device_job_get_results(nullptr,
                                           QDMI_JOB_RESULT_PROBABILITIES_DENSE,
                                           sizeof(probs), probs, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
  // Buffer too small must return INVALIDARGUMENT
  EXPECT_EQ(MY_QDMI_device_job_get_results(job,
                                           QDMI_JOB_RESULT_PROBABILITIES_DENSE,
                                           sizeof(probs) - 1, probs, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
  // Unsupported result type must return NOTSUPPORTED
  EXPECT_EQ(MY_QDMI_device_job_get_results(job, QDMI_JOB_RESULT_MAX,
                                           sizeof(probs), probs, nullptr),
            QDMI_ERROR_NOTSUPPORTED);

  ASSERT_EQ(MY_QDMI_device_job_get_results(job,
                                           QDMI_JOB_RESULT_PROBABILITIES_DENSE,
                                           sizeof(probs), probs, nullptr),
            QDMI_SUCCESS)
      << "Checkpoint 4 Failed: Could not retrieve simulated job results.";

  MY_QDMI_device_job_free(job);
}
