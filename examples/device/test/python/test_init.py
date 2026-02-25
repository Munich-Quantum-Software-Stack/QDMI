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

"""Tests for the Python distribution of the CXX QDMI device library."""

from __future__ import annotations

from pathlib import Path

from cxx.qdmi import CXX_QDMI_CMAKE_DIR, CXX_QDMI_INCLUDE_DIR, CXX_QDMI_LIBRARY_PATH, __version__


def test_version_exists() -> None:
    """Test that __version__ is defined and non-empty."""
    assert __version__
    assert isinstance(__version__, str)
    assert len(__version__) > 0


def test_include_dir_exists() -> None:
    """Test that CXX_QDMI_INCLUDE_DIR exists and is a directory."""
    assert CXX_QDMI_INCLUDE_DIR.exists()
    assert CXX_QDMI_INCLUDE_DIR.is_dir()
    assert "include" in str(CXX_QDMI_INCLUDE_DIR)


def test_include_dir_has_cxx_qdmi_headers() -> None:
    """Test that the include directory contains CXX QDMI headers."""
    cxx_qdmi_include = CXX_QDMI_INCLUDE_DIR / "cxx_qdmi"
    assert cxx_qdmi_include.exists()
    assert cxx_qdmi_include.is_dir()


def test_cmake_dir_exists() -> None:
    """Test that CXX_QDMI_CMAKE_DIR exists and is a directory."""
    assert CXX_QDMI_CMAKE_DIR.exists()
    assert CXX_QDMI_CMAKE_DIR.is_dir()
    assert "cmake" in str(CXX_QDMI_CMAKE_DIR)


def test_library_path_exists() -> None:
    """Test that CXX_QDMI_LIBRARY_PATH exists and is a file."""
    assert CXX_QDMI_LIBRARY_PATH.exists()
    assert CXX_QDMI_LIBRARY_PATH.is_file()
    assert "cxx-qdmi-device" in str(CXX_QDMI_LIBRARY_PATH)


def test_paths_are_pathlib_objects() -> None:
    """Test that all path variables are pathlib.Path objects."""
    assert isinstance(CXX_QDMI_INCLUDE_DIR, Path)
    assert isinstance(CXX_QDMI_CMAKE_DIR, Path)
    assert isinstance(CXX_QDMI_LIBRARY_PATH, Path)


def test_paths_are_absolute() -> None:
    """Test that all paths are absolute."""
    assert CXX_QDMI_INCLUDE_DIR.is_absolute()
    assert CXX_QDMI_CMAKE_DIR.is_absolute()
    assert CXX_QDMI_LIBRARY_PATH.is_absolute()
