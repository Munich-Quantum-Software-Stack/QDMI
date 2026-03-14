# Contributing {#contributing}

<!-- [DOXYGEN] -->

Thank you for your interest in contributing to this project.
We value contributions from people with all levels of experience.

We use GitHub to host code, track issues and feature requests, and review pull requests. See
<https://docs.github.com/en/get-started/quickstart> for a general introduction to working with
GitHub and contributing to projects.

## Types of Contributions {#types-of-contributions}

Pick the path that fits your time and interests:

- Report bugs:

  Use the bug report issue template and include steps to reproduce, expected vs. actual behavior,
  environment details, and a minimal example.

- Fix bugs:

  Browse issues labeled `bug`, `help wanted`, or `good first issue`.
  Open a draft PR early to get feedback.

- Propose features:

  Use the feature request issue template and describe motivation, alternatives considered, and a
  rough implementation sketch.

- Implement features:

  Pick issues labeled `feature` or `enhancement`.
  Coordinate in the issue first for larger changes.

- Improve documentation:

  Refine guides, comments, and examples, and clarify explanations.
  Documentation-only PRs are welcome.

## Guidelines {#guidelines}

Please follow these guidelines to keep reviews efficient and maintain code quality.

### Core Guidelines {#core-guidelines}

- ["Commit early and push often"](https://www.worklytics.co/blog/commit-early-push-often).
- Focus on a single feature or bug at a time and only touch relevant files. Split multiple features
  into separate contributions.
- Add tests for new features to ensure they work as intended.
- Document new features.
- Add tests for bug fixes to demonstrate the fix.
- Document your code thoroughly and ensure it is readable.
- Keep your code clean by removing debug statements, leftover comments, and unrelated code.
- Check your code for style and linting errors before committing.
- Follow the project's coding standards and conventions.
- Be open to feedback and willing to make necessary changes based on code reviews.

### AI-assisted contributions {#ai-assisted-contributions}

We acknowledge the utility of AI-based coding assistants (e.g., GitHub Copilot, ChatGPT) in modern software development.
However, their use requires a high degree of responsibility and transparency to maintain code quality and licensing compliance.

**You are responsible for every line of code you submit**, and a **human must always be in the loop**.
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

## Get Started {#get-started}

Ready to contribute?
If this is your first PR, it does not need to be perfect; iterate with reviewer feedback.

### Initial Setup {#initial-setup}

1. Get the code

   **External Contribution**

   If you do not have write access to the repository, fork the repository on GitHub and clone your
   fork locally.

   ```bash
   git clone git@github.com:your_name_here/repository_name.git
   ```

   **Internal Contribution**

   If you have write access, clone the repository directly.

   ```bash
   git clone git@github.com:org-name/repository_name.git
   ```

2. Change into the project directory

   ```bash
   cd repository_name
   ```

3. Create a branch for local development

   ```bash
   git checkout -b name-of-your-bugfix-or-feature
   ```

   Now you can make your changes locally.

4. (Optional, highly recommended) Install [prek](https://prek.j178.dev/) to run checks before each
   commit.

   ```bash
   uv tool install prek
   prek install
   ```

### Working on Source Code {#working-on-source-code}

Building this project requires a C++ compiler supporting _C++20_ and CMake _3.24_ or newer.

#### Configure and Build {#configure-and-build}

This project uses CMake as the main build system.
First, configure the project:

```console
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_MY_QDMI_TESTS=ON
```

This configures a release build and enables tests.
You can also add:

- `-DENABLE_COVERAGE=ON` to enable coverage support,
- `-DBUILD_MY_QDMI_DOCS=ON` to enable documentation targets.

Then build the project:

```console
cmake --build build --config Release
```

You can append `--parallel <NUMBER_OF_THREADS>` to speed up builds.

#### Running the C++ Tests and Code Coverage {#running-the-c-tests-and-code-coverage}

After building, run the test suite with CTest:

```console
ctest -C Release --test-dir build --output-on-failure
```

To generate a coverage report, configure with `-DENABLE_COVERAGE=ON` and run your coverage tooling
in the build directory.

#### Code Formatting and Linting {#code-formatting-and-linting}

This project mostly follows the LLVM coding style and uses:

- [clang-tidy](https://clang.llvm.org/extra/clang-tidy/) for static analysis,
- [clang-format](https://clang.llvm.org/docs/ClangFormat.html) for code formatting.

Most IDEs can run these tools automatically, and CI validates formatting and linting for pull
requests.

#### C++ Documentation {#cpp-documentation}

We expect new code to include Doxygen comments where appropriate.
When touching existing areas, please improve missing or unclear documentation.

For practical guidance, see the Doxygen manual at
<https://www.doxygen.nl/manual/docblocks.html>.

### Working on the Documentation {#working-on-documentation}

Documentation is written in Markdown and built with Doxygen.
After configuring with `-DBUILD_MY_QDMI_DOCS=ON`, build the docs with:

```console
cmake --build build --target my_qdmi_device_docs
```

The generated site is available at `build/docs/html/index.html`.

<!-- [DOXYGEN] -->
