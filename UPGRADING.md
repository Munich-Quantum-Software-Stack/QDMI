# Upgrade Guide

This document describes breaking changes and how to upgrade. For a complete list of changes
including minor and patch releases, please refer to the [changelog](CHANGELOG.md).

## [Unreleased]

### Introduction of Length and Duration Units

With this release, the device can use its own length and duration units to represent the respective
properties of the device, sites, and operations. The value type of all length and duration related
properties has been changed from `double` to `int64_t` or `uint64_t` (for exclusively positive
values) to represent the values in multiples of the device's own length and duration units.

The device's length and duration units can be queried using the new properties:

- `QDMI_DEVICE_PROPERTY_LENGTHUNIT`: The length unit of the device, e.g., "um" for micrometers.
- `QDMI_DEVICE_PROPERTY_DURATIONUNIT`: The duration unit of the device, e.g., "ns" for nanoseconds.
- `QDMI_DEVICE_PROPERTY_LENGTHSCALEFACTOR`: The factor by which the raw unscaled values returned by
  the device must be multiplied to obtain the actual length in the units reported by
  `QDMI_DEVICE_PROPERTY_LENGTHUNIT`.
- `QDMI_DEVICE_PROPERTY_DURATIONSCALEFACTOR`: The factor by which the raw unscaled values returned
  by the device must be multiplied to obtain the actual duration in the units reported by
  `QDMI_DEVICE_PROPERTY_DURATIONUNIT`.

The provision of the units is mandatory for all devices if they report respective properties.

### General Changes

The enum value `QDMI_SITE_PROPERTY_ID` has been renamed to `QDMI_SITE_PROPERTY_INDEX`. The
operations received a new property `QDMI_OPERATION_PROPERTY_SITES` which returns a list of sites on
which the operation can be performed.

### Neutral Atom Device Properties

This release introduces a set of new properties to represent neutral atom-based device
characteristics and capabilities. Following is a list of all new properties related to neutral atom
devices grouped by their respective categories:

#### Device Properties

- `QDMI_DEVICE_PROPERTY_MINATOMDISTANCE`: The minimum distance between two atoms in the device that
  must be maintained also during rearrangements.

#### Site Properties

- `QDMI_SITE_PROPERTY_{X,Y,Z}COORDINATE`: The X/Y/Z-coordinate of the site relative to some origin
  of the device.
- `QDMI_SITE_PROPERTY_ISZONE`: A boolean property indicating whether the site is a zone site
  representing a zone of the device with an extent.
- `QDMI_SITE_PROPERTY_{X,Y,Z}EXTENT`: The X/Y/Z-extent of the zone site (returns
  `QDMI_ERROR_NOTSUPPORTED` for regular sites).
- `QDMI_SITE_PROPERTY_MODULEINDEX`: The index of the module to which the site belongs.
- `QDMI_SITE_PROPERTY_SUBMODULEINDEX`: The index of the submodule to which the site belongs. The
  module and submodule indices facilitate the reconstruction of the site layout in a client.

#### Operation Properties

- `QDMI_OPERATION_PROPERTY_INTERACTIONRADIUS`: The interaction radius of a multi-qubit operation,
  such as a CZ-gate performed via a Rydberg interaction.
- `QDMI_OPERATION_PROPERTY_BLOCKINGRADIUS`: The blocking radius of a multi-qubit operation, such as
  a CZ-gate performed via a Rydberg interaction. While performing a multi-qubit operation, no other
  operations can be performed on sites within the blocking radius of the operation.
- `QDMI_OPERATION_PROPERTY_ISZONED`: A boolean property indicating whether the operation is a zoned
  operation, meaning that it can be performed on a zone of sites rather than individual sites.
- `QDMI_OPERATION_PROPERTY_IDLINGFIDELITY`: The idling fidelity only applies to zoned operations and
  indicates the fidelity of the operation on atoms that are not involved in the operation but are
  still affected by the laser performing the operation.
- `QDMI_OPERATION_PROPERTY_MEANSHUTTLINGSPEED`: The mean shuttling speed of the operation, which is
  the average speed at which atoms are moved during a shuttling operation.

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
