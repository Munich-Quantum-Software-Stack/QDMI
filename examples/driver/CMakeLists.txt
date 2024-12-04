# ------------------------------------------------------------------------------
# Copyright 2024 Munich Quantum Software Stack Project
#
# Licensed under the Apache License, Version 2.0 with LLVM Exceptions (the
# "License"); you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# https://github.com/Munich-Quantum-Software-Stack/QDMI/blob/develop/LICENSE
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
# ------------------------------------------------------------------------------

# add C++ language support
enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(qdmi_example_driver qdmi_example_driver.cpp qdmi_example_driver.h)
target_link_libraries(qdmi_example_driver PRIVATE qdmi::qdmi
                                                  qdmi::project_warnings)
target_include_directories(qdmi_example_driver
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(qdmi_example_driver PROPERTIES POSITION_INDEPENDENT_CODE
                                                     ON)
add_library(qdmi::example_driver ALIAS qdmi_example_driver)
