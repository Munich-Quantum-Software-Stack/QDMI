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

#include "utils/provider/test_provider.hpp"

#include "utils/job/test_job.hpp"
#include "utils/qpu/test_qpu.hpp"
#include "utils/superconducting/test_sc.hpp"

#include <gtest/gtest.h>

namespace qdmi::test {
INSTANTIATE_TEST_SUITE_P(
    // Custom instantiation name
    ExampleProvider,
    // Test suite name
    QDMITest,
    // Parameters to test with
    ::testing::ValuesIn(
        QDMITest::generate_test_params({{QDMI_EXAMPLE_DEVICE_PATH, "P"}})));

INSTANTIATE_TEST_SUITE_P(
    // Custom instantiation name
    ExampleProvider,
    // Test suite name
    QDMIModuleTest,
    // Parameters to test with
    ::testing::ValuesIn(QDMIModuleTest::generate_test_params(
        {{QDMI_EXAMPLE_DEVICE_PATH, "P"}}, {"provider"})));

INSTANTIATE_TEST_SUITE_P(
    // Custom instantiation name
    ExampleProvider,
    // Test suite name
    QDMIProviderTest,
    // Parameters to test with
    ::testing::ValuesIn(QDMIProviderTest::generate_test_params(
        {{QDMI_EXAMPLE_DEVICE_PATH, "P"}})));

INSTANTIATE_TEST_SUITE_P(
    // Custom instantiation name
    ExampleProviderQPU,
    // Test suite name
    QDMIModuleTest,
    // Parameters to test with
    ::testing::ValuesIn(QDMIModuleTest::generate_test_params(
        {{QDMI_EXAMPLE_DEVICE_PATH, "P"}}, {"qpu", "sc"}, {"q2"})));

INSTANTIATE_TEST_SUITE_P(
    // Custom instantiation name
    ExampleProviderQPU,
    // Test suite name
    QDMIQpuTest,
    // Parameters to test with
    ::testing::ValuesIn(QDMIQpuTest::generate_test_params(
        {{QDMI_EXAMPLE_DEVICE_PATH, "P"}}, {"q2"})));

INSTANTIATE_TEST_SUITE_P(
    // Custom instantiation name
    ExampleProviderQPU,
    // Test suite name
    QDMIJobTest,
    // Parameters to test with
    ::testing::ValuesIn(QDMIJobTest::generate_test_params(
        {{QDMI_EXAMPLE_DEVICE_PATH, "P"}}, {"q2"})));

INSTANTIATE_TEST_SUITE_P(
    // Custom instantiation name
    ExampleProviderQPU,
    // Test suite name
    QDMIScTest,
    // Parameters to test with
    ::testing::ValuesIn(QDMIScTest::generate_test_params(
        {{QDMI_EXAMPLE_DEVICE_PATH, "P"}}, {"q2"})));
} // namespace qdmi::test
