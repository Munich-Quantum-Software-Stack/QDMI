/*
 * Copyright (c) 2024 - 2026 QDMI Maintainers
 * All rights reserved.
 *
 * Licensed under the Apache License v2.0 with LLVM Exceptions (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * https://llvm.org/LICENSE.txt
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

/** @file
 * @brief Defines macros for controlling the visibility of symbols.
 */

#pragma once

#ifdef QDMI_STATIC_DEFINE
/// Mark symbols as visible to other modules (no effect on static library
/// builds).
#define QDMI_EXPORT
/// Mark symbols as not visible to other modules (no effect on static library
/// builds).
#define QDMI_NO_EXPORT
#else
#ifndef QDMI_EXPORT
#ifdef QDMI_device_EXPORTS
/* We are building this library */
#ifdef _WIN32
/// Mark symbols as visible to other modules.
#define QDMI_EXPORT __declspec(dllexport)
#else
/// Mark symbols as visible to other modules.
#define QDMI_EXPORT __attribute__((visibility("default")))
#endif
#else
/* We are using this library */
#ifdef _WIN32
/// Mark symbols as visible to other modules.
#define QDMI_EXPORT __declspec(dllimport)
#else
/// Mark symbols as visible to other modules.
#define QDMI_EXPORT __attribute__((visibility("default")))
#endif
#endif
#endif

#ifndef QDMI_NO_EXPORT
#ifdef _WIN32
/// Mark symbols as not visible to other modules.
#define QDMI_NO_EXPORT
#else
/// Mark symbols as not visible to other modules.
#define QDMI_NO_EXPORT __attribute__((visibility("hidden")))
#endif
#endif
#endif

#ifndef QDMI_DEPRECATED
#ifdef _WIN32
/// Mark symbols as deprecated.
#define QDMI_DEPRECATED __declspec(deprecated)
#else
/// Mark symbols as deprecated.
#define QDMI_DEPRECATED __attribute__((__deprecated__))
#endif
#endif

#ifndef QDMI_DEPRECATED_EXPORT
/// Mark symbols as deprecated but export them.
#define QDMI_DEPRECATED_EXPORT QDMI_EXPORT QDMI_DEPRECATED
#endif

#ifndef QDMI_DEPRECATED_NO_EXPORT
/// Mark symbols as deprecated but do not export them.
#define QDMI_DEPRECATED_NO_EXPORT QDMI_NO_EXPORT QDMI_DEPRECATED
#endif
