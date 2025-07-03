# Changelog

All notable changes to this project will be described in this document. The format is based on a
mixture of [Keep a Changelog] and [Common Changelog].

This project adheres to [Semantic Versioning], with the exception that minor releases may include
breaking changes. Particularly, QDMI device implementations compiled against a particular version
`1.x.y` of the QDMI headers are expected to be compatible with QDMI drivers and, hence, QDMI clients
compiled against any patch release in the `1.x` series, but not necessarily with QDMI drivers and
clients compiled against a different minor or major version.

## [Unreleased]

### Added

- 🔥 Remove example device implementation in C ([#169]) ([\@ystade])
- 📝 Add changelog and upgrade guide ([#160]) ([\@ystade], [\@burgholzer])
- 🚸 Support querying of job properties incl. previously set parameters values ([#160]) ([\@ystade])
- 🚸 Add new authentication options to `QDMI_SESSION_PARAMETER` and `QDMI_DEVICE_SESSION_PARAMETER`
  enums ([#160]) ([\@ystade], [\@burgholzer])
- 🚸 Add additional `QDMI_JOB_STATUS` enum values for better job cycle management ([#160])
  ([\@ystade])
- ✨ Add required `timeout` parameter for the `QDMI_device_job_wait` and `QDMI__job_wait` functions
  ([#160]) ([\@burgholzer])

### Changed

- 🚸 Change order of `QDMI_SESSION_PARAMETER` and `QDMI_DEVICE_SESSION` enum values due to new
  authentication options ([#160]) ([\@ystade], [\@burgholzer])
- 🚸 Change order of `QDMI_JOB_STATUS` enum values to better reflect job cycle ([#160]) ([\@ystade])
- 🚚 Rename `QDMI_SITE_PROPERTY_ID` to `QDMI_SITE_PROPERTY_INDEX` ([#160]) ([\@ystade])

## [1.1.0] - 2025-01-10

_📚 Refer to the
[GitHub Release Notes](https://github.com/Munich-Quantum-Software-Stack/QDMI/releases) for previous
changelogs._

<!-- Version links -->

[unreleased]: https://github.com/Munich-Quantum-Software-Stack/QDMI/compare/v1.1.0...HEAD
[1.1.0]: https://github.com/Munich-Quantum-Software-Stack/QDMI/releases/tag/v1.1.0

<!-- PR links -->

[#160]: https://github.com/Munich-Quantum-Software-Stack/QDMI/pull/160

<!-- Contributor -->

[\@burgholzer]: https://github.com/burgholzer
[\@ystade]: https://github.com/ystade

<!-- General links -->

[Keep a Changelog]: https://keepachangelog.com/en/1.1.0/
[Common Changelog]: https://common-changelog.org
[Semantic Versioning]: https://semver.org/spec/v2.0.0.html
[GitHub Release Notes]: https://github.com/Munich-Quantum-Software-Stack/QDMI/releases
