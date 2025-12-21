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

"""Site customization shim to enable multiprocess coverage collection in tests.

See https://coverage.readthedocs.io/en/latest/subprocess.html.
"""

from __future__ import annotations

try:
    import coverage

    coverage.process_startup()
except ImportError:
    # The 'coverage' module is optional
    # If it is not installed, we do not enable multiprocess coverage collection
    pass
