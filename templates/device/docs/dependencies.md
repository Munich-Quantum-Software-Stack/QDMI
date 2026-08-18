# Dependencies

This project relies on several third-party libraries and tools. Below is a
comprehensive list of these dependencies along with their purposes, versions,
and licenses.

## Core Runtime Dependencies

These dependencies are linked into the shared library and
**shipped with every built wheel or binary**.

| Dependency | Version | License                        | Purpose                                  |
| :--------- | :------ | :----------------------------- | :--------------------------------------- |
| [QDMI]     | 1.3.3   | Apache-2.0 with LLVM-exception | QDMI specification and interface headers |

## Test Dependencies

Used for testing only, **not shipped** in any binary or wheel.

### C++ Tests

| Dependency   | Version | License      | Purpose                          |
| :----------- | :------ | :----------- | :------------------------------- |
| [googletest] | 1.17.0  | BSD-3-Clause | C++ unit and integration testing |

### Python Tests

| Dependency               | Version | License      | Purpose                         |
| :----------------------- | :------ | :----------- | :------------------------------ |
| [pytest]                 | ≥9.0.3  | MIT License  | Testing framework               |
| [pytest-console-scripts] | ≥1.4.1  | MIT License  | Testing CLI entry points        |
| [pytest-cov]             | ≥7.1.0  | MIT License  | Test coverage reporting         |
| [pytest-sugar]           | ≥1.1.1  | BSD-3-Clause | Prettier test output formatting |
| [pytest-xdist]           | ≥3.8.0  | MIT License  | Parallel test execution         |

## Documentation Dependencies

Used to generate the API documentation, **not shipped** in any binary or wheel.

| Dependency            | Version | License     | Purpose                      |
| :-------------------- | :------ | :---------- | :--------------------------- |
| [Doxygen]             | 1.16.1  | GNU GPL v2  | API documentation generation |
| [doxygen-awesome-css] | 2.4.1   | MIT License | Modern Doxygen theme         |

> [!NOTE]
> Doxygen is licensed under GNU GPL v2, but
> [documents produced by Doxygen are derivative works of the input, not of Doxygen itself](https://www.doxygen.nl/manual/),
> and are therefore not affected by the GPL. The generated documentation remains
> under the project's own license terms.

[QDMI]: https://github.com/Munich-Quantum-Software-Stack/qdmi
[googletest]: https://github.com/google/googletest
[pytest]: https://github.com/pytest-dev/pytest
[pytest-console-scripts]: https://github.com/pytest-dev/pytest-console-scripts
[pytest-cov]: https://github.com/pytest-dev/pytest-cov
[pytest-sugar]: https://github.com/Teemu/pytest-sugar
[pytest-xdist]: https://github.com/pytest-dev/pytest-xdist
[Doxygen]: https://doxygen.nl/
[doxygen-awesome-css]: https://github.com/jothepro/doxygen-awesome-css
