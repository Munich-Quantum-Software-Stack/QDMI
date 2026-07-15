# QDMI

## Build and Test

- Configure: `cmake --preset release`
- Build: `cmake --build --preset release`
- Test: `ctest --preset release`
- Single test binary: `./build/release/test/qdmi_test`
- For debug builds, replace `release` with `debug`.
- For coverage, use the `coverage` configure, build, and test presets.
- For all available options and presets, see `CMakeLists.txt` and
  `CMakePresets.json`.

The first configure may need network access because dependencies are managed
with CMake `FetchContent`.

## Documentation

- Sources: `docs/`
- Configure: `cmake -S . -B build/docs -G Ninja -DBUILD_QDMI_DOCS=ON`
- Build: `cmake --build build/docs --target qdmi-docs`
- Generated HTML: `build/docs/docs/html/index.html`

## Tech Stack and Layout

- Public interface: C11 headers in `include/qdmi/`
- Tests and example implementations: C++20
- Build system: CMake 3.24+ with presets and `FetchContent`
- Unit tests: GoogleTest in `test/`
- Example device, driver, tool, and FoMaC implementations: `examples/`
- Device project source template: `templates/device/`
- Documentation: Doxygen with Markdown sources in `docs/`
- Formatting and checks: `prek`, `clang-format`, `cmake-format`, `rumdl`,
  `typos`, and license checks
- Static analysis: `clang-tidy`

## Development Guidelines

### General

- MUST run `uvx prek run -a` after every batch of changes. All hooks from
  `.pre-commit-config.yaml` must pass before submitting.
- MUST add or update tests for every code change, including bug fixes.
- MUST follow existing patterns in neighboring files and keep changes focused
  on one feature or bug.
- MUST update `CHANGELOG.md` for noteworthy or user-facing changes and
  `UPGRADING.md` for breaking changes.
- MUST follow `docs/ai_usage.md`: a human must review and understand all
  AI-assisted work, and AI assistance must not be used for `good first issue`
  contributions, issue descriptions, PR comments, or code reviews.
- MUST include a commit footer attribution in the form
  `Assisted-by: [Model Name] via [Tool Name]` when AI tools helped prepare a
  commit.
- MUST preserve the Apache-2.0-with-LLVM-exception license headers used by the
  surrounding source files.
- PREFER targeted builds and tests during development, then run the full
  relevant preset before handoff.
- NEVER edit generated build output or dependency source trees under `build/`.

### C and C++

- MUST keep the public QDMI interface C11-compatible.
- MUST use C++20 for tests and example implementations.
- MUST follow the LLVM coding style and the repository's `.clang-format` and
  `.clang-tidy` configurations.
- MUST use Doxygen-style API comments. Provide `@brief`, document every
  parameter with `@param`, and document non-void return values with `@return`.
- MUST add new public headers below `include/qdmi/` and cover API changes in
  `test/`.
- PREFER existing QDMI status codes, types, naming, and ownership patterns over
  introducing parallel abstractions.

### Templates and Examples

- Treat `templates/device/` as the source for generated device projects; update
  the source template when changing generated-project behavior.
- Keep template placeholder forms (`MY`, `my_`, and `my-`) consistent with
  `cmake/GenerateTemplate.cmake`.
- When changing the template, build and test its instantiated targets as well as
  the main QDMI test suite.
- Keep example implementations representative of the public interface and
  update them when interface changes require it.

## Self-Review Checklist

- Did the relevant configure and build preset succeed?
- Did targeted tests and `ctest --preset <preset>` pass?
- Did `uvx prek run -a` pass without errors?
- Are all behavior changes covered by automated tests?
- Does the public interface remain C11-compatible?
- Were Doxygen comments and documentation updated for API changes?
- Were `CHANGELOG.md` and `UPGRADING.md` updated when appropriate?
- Were template and example consumers updated and tested when the interface or
  generation behavior changed?
