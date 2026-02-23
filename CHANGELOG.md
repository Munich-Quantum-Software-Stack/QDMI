# Changelog

All notable changes to this project will be described in this document. The format is based on a
mixture of [Keep a Changelog] and [Common Changelog].

This project adheres to [Semantic Versioning], with the exception that minor releases may include
breaking changes. Particularly, QDMI device implementations compiled against a particular version
`1.x.y` of the QDMI headers are expected to be compatible with QDMI drivers and, hence, QDMI clients
compiled against any patch release in the `1.x` series, but not necessarily with QDMI drivers and
clients compiled against a different minor or major version.

## [Unreleased]

### Changed

- 🚸 Export _all_ device headers (incl. `constants.h`) as part of device implementations such that they do not need to link against the header-only QDMI library anymore ([#325]) ([\@ystade], [\@burgholzer])
- 🚸 Increase robustness of the QDMI device template (complete install instructions, uv caching, code cleanup) ([#333]) ([\@burgholzer])

## [1.2.1] - 2025-12-22

_If you are upgrading: please see [`UPGRADING.md`](UPGRADING.md#121)._

### Added

- 🚸 Provide compile definitions for inferring device and QDMI versions automatically ([#272])
  ([\@burgholzer])

### Changed

- 🚸 Modernize QDMI device template (C++20, boilerplate docs, installation instructions, Python
  distribution, and more) ([#285]) ([\@burgholzer])
- ♻️ Update example libraries and test targets to C++20 ([#285]) ([\@burgholzer])
- 🚸 Improve prefix handling by making the device target name flexible ([#274], [#275])
  ([\@burgholzer])
- 👨‍💻 Turn off building QDMI documentation by default ([#269]) ([\@burgholzer])

### Fixed

- 🐛 Ensure devices can be instantiated from an installed version of QDMI ([#273]) ([\@burgholzer])
- 🐛 Fix target name mismatch for `qdmi_project_warnings` between source and installed version of
  QDMI ([#270]) ([\@burgholzer])

## [1.2.0] - 2025-12-01

_If you are upgrading: please see [`UPGRADING.md`](UPGRADING.md#120)._

### Added

- ✨ Add comprehensive properties to query neutral atom-based device characteristics and
  capabilities, including site coordinates, zone properties, module/submodule indices, interaction
  and blocking radii, idling fidelity, and shuttling speed ([#198], [#199], [#200], [#203], [#207],
  [#208], [#209], [#211]) ([\@ystade], [\@burgholzer])
- ✨ Add new program formats `QDMI_PROGRAM_FORMAT_QPY` (Qiskit QPY binary format) and
  `QDMI_PROGRAM_FORMAT_IQMJSON` (IQM JSON data transfer format) to the `QDMI_Program_Format` enum
  ([#234]) ([\@burgholzer])
- ✨ Add properties to query the device's duration unit and scale factor for proper interpretation
  of duration values ([#210]) ([\@ystade], [\@burgholzer])
- ✨ Add property to efficiently query the applicability of operations to specific sites ([#207])
  ([\@ystade], [\@burgholzer])
- ✨ Add device property `QDMI_DEVICE_PROPERTY_SUPPORTEDPROGRAMFORMATS` for efficiently querying the
  device's supported program formats ([#252]) ([\@burgholzer])
- ✨ Add device property `QDMI_DEVICE_PROPERTY_PULSESUPPORT` for querying pulse-level control
  support level ([#181]) ([\@mnfarooqi])
- 🚸 Add new functions `QDMI_job_query_property` and `QDMI_device_job_query_property` to support
  querying of job properties, including previously set parameter values ([#160]) ([\@ystade])
- 🚸 Add new authentication options (`AUTHFILE`, `AUTHURL`, `USERNAME`, `PASSWORD`) to
  `QDMI_SESSION_PARAMETER` and `QDMI_DEVICE_SESSION_PARAMETER` enums ([#160]) ([\@ystade],
  [\@burgholzer])
- 🚸 Add additional `QDMI_JOB_STATUS` enum values for improved job lifecycle management ([#160])
  ([\@ystade])
- 📝 Add comprehensive changelog and upgrade guide documentation ([#160]) ([\@ystade],
  [\@burgholzer])

### Changed

- 🚚 **Breaking**: Rename `QDMI_SITE_PROPERTY_ID` to `QDMI_SITE_PROPERTY_INDEX` for improved naming
  consistency ([#160]) ([\@ystade])
- ♻️ **Breaking**: Change types of duration-related properties from `double` to `int64_t` or
  `uint64_t`, representing values in device-specific units ([#210]) ([\@ystade], [\@burgholzer])
- ♻️ **Breaking**: Add required `timeout` parameter (in seconds, 0 for indefinite) to
  `QDMI_device_job_wait` and `QDMI_job_wait` functions ([#160]) ([\@burgholzer])
- 🚸 **Breaking**: Change order of `QDMI_SESSION_PARAMETER` and `QDMI_DEVICE_SESSION_PARAMETER` enum
  values due to new authentication options ([#160]) ([\@ystade], [\@burgholzer])
- 🚸 **Breaking**: Change order of `QDMI_JOB_STATUS` enum values to better reflect the job lifecycle
  ([#160]) ([\@ystade])
- 📦 **Breaking**: Raise minimum required CMake version to 3.24 ([#250]) ([\@burgholzer])
- 🔧 Improve library installation setup and header management for better downstream integration
  ([#228]) ([\@burgholzer])
- 🔧 Set C++ standard on a per-target basis rather than globally ([#165]) ([\@ystade])

### Fixed

- 🐛 Fix incorrect definitions of `QDMI_Site` and `QDMI_Operation` in device template ([#169])
  ([\@ystade])
- 🐛 Add missing `qdmi_project_warnings` target to CMake export configuration ([#214]) ([\@ystade])
- 🐛 Fix bug in example tool causing incorrect behavior ([#218]) ([\@rainij])
- 📝 Re-enable FetchContent for Doxygen and build documentation by default ([#250]) ([\@burgholzer])
- ✏️ Fix typo in error constant naming: `QDMI_ERROR_NOTIMPLEMENTED` ([#195]) ([\@burgholzer])

### Removed

- 🔥 Remove C language example device implementation (C++ implementation remains) ([#165])
  ([\@ystade])

## [1.1.0] - 2025-01-10

_📚 Refer to the
[GitHub Release Notes](https://github.com/Munich-Quantum-Software-Stack/QDMI/releases) for previous
changelogs._

<!-- Version links -->

[unreleased]: https://github.com/Munich-Quantum-Software-Stack/QDMI/compare/v1.2.1...HEAD
[1.2.1]: https://github.com/Munich-Quantum-Software-Stack/QDMI/releases/tag/v1.2.1
[1.2.0]: https://github.com/Munich-Quantum-Software-Stack/QDMI/releases/tag/v1.2.0
[1.1.0]: https://github.com/Munich-Quantum-Software-Stack/QDMI/releases/tag/v1.1.0

<!-- PR links -->

[#333]: https://github.com/Munich-Quantum-Software-Stack/QDMI/pull/333
[#325]: https://github.com/Munich-Quantum-Software-Stack/QDMI/pull/325
[#285]: https://github.com/Munich-Quantum-Software-Stack/QDMI/pull/285
[#275]: https://github.com/Munich-Quantum-Software-Stack/QDMI/pull/275
[#274]: https://github.com/Munich-Quantum-Software-Stack/QDMI/pull/274
[#273]: https://github.com/Munich-Quantum-Software-Stack/QDMI/pull/273
[#272]: https://github.com/Munich-Quantum-Software-Stack/QDMI/pull/272
[#270]: https://github.com/Munich-Quantum-Software-Stack/QDMI/pull/270
[#269]: https://github.com/Munich-Quantum-Software-Stack/QDMI/pull/269
[#252]: https://github.com/Munich-Quantum-Software-Stack/QDMI/pull/252
[#250]: https://github.com/Munich-Quantum-Software-Stack/QDMI/pull/250
[#234]: https://github.com/Munich-Quantum-Software-Stack/QDMI/pull/234
[#228]: https://github.com/Munich-Quantum-Software-Stack/QDMI/pull/228
[#218]: https://github.com/Munich-Quantum-Software-Stack/QDMI/pull/218
[#214]: https://github.com/Munich-Quantum-Software-Stack/QDMI/pull/214
[#211]: https://github.com/Munich-Quantum-Software-Stack/QDMI/pull/211
[#210]: https://github.com/Munich-Quantum-Software-Stack/QDMI/pull/210
[#209]: https://github.com/Munich-Quantum-Software-Stack/QDMI/pull/209
[#208]: https://github.com/Munich-Quantum-Software-Stack/QDMI/pull/208
[#207]: https://github.com/Munich-Quantum-Software-Stack/QDMI/pull/207
[#203]: https://github.com/Munich-Quantum-Software-Stack/QDMI/pull/203
[#200]: https://github.com/Munich-Quantum-Software-Stack/QDMI/pull/200
[#199]: https://github.com/Munich-Quantum-Software-Stack/QDMI/pull/199
[#198]: https://github.com/Munich-Quantum-Software-Stack/QDMI/pull/198
[#195]: https://github.com/Munich-Quantum-Software-Stack/QDMI/pull/195
[#181]: https://github.com/Munich-Quantum-Software-Stack/QDMI/pull/181
[#169]: https://github.com/Munich-Quantum-Software-Stack/QDMI/pull/169
[#165]: https://github.com/Munich-Quantum-Software-Stack/QDMI/pull/165
[#160]: https://github.com/Munich-Quantum-Software-Stack/QDMI/pull/160

<!-- Contributors -->

[\@burgholzer]: https://github.com/burgholzer
[\@ystade]: https://github.com/ystade
[\@mnfarooqi]: https://github.com/mnfarooqi
[\@rainij]: https://github.com/rainij

<!-- General links -->

[Keep a Changelog]: https://keepachangelog.com/en/1.1.0/
[Common Changelog]: https://common-changelog.org
[Semantic Versioning]: https://semver.org/spec/v2.0.0.html
[GitHub Release Notes]: https://github.com/Munich-Quantum-Software-Stack/QDMI/releases
