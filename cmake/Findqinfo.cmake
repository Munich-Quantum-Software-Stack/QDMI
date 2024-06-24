#-------------------------------------------------------------------------------
# Part of the MQSS Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#-------------------------------------------------------------------------------

include(FetchContent)

FetchContent_Declare(
    qinfo
    GIT_REPOSITORY git@github.com:Munich-Quantum-Software-Stack/QInfo.git
    GIT_TAG testing
)

FetchContent_MakeAvailable(qinfo)
