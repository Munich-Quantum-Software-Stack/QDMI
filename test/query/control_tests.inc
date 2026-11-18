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

#include <gtest/gtest-spi.h>
TEST(QueryUnitSymbols, CanonicalSpellings) {
  constexpr std::array<std::string_view, 25> prefixes{
      "q", "r", "y", "z", "a", "f", "p", "n", "u", "m", "c", "d", "",
      "da", "h", "k", "M", "G", "T", "P", "E", "Z", "Y", "R", "Q"};
  for (const auto prefix : prefixes) {
    EXPECT_TRUE(Is_canonical_unit(std::string(prefix) + "m", 'm'));
    EXPECT_TRUE(Is_canonical_unit(std::string(prefix) + "s", 's'));
  }
  for (const auto unit : {"", "UM", "u m", "millimetre", "µm", "μm", "mmx"}) {
    EXPECT_FALSE(Is_canonical_unit(unit, 'm'));
  }
  EXPECT_FALSE(Is_canonical_unit(std::string_view("m\0x", 3), 'm'));
  EXPECT_FALSE(Is_canonical_unit("ms", 'm'));
  EXPECT_FALSE(Is_canonical_unit("mm", 's'));
}

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
  const auto zone = Read<bool>(
      Site_query(session, expected_sites[0], QDMI_SITE_PROPERTY_ISZONE));
  ASSERT_TRUE(zone);
  EXPECT_TRUE(*zone);
  const auto nonzone = Read<bool>([](size_t size, void *value, size_t *size_ret) {
    if (value != nullptr && size < sizeof(bool)) {
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    if (size_ret != nullptr) {
      *size_ret = sizeof(bool);
    }
    if (value != nullptr) {
      *static_cast<bool *>(value) = false;
    }
    return QDMI_SUCCESS;
  });
  ASSERT_TRUE(nonzone);
  EXPECT_FALSE(*nonzone);
}

TEST(QueryReader, MissingProbeSize) {
  const auto missing_size = [](size_t, void *, size_t *) { return QDMI_SUCCESS; };
  EXPECT_NONFATAL_FAILURE((void)Read<size_t>(missing_size), "size probe did not set");

}

TEST_P(QDMIQueryTest, RetrievalFailure) {
  const auto string_query = [](size_t size, void *value, size_t *size_ret) {
    if (value != nullptr && size < 1) {
      return QDMI_ERROR_INVALIDARGUMENT;
    }
    if (value == nullptr && size_ret != nullptr) {
      *size_ret = 1;
    }
    return QDMI_SUCCESS;
  };
  if (GetParam().return_size) {
    EXPECT_NONFATAL_FAILURE((void)Read<char>(string_query, false), "payload size changed");
  } else {
    EXPECT_NONFATAL_FAILURE((void)Read<char>(string_query, false), "string payload must end in NUL");
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
  EXPECT_TRUE(values->empty());
  EXPECT_EQ(probes, 3U);
  EXPECT_EQ(retrievals, 1U);
}

TEST_P(QDMIQueryTest, RetrievalVariant) {
  size_t retrievals = 0;
  const auto variant = GetParam();
  const auto query = [&](size_t size, void *value, size_t *size_ret) {
    if (value != nullptr && size < sizeof(size_t)) {
      EXPECT_EQ(size_ret != nullptr, variant.return_size);
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
