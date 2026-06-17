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

#include "test_provider.hpp"

#include "qdmi/core.h"
#include "qdmi/provider.h"

#include <cstddef>
#include <gtest/gtest.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

namespace qdmi::test {

auto QDMIProviderTest::all_devices() const
    -> std::optional<std::vector<QDMI_Device>> {
  return std::visit(
      [this](const auto *iface) -> std::optional<std::vector<QDMI_Device>> {
        size_t device_count{};
        if (iface->session_query_devices(session_, 0, nullptr, &device_count) !=
            QDMI_SUCCESS) {
          return std::nullopt;
        }
        std::vector<QDMI_Device> devices(device_count, nullptr);
        if (iface->session_query_devices(session_, device_count, devices.data(),
                                         nullptr) != QDMI_SUCCESS) {
          return std::nullopt;
        }
        return devices;
      },
      provider_);
}

auto QDMIProviderTest::SetUp() -> void {
  QDMISessionTest::SetUp();
  if (!IsSkipped() && !HasFatalFailure()) {
    if (const auto *provider = get_provider_interface_if_present();
        provider != nullptr) {
      provider_ = provider;
    } else {
      const auto *ol = get_orchestration_layer_interface_if_present();
      ASSERT_NE(ol, nullptr)
          << "Failed to get provider or orchestration layer module.";
      provider_ = ol;
    }
  }
}

auto QDMIProviderTest::get_provider_interface_if_present() const
    -> const QDMI_Provider_Interface * {
  QDMI_Module module{};
  if (const auto ret{
          qdmi_->context_query_module_by_id(context_, "provider", &module)};
      ret != QDMI_SUCCESS) {
    return nullptr;
  }
  const QDMI_Provider_Interface *provider{};
  if (qdmi_->context_get_module_interface(
          // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
          context_, module, reinterpret_cast<const void **>(&provider)) !=
      QDMI_SUCCESS) {
    throw std::runtime_error("Failed to get provider interface.");
  }
  return provider;
}

auto QDMIProviderTest::get_orchestration_layer_interface_if_present() const
    -> const QDMI_OrchestrationLayer_Interface * {
  QDMI_Module module{};
  if (const auto ret{
          qdmi_->context_query_module_by_id(context_, "ol", &module)};
      ret != QDMI_SUCCESS) {
    return nullptr;
  }
  const QDMI_OrchestrationLayer_Interface *ol{};
  if (qdmi_->context_get_module_interface(
          // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
          context_, module, reinterpret_cast<const void **>(&ol)) !=
      QDMI_SUCCESS) {
    throw std::runtime_error("Failed to get orchestration layer interface.");
  }
  return ol;
}

TEST_P(QDMIProviderTest, QueryDevice) {
  EXPECT_EQ(std::visit(
                [this](const auto *iface) {
                  return iface->session_query_device_by_id(
                      session_, "non-existing", nullptr);
                },
                provider_),
            QDMI_ERROR_INVALIDARGUMENT);
  QDMI_Device actual_device = nullptr;
  EXPECT_EQ(std::visit(
                [this, &actual_device](const auto *iface) {
                  return iface->session_query_device_by_id(
                      session_, "non-existing", &actual_device);
                },
                provider_),
            QDMI_ERROR_NOTFOUND);
  EXPECT_EQ(std::visit(
                [this](const auto *iface) {
                  return iface->session_query_device_by_id(session_, nullptr,
                                                           nullptr);
                },
                provider_),
            QDMI_ERROR_INVALIDARGUMENT);
  EXPECT_EQ(std::visit(
                [this](const auto *iface) {
                  return iface->session_query_devices(session_, 0, nullptr,
                                                      nullptr);
                },
                provider_),
            QDMI_SUCCESS);
  size_t device_count = 0;
  ASSERT_EQ(std::visit(
                [this, &device_count](const auto *iface) {
                  return iface->session_query_devices(session_, 0, nullptr,
                                                      &device_count);
                },
                provider_),
            QDMI_SUCCESS);
  std::vector<QDMI_Device> devices(device_count, nullptr);
  EXPECT_EQ(std::visit(
                [this, &devices](const auto *iface) {
                  return iface->session_query_devices(session_, 0,
                                                      devices.data(), nullptr);
                },
                provider_),
            QDMI_ERROR_INVALIDARGUMENT);
  ASSERT_EQ(std::visit(
                [this, device_count, &devices](const auto *iface) {
                  return iface->session_query_devices(session_, device_count,
                                                      devices.data(), nullptr);
                },
                provider_),
            QDMI_SUCCESS);
  for (QDMI_Device expected_device : devices) {
    QDMI_Context dev_context{};
    ASSERT_EQ(std::visit(
                  [this, expected_device, &dev_context](const auto *iface) {
                    return iface->session_query_device_context(
                        session_, expected_device, &dev_context);
                  },
                  provider_),
              QDMI_SUCCESS);
    const QDMI_Core_Interface *dev_qdmi{};
    ASSERT_EQ(std::visit(
                  [this, expected_device, &dev_qdmi](const auto *iface) {
                    return iface->session_get_device_interface(
                        session_, expected_device, &dev_qdmi);
                  },
                  provider_),
              QDMI_SUCCESS);
    size_t id_size{};
    ASSERT_EQ(dev_qdmi->context_query_id(dev_context, 0, nullptr, &id_size),
              QDMI_SUCCESS);
    std::string id(id_size - 1, '\0');
    ASSERT_EQ(
        dev_qdmi->context_query_id(dev_context, id_size, id.data(), nullptr),
        QDMI_SUCCESS);
    ASSERT_FALSE(id.empty()) << "Devices must provide an ID.";
    EXPECT_EQ(std::visit(
                  [this, &actual_device, &id](const auto *iface) {
                    return iface->session_query_device_by_id(
                        session_, id.c_str(), &actual_device);
                  },
                  provider_),
              QDMI_SUCCESS);
    EXPECT_EQ(actual_device, expected_device) << "Device mismatch.";
  }
}
} // namespace qdmi::test
