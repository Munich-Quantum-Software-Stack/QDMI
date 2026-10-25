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
TEST_P(QDMIQueryTest, SuccessfulPayloadSnapshot) {
  const auto count =
      Read<size_t>(Device_query(session, QDMI_DEVICE_PROPERTY_QUBITSNUM));
  ASSERT_TRUE(count);
  EXPECT_EQ((*count)[0], size_t{0x11223344});

  const auto sites = Read_sites(session);
  ASSERT_TRUE(sites);
  std::array<Site, 3> expected_sites{};
  ASSERT_EQ(Device_query(session, QDMI_DEVICE_PROPERTY_SITES)(
                sizeof(expected_sites),
                static_cast<void *>(expected_sites.data()), nullptr),
            QDMI_SUCCESS);
  EXPECT_EQ(*sites,
            std::vector<Site>(expected_sites.begin(), expected_sites.end()));

  const auto operations = Read_operations(session);
  ASSERT_TRUE(operations);
  std::array<Operation, 2> expected_operations{};
  ASSERT_EQ(Device_query(session, QDMI_DEVICE_PROPERTY_OPERATIONS)(
                sizeof(expected_operations),
                static_cast<void *>(expected_operations.data()), nullptr),
            QDMI_SUCCESS);
  EXPECT_EQ(*operations, std::vector<Operation>(expected_operations.begin(),
                                                expected_operations.end()));

  // A one-byte bool advertises zero bytes in the undersized probe.
  const auto zone = Read_bool(
      Site_query(session, expected_sites[0], QDMI_SITE_PROPERTY_ISZONE));
  ASSERT_TRUE(zone);
  EXPECT_TRUE(*zone);
}

TEST_P(QDMIQueryTest, MissingArrayProbeSize) {
  const auto query = [](size_t, void *value, size_t *size_ret) {
    if (value != nullptr && size_ret != nullptr) {
      *size_ret = 0;
    }
    return QDMI_SUCCESS;
  };
  (void)Read<size_t>(query, false);
}

TEST_P(QDMIQueryTest, MissingBoolProbeSize) {
  const auto query = [](size_t size, void *value, size_t *size_ret) {
    if (value != nullptr && size < sizeof(bool)) {
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    if (size_ret != nullptr && value != nullptr) {
      *size_ret = sizeof(bool);
    }
    if (value != nullptr) {
      *static_cast<bool *>(value) = true;
    }
    return QDMI_SUCCESS;
  };
  (void)Read_bool(query);
}

TEST_P(QDMIQueryTest, MissingArrayRetrievalSize) {
  const auto query = [](size_t size, void *value, size_t *size_ret) {
    if (value != nullptr && size < sizeof(size_t)) {
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    if (size_ret != nullptr && value == nullptr) {
      *size_ret = sizeof(size_t);
    }
    if (value != nullptr) {
      *static_cast<size_t *>(value) = 1;
    }
    return QDMI_SUCCESS;
  };
  (void)Read<size_t>(query, false);
}

TEST_P(QDMIQueryTest, MissingBoolRetrievalSize) {
  const auto query = [](size_t size, void *value, size_t *size_ret) {
    if (value != nullptr && size < sizeof(bool)) {
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    if (size_ret != nullptr && value == nullptr) {
      *size_ret = sizeof(bool);
    }
    if (value != nullptr) {
      *static_cast<bool *>(value) = true;
    }
    return QDMI_SUCCESS;
  };
  (void)Read_bool(query);
}

TEST_P(QDMIQueryTest, MissingTerminator) {
  const auto variant = GetParam();
  const auto query = [variant](size_t size, void *value, size_t *size_ret) {
    if (value != nullptr && size < 4) {
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    if (size_ret != nullptr) {
      *size_ret = 4;
    }
    if (value != nullptr) {
      auto *text = static_cast<char *>(value);
      std::fill_n(text, 3, 'a');
      if (size != (variant.oversized ? 5U : 4U) ||
          (size_ret != nullptr) != variant.return_size) {
        text[3] = '\0';
      }
    }
    return QDMI_SUCCESS;
  };
  (void)Read<char>(query, false);
}

TEST_P(QDMIQueryTest, InvalidStatus) {
  const auto variant = GetParam();
  const auto query = [variant](size_t size, void *value, size_t *size_ret) {
    if (value != nullptr && size < sizeof(QDMI_Device_Status)) {
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    if (size_ret != nullptr) {
      *size_ret = sizeof(QDMI_Device_Status);
    }
    if (value != nullptr) {
      const bool selected =
          size == sizeof(QDMI_Device_Status) * (variant.oversized ? 2U : 1U) &&
          (size_ret != nullptr) == variant.return_size;
      *static_cast<QDMI_Device_Status *>(value) =
          selected ? QDMI_DEVICE_STATUS_MAX : QDMI_DEVICE_STATUS_IDLE;
    }
    return QDMI_SUCCESS;
  };
  const auto status = Read<QDMI_Device_Status>(query);
  if (status) {
    EXPECT_LT((*status)[0], QDMI_DEVICE_STATUS_MAX)
        << "device status must be a defined status";
  }
}

TEST_P(QDMIQueryTest, EmptyArrayPayload) {
  size_t probes = 0;
  size_t retrievals = 0;
  const auto query = [&](size_t, void *value, size_t *size_ret) {
    if (value == nullptr) {
      ++probes;
    } else {
      ++retrievals;
    }
    if (size_ret != nullptr) {
      *size_ret = 0;
    }
    return QDMI_SUCCESS;
  };
  const auto values = Read<size_t>(query, false);
  ASSERT_TRUE(values);
  EXPECT_TRUE(values && values->empty());
  EXPECT_EQ(probes, 3U);
  EXPECT_EQ(retrievals, 1U);
}

TEST_P(QDMIQueryTest, RetrievalVariant) {
  size_t retrievals = 0;
  const auto variant = GetParam();
  const auto query = [&](size_t size, void *value, size_t *size_ret) {
    if (value != nullptr && size < sizeof(size_t)) {
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    if (size_ret != nullptr) {
      *size_ret = sizeof(size_t);
    }
    if (value != nullptr) {
      ++retrievals;
      EXPECT_EQ(size, sizeof(size_t) * (variant.oversized ? 2U : 1U));
      EXPECT_EQ(size_ret != nullptr, variant.return_size);
      *static_cast<size_t *>(value) = retrievals;
    }
    return QDMI_SUCCESS;
  };
  const auto first = Read<size_t>(query);
  const auto second = Read<size_t>(query);
  ASSERT_TRUE(first);
  ASSERT_TRUE(second);
  EXPECT_EQ((*first)[0], 1U);
  EXPECT_EQ((*second)[0], 2U);
}
