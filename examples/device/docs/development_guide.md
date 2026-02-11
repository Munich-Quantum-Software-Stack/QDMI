# Development Guide

Ready to contribute to the project? This guide will get you started.

## Initial Setup

1. Get the code

   **External Contribution**

   If you do not have write access to the repository, fork the repository on GitHub (see
   <https://docs.github.com/en/get-started/quickstart/fork-a-repo>) and clone your fork locally.

   ```bash
   $ git clone git@github.com:your_name_here/repository_name.git
   ```

   **Internal Contribution**

   If you do have write access to the repository, clone the repository locally.

   ```bash
   $ git clone git@github.com/org-name/repository_name.git
   ```

2. Change into the project directory

   ```bash
   $ cd repository_name
   ```

3. Create a branch for local development

   ```bash
   $ git checkout -b name-of-your-bugfix-or-feature
   ```

   Now you can make your changes locally.

4. We highly recommend to install and set up [pre-commit](https://pre-commit.com/) to automatically
   run a set of checks before each commit. You can install it via your favorite package manager
   (e.g., `pip`, `brew`, `conda`, etc.).

   Afterwards, you can install the pre-commit hooks with

   ```bash
   $ pre-commit install
   ```

## Working on the C++ library

Building the project requires a C++ compiler supporting _C++20_ and CMake with a minimum version of
_3.24_.

### Configure and Build

This project uses _CMake_ as the main build configuration tool. Building a project using CMake is a
two-stage process. First, CMake needs to be _configured_ by calling

```console
$ cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_CXX_QDMI_TESTS=ON
```

This tells CMake to

- search the current directory `.` (passed via `-S`) for a `CMakeLists.txt` file.
- process it into a directory `build` (passed via `-B`).
- the flag `-DCMAKE_BUILD_TYPE=Release` tells CMake to configure a _Release_ build (as opposed to,
  e.g., a _Debug_ build).
- the flag `-DBUILD_CXX_QDMI_TESTS=ON` enables building the tests.
- the flag `-DCXX_QDMI_COVERAGE=ON` can be added to enable code coverage support (see below).
- the flag `-DBUILD_CXX_QDMI_DOCS=ON` can be added to enable building the documentation (see below).

After configuring with CMake, the project can be built by calling

```console
$ cmake --build build --config Release
```

This tries to build the project in the `build` directory (passed via `--build`). Some operating
systems and development environments explicitly require a configuration to be set, which is why the
`--config` flag is also passed to the build command. The flag `--parallel <NUMBER_OF_THREADS>` may
be added to trigger a parallel build.

Building the project this way generates

- the main project libraries in the `build/src` directory
- some test executables in the `build/test` directory

### C++ Testing and Code Coverage

We use the [GoogleTest](https://google.github.io/googletest/primer.html) framework for testing the
C++ library. The tests are located in the `test` directory. A corresponding CI pipeline on GitHub
also runs the tests and checks for any failures.

To run the tests, you can use `ctest` after building the project (as described above).

```console
$ ctest -C Release --test-dir build --output-on-failure
```

To generate a code coverage report, you can configure the project with `-DCXX_QDMI_COVERAGE=ON` and
then run `lcov` in the build directory.

### C++ Code Formatting and Linting

This project mostly follows the [LLVM Coding Standard](https://llvm.org/docs/CodingStandards.html),
which is a set of guidelines for writing C++ code. To ensure the quality of the code and that it
conforms to these guidelines, we use

- [clang-tidy](https://clang.llvm.org/extra/clang-tidy/) -- a static analysis tool that checks for
  common mistakes in C++ code, and
- [clang-format](https://clang.llvm.org/docs/ClangFormat.html) -- a tool that automatically formats
  C++ code according to a given style guide.

Our pre-commit configuration also includes clang-format. If you have installed pre-commit, it will
automatically run clang-format on your code before each commit.

Our CI pipeline will run clang-tidy over the changes in your pull request and report any issues it
finds.

### C++ Documentation

We expect any new additions to the code base to be documented using
[Doxygen](https://www.doxygen.nl/index.html) comments. When touching existing code, we encourage you
to add Doxygen comments to the code you touch or refactor.

For some tips on how to write good Doxygen comments, see the
[Doxygen Manual](https://www.doxygen.nl/manual/docblocks.html).

## Working on the Documentation

The documentation is written in Markdown and built using
[Doxygen](https://www.doxygen.nl/index.html). The documentation source files can be found in the
`docs/` directory.

To build the documentation locally, you need to have Doxygen installed. Then, after configuring the
project with CMake and the `-DBUILD_CXX_QDMI_DOCS=ON` flag, you can build the documentation by
running:

```console
$ cmake --build build --target cxx_qdmi_device_docs
```

The documentation will be generated in the `build/docs` directory as both HTML and LaTeX. You can
view the HTML documentation by opening the `index.html` file in a web browser.
