# Template

<!-- IMPORTANT: Keep the line above as the first line. -->

<!-- This file is a static page and included in the ./CMakeLists.txt file. -->

Together with QDMI, we provide a template meant to kick-start the implementation
of a new device. The following sections describe how to set up and use the
template.

\tableofcontents

## Creating a new Project {#template-create}

The code for the template is contained in the `template/` directory of the QDMI
repository. To start a new project based on the template, configure QDMI once to
define the prefix and output path, then explicitly build the `qdmi-template`
target that writes the files.

\note An internet connection is needed for this step as the QDMI repository will
be fetched from GitHub.

```sh
cmake -DQDMI_GENERATE_TEMPLATE=ON \
      -DTEMPLATE_PREFIX="PREFIX" \
      -DTEMPLATE_PATH="path/to/dir" \
      -S . -B build

# actually write the template files
cmake --build build --target qdmi-template
```

If the option `TEMPLATE_PATH` is not given it will be placed in
`PREFIX-qdmi-device` relative to the parent directory where QDMI was cloned in.

If you want to regenerate into an existing directory, use:

```sh
cmake --build build --target qdmi-template-force
```

After this step you can directly start implementing your device in C++. Example
implementations are provided in the `examples/` directory. See
[Examples](examples.md) for more information.

## Configuring the Template {#template-configure}

For stability, we recommend pinning the version of QDMI that you are using for
your implementation. You can use any valid git tag, branch, or commit hash for
that. To this end, adjust the `QDMI_REV` variable in
`cmake/ExternalDependecies.cmake` as follows:

```diff
-   set(QDMI_REV "develop"
+   set(QDMI_REV "v1.2.0"
```

The generated project assigns the package ID `prefix.default` to its device
target. Change the project-specific `PREFIX_QDMI_DEVICE_ID` CMake cache variable
if the device package needs a different ID. This build-system value identifies a
packaged device implementation. It is not the runtime `QDMI_DEVICE_PROPERTY_ID`,
which a Client driver assigns to a logical resource.

The device target calls `configure_qdmi_device_target` to export its package ID
and symbol prefix as the `QDMI_DEVICE_ID` and `QDMI_DEVICE_PREFIX` target
properties. Build-system consumers such as MQT Core can use this metadata to
package and register the device without project-specific loader code or a
runtime dependency from the device implementation to that consumer.

When you want to change the prefix after the creation of the template, you need
to change the prefix in a couple of places. We want to give you some hints where
you have to change it, but depending on your personal project setup, they might
be different or there might be more than the ones listed. All paths are given
relative to the root of the template project directory.

- `CMakeLists.txt`,
- `src/CMakeLists.txt`: the target `prefix_qdmi` and `prefix_qdmi.cpp`
- Rename `src/prefix_qdmi.cpp` accordingly
- `src/prefix_qdmi.cpp`: adapt the includes and the prefix of each function
- Rename `test/test_prefix_qdmi.cpp` accordingly
- `test/test_prefix_qdmi.cpp`: adapt the includes and the prefix of each
  function
- `pyproject.toml`: adapt the package name and several paths
- `python/prefix`: adapt the package namespace in the directory structure
- `python/prefix/qdmi/*`: adapt the prefix throughout the package
- `test/python/*`: adapt the prefix throughout the tests
- `docs/*`: the package name and several paths

## Working with the Template {#template-working}

The template is structured into five directories. The top-level `CMakeLists.txt`
contains settings for the entire project. Some additional CMake code that
imports required dependencies is outsourced into `cmake/`.

The most important directory for your implementation is `src/` and the `.cpp`
file located in that directory. Here you find stubs for all functions that have
to be implemented by a device. For every function the
`return QDMI_ERROR_NOTIMPLEMENTED;` should be replaced by a proper
implementation of the function. In particular, there should not be any
computation path at the end that returns \ref QDMI_ERROR_NOTIMPLEMENTED.
Instead, some other error code from \ref QDMI_STATUS should be returned in case
of an erroneous state.

QDMI entry points have C linkage, so exceptions must not escape them. Catch
exceptions directly at entry points that call throwing C++ code, map
`std::bad_alloc` to \ref QDMI_ERROR_OUTOFMEM, and map unexpected exceptions to
\ref QDMI_ERROR_FATAL. Catch provider-specific exceptions first when a more
precise QDMI status applies. A function-try-block keeps this boundary local and
does not require wrapping non-throwing entry points.

The implementation in the `src/` directory is complemented with a testing
framework in `test/`. The `.cpp` source file already contains some examples for
tests. They are meant to serve as an inspiration, and more tests should be
implemented to cover everything in your device implementation.

The `python` directory in combination with the `pyproject.toml` file contains
some basic packaging setup for distributing the device implementation as a
Python package.

The `docs/` directory contains the documentation for the template. This is
intended to be used as a starting point for your own documentation.

Some more files are present in the root directory of the template, such as
`LICENSE`, `README.md`, and `.gitignore`.

## Building the Template and Running the Tests {#template-building}

All following commands are meant to be executed from the root directory of the
template. After configuring your project (see
[Configuring the Template](#template-configure)), you can build your project
with the following command:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

If you only want to build a specific target, you can append, for example,
`--target prefix_device_test` to the command above, which will build the tests.
If you only want to build the device implementation, you can use
`--target prefix_qdmi`.

To run the tests, perform the following command:

```sh
ctest --test-dir build
```

For more details on the development process, also check out the
[Contributing Guide](contributing.md).
