![OS](https://img.shields.io/badge/os-linux%20%7C%20macos-blue?style=flat-square)
[![License: Apache-2.0 WITH LLVM-exception](https://img.shields.io/badge/license-Apache--2.0%20WITH%20LLVM--exception-blue.svg?style=flat-square)](LICENSE)
[![DOI](https://img.shields.io/badge/QCE-10.1109%2FQCE60285.2024.10411-blue.svg?style=flat-square)](https://doi.org/10.1109/QCE60285.2024.10411)
[![CI](https://img.shields.io/github/actions/workflow/status/Munich-Quantum-Software-Stack/QDMI/ci.yml?branch=develop&style=flat-square&logo=github&label=ci)](https://github.com/Munich-Quantum-Software-Stack/QDMI/actions/workflows/ci.yml)
[![codecov](https://img.shields.io/codecov/c/github/Munich-Quantum-Software-Stack/QDMI?style=flat-square&logo=codecov)](https://codecov.io/gh/Munich-Quantum-Software-Stack/QDMI)

# QDMI — Quantum Device Management Interface

[![Documentation](https://img.shields.io/badge/documentation-blue?style=for-the-badge)][docs]

<!-- [DOXYGEN MAIN] -->

**QDMI provides an open, vendor-neutral interface between quantum software and
quantum devices.** It standardizes device discovery, capability and calibration
queries, and job execution, helping integrate quantum computers into classical
computing infrastructure—from local systems and HPC centres to cloud services.

QDMI grew out of the
[Munich Quantum Valley (MQV)](https://www.munich-quantum-valley.de) initiative,
where it is used as part of the
[Munich Quantum Software Stack (MQSS)](https://doi.org/10.1145/3773656.3773669).
QDMI was created jointly by TUM's
[Chair for Design Automation](https://www.cda.cit.tum.de), TUM's
[Chair of Computer Architecture and Parallel Systems](https://www.ce.cit.tum.de/en/caps/homepage/),
and the [Leibniz Supercomputing Centre (LRZ)](https://www.lrz.de). Today, it
contributes to an international standardization effort for interoperable
quantum–classical software stacks.

QDMI is maintained by the
[MQV gGmbH](https://www.munich-quantum-valley.de/de/ueber-uns/mqv-ggmbh) and
[MQSC](https://mq.sc), with contributions from the wider community.

Its reach extends from use at LRZ and MQV gGmbH through MQSS to Germany's
FullStaQD reference architecture, EuroHPC-related integration work with EQS3,
LRZ, PSNC, CESGA, CINECA, and VTT, and exploration within the ORNL-hosted
openQSE initiative. Its growing ecosystem spans simulators, quantum hardware,
and cloud services. Prominent open-source examples include IQM hardware
integration, Amazon Braket access on AWS, the DDSIM simulator in MQT Core, and
IBM Quantum integration. The MQSS QDMI Devices Suite also provides Qaptiva
simulation and access to MQSS resources at LRZ. MQT Core and MQSS supply the
surrounding drivers, application bindings, compilation, and HPC integration.

<!-- [DOXYGEN MAIN] -->

## Start Here

| Your goal                          | Guide                                                                                                                                                        |
| ---------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| Evaluate or adopt QDMI             | [Ecosystem and Community](docs/ecosystem.md): deployments, integrations, publications, and stewardship                                                       |
| Develop applications or middleware | [Architecture](docs/rationale.md), [Client Interface][client-api], and [MQT Core's C++/Python, compiler, and SDK guides][core-qdmi]                          |
| Connect hardware or services       | [Device Interface][device-api], [examples](docs/examples.md), [device template](docs/templates.md), and [implementation smoke test](docs/getting_started.md) |
| Operate infrastructure             | [Deployment responsibilities](docs/ecosystem.md), provider guides, and [MQT Core's Slurm integration][core-slurm]                                            |

## What This Repository Provides

QDMI's standardized interface covers **sessions**, **queries**, and **jobs**.
This repository supplies C11 headers, CMake integration, reference examples,
tests, and a device implementation template. The interface package is
header-only; executing programs requires a compatible driver and device
implementation. Compilers, Python bindings, SDK adapters, and scheduler
integration are provided by other components of the stack.

To run circuits locally, start with
[MQT Core's executable simulator tutorial][core-tutorial]. To develop against
the interface, see [Using QDMI with CMake](docs/installation.md) and the
[local mock workflow](docs/getting_started.md), which checks integration with
synthetic results. Further answers are in the [FAQ](docs/faq.md) and the
[versioned documentation][docs].

## Contributing and Support

Using or implementing QDMI? [Open an issue][issues] or pull request to add your
integration, deployment, or research project. Include a short description, its
current status, and any public documentation or publication.

We welcome contributions from around the world and thank
[all repository contributors](https://github.com/Munich-Quantum-Software-Stack/QDMI/graphs/contributors).
Start with [Contributing](docs/contributing.md) and the
[AI usage guidelines](docs/ai_usage.md). For questions, see
[Support](.github/SUPPORT.md) or [discussions][discussions].

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

For deployment, hardware, cloud, and reference-architecture studies, see the
[annotated publications](docs/ecosystem.md).

[docs]: https://munich-quantum-software-stack.github.io/QDMI/
[client-api]: https://munich-quantum-software-stack.github.io/QDMI/v1.4.0/group__client__interface.html
[device-api]: https://munich-quantum-software-stack.github.io/QDMI/v1.4.0/group__device__interface.html
[issues]: https://github.com/Munich-Quantum-Software-Stack/QDMI/issues
[discussions]: https://github.com/Munich-Quantum-Software-Stack/QDMI/discussions
[core-qdmi]: https://mqt.readthedocs.io/projects/core/en/latest/qdmi/index.html
[core-slurm]: https://mqt.readthedocs.io/projects/core/en/latest/qdmi/slurm.html
[core-tutorial]: https://mqt.readthedocs.io/projects/core/en/latest/tutorials/qdmi_execution.html
