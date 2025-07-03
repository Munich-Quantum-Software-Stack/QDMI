# Upgrade Guide

This document describes breaking changes and how to upgrade. For a complete list of changes
including minor and patch releases, please refer to the [changelog](CHANGELOG.md).

## [Unreleased]

### General Changes

The enum value `QDMI_SITE_PROPERTY_ID` has been renamed to `QDMI_SITE_PROPERTY_INDEX`.

### Device-side Changes

- Devices now also need to implement the function `QDMI_device_job_query_property` for querying
  properties of jobs.

- The `QDMI_device_job_wait` function now has a new parameter `timeout` that specifies how long (in
  seconds) the function must wait for a result before returning with a `QDMI_ERROR_TIMEOUT` error. A
  timeout of `0` means that the function will wait indefinitely.

- New authentication options have been added to the `QDMI_DEVICE_SESSION_PARAMETER` enum.
  Specifically, besides the existing `QDMI_DEVICE_SESSION_PARAMETER_BASEURL` and
  `QDMI_DEVICE_SESSION_PARAMETER_TOKEN`, the following authentication options are now available:
  - `QDMI_DEVICE_SESSION_PARAMETER_AUTHFILE`, which allows the use of a file containing
    authentication information.
  - `QDMI_DEVICE_SESSION_PARAMETER_AUTHURL`, which allows the use of a URL for authentication.
  - `QDMI_DEVICE_SESSION_PARAMETER_USERNAME`, which allows the use of a username for authentication.
  - `QDMI_DEVICE_SESSION_PARAMETER_PASSWORD`, which allows the use of a password for authentication.

  As part of this change, the order of the enum values in `QDMI_DEVICE_SESSION_PARAMETER` has been
  changed to accommodate the new authentication options. It is the responsibility of the device to
  document the authentication options it supports.

For full compatibility with this QDMI version, the device needs to be recompiled with the latest
header file versions.

### Driver-internal Changes

- Drivers now also need to implement the function `QDMI_job_query_property` for querying properties
  of jobs.

- The function `QDMI_device_job_wait` may now also return after a timeout resulting in a new
  `QDMI_Status` code `QDMI_ERROR_TIMEOUT` that must be handled. Additionally, the `QDMI_job_wait`
  function now has a new parameter `timeout` that must be handled.

- New authentication options have been added to the `QDMI_SESSION_PARAMETER` enum. Specifically,
  besides the existing `QDMI_SESSION_PARAMETER_TOKEN` and `QDMI_SESSION_PARAMETER_PROJECTID`, the
  following authentication options are now available:
  - `QDMI_SESSION_PARAMETER_AUTHFILE`, which allows the use of a file containing authentication
    information.
  - `QDMI_SESSION_PARAMETER_AUTHURL`, which allows the use of a URL for authentication.
  - `QDMI_SESSION_PARAMETER_USERNAME`, which allows the use of a username for authentication.
  - `QDMI_SESSION_PARAMETER_PASSWORD`, which allows the use of a password for authentication.

  As part of this change, the order of the enum values in `QDMI_SESSION_PARAMETER` has been changed
  to accommodate the new authentication options. It is the responsibility of the driver to document
  the authentication options it supports.

For full compatibility with this QDMI version, the driver needs to be recompiled with the latest
header file versions.

### Client-side Changes

- A new `timeout` parameter has been added to the `QDMI_job_wait` function that influences how long
  (in seconds) the function will wait before eventually returning with `QDMI_ERROR_TIMEOUT`. A
  timeout of `0` means that the function will wait indefinitely.

- The function `QDMI_job_query_property` has been added to query properties of jobs.

- New authentication options may be provided by drivers and devices. This includes authentication
  options such as `QDMI_SESSION_PARAMETER_AUTHFILE`, `QDMI_SESSION_PARAMETER_AUTHURL`,
  `QDMI_SESSION_PARAMETER_USERNAME`, and `QDMI_SESSION_PARAMETER_PASSWORD`. The order of the enum
  values in `QDMI_SESSION_PARAMETER` has been changed to accommodate the new authentication options.

For full compatibility with this QDMI version, the client needs to be recompiled with the latest
header file versions.

[unreleased]: https://github.com/Munich-Quantum-Software-Stack/QDMI/compare/v1.1.0...HEAD
