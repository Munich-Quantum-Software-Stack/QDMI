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

"""Python wrapper for exposing the CXX QDMI device library."""

from importlib.metadata import distribution
from pathlib import Path

from ._version import version as __version__

__all__ = ["CXX_QDMI_CMAKE_DIR", "CXX_QDMI_INCLUDE_DIR", "CXX_QDMI_LIBRARY_PATH", "__version__"]


def __dir__() -> list[str]:
    return __all__


dist = distribution("cxx-qdmi")
located_include_dir = dist.locate_file("cxx/qdmi/data/include/cxx_qdmi")
resolved_include_dir = Path(str(located_include_dir)).resolve(strict=True)

CXX_QDMI_DATA = resolved_include_dir.parents[1]
assert CXX_QDMI_DATA.exists(), f"CXX_QDMI_DATA does not exist: {CXX_QDMI_DATA}"

CXX_QDMI_LIBRARY_DIR = CXX_QDMI_DATA / "lib"
if not CXX_QDMI_LIBRARY_DIR.exists():
    CXX_QDMI_LIBRARY_DIR = CXX_QDMI_DATA / "lib64"
assert CXX_QDMI_LIBRARY_DIR.exists(), f"CXX_QDMI_LIBRARY_DIR does not exist: {CXX_QDMI_LIBRARY_DIR}"

# the library is the sole file in the lib directory
library_files = list(CXX_QDMI_LIBRARY_DIR.glob("*cxx-qdmi-device*"))
if not library_files:
    msg = f"No CXX QDMI library found in: {CXX_QDMI_LIBRARY_DIR}"
    raise FileNotFoundError(msg)
CXX_QDMI_LIBRARY_PATH = library_files[0]

CXX_QDMI_INCLUDE_DIR = CXX_QDMI_DATA / "include"
assert CXX_QDMI_INCLUDE_DIR.exists(), f"CXX_QDMI_INCLUDE_DIR does not exist: {CXX_QDMI_INCLUDE_DIR}"

CXX_QDMI_CMAKE_DIR = CXX_QDMI_DATA / "share" / "cmake"
assert CXX_QDMI_CMAKE_DIR.exists(), f"CXX_QDMI_CMAKE_DIR does not exist: {CXX_QDMI_CMAKE_DIR}"

del dist, located_include_dir, resolved_include_dir
