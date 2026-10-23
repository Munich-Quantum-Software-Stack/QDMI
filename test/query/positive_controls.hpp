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

// Included after the shared suite only in the fixed fake-provider executable.
TEST_F(QDMIQueryFixture, SuccessfulPayloadSnapshot) {
  const auto count =
      Read<size_t>(Device_query(session, QDMI_DEVICE_PROPERTY_QUBITSNUM));
  ASSERT_TRUE(count);
  EXPECT_EQ((*count)[0], size_t{0x11223344});

  const auto sites = Read_sites(session);
  ASSERT_TRUE(sites);
  std::array<Site, 3> expected_sites{};
  ASSERT_EQ(Device_query(session, QDMI_DEVICE_PROPERTY_SITES)(
                sizeof(expected_sites), expected_sites.data(), nullptr),
            QDMI_SUCCESS);
  EXPECT_EQ(*sites,
            std::vector<Site>(expected_sites.begin(), expected_sites.end()));

  const auto operations = Read_operations(session);
  ASSERT_TRUE(operations);
  std::array<Operation, 2> expected_operations{};
  ASSERT_EQ(
      Device_query(session, QDMI_DEVICE_PROPERTY_OPERATIONS)(
          sizeof(expected_operations), expected_operations.data(), nullptr),
      QDMI_SUCCESS);
  EXPECT_EQ(*operations, std::vector<Operation>(expected_operations.begin(),
                                                expected_operations.end()));

  // A one-byte bool advertises zero bytes in the undersized probe.
  const auto zone = Read_bool(
      Site_query(session, expected_sites[0], QDMI_SITE_PROPERTY_ISZONE));
  ASSERT_TRUE(zone);
  EXPECT_TRUE(*zone);
}
