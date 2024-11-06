# ------------------------------------------------------------------------------
# Part of the MQSS Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
# ------------------------------------------------------------------------------

# add C++ language support
enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(qdmi_example_static_driver qdmi_example_static_driver.cpp
                                       qdmi_example_static_driver.h)
target_link_libraries(
  qdmi_example_static_driver PRIVATE qdmi::qdmi qdmi::c_device qdmi::cxx_device
                                     qdmi::project_warnings)
target_include_directories(qdmi_example_static_driver
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(qdmi_example_static_driver
                      PROPERTIES POSITION_INDEPENDENT_CODE ON)
add_library(qdmi::example_static_driver ALIAS qdmi_example_static_driver)
