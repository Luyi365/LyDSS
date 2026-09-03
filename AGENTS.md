# LyDSS Agent Guide

## Project Shape

- LyDSS is a C project that builds one static library per container in `src/`: array, bucket, list, pool, queue, set, stack, stream, and table.
- Public declarations are centralized in [`inc/lydss_api.h`](inc/lydss_api.h); configuration and assertion behavior are in [`inc/lydss_cfg.h`](inc/lydss_cfg.h).
- Keep container implementations in their matching `src/ly<name>.c` file and tests in `test/<name>_basic.c`.
- Use existing naming conventions: `ly<container>_<operation>` functions and `Ly<Container>_t` types, except for the legacy lower-case `lytable` API.

## Build And Test

Use the repository's CMake presets from PowerShell:

```powershell
cmake --preset GCC
cmake --build out/build/GCC
ctest --test-dir out/build/GCC --output-on-failure
```

`GCC` uses the repository-configured MinGW compiler. `Ninja` and `Clang` are alternative presets when their tools are available. Read [`CMakeLists.txt`](CMakeLists.txt) and [`CMakePresets.json`](CMakePresets.json) before changing targets or test registration.

If CMake reports that the source directory does not match the cache directory, use a new build directory or clear the affected cache only after confirming it belongs to this workspace. Do not change source files to work around a stale cache.

CTest currently runs two aggregate executables: [`examples/run_simple_test.c`](examples/run_simple_test.c) and [`examples/run_all_basic_test.c`](examples/run_all_basic_test.c). Tests are print-based smoke tests rather than an assertion framework, and some test files are empty or commented out. Check the runner and return status before treating a test result as meaningful coverage.

## API And Ownership

- Match the existing create/init and delete/destroy lifecycle for each container.
- Check whether a constructor is dynamic or static before changing cleanup behavior. Static constructors may bind caller-owned storage, and current implementations are not uniformly safe to release through dynamic deletion APIs.
- Queue, stack, and set copy fixed-size elements according to their configured element size. Sets also require caller-provided hash and equality callbacks.
- List, pool, table, and bucket APIs commonly retain external `void *` pointers; do not assume ownership of caller-owned business objects. The table copies keys but does not free values.
- Do not infer a public array or vector abstraction solely from an implementation that uses `items`, `buffer`, or index arithmetic. Queue, stack, and pool use array-like storage for their own container semantics; compare their public operations before proposing a new shared container.
- Assertions are enabled by default. `LYDSS_ASSERT` may enter a debugger breakpoint/infinite loop on failure, so do not assume invalid input produces a normal error return.

## Change Guidance

- Make small, local changes that preserve public APIs unless the task explicitly requires an API change.
- Read the matching implementation, public declarations, and nearest test before editing. Add a focused regression or smoke test when behavior changes.
- When evaluating a new container or feature name, first clarify its element model, core operations, ownership, capacity rules, and deletion behavior; then compare that contract with existing containers before adding an API.
- Do not use the legacy `examples/RedPandaIDE/makefile.win` as the primary build entry point.
- Verify the narrowest relevant test or build target after edits. When a failure is caused by existing test wiring, report it separately rather than masking it with unrelated changes.

## Known Repository Limitations

- The all-basic CMake target currently has stale or incomplete test wiring; inspect its link libraries and runner calls if it fails at link time or reports little coverage.
- CMake's public include-directory setup is limited; inspect target include paths before assuming an external consumer can include headers without additional configuration.
- The README is currently only a placeholder; rely on the headers, CMake files, implementations, and tests for authoritative behavior.
