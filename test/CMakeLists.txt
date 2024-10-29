# ------------------------------------------------------------------------------
# Part of the MQSS Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
# ------------------------------------------------------------------------------

if(TARGET qdmi_test)
  return()
endif()

# add CXX language support
enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(utils)

# ------------------------------------------------------------------------------
# Non-Functional Tests
# ------------------------------------------------------------------------------
# test that a device implementation implements all functions
#
# NOTE: This executable is not meant to be ever executed, only linked.
add_executable(qdmi_test_c_device_defs utils/test_defs.cpp)
target_link_libraries(
  qdmi_test_c_device_defs PRIVATE qdmi::qdmi qdmi::c_device
                                  qdmi::example_driver qdmi::project_warnings)

# test that a device implementation implements all functions NOTE: This
# executable is not meant to be ever executed, only linked.
add_executable(qdmi_test_cxx_device_defs utils/test_defs.cpp)
target_link_libraries(
  qdmi_test_cxx_device_defs PRIVATE qdmi::qdmi qdmi::cxx_device
                                    qdmi::example_driver qdmi::project_warnings)

# ------------------------------------------------------------------------------
# Functional Tests
# ------------------------------------------------------------------------------

# create an executable in which the tests will be stored
add_executable(qdmi_test test_qdmi.cpp)

# link the Google test infrastructure to the test executable.
target_link_libraries(
  qdmi_test
  PRIVATE qdmi::qdmi
          qdmi::test_impl
          qdmi::example_tool
          qdmi::example_fomac
          qdmi::example_driver
          qdmi::project_warnings
          gtest_main)

# discover tests
gtest_discover_tests(
  qdmi_test
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
             DISCOVERY_TIMEOUT 60)

# ensure c_device and cxx_device are built whenever qdmi_test is built. Those
# targets build the dynamic libraries that are loaded in the tests.
add_dependencies(qdmi_test qdmi::c_device qdmi::cxx_device)
# ensure qdmi_test_c_device_defs and qdmi_test_cxx_device_defs are built
# whenever qdmi_test is built. Those targets ensure that the respective devices
# implement all interface functions.
add_dependencies(qdmi_test qdmi_test_c_device_defs qdmi_test_cxx_device_defs)
# ensure my_device_test is built whenever qdmi_test is built. That target
# ensures that there is no compilation error in the templates provided with QDMI
# and their tests.
add_dependencies(qdmi_test my_device_test)
