# Development Guide

<!-- IMPORTANT: Keep the line above as the first line. -->
<!----------------------------------------------------------------------------
Copyright 2024 Munich Quantum Software Stack Project

Licensed under the Apache License, Version 2.0 with LLVM Exceptions (the
"License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at

https://github.com/Munich-Quantum-Software-Stack/QDMI/blob/develop/LICENSE

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
License for the specific language governing permissions and limitations under
the License.

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-------------------------------------------------------------------------- -->

<!-- This file is a static page and included in the CMakeLists.txt file. -->

Ready to contribute to QDMI? This guide will help you get started.

## Initial Setup

1. Fork the [QDMI](https://github.com/Munich-Quantum-Software-Stack/QDMI) repository on GitHub (see
   <https://docs.github.com/en/get-started/quickstart/fork-a-repo>).

2. Clone your fork locally

   ```sh
   git clone git@github.com:your_name_here/QDMI.git
   ```

3. Change into the project directory

   ```sh
   cd QDMI
   ```

4. Create a branch for local development

   ```sh
   git checkout -b name-of-your-bugfix-or-feature
   ```

   Now you can make your changes locally.

5. (Optional, **highly recommended**) Install [pre-commit](https://pre-commit.com/) to automatically
   run a set of checks before each commit.

   <div class="tabbed">

   - <b class="tab-title">via `uv`</b> The easiest way to install pre-commit is via
     [uv](https://docs.astral.sh/uv/).

     \code{.sh} uv tool install pre-commit \endcode

   - <b class="tab-title">via `brew`</b> If you use macOS, then pre-commit is in Homebrew, use

     \code{.shell} brew install pre-commit \endcode

   - <b class="tab-title">via `pipx`</b> If you prefer to use [pipx](https://pypa.github.io/pipx/),
     you can install pre-commit with

     \code{.shell} pipx install pre-commit \endcode

   - <b class="tab-title">via `pip`</b> If you prefer to use regular `pip` (preferably in a virtual
     environment), you can install pre-commit with

     \code{.shell} pip install pre-commit \endcode

   </div>
   Afterwards, you can install the pre-commit hooks with

   \code{.shell} pre-commit install \endcode

## Working on Source Code

Building the project requires a C compiler supporting _C11_ and a minimum CMake version of _3.19_.
The example devices and the tests require a C++ compiler supporting _C++17_.

### Configure and Build

QDMI uses CMake as its build system. Building a project using CMake is a two-step process:

First, the project needs to be _configured_ by calling

```shell
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
```

This tells CMake to generate the build system in the `build` directory for a release build.

After the configuration, the project can be _built_ by calling

```shell
cmake --build build --config Release
```

### Running Tests

We use the [GoogleTest](https://google.github.io/googletest/primer.html) framework for unit testing
of the QDMI interface. All tests are contained in the `test` directory. After building the project
(as described above), the C++ unit tests can be conveniently executed by running the following
command from the main project directory:

```shell
ctest -C Release --test-dir build
```

### Code Formatting and Linting

This project mostly follows the [LLVM Coding Standard](https://llvm.org/docs/CodingStandards.html),
a set of guidelines for writing C/C++ code. To ensure the quality of the code and that it conforms
to these guidelines, we use

- [clang-tidy](https://clang.llvm.org/extra/clang-tidy/) -- a static analysis tool that checks for
  common mistakes in C/C++ code, and
- [clang-format](https://clang.llvm.org/docs/ClangFormat.html) -- a tool that automatically formats
  C/C++ code according to a given style guide.

Common IDEs like [Visual Studio Code](https://code.visualstudio.com/) or
[CLion](https://www.jetbrains.com/clion/) have plugins that can automatically run clang-tidy on the
code and automatically format it with clang-format.

- If you are using Visual Studio Code, you can install the
  [clangd extension](https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd).
- If you are using CLion, you can configure the project to use the `.clang-tidy` and `.clang-format`
  files in the project root directory.

They will automatically execute clang-tidy on your code and highlight any issues. In many cases,
they also provide quick-fixes for these issues. Furthermore, they provide a command to automatically
format your code according to the given style.

\note After configuring CMake, you can run clang-tidy on a file by calling
`clang-tidy <FILE> -- -I <PATH_TO_INCLUDE_DIRECTORY>` where `<FILE>` is the file you want to analyze
and `<PATH_TO_INCLUDE_DIRECTORY>` is the path to the `include` directory of the project.

### Format for Comments

For the information to be displayed correctly in the documentation, it the comments need to follow
the format required by Doxygen. Below you find some tags that are commonly used within the
documentation of a function:

- `@brief` For a brief, one-line description of the function. Should always be provided.
- `@details` For a longer, detailed description of the function.
- `@param` To explain the usage of a parameter. Should be provided for each parameter.
- `@return` To explain the return value. Should be provided if the function returns a value.
- `@ref` To cross-reference another part of the documentation.

\note In the current setting, the long description is always prepended with the brief description.
So there is no need to repeat the brief description in the details.

## Working on the Documentation

The documentation is generated using [Doxygen](https://www.doxygen.nl/index.html) and its
configuration is seamlessly integrated into the CMake build system.

### Building the Documentation

The documentation can be built with the CMake target `qdmi_docs` via

```shell
cmake -S . -B build
cmake --build build --target qdmi_docs
```

The generated web page can be inspected by opening the file in `docs/html/index.html` in the CMake
build directory.

### Static Content

The generated web page also contains a couple of static sites, including the main page, the support
page, the FAQ page, and this development guide. The respective Markdown files that serve as the
source for those sites are contained in `docs/` where `index.md` contains the content of the main
page.

### Dynamic Content

To include source files to be listed among the menu item `API Reference/Files`, these files must be
marked as documented by adding a comment like the following to the top of the file. Right now, this
is done for all files in the include directory.

<!-- prettier-ignore-start -->
\verbatim

/** @file
 * @brief A brief description of the file.
 * @details Some details about the file.
 */

\endverbatim
<!-- prettier-ignore-end -->

### Further Links

- For more details, see the official documentation of Doxygen that can be found here:
  [https://www.doxygen.nl/manual/docblocks.html](https://www.doxygen.nl/manual/docblocks.html).
- More tags and commands can be found in the list provided here:
  [https://www.doxygen.nl/manual/commands.html#cmd_intro](https://www.doxygen.nl/manual/commands.html#cmd_intro)
