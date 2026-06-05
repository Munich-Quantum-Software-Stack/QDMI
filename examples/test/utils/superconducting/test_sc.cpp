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

#include "test_sc.hpp"

#include "qdmi/core.h"
#include "qdmi/superconducting.h"

#include <algorithm>
#include <cstddef>
#include <functional>
#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

namespace testing {
namespace {
auto String_concat5(const std::string &a, const std::string &b,
                    const std::string &c, const std::string &d,
                    const std::string &e) -> std::string {
  std::stringstream ss;
  ss << a << b << c << d << e;
  return ss.str();
}
// NOLINTBEGIN(readability-identifier-naming,cppcoreguidelines-avoid-const-or-ref-data-members)
MATCHER_P2(IsBetween, a, b,
           String_concat5(negation ? "isn't" : "is", " between ",
                          PrintToString(a), " and ", PrintToString(b))) {
  return a <= arg && arg <= b;
}
// NOLINTEND(readability-identifier-naming,cppcoreguidelines-avoid-const-or-ref-data-members)
} // namespace
} // namespace testing

namespace qdmi::test {
namespace {
// Hash/equality for std::array<QDMI_SCQubit, 2>
struct Qubit_pair_hash {
  std::size_t operator()(const std::array<QDMI_SCQubit, 2> &a) const noexcept {
    const auto h1 = std::hash<void *>()(a.front());
    const auto h2 = std::hash<void *>()(a.back());
    return h1 ^ h2;
  }
};
} // namespace

auto QDMIScTest::all_qubits() const -> std::vector<QDMI_SCQubit> {
  size_t size = 0;
  if (qdmi_sc_->session_query_sc_qubits(session_, 0, nullptr, &size) !=
      QDMI_SUCCESS) {
    return {};
  }
  std::vector<QDMI_SCQubit> qubits(size, nullptr);
  if (qdmi_sc_->session_query_sc_qubits(session_, size, qubits.data(),
                                        nullptr) != QDMI_SUCCESS) {
    return {};
  }
  return qubits;
}
auto QDMIScTest::all_operations() const -> std::vector<QDMI_SCOperation> {
  size_t size = 0;
  if (qdmi_sc_->session_query_sc_operations(session_, 0, nullptr, &size) !=
      QDMI_SUCCESS) {
    return {};
  }
  std::vector<QDMI_SCOperation> operations(size, nullptr);
  if (qdmi_sc_->session_query_sc_operations(session_, size, operations.data(),
                                            nullptr) != QDMI_SUCCESS) {
    return {};
  }
  return operations;
}
void QDMIScTest::SetUp() {
  QDMISessionTest::SetUp();
  if (!IsSkipped() && !HasFatalFailure()) {
    QDMI_Module ext = nullptr;
    ASSERT_EQ(qdmi_->context_query_module_by_id(context_, "sc", &ext),
              QDMI_SUCCESS)
        << "Failed to get superconducting module.";
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    ASSERT_EQ(qdmi_->context_get_module_interface(
                  context_, ext, reinterpret_cast<const void **>(&qdmi_sc_)),
              QDMI_SUCCESS)
        << "Failed to get superconducting interface.";
  }
}
TEST_P(QDMIScTest, QueryCouplingMap) {
  size_t size = 0;
  EXPECT_EQ(qdmi_sc_->session_query_coupling_map(nullptr, 0, nullptr, &size),
            QDMI_ERROR_INVALIDARGUMENT);
  ASSERT_EQ(qdmi_sc_->session_query_coupling_map(session_, 0, nullptr, &size),
            QDMI_SUCCESS);
  std::vector<std::array<QDMI_SCQubit, 2>> cm(size, {nullptr, nullptr});
  ASSERT_EQ(
      qdmi_sc_->session_query_coupling_map(
          session_, size, reinterpret_cast<QDMI_SCQubit *>(cm.data()), nullptr),
      QDMI_SUCCESS);
  EXPECT_EQ(cm.size() % 2, 0)
      << "Coupling map must consist of pairs of qubits.";
  const auto &qubit_vector = all_qubits();
  const std::unordered_set all_qubits(qubit_vector.begin(), qubit_vector.end());
  for (const auto [q1, q2] : cm) {
    for (auto *const q : {q1, q2}) {
      EXPECT_TRUE(all_qubits.contains(q));
    }
  }
}
TEST_P(QDMIScTest, QueryQubitProperties) {
  EXPECT_EQ(qdmi_sc_->session_query_sc_qubits(nullptr, 0, nullptr, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
  EXPECT_EQ(qdmi_sc_->session_query_sc_qubits(session_, 0, nullptr, nullptr),
            QDMI_SUCCESS);
  for (const auto expected_qubit : all_qubits()) {
    EXPECT_EQ(qdmi_sc_->session_query_sc_qubit_index(nullptr, expected_qubit,
                                                     nullptr),
              QDMI_ERROR_INVALIDARGUMENT);
    EXPECT_EQ(
        qdmi_sc_->session_query_sc_qubit_index(session_, nullptr, nullptr),
        QDMI_ERROR_INVALIDARGUMENT);
    size_t index = 0;
    ASSERT_EQ(qdmi_sc_->session_query_sc_qubit_index(session_, expected_qubit,
                                                     &index),
              QDMI_SUCCESS);
    QDMI_SCQubit actual_qubit = nullptr;
    ASSERT_EQ(qdmi_sc_->session_query_sc_qubit_by_index(session_, index,
                                                        &actual_qubit),
              QDMI_SUCCESS);
    EXPECT_EQ(actual_qubit, expected_qubit);
    EXPECT_THAT(
        qdmi_sc_->session_query_sc_qubit_name(nullptr, expected_qubit, 0,
                                              nullptr, nullptr),
        ::testing::AnyOf(QDMI_ERROR_INVALIDARGUMENT, QDMI_ERROR_NOTSUPPORTED));
    EXPECT_THAT(
        qdmi_sc_->session_query_sc_qubit_name(session_, nullptr, 0, nullptr,
                                              nullptr),
        ::testing::AnyOf(QDMI_ERROR_INVALIDARGUMENT, QDMI_ERROR_NOTSUPPORTED));
    size_t name_size = 0;
    const auto ret = qdmi_sc_->session_query_sc_qubit_name(
        session_, expected_qubit, 0, nullptr, &name_size);
    ASSERT_THAT(ret, ::testing::AnyOf(QDMI_SUCCESS, QDMI_ERROR_NOTSUPPORTED));
    if (ret == QDMI_SUCCESS) {
      std::string name(name_size - 1, '\0');
      ASSERT_EQ(qdmi_sc_->session_query_sc_qubit_name(
                    session_, expected_qubit, name_size, name.data(), nullptr),
                QDMI_SUCCESS);
    }
    EXPECT_EQ(qdmi_sc_->session_query_sc_qubit_coherence_t1(
                  nullptr, expected_qubit, nullptr),
              QDMI_ERROR_INVALIDARGUMENT);
    EXPECT_EQ(qdmi_sc_->session_query_sc_qubit_coherence_t1(session_, nullptr,
                                                            nullptr),
              QDMI_ERROR_INVALIDARGUMENT);
    uint64_t t1{};
    ASSERT_EQ(qdmi_sc_->session_query_sc_qubit_coherence_t1(
                  session_, expected_qubit, &t1),
              QDMI_SUCCESS);
    EXPECT_GT(t1, 0) << "Qubits must provide a positive T1 time.";
    EXPECT_EQ(qdmi_sc_->session_query_sc_qubit_coherence_t2(
                  nullptr, expected_qubit, nullptr),
              QDMI_ERROR_INVALIDARGUMENT);
    EXPECT_EQ(qdmi_sc_->session_query_sc_qubit_coherence_t2(session_, nullptr,
                                                            nullptr),
              QDMI_ERROR_INVALIDARGUMENT);
    uint64_t t2{};
    ASSERT_EQ(qdmi_sc_->session_query_sc_qubit_coherence_t2(
                  session_, expected_qubit, &t2),
              QDMI_SUCCESS);
    EXPECT_GT(t2, 0) << "Qubits must provide a positive T2 time.";
  }
}
TEST_P(QDMIScTest, QueryOperationProperties) {
  EXPECT_EQ(qdmi_sc_->session_query_sc_qubits(nullptr, 0, nullptr, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
  EXPECT_EQ(qdmi_sc_->session_query_sc_qubits(session_, 0, nullptr, nullptr),
            QDMI_SUCCESS);
  const auto &ops = all_operations();
  if (ops.empty()) {
    GTEST_SKIP() << "QPU provides no operations.";
  }
  const auto &qubits = all_qubits();
  if (qubits.empty()) {
    GTEST_SKIP() << "QPU provides no qubits.";
  }
  auto *const first_op = ops.front();
  EXPECT_EQ(qdmi_sc_->session_query_sc_operation_id(nullptr, first_op, 0,
                                                    nullptr, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
  EXPECT_EQ(qdmi_sc_->session_query_sc_operation_id(session_, nullptr, 0,
                                                    nullptr, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
  EXPECT_EQ(qdmi_sc_->session_query_sc_operation_id(session_, first_op, 0,
                                                    nullptr, nullptr),
            QDMI_SUCCESS);
  EXPECT_EQ(qdmi_sc_->session_query_sc_operation_name(nullptr, first_op, 0,
                                                      nullptr, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
  EXPECT_EQ(qdmi_sc_->session_query_sc_operation_name(session_, nullptr, 0,
                                                      nullptr, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
  EXPECT_EQ(qdmi_sc_->session_query_sc_operation_name(session_, first_op, 0,
                                                      nullptr, nullptr),
            QDMI_SUCCESS);
  EXPECT_EQ(qdmi_sc_->session_query_sc_operation_qubit_count(nullptr, first_op,
                                                             nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
  EXPECT_EQ(qdmi_sc_->session_query_sc_operation_qubit_count(session_, nullptr,
                                                             nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
  EXPECT_EQ(qdmi_sc_->session_query_sc_operation_qubit_count(session_, first_op,
                                                             nullptr),
            QDMI_SUCCESS);
  EXPECT_EQ(qdmi_sc_->session_query_sc_operation_qubits(nullptr, first_op, 0,
                                                        nullptr, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
  EXPECT_EQ(qdmi_sc_->session_query_sc_operation_qubits(session_, nullptr, 0,
                                                        nullptr, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
  EXPECT_EQ(qdmi_sc_->session_query_sc_operation_qubits(session_, first_op, 0,
                                                        nullptr, nullptr),
            QDMI_SUCCESS);
  size_t size{};
  for (auto *const op : ops) {
    ASSERT_EQ(qdmi_sc_->session_query_sc_operation_id(session_, op, 0, nullptr,
                                                      &size),
              QDMI_SUCCESS)
        << "Operations must provide an id.";
    std::string id(size - 1, '\0');
    ASSERT_EQ(qdmi_sc_->session_query_sc_operation_id(session_, op, size,
                                                      id.data(), nullptr),
              QDMI_SUCCESS)
        << "Operations must provide an id.";
    EXPECT_FALSE(id.empty()) << "Operations must provide an id.";
    ASSERT_EQ(qdmi_sc_->session_query_sc_operation_name(session_, op, 0,
                                                        nullptr, &size),
              QDMI_SUCCESS)
        << "Operations must provide a name.";
    std::string name(size - 1, '\0');
    ASSERT_EQ(qdmi_sc_->session_query_sc_operation_name(session_, op, size,
                                                        name.data(), nullptr),
              QDMI_SUCCESS)
        << "Operations must provide a name.";
    EXPECT_FALSE(name.empty()) << "Operations must provide a name.";
    size_t qubit_count{};
    ASSERT_EQ(qdmi_sc_->session_query_sc_operation_qubit_count(session_, op,
                                                               &qubit_count),
              QDMI_SUCCESS)
        << "Operations must provide the number of qubits.";
    ASSERT_EQ(qdmi_sc_->session_query_sc_operation_qubits(session_, op, 0,
                                                          nullptr, &size),
              QDMI_SUCCESS)
        << "Operations must provide the qubits.";
    if (qubit_count == 1) {
      std::vector<QDMI_SCQubit> op_qubits(size, nullptr);
      ASSERT_EQ(qdmi_sc_->session_query_sc_operation_qubits(
                    session_, op, size, op_qubits.data(), nullptr),
                QDMI_SUCCESS)
          << "Operations must provide the qubits.";
      const std::unordered_set op_qubit_set(op_qubits.begin(), op_qubits.end());
      for (auto *qubit : qubits) {
        double fidelity{};
        uint64_t duration{};
        if (op_qubit_set.contains(qubit)) {
          ASSERT_EQ(qdmi_sc_->session_query_sc_operation_fidelity(
                        session_, op, &qubit, nullptr, &fidelity),
                    QDMI_SUCCESS)
              << "Operations must provide fidelity for qubit operations.";
          EXPECT_THAT(fidelity, ::testing::IsBetween(0, 1))
              << "Fidelity must be between 0 and 1 for operation " << id << ".";
          ASSERT_EQ(qdmi_sc_->session_query_sc_operation_duration(
                        session_, op, &qubit, nullptr, &duration),
                    QDMI_SUCCESS)
              << "Operations must provide a duration for qubit operations.";
          EXPECT_GT(duration, 0)
              << "Duration must be positive for operation " << id << ".";
        } else {
          EXPECT_EQ(qdmi_sc_->session_query_sc_operation_fidelity(
                        session_, op, &qubit, nullptr, &fidelity),
                    QDMI_ERROR_NOTSUPPORTED)
              << "Operations must not provide fidelity for non-involved "
                 "qubits.";
          EXPECT_EQ(qdmi_sc_->session_query_sc_operation_duration(
                        session_, op, &qubit, nullptr, &duration),
                    QDMI_ERROR_NOTSUPPORTED)
              << "Operations must not provide a duration for non-involved "
                 "qubits.";
        }
      }
    } else if (qubit_count == 2) {
      std::vector<std::array<QDMI_SCQubit, 2>> op_qubit_pairs(
          size / 2, {nullptr, nullptr});
      ASSERT_EQ(qdmi_sc_->session_query_sc_operation_qubits(
                    session_, op, size,
                    reinterpret_cast<QDMI_SCQubit *>(op_qubit_pairs.data()),
                    nullptr),
                QDMI_SUCCESS)
          << "Operations must provide the qubits.";
      std::unordered_set<std::array<QDMI_SCQubit, 2>, Qubit_pair_hash>
          op_qubit_pair_set(op_qubit_pairs.begin(), op_qubit_pairs.end());
      std::ranges::transform(
          op_qubit_pairs,
          std::inserter(op_qubit_pair_set, op_qubit_pair_set.end()),
          [](const std::array<QDMI_SCQubit, 2> &pair) {
            return std::array{pair.back(), pair.front()};
          });
      for (auto *q1 : qubits) {
        for (auto *q2 : qubits) {
          double fidelity{};
          uint64_t duration{};
          if (const std::array qubit_pair{q1, q2};
              op_qubit_pair_set.contains(qubit_pair)) {
            ASSERT_EQ(qdmi_sc_->session_query_sc_operation_fidelity(
                          session_, op, qubit_pair.data(), nullptr, &fidelity),
                      QDMI_SUCCESS)
                << "Operations must provide fidelity for qubit operations.";
            EXPECT_THAT(fidelity, ::testing::IsBetween(0, 1))
                << "Fidelity must be between 0 and 1 for operation " << id
                << ".";
            ASSERT_EQ(qdmi_sc_->session_query_sc_operation_duration(
                          session_, op, qubit_pair.data(), nullptr, &duration),
                      QDMI_SUCCESS)
                << "Operations must provide duration for qubit operations.";
            EXPECT_GT(duration, 0)
                << "Duration must be positive for operation " << id << ".";
          } else {
            EXPECT_EQ(qdmi_sc_->session_query_sc_operation_fidelity(
                          session_, op, qubit_pair.data(), nullptr, &fidelity),
                      QDMI_ERROR_NOTSUPPORTED)
                << "Operations must not provide fidelity for non-involved "
                   "qubits.";
            EXPECT_EQ(qdmi_sc_->session_query_sc_operation_duration(
                          session_, op, qubit_pair.data(), nullptr, &duration),
                      QDMI_ERROR_NOTSUPPORTED)
                << "Operations must not provide a duration for non-involved "
                   "qubits.";
          }
        }
      }
    } else {
      GTEST_FAIL() << "Currently, the test only supports operations with one"
                      "or two qubits.";
    }
  }
}
} // namespace qdmi::test
