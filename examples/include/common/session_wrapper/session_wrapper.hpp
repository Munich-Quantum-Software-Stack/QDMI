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

/// @file
/// Implementation of the @ref qdmi::SessionWrapper RAII wrapper for @ref
/// QDMI_Session "QDMI sessions".

#pragma once

#include "qdmi/core.h"

namespace qdmi {
/// An RAII wrapper that allocates and manages the lifecycle of a @ref
/// QDMI_Session.
///
/// On construction, allocates a new session via @ref
/// QDMI_context_allocate_session and immediately initializes it via @ref
/// QDMI_session_initialize`. No authentication parameters are passed during
/// initialization. On destruction, the session is freed via @ref
/// QDMI_session_free.
///
/// Copying creates a new independent session on the same context. Moving
/// transfers ownership without re-allocation.
class SessionWrapper final {
public:
  /// @brief Constructs an empty SessionWrapper with no active session.
  SessionWrapper() = default;

  /// Constructs a SessionWrapper by allocating and initializing a new @ref
  /// QDMI_session.
  ///
  /// Calls @ref QDMI_context_allocate_session to create the session, then @ref
  /// QDMI_session_initialize to initialize it.
  ///
  /// @param context is the QDMI context handle in which to create the session.
  /// @param interface is a pointer to the QDMI core interface used to allocate
  /// and initialize the session. Must not be @c NULL.
  /// @param callback is an optional logging callback to use for logging
  /// messages from the session. If not provided, no logging will occur, or the
  /// logging from the context will be inherited if present.
  /// @param user_data is an optional pointer to user data to pass to the
  /// logging callback when logging messages. If not provided, @c NULL will be
  /// passed to the callback.
  ///
  /// @throws std::runtime_error if session allocation or initialization fails,
  /// with the QDMI return code included in the error message.
  SessionWrapper(QDMI_Context context, const QDMI_Core_Interface *interface,
                 QDMI_Log_Callback callback = nullptr,
                 void *user_data = nullptr);

  SessionWrapper(const SessionWrapper &other);
  SessionWrapper &operator=(const SessionWrapper &other);
  SessionWrapper(SessionWrapper &&other) noexcept;
  SessionWrapper &operator=(SessionWrapper &&other) noexcept;
  ~SessionWrapper();

  /// @returns the @ref QDMI_Session managed by this wrapper. If no session is
  /// currently allocated, returns a @c NULL.
  [[nodiscard]] auto get() const -> QDMI_Session;

private:
  /// Swaps the contents of this instance with @p other.
  ///
  /// Exchanges the context, interface, and session handles between this
  /// instance and @p other. Used to implement copy-and-swap and move-and-swap
  /// idioms.
  ///
  /// @param other is the SessionWrapper to swap with.
  auto swap(SessionWrapper &other) noexcept -> void;

  /// The logging callback to use for logging messages.
  QDMI_Log_Callback callback_{};

  /// The user data to pass to the logging callback when logging messages.
  void *user_data_{};

  /// The QDMI context handle in which the session was created.
  QDMI_Context context_{};
  /// The QDMI core interface used to allocate, initialize, and free the
  /// session.
  const QDMI_Core_Interface *interface_{};
  /// The managed QDMI session handle.
  QDMI_Session session_{};
};
} // namespace qdmi
