![OS](https://img.shields.io/badge/os-linux%20%7C%20macos-blue?style=flat-square)
[![License: Apache-2.0 WITH LLVM-exception](https://img.shields.io/badge/license-Apache--2.0%20WITH%20LLVM--exception-blue.svg?style=flat-square)](LICENSE)
[![DOI](https://img.shields.io/badge/QCE-10.1109%2FQCE60285.2024.10411-blue.svg?style=flat-square)](https://doi.org/10.1109/QCE60285.2024.10411)
[![CI](https://img.shields.io/github/actions/workflow/status/Munich-Quantum-Software-Stack/QDMI/ci.yml?branch=develop&style=flat-square&logo=github&label=ci)](https://github.com/Munich-Quantum-Software-Stack/QDMI/actions/workflows/ci.yml)
[![codecov](https://img.shields.io/codecov/c/github/Munich-Quantum-Software-Stack/QDMI?style=flat-square&logo=codecov)](https://codecov.io/gh/Munich-Quantum-Software-Stack/QDMI)

<p align="center">
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="https://raw.githubusercontent.com/Munich-Quantum-Software-Stack/QDMI/develop/docs/_static/mqss_logo_dark.svg" width="20%">
    <img src="https://raw.githubusercontent.com/Munich-Quantum-Software-Stack/QDMI/develop/docs/_static/mqss_logo.svg" width="20%">
  </picture>
</p>

# QDMI — Quantum Device Management Interface

<p align="center">
  <a href="https://munich-quantum-software-stack.github.io/QDMI/">
  <img style="min-width: 200px !important; width: 30%;" src="https://img.shields.io/badge/documentation-blue?style=for-the-badge&logo=data:image/svg%2bxml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCA0NDggNTEyIj48IS0tIUZvbnQgQXdlc29tZSBGcmVlIDYuNi4wIGJ5IEBmb250YXdlc29tZSAtIGh0dHBzOi8vZm9udGF3ZXNvbWUuY29tIExpY2Vuc2UgLSBodHRwczovL2ZvbnRhd2Vzb21lLmNvbS9saWNlbnNlL2ZyZWUgQ29weXJpZ2h0IDIwMjQgRm9udGljb25zLCBJbmMuLS0+PHBhdGggZmlsbD0iI2ZmZmZmZiIgZD0iTTk2IDBDNDMgMCAwIDQzIDAgOTZMMCA0MTZjMCA1MyA0MyA5NiA5NiA5NmwyODggMCAzMiAwYzE3LjcgMCAzMi0xNC4zIDMyLTMycy0xNC4zLTMyLTMyLTMybDAtNjRjMTcuNyAwIDMyLTE0LjMgMzItMzJsMC0zMjBjMC0xNy43LTE0LjMtMzItMzItMzJMMzg0IDAgOTYgMHptMCAzODRsMjU2IDAgMCA2NEw5NiA0NDhjLTE3LjcgMC0zMi0xNC4zLTMyLTMyczE0LjMtMzIgMzItMzJ6bTMyLTI0MGMwLTguOCA3LjItMTYgMTYtMTZsMTkyIDBjOC44IDAgMTYgNy4yIDE2IDE2cy03LjIgMTYtMTYgMTZsLTE5MiAwYy04LjggMC0xNi03LjItMTYtMTZ6bTE2IDQ4bDE5MiAwYzguOCAwIDE2IDcuMiAxNiAxNnMtNy4yIDE2LTE2IDE2bC0xOTIgMGMtOC44IDAtMTYtNy4yLTE2LTE2czcuMi0xNiAxNi0xNnoiLz48L3N2Zz4=" alt="Documentation" />
  </a>
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
Contributors include [MQSC](https://mq.sc), the
[Chair for Design Automation](https://www.cda.cit.tum.de) and the
[Chair of Computer Architecture and Parallel Systems](https://www.caps.in.tum.de)
at the Technical University of Munich, and the
[Leibniz Supercomputing Centre (LRZ)](https://www.lrz.de).

<!-- [DOXYGEN MAIN] -->

> [!IMPORTANT]
>
> QDMI's development process is open to the community, encouraging contributions
> and feedback. We value your input and invite you to participate in shaping
> QDMI's future. For the latest updates and to contribute, visit our
> [issues page](https://github.com/Munich-Quantum-Software-Stack/QDMI/issues).

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
| Try sessions, discovery, jobs, and results locally     | [Getting Started](docs/getting_started.md) with the bundled mock device                      |
| Use the interface in a C or C++ project                | [Using QDMI with CMake](docs/installation.md)                                                |
| Run programs through C++, Python, Qiskit, or PennyLane | [MQT Core's QDMI guides][core-qdmi]                                                          |
| Implement a device or driver                           | [Examples](docs/examples.md), [device template](docs/templates.md), and [API reference][api] |
| Integrate an HPC application                           | [MQT Core's Slurm guide][core-slurm] and the device implementation's deployment guide        |

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
[Getting Started](docs/getting_started.md), and [API reference][api].

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

## FAQ

<!-- [DOXYGEN FAQ] -->

### What is QDMI?

QDMI is a C interface for managing quantum devices. It connects an application's
Client Interface calls to device implementations through a driver. Its session,
query, and job interfaces cover configuration, capability discovery, submission,
and result retrieval. See the
[architecture](https://munich-quantum-software-stack.github.io/QDMI/latest/md_docs_2rationale.html).

### What is MQSS, and who develops QDMI?

The
[Munich Quantum Software Stack (MQSS)](https://www.munich-quantum-valley.de/research/research-areas/mqss)
connects quantum applications, compilation and runtime infrastructure, and
device implementations. QDMI is its device-management interface and can also be
used by other software stacks.

Contributors include [MQSC](https://mq.sc), the TUM Chairs of Design Automation
and Computer Architecture and Parallel Systems, and the Leibniz Supercomputing
Centre within the Munich Quantum Valley initiative. See
[Support](https://munich-quantum-software-stack.github.io/QDMI/latest/md_docs_2support.html)
for questions and bug reports.

### Does installing QDMI let me run a quantum program?

The interface package supplies headers and CMake helpers. Executing calls
requires a compatible driver and device implementation. For a local check, use
the
[bundled mock device](https://munich-quantum-software-stack.github.io/QDMI/latest/md_docs_2getting__started.html).
For execution APIs and a local simulator, see
[MQT Core](https://mqt.readthedocs.io/projects/core/en/latest/qdmi/index.html).

### Why is it written in C and not in Python?

The C interface lets native HPC software, drivers, and device libraries share a
common ABI. Opaque handles keep implementation details behind that boundary.
Other languages can use it through bindings, including the C++ and Python
wrappers supplied by MQT Core.

### Can I use QDMI from Python?

Yes. QDMI uses a C ABI that other languages can call. MQT Core provides C++ and
Python wrappers, plus Qiskit and PennyLane adapters. These wrappers and adapters
are implementation facilities, rather than part of the QDMI C specification.

### Does every device support the same programs and results?

No. Query the selected device's capabilities, including accepted program
formats, and check the return codes of optional queries and operations.
Available results depend on the program, device, and execution mode. For
example, statevectors are simulator capabilities, rather than hardware
measurement results.

### How do I select compatible versions?

Use the headers and documentation corresponding to your driver and device
libraries. QDMI minor releases can introduce breaking changes. The Client
Interface provides an ABI query; loaders compare major and minor versions and
ignore patch differences. See the
[CMake installation guide](https://munich-quantum-software-stack.github.io/QDMI/latest/md_docs_2installation.html),
[architecture](https://munich-quantum-software-stack.github.io/QDMI/latest/md_docs_2rationale.html),
and
[upgrade guide](https://munich-quantum-software-stack.github.io/QDMI/latest/md__u_p_g_r_a_d_i_n_g.html).

### Where can I get help, contribute, or find citation information?

Use
[Support](https://munich-quantum-software-stack.github.io/QDMI/latest/md_docs_2support.html)
for questions and bug reports and
[Contributing](https://munich-quantum-software-stack.github.io/QDMI/latest/md_docs_2contributing.html)
for contributions. The
[README](https://github.com/Munich-Quantum-Software-Stack/QDMI#cite-qdmi)
contains the citation and license information.

<!-- [DOXYGEN FAQ] -->

## Contributing and Support

We welcome bug reports, implementation feedback, and contributions. Start with
[Contributing](docs/contributing.md) and the
[AI usage guidelines](docs/ai_usage.md). Use [GitHub issues][issues] or
[discussions][discussions] for questions and feedback; see
[Support](.github/SUPPORT.md) for more details.

The development of this project is led by
[Martin Schulz](mailto:martin.w.j.schulz@tum.de) (TUM CAPS), and
[Robert Wille](mailto:robert.wille@tum.de) (TUM CDA / MQSC) on the management
side and [Lukas Burgholzer](mailto:lukas.burgholzer@tum.de) (TUM CDA / MQSC) as
well as [Jorge Echavarria](mailto:jorge.echavarria@munich-quantum-valley.de)
(MQV gGmbH) from the technical side.

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
[issues]: https://github.com/Munich-Quantum-Software-Stack/QDMI/issues
[discussions]: https://github.com/Munich-Quantum-Software-Stack/QDMI/discussions
[core]: https://github.com/munich-quantum-toolkit/core
[core-qdmi]: https://mqt.readthedocs.io/projects/core/en/latest/qdmi/index.html
[core-slurm]: https://mqt.readthedocs.io/projects/core/en/latest/qdmi/slurm.html
[braket]: https://github.com/munich-quantum-software/amazon-braket-qdmi-device
[braket-docs]: https://amazon-braket-qdmi-device.readthedocs.io/en/latest/
[iqm]: https://github.com/iqm-finland/QDMI-on-IQM
[iqm-docs]: https://iqm-finland.github.io/QDMI-on-IQM/
