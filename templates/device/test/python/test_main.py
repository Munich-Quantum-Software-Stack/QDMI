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

"""Tests for the Python distribution of the MY QDMI device library."""

from __future__ import annotations

from typing import TYPE_CHECKING

from my.qdmi import MY_QDMI_CMAKE_DIR, MY_QDMI_INCLUDE_DIR, MY_QDMI_LIBRARY_PATH, __version__

if TYPE_CHECKING:
    from pytest_console_scripts import ScriptRunner


def test_cli_help(script_runner: ScriptRunner) -> None:
    """Test CLI with --help."""
    result = script_runner.run(["my-qdmi", "--help"])
    assert result.success
    assert "Command line interface" in result.stdout
    assert "--include_dir" in result.stdout


def test_cli_version(script_runner: ScriptRunner) -> None:
    """Test CLI with --version."""
    result = script_runner.run(["my-qdmi", "--version"])
    assert result.success
    assert __version__ in result.stdout


def test_cli_include_dir(script_runner: ScriptRunner) -> None:
    """Test CLI with --include_dir."""
    result = script_runner.run(["my-qdmi", "--include_dir"])
    assert result.success
    assert str(MY_QDMI_INCLUDE_DIR) in result.stdout


def test_cli_cmake_dir(script_runner: ScriptRunner) -> None:
    """Test CLI with --cmake_dir."""
    result = script_runner.run(["my-qdmi", "--cmake_dir"])
    assert result.success
    assert str(MY_QDMI_CMAKE_DIR) in result.stdout


def test_cli_lib_path(script_runner: ScriptRunner) -> None:
    """Test CLI with --lib_path."""
    result = script_runner.run(["my-qdmi", "--lib_path"])
    assert result.success
    assert str(MY_QDMI_LIBRARY_PATH) in result.stdout
