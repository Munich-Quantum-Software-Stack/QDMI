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

# generate name-shifted headers
generate_prefixed_qdmi_headers("O" "OD")
generate_prefixed_qdmi_headers("OD")

foreach(identifier IN ITEMS "o1" "o2")
  # NOTE: If you change the target name, the name of the shared library will
  # also change. Hence, in the test you have to adopt the name of the shared
  # library accordingly.
  set(TARGET_NAME orchestration_layer_${identifier})

  # create library target
  add_library(${TARGET_NAME} SHARED)

  # link to qdmi core library
  target_link_libraries(
    ${TARGET_NAME}
    PRIVATE qdmi::qdmi
            qdmi::singleton
            qdmi::logger
            qdmi::library_wrapper
            qdmi::session_wrapper
            qdmi::qdmi_project_warnings
            spdlog::spdlog)

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
  target_compile_definitions(
    ${TARGET_NAME} PRIVATE QDMI_VERSION=${qdmi_packed_version}
                           QDMI_DEVICE_ID="${identifier}")
  if("${identifier}" STREQUAL "o1")
    target_compile_definitions(
      ${TARGET_NAME}
      PRIVATE
        "QDMI_DYN_LIBS=std::array{ \"$<TARGET_FILE:qdmi::qpu_q1>\", \"$<TARGET_FILE:provider_p1>\", \"$<TARGET_FILE:orchestration_layer_o2>\" }"
    )
  elseif("${identifier}" STREQUAL "o2")
    target_compile_definitions(
      ${TARGET_NAME}
      PRIVATE "QDMI_DYN_LIBS=std::array{ \"$<TARGET_FILE:qdmi::qpu_q3>\" }")
  else()
    message(FATAL_ERROR "Unknown identifier: ${identifier}")
  endif()

  # add sources to target
  target_sources(
    ${TARGET_NAME}
    PRIVATE orchestration_layer.cpp core_bindings.cpp
            orchestration_layer_bindings.cpp device_core_bindings.cpp
            device_superconducting_bindings.cpp)

  # add headers using file sets
  target_sources(
    ${TARGET_NAME}
    PRIVATE
      FILE_SET
      private_headers
      TYPE
      HEADERS
      BASE_DIRS
      ${QDMI_EXAMPLES_INCLUDE_DIR}
      FILES
      ${QDMI_EXAMPLES_INCLUDE_DIR}/orchestration_layer/orchestration_layer.hpp
      ${QDMI_EXAMPLES_INCLUDE_DIR}/orchestration_layer/core_bindings.hpp
      ${QDMI_EXAMPLES_INCLUDE_DIR}/orchestration_layer/orchestration_layer_bindings.hpp
      ${QDMI_EXAMPLES_INCLUDE_DIR}/orchestration_layer/device_core_bindings.hpp
      ${QDMI_EXAMPLES_INCLUDE_DIR}/orchestration_layer/device_superconducting_bindings.hpp
  )
  target_sources(
    ${TARGET_NAME}
    PUBLIC
      FILE_SET
      public_headers
      TYPE
      HEADERS
      BASE_DIRS
      ${CMAKE_CURRENT_BINARY_DIR}/include
      FILES
      ${CMAKE_CURRENT_BINARY_DIR}/include/o_qdmi/core.h
      ${CMAKE_CURRENT_BINARY_DIR}/include/o_qdmi/core/constants.h
      ${CMAKE_CURRENT_BINARY_DIR}/include/o_qdmi/core/functions.h
      ${CMAKE_CURRENT_BINARY_DIR}/include/o_qdmi/core/types.h
      ${CMAKE_CURRENT_BINARY_DIR}/include/o_qdmi/orchestration_layer.h
      ${CMAKE_CURRENT_BINARY_DIR}/include/o_qdmi/job/constants.h
      ${CMAKE_CURRENT_BINARY_DIR}/include/o_qdmi/job/functions.h
      ${CMAKE_CURRENT_BINARY_DIR}/include/o_qdmi/job/types.h
      ${CMAKE_CURRENT_BINARY_DIR}/include/o_qdmi/provider/functions.h
      ${CMAKE_CURRENT_BINARY_DIR}/include/o_qdmi/provider/types.h
      ${CMAKE_CURRENT_BINARY_DIR}/include/o_qdmi/orchestration_layer/functions.h
      ${CMAKE_CURRENT_BINARY_DIR}/include/o_qdmi/orchestration_layer/types.h
      ${CMAKE_CURRENT_BINARY_DIR}/include/od_qdmi/core.h
      ${CMAKE_CURRENT_BINARY_DIR}/include/od_qdmi/core/constants.h
      ${CMAKE_CURRENT_BINARY_DIR}/include/od_qdmi/core/functions.h
      ${CMAKE_CURRENT_BINARY_DIR}/include/od_qdmi/core/types.h
      ${CMAKE_CURRENT_BINARY_DIR}/include/od_qdmi/superconducting/functions.h
      ${CMAKE_CURRENT_BINARY_DIR}/include/od_qdmi/superconducting/types.h)

  # place the generated library in a predictable location where the executable
  # can find it
  set_target_properties(
    ${TARGET_NAME}
    PROPERTIES LIBRARY_OUTPUT_DIRECTORY
               "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}"
               ARCHIVE_OUTPUT_DIRECTORY
               "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}"
               RUNTIME_OUTPUT_DIRECTORY
               "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")

  # add dependencies to ensure they are built before the executable
  if("${identifier}" STREQUAL "o1")
    add_dependencies(${TARGET_NAME} qdmi::qpu_q1 qdmi::provider_p1
                     qdmi::orchestration_layer_o2)
  elseif("${identifier}" STREQUAL "o2")
    add_dependencies(${TARGET_NAME} qdmi::qpu_q3)
  else()
    message(FATAL_ERROR "Unknown orchestration layer identifier: ${identifier}")
  endif()

  # set the folder of the target for IDEs
  set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "examples/src")
  if(TARGET ${TARGET_NAME}_verify_interface_header_sets)
    set_target_properties(${TARGET_NAME}_verify_interface_header_sets
                          PROPERTIES FOLDER "examples/src")
  endif()

  # set an alias for the target
  add_library(qdmi::${TARGET_NAME} ALIAS ${TARGET_NAME})
endforeach()
