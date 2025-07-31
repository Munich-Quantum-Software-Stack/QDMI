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

add_library(qdmi_example_tool example_tool.cpp example_tool.hpp)
target_link_libraries(qdmi_example_tool PRIVATE qdmi::qdmi qdmi::example_fomac)
target_include_directories(qdmi_example_tool PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(qdmi_example_tool PRIVATE cxx_std_17)
add_library(qdmi::example_tool ALIAS qdmi_example_tool)
