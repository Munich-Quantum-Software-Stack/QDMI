# Copyright (c) 2024 - 2026 QDMI Maintainers
# All rights reserved.
#
# Licensed under the Apache License v2.0 with LLVM Exceptions (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# https://llvm.org/LICENSE.txt
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

# Build a dependency with default visibility, as third-party archives may have.
add_library(qdmi_export_dependency STATIC dependency.cpp)
set_target_properties(qdmi_export_dependency
                      PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_link_libraries(
  cxx-qdmi-device
  PRIVATE "$<LINK_LIBRARY:WHOLE_ARCHIVE,qdmi_export_dependency>")
target_link_libraries(
  my-qdmi-device PRIVATE "$<LINK_LIBRARY:WHOLE_ARCHIVE,qdmi_export_dependency>")
add_subdirectory(fixture)

# Reuse the example driver's objects so coverage measures each source once.
get_target_property(driver_type qdmi_example_driver TYPE)
if(driver_type STREQUAL "STATIC_LIBRARY")
  add_library(qdmi_export_driver SHARED dependency.cpp)
  target_link_libraries(
    qdmi_export_driver
    PRIVATE "$<LINK_LIBRARY:WHOLE_ARCHIVE,qdmi_example_driver>"
            ${CMAKE_DL_LIBS})
  configure_qdmi_exports(
    TARGET
    qdmi_export_driver
    INTERFACE
    client
    EXTRA_SYMBOLS
    QDMI_driver_init
    QDMI_driver_shutdown)
  set(export_driver qdmi_export_driver)
else()
  target_link_libraries(
    qdmi_example_driver
    PRIVATE "$<LINK_LIBRARY:WHOLE_ARCHIVE,qdmi_export_dependency>")
  set(export_driver qdmi_example_driver)
endif()

# Use the prefix registry independently of the helper's declaration parser.
set(prefix_dir "${QDMI_CMAKE_DIR}")
if(NOT prefix_dir)
  set(prefix_dir "${qdmi_CMAKE_DIR}")
endif()
file(STRINGS "${prefix_dir}/prefix_defs.txt" device_functions
     REGEX "^QDMI_device_[a-z0-9_]+$")
set(template_functions ${device_functions})
list(TRANSFORM device_functions PREPEND "CXX_")
list(TRANSFORM template_functions PREPEND "MY_")
list(JOIN device_functions "\", \"" device_functions)
list(JOIN template_functions "\", \"" template_functions)
file(
  CONFIGURE
  OUTPUT
  "${CMAKE_CURRENT_BINARY_DIR}/device_functions.h"
  CONTENT
  "constexpr const char* DEVICE_FUNCTIONS[] = {\"@device_functions@\"};
constexpr const char* TEMPLATE_FUNCTIONS[] = {\"@template_functions@\"};
"
  @ONLY)
add_executable(qdmi_export_test test_exports.cpp)
target_include_directories(qdmi_export_test
                           PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(qdmi_export_test PRIVATE GTest::gtest_main
                                               ${CMAKE_DL_LIBS})
target_compile_features(qdmi_export_test PRIVATE cxx_std_20)
add_dependencies(qdmi_export_test cxx-qdmi-device my-qdmi-device
                 qdmi_export_fixture ${export_driver})
target_compile_definitions(
  qdmi_export_test
  PRIVATE DEVICE_LIBRARY="$<TARGET_FILE:cxx-qdmi-device>"
          TEMPLATE_LIBRARY="$<TARGET_FILE:my-qdmi-device>"
          FIXTURE_LIBRARY="$<TARGET_FILE:qdmi_export_fixture>"
          DRIVER_LIBRARY="$<TARGET_FILE:${export_driver}>")
add_test(NAME QdmiExports COMMAND qdmi_export_test)

add_dependencies(qdmi_test qdmi_export_test)
