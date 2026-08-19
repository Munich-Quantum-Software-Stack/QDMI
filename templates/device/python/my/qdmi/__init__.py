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

# ruff: file-ignore[non-empty-init-module]

import sys
from importlib.metadata import distribution
from pathlib import Path

from ._version import version as __version__

__all__ = ["MY_QDMI_CMAKE_DIR", "MY_QDMI_INCLUDE_DIR", "MY_QDMI_LIBRARY_PATH", "__version__"]


def __dir__() -> list[str]:
    return __all__


def _resolve_library_dir() -> Path:
    """Return the directory containing the packaged MY QDMI shared library.

    Raises:
        FileNotFoundError: If the expected library directory does not exist.
    """
    if sys.platform == "win32":
        library_dir = _MY_QDMI_DATA / "bin"
        if library_dir.exists():
            return library_dir
        msg = f"Expected 'bin' directory for MY QDMI library on Windows, but it does not exist: {library_dir}"
        raise FileNotFoundError(msg)

    library_dir = _MY_QDMI_DATA / "lib"
    if library_dir.exists():
        return library_dir

    library_dir = _MY_QDMI_DATA / "lib64"
    if library_dir.exists():
        return library_dir

    msg = (
        "Expected 'lib' or 'lib64' directory for MY QDMI library on Unix-like systems, "
        f"but neither exists: {library_dir}"
    )
    raise FileNotFoundError(msg)


dist = distribution("my-qdmi")
located_include_dir = dist.locate_file("my/qdmi/data/include/my_qdmi")
resolved_include_dir = Path(str(located_include_dir)).resolve(strict=True)

_MY_QDMI_DATA = resolved_include_dir.parents[1]
assert _MY_QDMI_DATA.exists(), f"MY_QDMI_DATA does not exist: {_MY_QDMI_DATA}"

_MY_QDMI_LIBRARY_DIR = _resolve_library_dir()

# the library is the sole file in the packaged library directory
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
