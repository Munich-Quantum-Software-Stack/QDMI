# ------------------------------------------------------------------------------
# Part of the MQSS Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
# ------------------------------------------------------------------------------

if(NOT CXX_DEVICE)
  # add CXX language support
  enable_language(CXX)
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
endif()

# ------------------------------------------------------------------------------
# Non-Functional Tests
# ------------------------------------------------------------------------------

# test that a device implementation implements all functions
#
# NOTE: This executable is not meant to be ever executed, only linked.
add_executable(qdmi_test_my_device_defs ${QDMI_TEST_DIR}/utils/test_defs.cpp)
target_link_libraries(qdmi_test_my_device_defs
                      PRIVATE qdmi::qdmi qdmi::my_device qdmi::project_warnings)

# ------------------------------------------------------------------------------
# Functional Tests
# ------------------------------------------------------------------------------

# create an executable in which the tests will be stored
add_executable(my_device_test test_my_device.cpp)

# link the Google test infrastructure to the test executable.
target_link_libraries(my_device_test PRIVATE gtest_main qdmi::qdmi my_device
                                             qdmi::project_warnings)

# ensure my_device and my_device_definitions_test are built whenever
# my_device_test is built
add_dependencies(my_device_test qdmi_test_my_device_defs)

# only discover tests if this is the main project
if(NOT MY_DEVICE_MASTER_PROJECT)
  return()
endif()

gtest_discover_tests(
  my_device_test
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
             DISCOVERY_TIMEOUT 60)
