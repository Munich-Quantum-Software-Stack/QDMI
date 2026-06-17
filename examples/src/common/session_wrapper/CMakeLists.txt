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

include(MakeVersion)

set(TARGET_NAME session_wrapper)

# create library target
add_library(${TARGET_NAME})

# link to qdmi project warnings
target_link_libraries(
  ${TARGET_NAME}
  PUBLIC qdmi::qdmi
  PRIVATE qdmi::qdmi_project_warnings)

# set C++ standard to C++20
target_compile_features(${TARGET_NAME} PRIVATE cxx_std_20)

# set version information
set_target_properties(
  ${TARGET_NAME}
  PROPERTIES VERSION ${PROJECT_VERSION}
             SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
             EXPORT_NAME Core${ARG_ALIAS_NAME})

# define version macros
get_target_property(qdmi_library_VERSION qdmi::qdmi VERSION)
qdmi_make_version(qdmi_packed_version ${qdmi_library_VERSION})
target_compile_definitions(${TARGET_NAME}
                           PRIVATE QDMI_VERSION=${qdmi_packed_version})

# add sources to target
target_sources(${TARGET_NAME} PRIVATE session_wrapper.cpp)

# add headers using file sets
target_sources(
  ${TARGET_NAME}
  PUBLIC FILE_SET
         public_headers
         TYPE
         HEADERS
         BASE_DIRS
         ${QDMI_EXAMPLES_INCLUDE_DIR}
         FILES
         ${QDMI_EXAMPLES_INCLUDE_DIR}/common/session_wrapper/session_wrapper.hpp
)

# place the generated library in a predictable location where the executable can
# find it
set_target_properties(
  ${TARGET_NAME}
  PROPERTIES LIBRARY_OUTPUT_DIRECTORY
             "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}"
             ARCHIVE_OUTPUT_DIRECTORY
             "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}"
             RUNTIME_OUTPUT_DIRECTORY
             "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")

# set the folder of the target for IDEs
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "examples/src")
if(TARGET ${TARGET_NAME}_verify_interface_header_sets)
  set_target_properties(${TARGET_NAME}_verify_interface_header_sets
                        PROPERTIES FOLDER "examples/src")
endif()

# set an alias for the target
add_library(qdmi::${TARGET_NAME} ALIAS ${TARGET_NAME})
