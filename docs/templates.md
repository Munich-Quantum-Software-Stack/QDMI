# Template

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

<!-- This file is a static page and included in the ./CMakeLists.txt file. -->

Together with QDMI, we provide a template meant to kick-start the implementation of a new device.
The following sections describe how to set up and use the template.

\tableofcontents

## Creating a new Project {#template-create}

The code for the template is contained in the `template/` directory of the QDMI repository. To start
a new project based on the template, perform the following CMake command from the main project
directory. Keep in mind to replace the prefix and the path as desired. To follow the naming
conventions in QDMI, pick a prefix with uppercase letters.

\note An internet connection is needed for this step as the QDMI repository will be fetched from
GitHub.

```sh
cmake -DCONFIGURE_TEMPLATE=ON \       # activate template creation
      -DTEMPLATE_PREFIX="PREFIX" \    # set your prefix here
      -DTEMPLATE_PATH="path/to/dir" \ # set the path to your project
      -S . -B build
```

This command adds a `build/` directory to your project that stores all the build files. The
configure step above only needs to be performed once. If the option `TEMPLATE_PATH` is not given it
will be placed in `PREFIX_qdmi_device` relative to the parent directory where QDMI was cloned in.
After this step you can directly start implementing your device in C++. If you want to implement in
C, see also the next section. Example implementations are provided in the `examples/` directory. See
[Examples](examples.md) for more information.

## Configuring the Template {#template-configure}

The default template is set up as a C++ project with the specified prefix. You can switch the
project to a purely C project by setting the `CXX_DEVICE` option to `OFF` in the `CMakeLists.txt`
file in the root directory of the template.

For stability, we recommend pining the version of QDMI that you are using for your implementation.
You can use any valid git tag, branch, or commit hash for that. To this end, adjust the `QDMI_REV`
variable in `cmake/ExternalDependecies.cmake` as follows:

```diff
-   set(QDMI_REV "develop"
+   set(QDMI_REV "v1.0.0"
```

When you want to change the prefix after the creation of the template, you need to change the prefix
in a couple of places. We want to give you some hints where you have to change it, but depending on
your personal project setup, they might be different or there might be more than the ones listed.
All paths are given relative to the root of the template project directory.

- `CMakeLists.txt`,
- `src/CMakeLists.txt`: the target `prefix_qdmi` and `prefix_qdmi.cpp`
- Rename `src/prefix_qdmi.cpp` accordingly
- `src/prefix_qdmi.cpp`: adopt the includes and the prefix of each function
- Rename `test/test_prefix_qdmi.cpp` accordingly
- `test/test_prefix_qdmi.cpp`: adopt the includes and the prefix of each function

## Working with the Template {#template-working}

The template is structured into three directories plus the top-level `CMakeLists.txt`. The top-level
`CMakeLists.txt` contains settings for the entire project. Some additional CMake code that imports
required dependencies is outsourced into `cmake/`.

The most important directory for your implementation is `src/` and the `.cpp` file located in that
directory. Here you find stubs for all functions that have to be implemented by a device. For every
function the `return QDMI_ERROR_NOTIMPLEMENTED;` should be replaced by a proper implementation of
the function. In particular, there should not be any computation path at the end that returns \ref
QDMI_ERROR_NOTIMPLEMENTED. Instead, some other error code from \ref QDMI_STATUS should be returned
in case of an erroneous state.

The implementation in the `src/` directory is complemented with a testing framework in `test/`. The
`.cpp` source file already contains some examples for tests. They are meant to serve as an
inspiration, and more tests should be implemented to cover everything in your device implementation.

## Building the Template and Running the Tests {#template-building}

All following commands are meant to be executed from the root directory of the template. After
configuring your project (see [Configuring the Template](#template-configure)), you can build your
project with the following command:

```sh
cmake --build build
```

If you only want to build a specific target, you can append, for example,
`--target prefix_device_test` to the command above, which will build the tests. If you only want to
build the device implementation, you can use `--target prefix_qdmi`.

To run the tests, perform the following command:

```sh
ctest --test-dir build
```

For more details on the development process, also check out the [Development Guide](guide.md).
