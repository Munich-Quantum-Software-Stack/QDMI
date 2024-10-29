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

add_library(qdmi_example_fomac example_fomac.cpp example_fomac.hpp)
target_link_libraries(qdmi_example_fomac PRIVATE qdmi::qdmi)
target_include_directories(qdmi_example_fomac
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_library(qdmi::example_fomac ALIAS qdmi_example_fomac)
