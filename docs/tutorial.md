# Tutorial: Implementing a QDMI Device

<!-- IMPORTANT: Keep the line above as the first line. -->

Welcome! This tutorial is the **definitive, self-contained guide** for implementing a QDMI device. It replaces and expands upon the basic template documentation to provide a hands-on, interactive learning experience. By the end of this guide, you will have a working device project capable of accepting queries and jobs.

Together with QDMI, we provide a template meant to kick-start the implementation of a new device. The following sections describe how to set up, configure, and verify your implementation from the ground up.

\tableofcontents

## What is QDMI? {#tutorial-concepts}

The **Quantum Device Management Interface (QDMI)** serves as the standardized hardware abstraction layer for the quantum ecosystem. Much like how OpenGL or DirectX abstracts graphics hardware for application developers, QDMI provides a unified interface that decouples high-level quantum software from the underlying physical implementation.

By adhering to this standard, you enable interoperability across the entire Munich Quantum Software Stack, allowing your hardware to be seamlessly integrated with advanced drivers and optimization tools.

### Core Architecture Concepts

To build an effective QDMI device, it is essential to understand the primary entities and the "Opaque Pointer" pattern that defines the interface:

- **Implementation**: This is the logic you will develop. It acts as the bridge between the standardized QDMI C-API and your proprietary hardware controller or high-performance simulator.
- **Driver**: A client application (e.g., MQT Core) that orchestrates quantum workloads. The driver is agnostic to your hardware's internals, communicating only through the predefined QDMI protocol.
- **Handle Hierarchy**: QDMI utilizes "handles" (opaque pointers) to manage state without exposing sensitive implementation details.
  - `Session Handle`: A persistent context representing a single connection. It manages lifecycle tasks like authentication, configuration, and property discovery.
  - `Job Handle`: An ephemeral object representing a single execution unit (e.g., a quantum circuit). It encapsulates the program, execution parameters, and status tracking.

### High-Level Architecture

```mermaid
graph LR
    subgraph "The World"
    A[Quantum Algorithm] --> B[Driver / User App]
    end

    B -->|1. Submit Job / Query| C["QDMI Interface (API)"]

    subgraph "Your Project (This Tutorial)"
    C -->|2. Invoke Stubs| D["Your Device Logic (prefix_device.cpp)"]
    D <-->|3. Control / Read| E[Hardware or Simulator]
    end

    D -->|4. Return Results| C
    C -->|5. Deliver Status/Data| B
```

## Prerequisites {#tutorial-prerequisites}

Before you start, ensure you have the following installed on your system:

- **C++ Compiler**: A compiler supporting **C++20** (e.g., GCC 10+, Clang 10+, or MSVC 19.29+).
- **C Compiler**: A compiler supporting **C11**.
- **CMake**: Version **3.24** or higher.
- **Python**: Version **3.10** or higher (required for Python bindings and some build tools).
- **uv** (Optional but recommended): For managing Python dependencies and pre-commit hooks.

## Project Dependencies {#tutorial-dependencies}

When you build the template project, CMake will automatically fetch the following dependencies:

| Dependency                                                             | Version | Purpose                              |
| :--------------------------------------------------------------------- | :------ | :----------------------------------- |
| [QDMI Core](https://github.com/Munich-Quantum-Software-Stack/QDMI)     | 1.3.0   | Core interface definitions           |
| [GoogleTest](https://github.com/google/googletest)                     | 1.17.0  | C++ testing framework                |
| [Doxygen](https://www.doxygen.nl/)                                     | 1.15.0  | Documentation generation             |
| [Doxygen Awesome](https://github.com/jothepro/doxygen-awesome-css)     | 2.4.1   | Modern CSS for Doxygen documentation |
| [scikit-build-core](https://github.com/scikit-build/scikit-build-core) | 0.11.6+ | Python build backend                 |

These dependencies are managed via `FetchContent` in `cmake/ExternalDependencies.cmake` or defined in `pyproject.toml`.

## Creating a new Project {#tutorial-create}

The code for the template is contained in the `templates/` directory of the QDMI repository. To start
a new project based on the template, configure QDMI once to define the prefix and output path, then
explicitly build the `qdmi-template` target that writes the files.

\note An internet connection is needed for this step as the QDMI repository will be fetched from
GitHub.

```sh
cmake -DQDMI_GENERATE_TEMPLATE=ON \
      -DTEMPLATE_PREFIX="PREFIX" \
      -DTEMPLATE_PATH="path/to/dir" \
      -S . -B build

# actually write the template files
cmake --build build --target qdmi-template
```

```mermaid
graph TD
    subgraph "Your Environment"
    CONF["1. Configure CMake"]
    BUILD["2. Build 'qdmi-template'"]
    DIR["3. New Project Directory"]
    end

    subgraph "QDMI Repository (Remote)"
    RE["Template Source & Logic"]
    end

    CONF -->|"-DTEMPLATE_PREFIX=MY"| CONF
    CONF -.->|FetchContent| RE
    RE -->|Pull Metadata| CONF
    CONF --> BUILD
    BUILD -->|Inject Prefix & Logic| DIR
    DIR -->|Output| SRC["my_device.cpp"]
    DIR -->|Output| TST["test_my_device.cpp"]
```

### Troubleshooting Fetch Failures

Since the template is fetched from GitHub during configuration, you might encounter issues:

- **GitHub Rate Limits**: If you see "Access Denied" or "403" errors, you may have reached the unauthenticated API rate limit.
  - _Solution_: Set the environment variable `GITHUB_TOKEN` to a personal access token, and CMake will use it for `FetchContent`.
- **SSL/Certificate Errors**: If your environment has old SSL certificates, the fetch might fail.
  - _Solution_: Ensure your system's `ca-certificates` are up to date, or as a last resort, use `-DQDMI_FETCH_VERIFY_SSL=OFF` (not recommended for production).
- **Missing Git**: CMake requires a local `git` installation to clone the repository.
  - _Solution_: Verify `git --version` works in your terminal.

If the option `TEMPLATE_PATH` is not given, it will be placed in `PREFIX_qdmi_device` relative to the
parent directory where QDMI was cloned.

If you want to regenerate into an existing directory, use:

```sh
cmake --build build --target qdmi-template-clean
```

> [!NOTE]
> The previous command `qdmi-template-force` has been updated to `qdmi-template-clean` to better reflect its function of cleaning and regenerating.

After this step, you can directly start implementing your device in C++.
Example implementations are provided in the `examples/` directory. See
[Examples](examples.md) for more information.

## Configuring the Template {#tutorial-configure}

For stability, we recommend pinning the version of QDMI that you are using for your implementation.
You can use any valid git tag, branch, or commit hash for that. To this end, adjust the `QDMI_REV`
variable in `cmake/ExternalDependencies.cmake` as follows:

```diff
-   set(QDMI_REV "develop"
+   set(QDMI_REV "v1.3.0"
```

When you want to change the prefix after the creation of the template, you need to change the prefix
in a couple of places. All paths are given relative to the root of the template project directory.

- `CMakeLists.txt`
- `src/CMakeLists.txt`: the target name `${QDMI_TARGET_NAME}` (defaults to `prefix-qdmi-device`)
- `src/prefix_device.cpp`: the source file for your device
- `test/test_prefix_device.cpp`: the source file for your tests
- `pyproject.toml`: adapt the package name and several paths
- `python/prefix`: adapt the package namespace in the directory structure
- `python/prefix/qdmi/*.py`: adapt the prefix throughout the package (e.g., `import prefix_qdmi.device`)
- `test/python/*.py`: adapt the prefix throughout the tests
- `docs/index.md` and other documentation files

## Working with the Template {#tutorial-working}

The template is structured into several directories.

```text
.
├── cmake/              # Build system logic & dependencies
├── docs/               # Documentation source
├── src/                # YOUR SOURCE CODE
│   └── prefix_device.cpp
├── test/               # YOUR TESTS
│   └── test_prefix_device.cpp
├── python/             # Python bindings
├── CMakeLists.txt      # Main build configuration
└── pyproject.toml      # Python package configuration
```

The top-level `CMakeLists.txt` contains settings for the entire project. Some additional CMake code that imports
required dependencies is outsourced into `cmake/`.

The most important directory for your implementation is `src/` and the `.cpp` file located in that
directory (e.g., `prefix_device.cpp`). Here you find stubs for all functions that have to be implemented by a device.

### Headers and Status Codes

To implementation the functions in this tutorial, ensure your `prefix_device.cpp` includes the following headers:

```cpp
#include "prefix_qdmi/device.h"
#include <string>   // For std::string
#include <cstring>  // For std::memcpy
#include <cstdint>  // For uint8_t
```

**Status Codes**: Every QDMI function returns an `int` representing a status.

- `QDMI_SUCCESS`: The operation completed successfully.
- `QDMI_ERROR_NOTIMPLEMENTED`: The default for stubs. You must replace this!
- `QDMI_ERROR_INVALIDARGUMENT`: Used if the driver passes a `NULL` pointer or an invalid size.
- `QDMI_ERROR_BADSTATE`: Used if a function is called at the wrong time (e.g., querying before initialization).

For every function, the `return QDMI_ERROR_NOTIMPLEMENTED;` should be replaced by a proper implementation of
the function. In particular, there should not be any computation path at the end that returns \ref
QDMI_ERROR_NOTIMPLEMENTED. Instead, some other error code from \ref QDMI_STATUS should be returned
in case of an erroneous state.

The implementation in the `src/` directory is complemented with a testing framework in `test/`. The
`.cpp` source file (e.g., `test_prefix_device.cpp`) already contains some examples for tests. They are meant to serve as an
inspiration, and more tests should be implemented to cover everything in your device implementation.

The `python` directory in combination with the `pyproject.toml` file contains some basic packaging setup
for distributing the device implementation as a Python package.

The `docs/` directory contains the documentation for the template. This is intended to be used as a
starting point for your own documentation.

Some more files are present in the root directory of the template, such as `LICENSE.md`, `README.md`, and
`.gitignore`.

## Session Handling {#tutorial-session}

To interact with a device, a driver must first establish a session. In QDMI, a session is an opaque handle (`QDMI_Device_Session`) that encapsulates the connection state, including authentication.

The lifecycle of a session follows these steps:

1.  **Allocation**: `QDMI_device_session_alloc` creates a new session object.
2.  **Configuration**: `QDMI_device_session_set_parameter` is used to provide credentials (like an API token).
3.  **Initialization**: `QDMI_device_session_init` validates the parameters and prepares the session for use.
4.  **Usage**: Once initialized, the session handle is passed to querying and job submission functions.
5.  **Clean-up**: `QDMI_device_session_free` releases all resources associated with the session.

```mermaid
stateDiagram-v2
    [*] --> Allocated: session_alloc()
    Allocated --> Allocated: set_parameter(TOKEN)
    Allocated --> Initialized: session_init()
    Initialized --> Initialized: Query Property / Submit Job
    Initialized --> [*]: session_free()
    Allocated --> [*]: session_free()
```

### Implementation Example

In your `prefix_device.cpp`, you should define a structure to hold the session state:

```cpp
enum class DEVICE_SESSION_STATUS : uint8_t { ALLOCATED, INITIALIZED };

struct PREFIX_QDMI_Device_Session_impl_d {
  std::string token;
  DEVICE_SESSION_STATUS status = DEVICE_SESSION_STATUS::ALLOCATED;
};
```

Then, implement the core session functions. Here is how you might handle an authentication token and track the session state:

```cpp
int PREFIX_QDMI_device_session_alloc(PREFIX_QDMI_Device_Session *session) {
  if (session == nullptr) return QDMI_ERROR_INVALIDARGUMENT;
  *session = new PREFIX_QDMI_Device_Session_impl_d();
  return QDMI_SUCCESS;
}

int PREFIX_QDMI_device_session_set_parameter(PREFIX_QDMI_Device_Session session,
                                            QDMI_Device_Session_Parameter param,
                                            const size_t size, const void *value) {
  if (session == nullptr || (value != nullptr && size == 0)) return QDMI_ERROR_INVALIDARGUMENT;
  if (session->status != DEVICE_SESSION_STATUS::ALLOCATED) return QDMI_ERROR_BADSTATE;

  if (param == QDMI_DEVICE_SESSION_PARAMETER_TOKEN && value != nullptr) {
    session->token = std::string(static_cast<const char *>(value), size);
    return QDMI_SUCCESS;
  }
  return QDMI_ERROR_NOTSUPPORTED;
}

int PREFIX_QDMI_device_session_init(PREFIX_QDMI_Device_Session session) {
  if (session == nullptr) return QDMI_ERROR_INVALIDARGUMENT;
  if (session->token.empty()) return QDMI_ERROR_PERMISSIONDENIED; // Token is required

  session->status = DEVICE_SESSION_STATUS::INITIALIZED;
  return QDMI_SUCCESS;
}

void PREFIX_QDMI_device_session_free(PREFIX_QDMI_Device_Session session) {
  delete session;
}
```

By requiring a token before initialization, you ensure that only authenticated drivers can query your device or submit jobs.

## First Query: Device Name {#tutorial-query}

One of the most basic tasks a driver performs is querying the device for its characteristics. In QDMI, this is done via "Property Queries". Let's implement a query to retrieve the device's name.

### Implementation Logic

In your `prefix_device.cpp`, look for the `PREFIX_QDMI_device_session_query_device_property` function. You'll need to handle the `QDMI_DEVICE_PROPERTY_NAME` case.

QDMI uses a **two-step retrieval pattern** for data of variable size (like strings):

1.  **Request Size**: The driver calls the function with a `NULL` value pointer. Your implementation should set `size_ret` to the required buffer size (including the null terminator).
2.  **Retrieve Data**: The driver provides a buffer of the requested size. Your implementation copies the data into the buffer.

```cpp
int PREFIX_QDMI_device_session_query_device_property(
    PREFIX_QDMI_Device_Session session, const QDMI_Device_Property prop,
    const size_t size, void *value, size_t *size_ret) {

  if (session == nullptr) return QDMI_ERROR_INVALIDARGUMENT;
  if (session->status != DEVICE_SESSION_STATUS::INITIALIZED) return QDMI_ERROR_BADSTATE;

  if (prop == QDMI_DEVICE_PROPERTY_NAME) {
    const std::string name = "MyQDMI-Tutorial-Device";
    const size_t name_size = name.size() + 1; // Include null terminator

    if (size_ret != nullptr) *size_ret = name_size;
    if (value == nullptr) return QDMI_SUCCESS; // Size request only

    if (size < name_size) return QDMI_ERROR_INVALIDARGUMENT; // Buffer too small
    std::memcpy(value, name.c_str(), name_size);
    return QDMI_SUCCESS;
  }

  return QDMI_ERROR_NOTSUPPORTED;
}
```

### Visualizing the Two-Step Query

```mermaid
sequenceDiagram
    participant D as Driver
    participant V as Device (Your Code)
    D->>V: query_property(prop, value=NULL, &size_ret)
    Note over V: Calculate required size
    V-->>D: QDMI_SUCCESS, size_ret=N
    D->>V: query_property(prop, value=Buffer[N], size=N)
    Note over V: Copy data to buffer
    V-->>D: QDMI_SUCCESS
```

## Job Handling {#tutorial-jobs}

The primary purpose of a QDMI device is to execute jobs (typically quantum programs). The lifecycle of a job involves several state transitions managed by the device.

### Job State Transitions

```mermaid
stateDiagram-v2
    [*] --> Created: session_create_device_job()
    Created --> Created: job_set_parameter(PROGRAM)
    Created --> Submitted: job_submit()
    Submitted --> Done: Simulated Execution
    Done --> [*]: job_free()
```

### Implementation Logic

First, define your job structure in `prefix_device.cpp`. It should track the associated session, the program to run, and the job's current status.

```cpp
struct PREFIX_QDMI_Device_Job_impl_d {
  PREFIX_QDMI_Device_Session session = nullptr;
  std::string program;
  QDMI_Job_Status status = QDMI_JOB_STATUS_CREATED;
};
```

Then, implement the core job management functions. For this tutorial, we will simulate immediate execution.

```cpp
int PREFIX_QDMI_device_session_create_device_job(PREFIX_QDMI_Device_Session session,
                                                PREFIX_QDMI_Device_Job *job) {
  if (session == nullptr || job == nullptr) return QDMI_ERROR_INVALIDARGUMENT;
  if (session->status != DEVICE_SESSION_STATUS::INITIALIZED) return QDMI_ERROR_BADSTATE;

  *job = new PREFIX_QDMI_Device_Job_impl_d();
  (*job)->session = session;
  return QDMI_SUCCESS;
}

int PREFIX_QDMI_device_job_set_parameter(PREFIX_QDMI_Device_Job job,
                                        QDMI_Device_Job_Parameter param,
                                        size_t size, const void *value) {
  if (job == nullptr || (value != nullptr && size == 0)) return QDMI_ERROR_INVALIDARGUMENT;

  if (param == QDMI_DEVICE_JOB_PARAMETER_PROGRAM && value != nullptr) {
    job->program = std::string(static_cast<const char *>(value), size);
    return QDMI_SUCCESS;
  }
  return QDMI_ERROR_NOTSUPPORTED;
}

int PREFIX_QDMI_device_job_submit(PREFIX_QDMI_Device_Job job) {
  if (job == nullptr) return QDMI_ERROR_INVALIDARGUMENT;
  if (job->program.empty()) return QDMI_ERROR_BADSTATE; // Must have a program

  // Simulate execution: transition directly to DONE
  job->status = QDMI_JOB_STATUS_DONE;
  return QDMI_SUCCESS;
}

int PREFIX_QDMI_device_job_check(PREFIX_QDMI_Device_Job job, QDMI_Job_Status *status) {
  if (job == nullptr || status == nullptr) return QDMI_ERROR_INVALIDARGUMENT;
  *status = job->status;
  return QDMI_SUCCESS;
}

int PREFIX_QDMI_device_job_get_results(PREFIX_QDMI_Device_Job job, QDMI_Job_Result result,
                                      size_t size, void *data, size_t *size_ret) {
  if (job == nullptr) return QDMI_ERROR_INVALIDARGUMENT;
  if (job->status != QDMI_JOB_STATUS_DONE) return QDMI_ERROR_INVALIDARGUMENT;

  if (result == QDMI_JOB_RESULT_PROBABILITIES_DENSE) {
    // Simulate probability results (e.g., 50/50 for a simple circuit)
    const double probabilities[] = {0.5, 0.5};
    const size_t res_size = sizeof(probabilities);

    if (size_ret != nullptr) *size_ret = res_size;
    if (data == nullptr) return QDMI_SUCCESS;

    if (size < res_size) return QDMI_ERROR_INVALIDARGUMENT;
    std::memcpy(data, probabilities, res_size);
    return QDMI_SUCCESS;
  }
  return QDMI_ERROR_NOTSUPPORTED;
}

void PREFIX_QDMI_device_job_free(PREFIX_QDMI_Device_Job job) {
  delete job;
}
```

### Data Flow: Retrieving Results

The following diagram illustrates how the probability data travels from your device logic back to the driver.

```mermaid
graph TD
    subgraph "Driver"
    DR["Request Result (prop=PROBABILITIES_DENSE)"]
    BU["Buffer [0.5, 0.5]"]
    end
    subgraph "Your Device (prefix_device.cpp)"
    IM["PREFIX_QDMI_device_job_get_results"]
    LO{"Check Status"}
    DA["Simulated Probabilities: {0.5, 0.5}"]
    end
    DR -->|1. Call with data=NULL| IM
    IM -->|2. Return size_ret=16 bytes| DR
    DR -->|3. Call with data=BU| IM
    IM --> LO
    LO -->|Done| DA
    DA -->|4. memcpy to BU| BU
```

## Test Environment {#tutorial-testing}

A key part of implementing your QDMI device is verifying each step as you go. The template comes with a built-in test suite tailored to give you immediate feedback, similar to an interactive coding platform.

```mermaid
graph LR
    A["Implement Step"] --> B["Run 'ctest'"]
    B --> C{Check Result}
    C -->|Fail| D["Read Feedback (e.g. 'Checkpoint X Failed')"]
    D --> A
    C -->|Pass| E["Move to Next Step"]
```

### Running Verification Tests

The template comes with a basic test suite in `test/test_prefix_device.cpp`. To get the **Checkpoint feedback** described below, update your test suite with the following code. This provides descriptive messages when a specific implementation step is missing or incorrect.

#### Verification Test Suite (`test/test_prefix_device.cpp`)

```cpp
#include "prefix_qdmi/device.h"
#include <gtest/gtest.h>

class QDMIImplementationTest : public ::testing::Test {
protected:
  PREFIX_QDMI_Device_Session session = nullptr;

  void SetUp() override {
    ASSERT_EQ(PREFIX_QDMI_device_initialize(), QDMI_SUCCESS)
        << "Checkpoint 0 Failed: Basic device initialization returned an error.";

    ASSERT_EQ(PREFIX_QDMI_device_session_alloc(&session), QDMI_SUCCESS)
        << "Checkpoint 1 Failed: Could not allocate a session handle.";

    // Provide a dummy token for initialization, reflecting the Session Handling step.
    const std::string dummy_token = "tutorial_token";
    ASSERT_EQ(PREFIX_QDMI_device_session_set_parameter(
                  session, QDMI_DEVICE_SESSION_PARAMETER_TOKEN,
                  dummy_token.size(), dummy_token.c_str()),
              QDMI_SUCCESS)
        << "Checkpoint 2 Failed: Could not set the authentication token. "
           "Did you implement QDMI_DEVICE_SESSION_PARAMETER_TOKEN?";

    ASSERT_EQ(PREFIX_QDMI_device_session_init(session), QDMI_SUCCESS)
        << "Checkpoint 2 Failed: Session initialization failed. "
           "Common causes: Token validation logic error or device is offline.";
  }

  void TearDown() override { PREFIX_QDMI_device_finalize(); }
};

TEST_F(QDMIImplementationTest, QueryDeviceName) {
  size_t size = 0;
  ASSERT_EQ(PREFIX_QDMI_device_session_query_device_property(
                session, QDMI_DEVICE_PROPERTY_NAME, 0, nullptr, &size),
            QDMI_SUCCESS)
      << "Checkpoint 3 Failed: Your device must report the size of its name.";

  std::string value(size - 1, '\0');
  ASSERT_EQ(PREFIX_QDMI_device_session_query_device_property(
                session, QDMI_DEVICE_PROPERTY_NAME, size, value.data(), nullptr),
            QDMI_SUCCESS)
      << "Checkpoint 3 Failed: Your device failed to return its name into the buffer.";

  ASSERT_EQ(value, "MyQDMI-Tutorial-Device")
      << "Checkpoint 3 Failed: The returned device name does not match the expected tutorial value.";
}

TEST_F(QDMIImplementationTest, SubmitAndVerifyJob) {
  PREFIX_QDMI_Device_Job job = nullptr;
  ASSERT_EQ(PREFIX_QDMI_device_session_create_device_job(session, &job), QDMI_SUCCESS)
      << "Checkpoint 4 Failed: Could not create a device job.";

  const std::string qasm = "OPENQASM 2.0; qreg q[1]; creg c[1]; h q[0]; measure q[0] -> c[0];";
  ASSERT_EQ(PREFIX_QDMI_device_job_set_parameter(
                job, QDMI_DEVICE_JOB_PARAMETER_PROGRAM, qasm.size(), qasm.c_str()),
            QDMI_SUCCESS)
      << "Checkpoint 4 Failed: Could not set the QASM program for the job.";

  ASSERT_EQ(PREFIX_QDMI_device_job_submit(job), QDMI_SUCCESS)
      << "Checkpoint 4 Failed: Job submission failed.";

  QDMI_Job_Status status = QDMI_JOB_STATUS_CREATED;
  ASSERT_EQ(PREFIX_QDMI_device_job_check(job, &status), QDMI_SUCCESS);
  ASSERT_EQ(status, QDMI_JOB_STATUS_DONE)
      << "Checkpoint 4 Failed: Job status should be DONE after submission in this tutorial.";

  double probs[2];
  size_t size = 0;
  ASSERT_EQ(PREFIX_QDMI_device_job_get_results(
                job, QDMI_JOB_RESULT_PROBABILITIES_DENSE, sizeof(probs), probs, &size),
            QDMI_SUCCESS)
      << "Checkpoint 4 Failed: Could not retrieve simulated job results.";

  EXPECT_EQ(probs[0], 0.5);
  EXPECT_EQ(probs[1], 0.5);

  PREFIX_QDMI_device_job_free(job);
}
```

#### Run All Tests

To check your overall progress:

```sh
ctest --test-dir build --output-on-failure
```

#### Verification Checkpoints

If a test fails, it will provide a descriptive "Checkpoint" message to help you identify what's missing:

| Checkpoint | Target Feature | What it verifies                                                 |
| :--------- | :------------- | :--------------------------------------------------------------- |
| **0**      | Device Init    | Basic `QDMI_device_initialize` implementation.                   |
| **1**      | Session Alloc  | Successful `QDMI_device_session_alloc` and memory management.    |
| **2**      | Authentication | Implementation of token handling in `set_parameter` and `init`.  |
| **3**      | First Query    | Retrieval of the device name using the two-step pattern.         |
| **4**      | Job Handling   | Complete job lifecycle: Create, Set Program, Submit, Get Result. |

### Understanding the Feedback

When a test fails, pay close attention to the custom failure messages. They are designed to point you toward the specific function or logic error in your code. For example:

> `Checkpoint 2 Failed: Could not set the authentication token.`
> `Did you implement QDMI_DEVICE_SESSION_PARAMETER_TOKEN?`

As you implement each function described in this tutorial, keep the test suite running to ensure you stay on the right track!

## Building the Template and Running the Tests {#tutorial-building}

All following commands are meant to be executed from the root directory of the template. After
configuring your project (see [Configuring the Template](#tutorial-configure)), you can build your
project with the following command:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

If you only want to build a specific target, you can append, for example,
`--target prefix-qdmi-device-test` to the command above, which will build the tests. If you only want to
build the device implementation, you can use `--target prefix-qdmi-device`.

To run the tests, perform the following command:

```sh
ctest --test-dir build
```

For more details on the development process, also check out the [Development Guide](guide.md).
