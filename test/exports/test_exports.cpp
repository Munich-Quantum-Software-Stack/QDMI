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

#include "device_functions.h"

#include <array>
#include <dlfcn.h>
#include <gtest/gtest.h>

namespace {
void Check_exports(const char *path, const auto &required) {
  auto *library = dlopen(path, RTLD_NOW | RTLD_LOCAL);
  ASSERT_NE(library, nullptr) << dlerror();
  for (const auto *symbol : required) {
    EXPECT_NE(dlsym(library, symbol), nullptr) << symbol;
  }
  EXPECT_EQ(dlsym(library, "Qdmi_bundled_dependency"), nullptr);
  EXPECT_EQ(dlsym(library, "CXX_QDMI_device_internal"), nullptr);
  EXPECT_EQ(dlclose(library), 0);
}

TEST(Exports, Device) { Check_exports(DEVICE_LIBRARY, DEVICE_FUNCTIONS); }

TEST(Exports, Client) {
  constexpr std::array required = {"QDMI_session_alloc",
                                   "QDMI_session_set_parameter",
                                   "QDMI_session_init",
                                   "QDMI_session_query_session_property",
                                   "QDMI_session_free",
                                   "QDMI_device_query_device_property",
                                   "QDMI_device_query_site_property",
                                   "QDMI_device_query_operation_property",
                                   "QDMI_device_create_job",
                                   "QDMI_session_retrieve_job_by_id",
                                   "QDMI_job_set_parameter",
                                   "QDMI_job_query_property",
                                   "QDMI_job_submit",
                                   "QDMI_job_cancel",
                                   "QDMI_job_check",
                                   "QDMI_job_wait",
                                   "QDMI_job_get_results",
                                   "QDMI_job_free",
                                   "QDMI_driver_init",
                                   "QDMI_driver_shutdown"};
  Check_exports(DRIVER_LIBRARY, required);
}
} // namespace
