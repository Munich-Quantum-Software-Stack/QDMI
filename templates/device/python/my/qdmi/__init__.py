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

"""Python wrapper for exposing the MY QDMI device library."""

from importlib.metadata import distribution
from pathlib import Path

from ._version import version as __version__

__all__ = ["MY_QDMI_CMAKE_DIR", "MY_QDMI_INCLUDE_DIR", "MY_QDMI_LIBRARY_PATH", "__version__"]


def __dir__() -> list[str]:
    return __all__


dist = distribution("my-qdmi")
located_include_dir = dist.locate_file("my/qdmi/data/include/my_qdmi")
resolved_include_dir = Path(str(located_include_dir)).resolve(strict=True)

_MY_QDMI_DATA = resolved_include_dir.parents[1]
assert _MY_QDMI_DATA.exists(), f"MY_QDMI_DATA does not exist: {_MY_QDMI_DATA}"

_MY_QDMI_LIBRARY_DIR = _MY_QDMI_DATA / "lib"
if not _MY_QDMI_LIBRARY_DIR.exists():
    _MY_QDMI_LIBRARY_DIR = _MY_QDMI_DATA / "lib64"
assert _MY_QDMI_LIBRARY_DIR.exists(), f"MY_QDMI_LIBRARY_DIR does not exist: {_MY_QDMI_LIBRARY_DIR}"

# the library is the sole file in the lib directory
library_files = list(_MY_QDMI_LIBRARY_DIR.glob("*my-qdmi-device*"))
if not library_files:
    msg = f"No MY QDMI library found in: {_MY_QDMI_LIBRARY_DIR}"
    raise FileNotFoundError(msg)
MY_QDMI_LIBRARY_PATH = library_files[0]

MY_QDMI_INCLUDE_DIR = _MY_QDMI_DATA / "include"
assert MY_QDMI_INCLUDE_DIR.exists(), f"MY_QDMI_INCLUDE_DIR does not exist: {MY_QDMI_INCLUDE_DIR}"

MY_QDMI_CMAKE_DIR = _MY_QDMI_DATA / "share" / "cmake"
assert MY_QDMI_CMAKE_DIR.exists(), f"MY_QDMI_CMAKE_DIR does not exist: {MY_QDMI_CMAKE_DIR}"

del dist, located_include_dir, resolved_include_dir
