# Copyright (c) 2024 - 2025 Munich Quantum Software Stack Project
# All rights reserved.
#
# Licensed under the Apache License v2.0 with LLVM Exceptions (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# https://github.com/Munich-Quantum-Software-Stack/QDMI/blob/develop/LICENSE.md
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

# enable extensive compiler warnings from here:
# https://github.com/lefticus/cppbestpractices/blob/master/02-Use_the_Tools_Available.md
function(set_project_warnings target_name)
  option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

  set(microsoft_msvc_warnings
      /W4 # Baseline reasonable warnings
      /w14242 # 'identifier': conversion from 'type1' to 'type2', possible loss
              # of data
      /w14254 # 'operator': conversion from 'type1:field_bits' to
              # 'type2:field_bits', possible loss of data
      /w14263 # 'function': member function does not override any base class
              # virtual member function
      /w14265 # 'classname': class has virtual functions, but destructor is not
              # virtual instances of this class may not
      # be destructed correctly
      /w14287 # 'operator': unsigned/negative constant mismatch
      /we4289 # nonstandard extension used: 'variable': loop control variable
              # declared in the for-loop is used outside
      # the for-loop scope
      /w14296 # 'operator': expression is always 'boolean_value'
      /w14311 # 'variable': pointer truncation from 'type1' to 'type2'
      /w14545 # expression before comma evaluates to a function which is missing
              # an argument list
      /w14546 # function call before comma missing argument list
      /w14547 # 'operator': operator before comma has no effect; expected
              # operator with side-effect
      /w14549 # 'operator': operator before comma has no effect; did you intend
              # 'operator'?
      /w14555 # expression has no effect; expected expression with side- effect
      /w14619 # pragma warning: there is no warning number 'number'
      /w14640 # Enable warning on thread un-safe static member initialization
      /w14826 # Conversion from 'type1' to 'type_2' is sign-extended. This may
              # cause unexpected runtime behavior.
      /w14905 # wide string literal cast to 'LPSTR'
      /w14906 # string literal cast to 'LPWSTR'
      /w14928 # illegal copy-initialization; more than one user-defined
              # conversion has been implicitly applied
      /permissive- # standards conformance mode for MSVC compiler.
  )

  set(clang_warnings
      -Wall
      -Wextra # reasonable and standard
      -Wshadow # warn the user if a variable declaration shadows one from a
               # parent context
      -Wcast-align # warn for potential performance problem casts
      -Wno-unused # warn on anything being unused
      -Wpedantic # warn if non-standard C++ is used
      -Wconversion # warn on type conversions that may lose data
      -Wsign-conversion # warn on sign conversions
      -Wnull-dereference # warn if a null dereference is detected
      -Wdouble-promotion # warn if float is implicit promoted to double
      -Wformat=2 # warn on security issues around functions that format output
                 # (ie printf)
      -Wno-unknown-pragmas # do not warn if encountering unknown pragmas
      -Wno-pragmas # do not warn if encountering unknown pragma options
      $<$<CXX_COMPILER_ID:AppleClang,Clang>:-Wno-unknown-warning-option> # do not
                                                                         # warn if
                                                                         # encountering
                                                                         # unknown
                                                                         # warning
                                                                         # options
  )

  if(WARNINGS_AS_ERRORS)
    set(clang_warnings ${clang_warnings} -Werror)
    set(microsoft_msvc_warnings ${microsoft_msvc_warnings} /WX)
  endif()

  set(gcc_warnings
      ${clang_warnings}
      -Wmisleading-indentation # warn if indentation implies blocks where blocks
                               # do not exist
      -Wduplicated-cond # warn if if / else chain has duplicated conditions
      -Wduplicated-branches # warn if if / else branches have duplicated code
      -Wlogical-op # warn about logical operations being used where bitwise were
                   # probably wanted
      -Wsuggest-attribute=noreturn # suggest attribute noreturn for functions
                                   # that do not return
      -Wmissing-noreturn # warn if a function declared with attribute noreturn
                         # does in fact return
  )

  if(MSVC)
    set(project_warnings ${microsoft_msvc_warnings})
  elseif(CMAKE_C_COMPILER_ID MATCHES ".*Clang")
    set(project_warnings ${clang_warnings})
  elseif(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    set(project_warnings ${gcc_warnings})
  else()
    message(
      AUTHOR_WARNING
        "No compiler warnings set for '${CMAKE_C_COMPILER_ID}' compiler.")
  endif()

  target_compile_options(${target_name} INTERFACE ${project_warnings})

  if(MSVC)
    add_compile_options(/bigobj)
  endif()

endfunction()
