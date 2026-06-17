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

#include "test_core.hpp"

#include "qdmi/core.h"
#include "qdmi/orchestration_layer.h"
#include "qdmi/provider.h"

#include <algorithm>
#include <cstddef>
#include <functional>
#include <gtest/gtest.h>
#include <iterator>
#include <spdlog/spdlog.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

namespace qdmi::test {
TestParam::operator std::string() const {
  std::ostringstream oss;
  oss << this;
  return oss.str();
}
auto Log_callback(const QDMI_Log_Level level, const char *message,
                  void *user_data) -> void {
  static_cast<spdlog::logger *>(user_data)->log(
      static_cast<spdlog::level::level_enum>(level), message);
}
auto operator<<(std::ostream &os, const TestParam &param) -> std::ostream & {
  const size_t sep = param.lib_path.find_last_of("/\\");
  const size_t dot = param.lib_path.find_first_of('.');
  std::string filename = (sep == std::string::npos)
                             ? param.lib_path.substr(0, dot)
                             : param.lib_path.substr(sep + 1, dot - sep - 1);
  // Strip the 'lib' prefix if it exists
  constexpr std::string prefix = "lib";
  if (filename.starts_with(prefix)) {
    filename = filename.substr(prefix.size());
  }
  os << filename;
  if (!param.mod_id.empty()) {
    os << "_" << param.mod_id;
  }
  if (!param.dev_id.empty()) {
    os << "_" << param.dev_id;
  }
  return os;
}
QDMITest::QDMITest() {}
auto QDMITest::SetUp() -> void {
  TestWithParam::SetUp();
  if (!IsSkipped() && !HasFatalFailure()) {
    ASSERT_NO_THROW(library_ = LibraryWrapper(
                        Dylib(GetParam().lib_path), GetParam().lib_prefix,
                        Log_callback, spdlog::default_logger().get()););
    context_ = library_.context();
    ASSERT_EQ(library_.get()->get_interface(&qdmi_), QDMI_SUCCESS);
  }
}
auto QDMITest::generate_test_params(
    const std::vector<std::pair<std::string, std::string>> &libs)
    -> std::vector<TestParam> {
  std::vector<TestParam> params;
  std::ranges::transform(libs, std::back_inserter(params), [](const auto &lib) {
    return TestParam{lib.first, lib.second};
  });
  return params;
}
auto QDMISessionTest::SetUp() -> void {
  QDMITest::SetUp();
  if (!IsSkipped() && !HasFatalFailure()) {
    ASSERT_NO_THROW(session_wrapper_ =
                        SessionWrapper(context_, qdmi_, Log_callback,
                                       spdlog::default_logger().get()));
    session_ = session_wrapper_.get();
    if (!GetParam().dev_id.empty()) {
      QDMI_Module module{};
      std::variant<const QDMI_Provider_Interface *,
                   const QDMI_OrchestrationLayer_Interface *>
          iface{};
      if (const auto ret =
              qdmi_->context_query_module_by_id(context_, "provider", &module);
          ret == QDMI_SUCCESS) {
        const QDMI_Provider_Interface *provider{};
        ASSERT_EQ(
            qdmi_->context_get_module_interface(
                context_, module, reinterpret_cast<const void **>(&provider)),
            QDMI_SUCCESS)
            << "Failed to get provider interface.";
        iface = provider;
      } else {
        ASSERT_EQ(ret, QDMI_ERROR_NOTFOUND);
        ASSERT_EQ(qdmi_->context_query_module_by_id(context_, "ol", &module),
                  QDMI_SUCCESS)
            << "Failed to get provider or orchestration layer module.";
        const QDMI_OrchestrationLayer_Interface *ol{};
        ASSERT_EQ(qdmi_->context_get_module_interface(
                      context_, module, reinterpret_cast<const void **>(&ol)),
                  QDMI_SUCCESS)
            << "Failed to get provider interface.";
        iface = ol;
      }
      std::visit(
          [this](const auto iface) {
            QDMI_Device device{};
            ASSERT_EQ(iface->session_query_device_by_id(
                          session_, GetParam().dev_id.c_str(), &device),
                      QDMI_SUCCESS)
                << "Failed to query device.";
            ASSERT_EQ(iface->session_query_device_context(session_, device,
                                                          &context_),
                      QDMI_SUCCESS)
                << "Failed to get device interface.";
            ASSERT_EQ(
                iface->session_get_device_interface(session_, device, &qdmi_),
                QDMI_SUCCESS)
                << "Failed to query device context.";
          },
          iface);
      parent_session_wrapper_ = std::move(session_wrapper_);
      session_wrapper_ = SessionWrapper(context_, qdmi_, Log_callback,
                                        spdlog::default_logger().get());
      session_ = session_wrapper_.get();
    }
  }
}
auto QDMISessionTest::generate_test_params(
    const std::vector<std::pair<std::string, std::string>> &libs,
    const std::vector<std::string> &dev_ids) -> std::vector<TestParam> {
  std::vector<TestParam> params;
  std::ranges::for_each(libs, [&dev_ids, &params](const auto &lib) {
    if (dev_ids.empty()) {
      params.emplace_back(lib.first, lib.second, "");
    } else {
      std::ranges::transform(dev_ids, std::back_inserter(params),
                             [&lib](const auto &dev_id) {
                               return TestParam{lib.first, lib.second, dev_id};
                             });
    }
  });
  return params;
}
auto QDMIModuleTest::SetUp() -> void {
  QDMITest::SetUp();
  if (!IsSkipped() && !HasFatalFailure()) {
    if (!GetParam().dev_id.empty()) {
      ASSERT_NO_THROW(session_wrapper_ =
                          SessionWrapper(context_, qdmi_, Log_callback,
                                         spdlog::default_logger().get()));
      auto *session = session_wrapper_->get();
      QDMI_Module module{};
      std::variant<const QDMI_Provider_Interface *,
                   const QDMI_OrchestrationLayer_Interface *>
          iface{};
      if (const auto ret =
              qdmi_->context_query_module_by_id(context_, "provider", &module);
          ret == QDMI_SUCCESS) {
        const QDMI_Provider_Interface *provider{};
        ASSERT_EQ(
            qdmi_->context_get_module_interface(
                context_, module, reinterpret_cast<const void **>(&provider)),
            QDMI_SUCCESS)
            << "Failed to get provider interface.";
        iface = provider;
      } else {
        ASSERT_EQ(ret, QDMI_ERROR_NOTFOUND);
        ASSERT_EQ(qdmi_->context_query_module_by_id(context_, "ol", &module),
                  QDMI_SUCCESS)
            << "Failed to get provider or orchestration layer module.";
        const QDMI_OrchestrationLayer_Interface *ol{};
        ASSERT_EQ(qdmi_->context_get_module_interface(
                      context_, module, reinterpret_cast<const void **>(&ol)),
                  QDMI_SUCCESS)
            << "Failed to get provider interface.";
        iface = ol;
      }
      std::visit(
          [this, &session](const auto iface) {
            QDMI_Device device{};
            ASSERT_EQ(iface->session_query_device_by_id(
                          session, GetParam().dev_id.c_str(), &device),
                      QDMI_SUCCESS)
                << "Failed to query device.";
            ASSERT_EQ(
                iface->session_query_device_context(session, device, &context_),
                QDMI_SUCCESS)
                << "Failed to get device interface.";
            ASSERT_EQ(
                iface->session_get_device_interface(session, device, &qdmi_),
                QDMI_SUCCESS)
                << "Failed to query device context.";
          },
          iface);
    }
    ASSERT_EQ(qdmi_->context_query_module_by_id(
                  context_, GetParam().mod_id.c_str(), &module_),
              QDMI_SUCCESS);
  }
}
auto QDMIModuleTest::generate_test_params(
    const std::vector<std::pair<std::string, std::string>> &libs,
    const std::vector<std::string> &mod_ids,
    const std::vector<std::string> &dev_ids) -> std::vector<TestParam> {
  std::vector<TestParam> params;
  std::ranges::for_each(libs, [&mod_ids, &dev_ids, &params](const auto &lib) {
    std::ranges::for_each(
        mod_ids, [&lib, &dev_ids, &params](const auto &mod_id) {
          if (dev_ids.empty()) {
            params.emplace_back(lib.first, lib.second, "", mod_id);
          }
          std::ranges::transform(dev_ids, std::back_inserter(params),
                                 [&lib, &mod_id](const auto &dev_id) {
                                   return TestParam{lib.first, lib.second,
                                                    dev_id, mod_id};
                                 });
        });
  });
  return params;
}
TEST_P(QDMITest, QueryID) {
  EXPECT_EQ(qdmi_->context_query_id(context_, 0, nullptr, nullptr),
            QDMI_SUCCESS);
  size_t size = 0;
  ASSERT_EQ(qdmi_->context_query_id(context_, 0, nullptr, &size), QDMI_SUCCESS);
  std::string value(size - 1, '\0');
  EXPECT_EQ(qdmi_->context_query_id(context_, 0, value.data(), nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
  ASSERT_EQ(qdmi_->context_query_id(context_, size, value.data(), nullptr),
            QDMI_SUCCESS);
  EXPECT_FALSE(value.empty()) << "Devices must provide an ID.";
}
TEST_P(QDMITest, QueryName) {
  EXPECT_EQ(qdmi_->context_query_name(context_, 0, nullptr, nullptr),
            QDMI_SUCCESS);
  size_t size = 0;
  ASSERT_EQ(qdmi_->context_query_name(context_, 0, nullptr, &size),
            QDMI_SUCCESS);
  std::string value(size - 1, '\0');
  EXPECT_EQ(qdmi_->context_query_name(context_, 0, value.data(), nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
  ASSERT_EQ(qdmi_->context_query_name(context_, size, value.data(), nullptr),
            QDMI_SUCCESS);
  EXPECT_FALSE(value.empty()) << "Devices must provide a name.";
}
TEST_P(QDMITest, QueryVersion) {
  EXPECT_EQ(qdmi_->context_query_version(context_, nullptr), QDMI_SUCCESS);
  size_t version = 0;
  ASSERT_EQ(qdmi_->context_query_version(context_, &version), QDMI_SUCCESS);
  EXPECT_GT(version, 0) << "Devices must provide a version.";
}
TEST_P(QDMITest, QueryAuthenticationOptions) {
  EXPECT_EQ(qdmi_->context_query_authentication_options(context_, 0, nullptr,
                                                        nullptr),
            QDMI_SUCCESS);
  size_t size = 0;
  ASSERT_EQ(
      qdmi_->context_query_authentication_options(context_, 0, nullptr, &size),
      QDMI_SUCCESS);
  EXPECT_GT(size, 0)
      << "Devices must provide at least one authentication option.";
  std::vector options(size, QDMI_AUTHENTICATION_OPTION_NONE);
  EXPECT_EQ(qdmi_->context_query_authentication_options(
                context_, 0, options.data(), nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
  EXPECT_EQ(qdmi_->context_query_authentication_options(
                context_, size, options.data(), nullptr),
            QDMI_SUCCESS);
}
TEST_P(QDMITest, QueryModule) {
  EXPECT_EQ(
      qdmi_->context_query_module_by_id(context_, "non-existing", nullptr),
      QDMI_ERROR_INVALIDARGUMENT);
  QDMI_Module actual_module = nullptr;
  EXPECT_EQ(qdmi_->context_query_module_by_id(context_, "non-existing",
                                              &actual_module),
            QDMI_ERROR_NOTFOUND);
  EXPECT_EQ(qdmi_->context_query_module_by_id(context_, nullptr, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
  EXPECT_EQ(qdmi_->context_query_modules(context_, 0, nullptr, nullptr),
            QDMI_SUCCESS);
  size_t size = 0;
  ASSERT_EQ(qdmi_->context_query_modules(context_, 0, nullptr, &size),
            QDMI_SUCCESS);
  std::vector<QDMI_Module> modules(size, nullptr);
  EXPECT_EQ(qdmi_->context_query_modules(context_, 0, modules.data(), nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
  ASSERT_EQ(
      qdmi_->context_query_modules(context_, size, modules.data(), nullptr),
      QDMI_SUCCESS);
  for (QDMI_Module expected_module : modules) {
    EXPECT_EQ(qdmi_->context_query_module_id(context_, expected_module, 0,
                                             nullptr, nullptr),
              QDMI_SUCCESS);
    ASSERT_EQ(qdmi_->context_query_module_id(context_, expected_module, 0,
                                             nullptr, &size),
              QDMI_SUCCESS);
    std::string id(size - 1, '\0');
    EXPECT_EQ(qdmi_->context_query_module_id(context_, expected_module, 0,
                                             id.data(), nullptr),
              QDMI_ERROR_INVALIDARGUMENT);
    ASSERT_EQ(qdmi_->context_query_module_id(context_, expected_module, size,
                                             id.data(), nullptr),
              QDMI_SUCCESS);
    ASSERT_FALSE(id.empty()) << "Modules must provide an ID.";
    EXPECT_EQ(qdmi_->context_query_module_by_id(context_, id.c_str(), nullptr),
              QDMI_ERROR_INVALIDARGUMENT);
    actual_module = nullptr;
    ASSERT_EQ(
        qdmi_->context_query_module_by_id(context_, id.c_str(), &actual_module),
        QDMI_SUCCESS);
    EXPECT_EQ(actual_module, expected_module) << "Module mismatch.";
  }
}
TEST_P(QDMITest, SessionAlloc) {
  EXPECT_EQ(
      qdmi_->context_allocate_session(context_, nullptr, nullptr, nullptr),
      QDMI_ERROR_INVALIDARGUMENT);
  QDMI_Session session{};
  EXPECT_EQ(
      qdmi_->context_allocate_session(nullptr, nullptr, nullptr, &session),
      QDMI_ERROR_INVALIDARGUMENT);
}
TEST_P(QDMITest, SessionInitialize) {
  EXPECT_EQ(qdmi_->session_initialize(nullptr), QDMI_ERROR_INVALIDARGUMENT);
  size_t size = 0;
  ASSERT_EQ(
      qdmi_->context_query_authentication_options(context_, 0, nullptr, &size),
      QDMI_SUCCESS);
  std::vector options(size, QDMI_AUTHENTICATION_OPTION_NONE);
  ASSERT_EQ(qdmi_->context_query_authentication_options(
                context_, size, options.data(), nullptr),
            QDMI_SUCCESS);
  for (const auto &option : options) {
    QDMI_Session session = nullptr;
    ASSERT_EQ(
        qdmi_->context_allocate_session(context_, nullptr, nullptr, &session),
        QDMI_SUCCESS);
    switch (option) {
    case QDMI_AUTHENTICATION_OPTION_NONE:
      break;
    case QDMI_AUTHENTICATION_OPTION_TOKEN:
      ASSERT_EQ(qdmi_->session_set_token(session, "0123456789"), QDMI_SUCCESS);
      break;
    case QDMI_AUTHENTICATION_OPTION_PW:
      ASSERT_EQ(qdmi_->session_set_username(session, "user"), QDMI_SUCCESS);
      ASSERT_EQ(qdmi_->session_set_password(session, "1234"), QDMI_SUCCESS);
      break;
    case QDMI_AUTHENTICATION_OPTION_URL:
      ASSERT_EQ(
          qdmi_->session_set_authentication_url(session, "https://example.com"),
          QDMI_SUCCESS);
      break;
    case QDMI_AUTHENTICATION_OPTION_URLTOKEN:
      ASSERT_EQ(
          qdmi_->session_set_authentication_url(session, "https://example.com"),
          QDMI_SUCCESS);
      ASSERT_EQ(qdmi_->session_set_token(session, "0123456789"), QDMI_SUCCESS);
      break;
    case QDMI_AUTHENTICATION_OPTION_URLPW:
      ASSERT_EQ(
          qdmi_->session_set_authentication_url(session, "https://example.com"),
          QDMI_SUCCESS);
      ASSERT_EQ(qdmi_->session_set_username(session, "user"), QDMI_SUCCESS);
      ASSERT_EQ(qdmi_->session_set_password(session, "1234"), QDMI_SUCCESS);
      break;
    }
    EXPECT_EQ(qdmi_->session_initialize(session), QDMI_SUCCESS);
    qdmi_->session_free(session);
  }
}
TEST_P(QDMIModuleTest, QueryId) {
  EXPECT_EQ(
      qdmi_->context_query_module_id(context_, module_, 0, nullptr, nullptr),
      QDMI_SUCCESS);
  size_t size = 0;
  EXPECT_EQ(
      qdmi_->context_query_module_id(context_, nullptr, 0, nullptr, &size),
      QDMI_ERROR_INVALIDARGUMENT);
  ASSERT_EQ(
      qdmi_->context_query_module_id(context_, module_, 0, nullptr, &size),
      QDMI_SUCCESS);
  std::string id(size - 1, '\0');
  EXPECT_EQ(
      qdmi_->context_query_module_id(context_, module_, 0, id.data(), nullptr),
      QDMI_ERROR_INVALIDARGUMENT);
  ASSERT_EQ(qdmi_->context_query_module_id(context_, module_, size, id.data(),
                                           nullptr),
            QDMI_SUCCESS);
  EXPECT_FALSE(id.empty()) << "Modules must provide an id.";
}
TEST_P(QDMIModuleTest, GetInterface) {
  EXPECT_EQ(qdmi_->context_get_module_interface(context_, module_, nullptr),
            QDMI_ERROR_INVALIDARGUMENT);
  const void *interface = nullptr;
  EXPECT_EQ(qdmi_->context_get_module_interface(context_, nullptr, &interface),
            QDMI_ERROR_INVALIDARGUMENT);
  ASSERT_EQ(qdmi_->context_get_module_interface(context_, module_, &interface),
            QDMI_SUCCESS);
  EXPECT_NE(interface, nullptr) << "Module must provide an interface.";
}
} // namespace qdmi::test
