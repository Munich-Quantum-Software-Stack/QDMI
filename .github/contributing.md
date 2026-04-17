# Contributing {#contributing}

<!-- [DOXYGEN] -->

Thank you for your interest in contributing to QDMI.
We value contributions from people with all levels of experience.

We use GitHub to [host code](https://github.com/Munich-Quantum-Software-Stack/QDMI), to
[track issues and feature requests][issues],
as well as accept [pull requests](https://github.com/Munich-Quantum-Software-Stack/QDMI/pulls). See
<https://docs.github.com/en/get-started/quickstart> for a general introduction to working with
GitHub and contributing to projects.

## Types of Contributions {#types-of-contributions}

Pick the path that fits your time and interests:

- 🐛 Report bugs:

  Use the _🐛 Bug report_ template at <https://github.com/Munich-Quantum-Software-Stack/QDMI/issues>.
  Include steps to reproduce, expected vs. actual behavior, environment, and a minimal example.

- 🛠️ Fix bugs:

  Browse [issues][issues], especially those labeled "bug", "help wanted", or "good first issue".
  Open a draft PR early to get feedback.

- 💡 Propose features:

  Use the _✨ Feature request_ template at <https://github.com/Munich-Quantum-Software-Stack/QDMI/issues>.
  Describe the motivation, alternatives considered, and (optionally) a small API sketch.

- ✨ Implement features:

  Pick items labeled "feature" or "enhancement".
  Coordinate in the issue first if the change is substantial; start with a draft PR.

- 📝 Improve documentation:

  Add or refine docstrings, tutorials, and examples; fix typos; clarify explanations.
  Small documentation-only PRs are very welcome.

- ⚡️ Performance and reliability:

  Profile hot paths, add benchmarks, reduce allocations, deflake tests, and improve error messages.

- 📦 Packaging and tooling:

  Improve build configuration, type hints/stubs, CI workflows, and platform wheels.
  Incremental tooling fixes have a big impact.

- 🙌 Community support:

  Triage issues, reproduce reports, and answer questions in Discussions:
  <https://github.com/Munich-Quantum-Software-Stack/QDMI/discussions>.

## Guidelines {#guidelines}

Please adhere to the following guidelines to help the project grow sustainably.
Contributions that do not comply with these guidelines or violate our [AI Usage Guidelines][ai_usage] may be rejected without further review.

### Core Guidelines {#core-guidelines}

- ["Commit early and push often"](https://www.worklytics.co/blog/commit-early-push-often).
- Write meaningful commit messages, preferably using [gitmoji](https://gitmoji.dev) for additional
  context.
- Focus on a single feature or bug at a time and only touch relevant files. Split multiple features
  into separate contributions.
- Add tests for new features to ensure they work as intended.
- Document new features.
  For user-facing changes, add a changelog entry; for breaking changes, update the upgrade guide.
  For details, see [the section on maintaining the changelog and upgrade guide](#maintaining-changelog-upgrade-guide).
- Add tests for bug fixes to demonstrate the fix.
- Document your code thoroughly and ensure it is readable.
- Keep your code clean by removing debug statements, leftover comments, and unrelated code.
- Check your code for style and linting errors before committing.
- Follow the project's coding standards and conventions.
- Be open to feedback and willing to make necessary changes based on code reviews.

### AI-assisted contributions {#ai-assisted-contributions}

We acknowledge the utility of AI-based coding assistants (e.g., GitHub Copilot, ChatGPT) in modern software development.
However, their use requires a high degree of responsibility and transparency to maintain code quality and licensing compliance.

Please carefully read and follow our dedicated [AI Usage Guidelines][ai_usage] before submitting any AI-assisted contribution.
In short: **You are responsible for every line of code you submit**, and a **human must always be in the loop**.
We require disclosure of AI tool usage in your PR description.

### Pull Request Workflow {#pull-request-workflow}

- Create PRs early.
  Work-in-progress PRs are welcome; mark them as drafts on GitHub.
- Use a clear title, reference related issues by number, and describe the changes.
  Follow the PR template; only omit the issue reference if not applicable.
- CI runs on all supported platforms and Python versions to build, test, format, and lint.
  All checks must pass before merging.
- When ready, convert the draft to a regular PR and request a review from a maintainer.
  If unsure, ask in PR comments.
  If you are a first-time contributor, mention a maintainer in a comment to request a review.
- If your PR gets a "Changes requested" review, address the feedback and push updates to the same branch.
  Do not close and reopen a new PR.
  Respond to comments to signal that you have addressed the feedback.
  Do not resolve review comments yourself; the reviewer will do so once satisfied.
- If the reviewer suggested changes with explicit code suggestions as part of the comments, apply these directly using the GitHub UI.
  This attributes the changes to the reviewer and automatically resolves the respective comments (this is an exception to the rule above).
  If there are multiple suggestions that you want to apply at once, you can batch them into a single commit.
  Go to the "Files changed" tab of the PR, and then click "Add suggestion to batch" for each suggestion you want to include.
  Once you are done selecting suggestions, click "Commit suggestions".
  Only apply suggestions manually if using the GitHub UI is not feasible.
- Re-request a review after pushing changes that address feedback.
- Do not squash commits locally; maintainers typically squash on merge.
  Avoid rebasing or force-pushing before reviews; you may rebase after addressing feedback if desired.

## Get Started 🎉 {#get-started}

Ready to contribute?
We value contributions from people with all levels of experience.
In particular, if this is your first PR, not everything has to be perfect.
We will guide you through the process.

### Initial Setup {#initial-setup}

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

5. (Optional, **highly recommended**) Install [prek](https://prek.j178.dev/) to automatically
   run a set of checks before each commit.

<!-- prettier-ignore-start -->
   <div class="tabbed">

   - <b class="tab-title">via `uv`</b> The easiest way to install prek is via [uv](https://docs.astral.sh/uv/).

     \code{.shell} uv tool install prek \endcode

   - <b class="tab-title">via `brew`</b> If you use macOS, then prek is in Homebrew, use

     \code{.shell} brew install prek \endcode

   - <b class="tab-title">via the official installers</b> See <https://prek.j178.dev/installation/> for more details.

   </div>
<!-- prettier-ignore-end -->
   Afterward, you can install the prek hooks with

   \code{.shell} prek install \endcode

### Working on Source Code {#working-on-source-code}

Building the project requires a C compiler supporting _C11_ and a minimum CMake version of _3.24_.
The example devices and the tests require a C++ compiler supporting _C++20_.

#### Configure and Build {#configure-and-build}

\note
We recommend using an IDE like [CLion][clion] or [Visual Studio Code][vscode] for development.
Both IDEs have excellent support for CMake projects and provide a convenient way to run CMake and build the project.
If you prefer to work on the command line, the following instructions will guide you through the process.

Our projects use CMake as the main build configuration tool.
Building a project using CMake is a two-stage process.
First, CMake needs to be _configured_ by calling:

```console
$ cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
```

This tells CMake to

- search the current directory `.` (passed via `-S`) for a `CMakeLists.txt` file,
- process it into a directory `build` (passed via `-B`), and
- configure a `Release` build (passed via `-DCMAKE_BUILD_TYPE`) as opposed to, e.g., a `Debug` build.

After configuring CMake, the project can be _built_ by calling:

```console
$ cmake --build build --config Release
```

This builds the project in the `build` directory (passed via `--build`).
Some operating systems and development environments explicitly require a configuration to be set, which is why the `--config` flag is also passed to the build command.
The flag `--parallel <NUMBER_OF_THREADS>` may be added to trigger a parallel build.

Building the project this way generates

- the main project libraries in the `build/src` directory and
- some test executables in the `build/test` directory.

\note \parblock
This project uses CMake's [`FetchContent`](https://cmake.org/cmake/help/latest/module/FetchContent.html) module to download and build its dependencies.
Because of this, the first time you configure the project, you will need an active internet connection to fetch the required libraries.

However, there are several ways to bypass these downloads:

- **Use system-installed dependencies**:
  If the dependencies are already installed on your system and Find-modules exist for them, `FetchContent` will use those versions instead of downloading them.
- **Provide a local copy**:
  If you have local copies of the dependencies (from a previous build or another project), you can point `FetchContent` to them by passing the [`-DFETCHCONTENT_SOURCE_DIR_<uppercaseName>`](https://cmake.org/cmake/help/latest/module/FetchContent.html#variable:FETCHCONTENT_SOURCE_DIR_%3CuppercaseName%3E) flag to your CMake configure step.
  The `<uppercaseName>` should be replaced with the name of the dependency as specified in the project's CMake files.
- **Use project-specific options**:
  Some projects provide specific CMake options to use a system-wide dependency instead of downloading it.
  Check the project's documentation or CMake files for these types of flags.

\endparblock

#### Running the C++ Tests and Code Coverage {#running-the-c-tests-and-code-coverage}

We use the [GoogleTest](https://google.github.io/googletest/primer.html) framework for unit testing of the C++ library.
All tests are contained in the `test` directory, which is further divided into subdirectories for different parts of the library.
You are expected to write tests for any new features you implement and ensure that all tests pass.
Our CI pipeline on GitHub will also run the tests and check for any failures.
It will also collect code coverage information and upload it to [Codecov](https://codecov.io/gh/munich-quantum-toolkit/core).
Our goal is to have new contributions at least maintain the current code coverage level, while striving for covering as much of the code as possible.
Try to write meaningful tests that actually test the correctness of the code and not just exercise the code paths.

Most IDEs like [CLion][clion] or [Visual Studio Code][vscode] provide a convenient way to run the tests directly from the IDE.
If you prefer to run the tests from the command line, you can use CMake's test runner [CTest](https://cmake.org/cmake/help/latest/manual/ctest.1.html).
To run the tests, run the following command from the main project directory after building the project as described above:

```console
$ ctest -C Release --test-dir build
```

\note
If you want to disable configuring and building the C++ tests, you can pass `-DBUILD_QDMI_TESTS=OFF` to the CMake configure step.

#### Code Formatting and Linting {#code-formatting-and-linting}

This project mostly follows the [LLVM Coding Standard](https://llvm.org/docs/CodingStandards.html),
a set of guidelines for writing C/C++ code. To ensure the quality of the code and that it conforms
to these guidelines, we use

- [clang-tidy](https://clang.llvm.org/extra/clang-tidy/) -- a static analysis tool that checks for
  common mistakes in C/C++ code, and
- [clang-format](https://clang.llvm.org/docs/ClangFormat.html) -- a tool that automatically formats
  C/C++ code according to a given style guide.

Common IDEs like [CLion][clion] or [Visual Studio Code][vscode] have plugins that can automatically run `clang-tidy` on the code and automatically format it with `clang-format`.

- If you are using CLion, you can configure the project to use the `.clang-tidy` and `.clang-format` files in the project root directory.
- If you are using Visual Studio Code, you can install the [clangd extension](https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd).

They will automatically execute `clang-tidy` on your code and highlight any issues.
In many cases, they also provide quick-fixes for these issues.
Furthermore, they provide a command to automatically format your code according to the given style.

\note After configuring CMake, you can run clang-tidy on a file by calling
`clang-tidy <FILE> -- -I <PATH_TO_INCLUDE_DIRECTORY>` where `<FILE>` is the file you want to analyze
and `<PATH_TO_INCLUDE_DIRECTORY>` is the path to the `include` directory of the project.

Our pre-commit hook configuration also includes `clang-format`.
If you have installed `prek`, it will automatically run `clang-format` on your code before each commit.
If you do not have `prek` set up, the [pre-commit.ci](https://pre-commit.ci) bot will run `clang-format` on your code and automatically format it according to the style guide.

\note Remember to pull the changes back into your local repository after the bot has formatted your code to avoid merge conflicts.

Our CI pipeline will also run `clang-tidy` over the changes in your PR and report any issues it finds.
Due to technical limitations, the workflow can only post PR comments if the changes are not coming from a fork.
If you are working on a fork, you can still see the `clang-tidy` results either in the GitHub Actions logs, on the workflow summary page, or in the "Files changed" tab of the PR.

#### Format for Comments {#format-for-comments}

For the information to be displayed correctly in the documentation, the comments need to follow the
format required by Doxygen. Below you find some tags that are commonly used within the documentation
of a function:

- `@brief` For a brief, one-line description of the function. Should always be provided.
- `@details` For a longer, detailed description of the function.
- `@param` To explain the usage of a parameter. Should be provided for each parameter.
- `@return` To explain the return value. Should be provided if the function returns a value.
- `@ref` To cross-reference another part of the documentation.

\note In the current setting, the long description is always prepended with the brief description.
So there is no need to repeat the brief description in the details.

### Working on the Documentation {#working-on-the-documentation}

The documentation is generated using [Doxygen](https://www.doxygen.nl/index.html) and its
configuration is seamlessly integrated into the CMake build system.

#### Building the Documentation {#building-the-documentation}

The documentation can be built with the CMake target `qdmi-docs` via

```shell
cmake -S . -B build
cmake --build build --target qdmi-docs
```

The generated web page can be inspected by opening the file in `docs/html/index.html` in the CMake
build directory.

#### Static Content {#static-content}

The generated web page also contains a couple of static sites, including the main page, the support
page, the FAQ page, and this development guide. The respective Markdown files that serve as the
source for those sites are contained in `docs/` where `index.md` contains the content of the main
page.

#### Dynamic Content {#dynamic-content}

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

#### Further Links and Resources {#further-links-and-resources}

- For more details, see the official documentation of Doxygen that can be found here:
  [https://www.doxygen.nl/manual/docblocks.html](https://www.doxygen.nl/manual/docblocks.html).
- More tags and commands can be found in the list provided here:
  [https://www.doxygen.nl/manual/commands.html#cmd_intro](https://www.doxygen.nl/manual/commands.html#cmd_intro)

### Maintaining the Changelog and Upgrade Guide {#maintaining-changelog-upgrade-guide}

QDMI adheres to [Semantic Versioning], with the exception that minor releases may include breaking changes.
To inform users about changes to the project, we maintain a {doc}`changelog <CHANGELOG>` and an {doc}`upgrade guide <UPGRADING>`.

If your PR includes noteworthy changes, please update the changelog.
The format is based on a mixture of [Keep a Changelog] and [Common Changelog].
There are the following categories:

- `Added` for new features.
- `Changed` for changes in existing functionality.
- `Deprecated` for soon-to-be removed features.
- `Removed` for now removed features.
- `Fixed` for any bug fixes.
- `Security` in case of vulnerabilities.

When updating the changelog, follow these guidelines:

- Add a changelog entry for every user-facing change in your PR.
- Write entries in the imperative mood (e.g., "Add support for X" or "Fix bug in Y").
- A single PR may result in multiple changelog entries.
- Entries in each category are sorted by merge time, with the latest PR appearing first.
- Each entry links to the PR and all contributing authors.
  The links are defined at the bottom of the file.
  If this is your first contribution to this project, do not forget to add a link to your GitHub profile.

If your PR introduces major or breaking changes, or if you think additional context would help users, please also add a section to the upgrade guide.
The upgrade guide is intended to provide a general overview of significant changes in a more descriptive and prose-oriented form than the changelog.
Use it to explain how users may need to adapt their usage of QDMI, highlight new workflows, or clarify the impact of important updates.
Feel free to write in a style that is helpful and accessible for users seeking to understand the broader implications of recent changes.

---

This document was inspired by and partially adapted from

- <https://github.com/munich-quantum-toolkit/core/blob/main/.github/contributing.md>
- <https://matplotlib.org/stable/devel/coding_guide.html>
- <https://opensource.creativecommons.org/contributing-code/pr-guidelines/>
- <https://yeoman.io/contributing/pull-request.html>
- <https://github.com/scikit-build/scikit-build>

[clion]: https://www.jetbrains.com/clion/
[vscode]: https://code.visualstudio.com/
[issues]: https://github.com/Munich-Quantum-Software-Stack/QDMI/issues
[ai_usage]: ai_usage.md
[Keep a Changelog]: https://keepachangelog.com/en/1.1.0/
[Common Changelog]: https://common-changelog.org
[Semantic Versioning]: https://semver.org/spec/v2.0.0.html

<!-- [DOXYGEN] -->
