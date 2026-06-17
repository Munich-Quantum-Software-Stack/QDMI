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

#include "adapter/v1_library.hpp"

namespace qdmi::adapter {
auto V1Library::session_alloc(V1Session *session) const -> int {
  if (!session_alloc_) {
    throw std::runtime_error(
        "Session allocation function not found in library");
  }
  return session_alloc_(session);
}
auto V1Library::session_set_parameter(V1Session session,
                                      const V1_SESSION_PARAMETER param,
                                      const size_t size,
                                      const void *value) const -> int {
  if (!session_set_parameter_) {
    throw std::runtime_error(
        "Session set parameter function not found in library");
  }
  return session_set_parameter_(session, param, size, value);
}
auto V1Library::session_init(V1Session session) const -> int {
  if (!session_init_) {
    throw std::runtime_error(
        "Session initialization function not found in library");
  }
  return session_init_(session);
}
auto V1Library::session_free(V1Session session) const -> void {
  if (!session_free_) {
    throw std::runtime_error("Session free function not found in library");
  }
  session_free_(session);
}
auto V1Library::session_query_device_property(V1Session session,
                                              const V1_DEVICE_PROPERTY prop,
                                              const size_t size, void *value,
                                              size_t *size_ret) const -> int {
  if (!session_query_device_property_) {
    throw std::runtime_error(
        "Session query device property function not found in library");
  }
  return session_query_device_property_(session, prop, size, value, size_ret);
}
auto V1Library::session_query_site_property(V1Session session, V1Site site,
                                            const V1_SITE_PROPERTY prop,
                                            const size_t size, void *value,
                                            size_t *size_ret) const -> int {
  if (!session_query_site_property_) {
    throw std::runtime_error(
        "Session query site property function not found in library");
  }
  return session_query_site_property_(session, site, prop, size, value,
                                      size_ret);
}
auto V1Library::session_query_operation_property(
    V1Session session, V1Operation operation, const size_t num_sites,
    const V1Site *sites, const size_t num_params, const double *params,
    const V1_OPERATION_PROPERTY prop, const size_t size, void *value,
    size_t *size_ret) const -> int {
  if (!session_query_operation_property_) {
    throw std::runtime_error(
        "Session query operation property function not found in library");
  }
  return session_query_operation_property_(session, operation, num_sites, sites,
                                           num_params, params, prop, size,
                                           value, size_ret);
}
auto V1Library::session_create_job(V1Session session, V1Job *job) const -> int {
  if (!session_create_job_) {
    throw std::runtime_error(
        "Session create job function not found in library");
  }
  return session_create_job_(session, job);
}
auto V1Library::job_set_parameter(V1Job job, const V1_JOB_PARAMETER param,
                                  const size_t size, const void *value) const
    -> int {
  if (!job_set_parameter_) {
    throw std::runtime_error("Job set parameter function not found in library");
  }
  return job_set_parameter_(job, param, size, value);
}
auto V1Library::job_query_property(V1Job job, const V1_JOB_PROPERTY prop,
                                   const size_t size, void *value,
                                   size_t *size_ret) const -> int {
  if (!job_query_property_) {
    throw std::runtime_error(
        "Job query property function not found in library");
  }
  return job_query_property_(job, prop, size, value, size_ret);
}
auto V1Library::job_submit(V1Job job) const -> int {
  if (!job_submit_) {
    throw std::runtime_error("Job submit function not found in library");
  }
  return job_submit_(job);
}
auto V1Library::job_cancel(V1Job job) const -> int {
  if (!job_cancel_) {
    throw std::runtime_error("Job cancel function not found in library");
  }
  return job_cancel_(job);
}
auto V1Library::job_check(V1Job job, V1_JOB_STATUS *status) const -> int {
  if (!job_check_) {
    throw std::runtime_error("Job check function not found in library");
  }
  return job_check_(job, status);
}
auto V1Library::job_wait(V1Job job, size_t timeout) const -> int {
  if (!job_wait_) {
    throw std::runtime_error("Job wait function not found in library");
  }
  return job_wait_(job, timeout);
}
auto V1Library::job_get_results(V1Job job, const V1_JOB_RESULT result,
                                const size_t size, void *value,
                                size_t *size_ret) const -> int {
  if (!job_get_results_) {
    throw std::runtime_error("Job get results function not found in library");
  }
  return job_get_results_(job, result, size, value, size_ret);
}
auto V1Library::job_free(V1Job job) const -> void {
  if (!job_free_) {
    throw std::runtime_error("Job free function not found in library");
  }
  job_free_(job);
}
V1Library::V1Library(Dylib dylib, std::string prefix)
    : dylib_{std::move(dylib)}, prefix_{std::move(prefix)} {
  dylib_.get_symbol(initialize_, prefix_ + "_QDMI_device_initialize");
  dylib_.get_symbol(finalize_, prefix_ + "_QDMI_device_finalize");
  dylib_.get_symbol(session_alloc_, prefix_ + "_QDMI_device_session_alloc");
  dylib_.get_symbol(session_set_parameter_,
                    prefix_ + "_QDMI_device_session_set_parameter");
  dylib_.get_symbol(session_init_, prefix_ + "_QDMI_device_session_init");
  dylib_.get_symbol(session_free_, prefix_ + "_QDMI_device_session_free");
  dylib_.get_symbol(session_query_device_property_,
                    prefix_ + "_QDMI_device_session_query_device_property");
  dylib_.get_symbol(session_query_site_property_,
                    prefix_ + "_QDMI_device_session_query_site_property");
  dylib_.get_symbol(session_query_operation_property_,
                    prefix_ + "_QDMI_device_session_query_operation_property");
  dylib_.get_symbol(session_create_job_,
                    prefix_ + "_QDMI_device_session_create_device_job");
  dylib_.get_symbol(job_set_parameter_,
                    prefix_ + "_QDMI_device_job_set_parameter");
  dylib_.get_symbol(job_query_property_,
                    prefix_ + "_QDMI_device_job_query_property");
  dylib_.get_symbol(job_submit_, prefix_ + "_QDMI_device_job_submit");
  dylib_.get_symbol(job_cancel_, prefix_ + "_QDMI_device_job_cancel");
  dylib_.get_symbol(job_check_, prefix_ + "_QDMI_device_job_check");
  dylib_.get_symbol(job_wait_, prefix_ + "_QDMI_device_job_wait");
  dylib_.get_symbol(job_get_results_, prefix_ + "_QDMI_device_job_get_results");
  dylib_.get_symbol(job_free_, prefix_ + "_QDMI_device_job_free");

  if (initialize_) {
    if (initialize_() != SUCCESS) {
      throw std::runtime_error("Failed to initialize the device");
    }
  }
}
V1Library::V1Library(const V1Library &other)
    : initialize_{other.initialize_}, finalize_{other.finalize_},
      session_alloc_{other.session_alloc_},
      session_set_parameter_{other.session_set_parameter_},
      session_init_{other.session_init_}, session_free_{other.session_free_},
      session_query_device_property_{other.session_query_device_property_},
      session_query_site_property_{other.session_query_site_property_},
      session_query_operation_property_{
          other.session_query_operation_property_},
      session_create_job_{other.session_create_job_},
      job_set_parameter_{other.job_set_parameter_},
      job_query_property_{other.job_query_property_},
      job_submit_{other.job_submit_}, job_cancel_{other.job_cancel_},
      job_check_{other.job_check_}, job_wait_{other.job_wait_},
      job_get_results_{other.job_get_results_}, job_free_{other.job_free_},
      dylib_{other.dylib_}, prefix_{other.prefix_} {
  if (initialize_) {
    if (initialize_() != SUCCESS) {
      throw std::runtime_error("Failed to initialize the device after copying");
    }
  }
}
V1Library &V1Library::operator=(const V1Library &other) {
  if (this != &other) {
    V1Library tmp{other};
    swap(tmp);
  }
  return *this;
}
V1Library::V1Library(V1Library &&other) noexcept
    : initialize_{std::move(other.initialize_)},
      finalize_{std::move(other.finalize_)},
      session_alloc_{std::move(other.session_alloc_)},
      session_set_parameter_{std::move(other.session_set_parameter_)},
      session_init_{std::move(other.session_init_)},
      session_free_{std::move(other.session_free_)},
      session_query_device_property_{
          std::move(other.session_query_device_property_)},
      session_query_site_property_{
          std::move(other.session_query_site_property_)},
      session_query_operation_property_{
          std::move(other.session_query_operation_property_)},
      session_create_job_{std::move(other.session_create_job_)},
      job_set_parameter_{std::move(other.job_set_parameter_)},
      job_query_property_{std::move(other.job_query_property_)},
      job_submit_{std::move(other.job_submit_)},
      job_cancel_{std::move(other.job_cancel_)},
      job_check_{std::move(other.job_check_)},
      job_wait_{std::move(other.job_wait_)},
      job_get_results_{std::move(other.job_get_results_)},
      job_free_{std::move(other.job_free_)}, dylib_{std::move(other.dylib_)},
      prefix_{std::move(other.prefix_)} {}
V1Library &V1Library::operator=(V1Library &&other) noexcept {
  if (this != &other) {
    swap(other);
  }
  return *this;
}
V1Library::~V1Library() {
  if (finalize_) {
    std::ignore = finalize_();
  }
}
auto V1Library::swap(V1Library &other) noexcept -> void {
  std::swap(initialize_, other.initialize_);
  std::swap(finalize_, other.finalize_);
  std::swap(session_alloc_, other.session_alloc_);
  std::swap(session_set_parameter_, other.session_set_parameter_);
  std::swap(session_init_, other.session_init_);
  std::swap(session_free_, other.session_free_);
  std::swap(session_query_device_property_,
            other.session_query_device_property_);
  std::swap(session_query_site_property_, other.session_query_site_property_);
  std::swap(session_query_operation_property_,
            other.session_query_operation_property_);
  std::swap(session_create_job_, other.session_create_job_);
  std::swap(job_set_parameter_, other.job_set_parameter_);
  std::swap(job_query_property_, other.job_query_property_);
  std::swap(job_submit_, other.job_submit_);
  std::swap(job_cancel_, other.job_cancel_);
  std::swap(job_check_, other.job_check_);
  std::swap(job_wait_, other.job_wait_);
  std::swap(job_get_results_, other.job_get_results_);
  std::swap(job_free_, other.job_free_);
  std::swap(dylib_, other.dylib_);
  std::swap(prefix_, other.prefix_);
}
} // namespace qdmi::adapter
