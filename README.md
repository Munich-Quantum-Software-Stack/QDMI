# QDMI — Quantum Device Management Interface

![OS](https://img.shields.io/badge/os-linux%20%7C%20macos-blue?style=flat-square)
[![License: Apache-2.0 WITH LLVM-exception](https://img.shields.io/badge/license-Apache--2.0%20WITH%20LLVM--exception-blue.svg?style=flat-square)](LICENSE)
[![DOI](https://img.shields.io/badge/QCE-10.1109%2FQCE60285.2024.10411-blue.svg?style=flat-square)](https://doi.org/10.1109/QCE60285.2024.10411)
[![CI](https://img.shields.io/github/actions/workflow/status/Munich-Quantum-Software-Stack/QDMI/ci.yml?branch=develop&style=flat-square&logo=github&label=ci)](https://github.com/Munich-Quantum-Software-Stack/QDMI/actions/workflows/ci.yml)
[![codecov](https://img.shields.io/codecov/c/github/Munich-Quantum-Software-Stack/QDMI?style=flat-square&logo=codecov)](https://codecov.io/gh/Munich-Quantum-Software-Stack/QDMI)
[![Documentation](https://img.shields.io/badge/docs-Doxygen-blue?style=flat-square)](https://munich-quantum-software-stack.github.io/QDMI/)

<p align="center">
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="https://raw.githubusercontent.com/Munich-Quantum-Software-Stack/QDMI/develop/docs/_static/mqss_logo_dark.svg" width="20%">
    <img src="https://raw.githubusercontent.com/Munich-Quantum-Software-Stack/QDMI/develop/docs/_static/mqss_logo.svg" width="20%">
  </picture>
</p>

<!-- [DOXYGEN MAIN] -->

**QDMI connects quantum software to quantum devices through a common C
interface.** Applications can discover device capabilities, configure sessions,
submit programs, and retrieve results through a driver. Device implementations
translate these calls into simulator operations or hardware and cloud APIs.

QDMI is part of the
[Munich Quantum Software Stack (MQSS)](https://www.munich-quantum-valley.de/research/research-areas/mqss),
developed within the
[Munich Quantum Valley (MQV)](https://www.munich-quantum-valley.de).
Contributors include the
[Munich Quantum Software Company (MQSC)](https://mq.sc), the
[Chair for Design Automation](https://www.cda.cit.tum.de) and the
[Chair of Computer Architecture and Parallel Systems](https://www.caps.in.tum.de)
at the Technical University of Munich, and the
[Leibniz Supercomputing Centre (LRZ)](https://www.lrz.de).

<!-- [DOXYGEN MAIN] -->

## What QDMI Provides

- **Sessions** to configure and initialize access through a driver and its
  device implementations.
- **Queries** for device properties, sites, operations, connectivity, and
  available calibration data.
- **Jobs** to submit supported program formats, track execution, and retrieve
  available results.

This repository provides the C11 interface headers, CMake integration, reference
examples, tests, and a device implementation template. To run a program, use a
compatible driver and device implementation. QDMI's interface package is
header-only; compilers, Python bindings, SDK adapters, and scheduler integration
are supplied by other components of the stack.

## Getting Started

| I want to…                                             | Start here                                                                                   |
| ------------------------------------------------------ | -------------------------------------------------------------------------------------------- |
| Understand the components and their responsibilities   | [Architecture and rationale](docs/rationale.md)                                              |
| Try sessions, discovery, jobs, and results locally     | [Getting started](docs/getting_started.md) with the bundled mock device                      |
| Use the interface in a C or C++ project                | [Using QDMI with CMake](docs/installation.md)                                                |
| Run programs through C++, Python, Qiskit, or PennyLane | [MQT Core's QDMI guides][core-qdmi]                                                          |
| Implement a device or driver                           | [Examples](docs/examples.md), [device template](docs/templates.md), and [API reference][api] |
| Integrate an HPC application                           | [MQT Core's Slurm guide][core-slurm] and the device implementation's deployment guide        |

> [!NOTE]
> This documentation describes **QDMI 1.4**. Select matching interface, driver,
> and device versions from the [releases page][releases], and consult the
> [upgrade guide](UPGRADING.md) when moving between minor versions.

## Implementations and Integrations

| Project                             | What it provides                                                                                 | Documentation                                                 |
| ----------------------------------- | ------------------------------------------------------------------------------------------------ | ------------------------------------------------------------- |
| [MQT Core][core]                    | QDMI driver, C++ and Python APIs, local DDSIM execution, compilation targets, and SDK adapters   | [Driver and device configuration][core-qdmi]                  |
| [Amazon Braket QDMI device][braket] | Access to Amazon Braket gate-model QPUs and simulators through a device library                  | [Installation, catalogue, and AWS configuration][braket-docs] |
| [QDMI-on-IQM][iqm]                  | Access to IQM systems through the IQM Server API, including architecture and calibration queries | [Installation, usage, and deployment][iqm-docs]               |

These projects own their installation instructions, device catalogues,
credentials, accepted program formats, and deployment options. Consult their
current documentation when selecting compatible versions. A common interface
does not make every program format or result available on every device.

## Documentation and Background

The [documentation][docs] includes the [FAQ](docs/faq.md),
[architecture and rationale](docs/rationale.md),
[local onboarding](docs/getting_started.md), and [API reference][api].

For the architecture and deployment context, see:

- [The Munich Quantum Software Stack](https://doi.org/10.1145/3773656.3773669)
  (SCA/HPCAsia 2026).
- [Standardizing Access to Heterogeneous Quantum Backends](https://arxiv.org/abs/2603.05138)
  (Amazon Braket case study).
- [Practical HPCQC Integration with QDMI](https://arxiv.org/abs/2604.19869)
  (IQM case study).

The papers describe the architecture and particular implementation snapshots.
Use the documentation and headers for your selected version as the API
reference.

## Contributing and Support

We welcome bug reports, implementation feedback, and contributions. Start with
[Contributing](docs/contributing.md) and the
[AI usage guidelines](docs/ai_usage.md). Use [GitHub issues][issues] or
[discussions][discussions] for questions and feedback; see
[Support](.github/SUPPORT.md) for more details.

Development is led by Martin Schulz (TUM CAPS) and Robert Wille (TUM CDA /
MQSC), with Lukas Burgholzer (TUM CDA / MQSC) and Jorge Echavarria (MQV)
providing technical leadership.

QDMI is released under [Apache-2.0 with LLVM exceptions](LICENSE).

## Cite QDMI

If you use QDMI in your research, please cite:

```bibtex
@inproceedings{qdmi,
    title = {{QDMI -- Quantum Device Management Interface: Hardware-Software Interface for the Munich Quantum Software Stack}},
    shorttitle = {{QDMI -- Quantum Device Management Interface}},
    booktitle = {IEEE International Conference on Quantum Computing and Engineering (QCE)},
    author = {Wille, Robert and Schmid, Ludwig and Stade, Yannick and Echavarria, Jorge and Schulz, Martin and Schulz, Laura and Burgholzer, Lukas},
    date = {2024},
    doi = {10.1109/QCE60285.2024.10411},
}
```

[docs]: https://munich-quantum-software-stack.github.io/QDMI/
[api]: https://munich-quantum-software-stack.github.io/QDMI/latest/files.html
[releases]: https://github.com/Munich-Quantum-Software-Stack/QDMI/releases
[issues]: https://github.com/Munich-Quantum-Software-Stack/QDMI/issues
[discussions]: https://github.com/Munich-Quantum-Software-Stack/QDMI/discussions
[core]: https://github.com/munich-quantum-toolkit/core
[core-qdmi]: https://mqt.readthedocs.io/projects/core/en/latest/qdmi/index.html
[core-slurm]: https://mqt.readthedocs.io/projects/core/en/latest/qdmi/slurm.html
[braket]: https://github.com/munich-quantum-software/amazon-braket-qdmi-device
[braket-docs]: https://amazon-braket-qdmi-device.readthedocs.io/en/latest/
[iqm]: https://github.com/iqm-finland/QDMI-on-IQM
[iqm-docs]: https://iqm-finland.github.io/QDMI-on-IQM/
