# LyDSS Agent Guide

## Project Shape

- LyDSS stands for **DataStore Struct**. The spelling is `LyDSS` everywhere; `LyDDS` is a historical typo that was fully removed. Do not reintroduce it.
- LyDSS is a C project that builds one static library per container in `src/`: array, bucket, list, pool, queue, set, stack, stream, table, and vector. All ten libraries are independent — no container links against another.
- Public declarations are centralized in [`inc/lydss_api.h`](inc/lydss_api.h); configuration and assertion behavior are in [`inc/lydss_cfg.h`](inc/lydss_cfg.h).
- Keep container implementations in their matching `src/ly<name>.c` file and tests in `test/<name>_basic.c`.
- Use existing naming conventions: `ly<container>_<operation>` functions and `Ly<Container>_t` types, except for the legacy lower-case `lytable` API.
- Header guards follow the project style rule `文件名_H`: `LYDSS_API_H`, `LYDSS_CFG_H`. Double-underscore-wrapped guards are reserved identifiers and must not be used.
- Assertion and config macros are prefixed `LYDSS_`: `LYDSS_ASSERT`, `LYDSS_PRINTF`, `LYDSS_ASSERT_ENABLED`.
- Sources include the public header by relative path (`#include "../inc/lydss_api.h"`), so no include directories are configured in CMake.

## Build And Test

**`CMakePresets.json` does not exist in this repository and is listed in `.gitignore`.** Any command of the form `cmake --preset GCC` will fail. Use an explicit generator and compiler instead:

```powershell
cmake -S . -B out/build/verify -G Ninja -DCMAKE_C_COMPILER=gcc -DCMAKE_C_FLAGS="-Wall -Wextra"
cmake --build out/build/verify
ctest --test-dir out/build/verify --output-on-failure
```

`out/` is gitignored, so it is a safe scratch location. Clean it up when finished.

Toolchain notes for this machine:

- `gcc` 13.2 resolves to `D:\Program Files\mingw-w64-gcc-13.2-stable-r40\bin`. It targets **i686-w64-mingw32 (32-bit)**, so `unsigned long` and `size_t` are both 32-bit. Overflow guards written against `ULONG_MAX` are therefore tighter than on a 64-bit target — keep them.
- Measured handle sizes on that target (`void *` / `size_t` / `unsigned long` all 4 bytes): `LyArray` 16, `LyQueue` 28, `LyStack` 20, `LyStream` 24, `LyPool` 12, `LyPoolItem` 8. These matter if the static-construction problem below is ever solved by publishing a size, and they are target-specific — re-measure rather than hardcoding them for another ABI.
- `clang` and `ninja` are also on PATH. The path `C:\Program Files\mingw64\bin\gcc.exe` referenced by [`.vscode/tasks.json`](.vscode/tasks.json) does **not** exist; that task is stale.
- Linking occasionally fails once on Windows with no compiler diagnostic (transient file lock, typically on `libLyQueue.a`). Re-run the build before investigating — a second invocation normally succeeds.

CTest runs two aggregate executables, `LyDSS_SimpleTest` and `LyDSS_AllBasicTest`, built from [`examples/run_simple_test.c`](examples/run_simple_test.c) and [`examples/run_all_basic_test.c`](examples/run_all_basic_test.c). Both runners now accumulate results with `status |=` and return that status, so a non-zero return from any test function does fail the CTest run. Tests are still print-based smoke tests, not assertions — passing means "did not crash and returned 0", not "output was verified".

The all-basic target compiles **every** file in `test/` via `file(GLOB_RECURSE SOURCES_TEST test/*.c)`, but only the functions explicitly called by the runner actually execute. See Testing Status below.

## Working Agreement

- **This file is the single source of truth for project rules.** There is no spec directory. `.kiro/` was removed deliberately: for a single-developer, exploratory project the owner chose one durable document over per-feature requirements/design/tasks artifacts. Do not recreate `.kiro/specs/` or reference it.
- Development is conversational and incremental. When a rule, constraint, or decision emerges during a session, **append it to this file in the same session** — do not defer it, and do not park it in a throwaway planning document.
- Record decisions with their reasoning, not just the outcome. The Architecture Decisions section exists because these questions were already settled once; without the reasoning they get reopened.
- `out/` and `build/` belong to the build toolchain. They are output directories only — never write source files, probe files, or scratch code into them. Probe or scratch sources that must persist belong in the repository proper (`cmake/` for build-time checks, `test/` for run-time cases).
- `.cache/clangd/` is the clangd language-server symbol index. It is generated automatically and safe to delete; it is not part of the project.

## Container Contracts

| Container | Element model | Ownership | Capacity | Static ctor |
|---|---|---|---|---|
| array | fixed-size value copy, all `length` slots exist from creation | copies values | fixed at creation | yes |
| vector | fixed-size value copy, `[0, size)` valid | copies values | grows 2x, `realloc` | no (growth needs realloc) |
| queue | fixed-size value copy, ring FIFO | copies values | fixed | yes |
| stack | fixed-size value copy, LIFO | copies values | fixed | yes |
| stream | `cell_t` (unsigned char) bytes, ring FIFO | copies bytes | fixed | yes |
| set | fixed-size value copy, chained hash | copies values, caller supplies `Hash_t`/`Equals_t` | bucket count fixed, no rehash | no |
| table | copies key string, stores value pointer | does **not** free values | grows at load factor 0.75 | no |
| pool | stores `name` pointer and object pointer, linear scan | owns neither | fixed, silently drops when full (`TODO` in source) | yes (declaration commented out in header) |
| bucket | stores `name` and item pointers plus a copied `LyBucketMeta_t` | owns meta copies only | grows by node allocation, `count` is `uint8_t` so capped at 255 | no |
| list | stores `void *data` per node | owns nodes, not data | unbounded | no |

Boundary conventions differ and matter:

- **array** checks indices against `length`. Every slot is readable and writable from creation; there is no notion of "used count". `lyarray_create` zeroes storage via `calloc`; `lyarray_create_static` deliberately leaves caller memory untouched.
- **vector** checks indices against `size`. Free capacity beyond `size` is not addressable through the public API and is not initialized.
- **stream** `in`/`out` return the number of cells actually transferred and may transfer less than requested. **queue** `send`/`receive` are all-or-nothing and return `bool`. This difference is intentional; see Architecture Decisions.

## Architecture Decisions

These were settled deliberately. Do not re-litigate them without new information.

**`lyarray` is a fixed-length storage primitive, not a sequence container.** Its purpose is to give C's native array the runtime metadata it lacks: length and element size, carried by the handle. It deliberately does **not** track a used count, and it has no `push`/`pop`/`insert`/`remove`. Those are sequence semantics that belong to the layer above. Its eight functions are `create`, `create_static`, `set`, `get`, `read`, `delete`, `length`, `item_size`.

**`lyarray` is intended as the eventual storage base for queue and stack.** Field-level evidence supports this: `LyStack` is `LyArray` plus `top`, and `LyQueue` is `LyArray` plus two cursors and a count. **The substitution has not been performed** and was explicitly deferred by the owner. Do not rewire queue or stack onto array unless asked.

**`lyvector` is deliberately standalone and must not depend on `lyarray`.** An earlier version composed vector over array; the owner required it to be independent. It therefore owns its own `buffer`/`capacity`/`item_size`/`size` and grows with `realloc`. Keep it that way. The tradeoff accepted here is duplicated growth logic in exchange for `realloc`'s in-place expansion.

**`lystream` and `lyqueue` must stay separate.** Their structs and cursor arithmetic look nearly identical because both are implemented over a ring buffer, which is the normal shared substrate industry-wide (compare Linux `kfifo`). But they sit at different levels of abstraction: a queue is a container that holds typed fixed-size elements, a stream is a transport for untyped bytes. The partial-transfer contract (`in`/`out` returning actual counts) is the stream-side signature and is correct as written. A previous suggestion to add `send_multi`/`receive_multi` to queue and retire stream was **withdrawn as wrong**. If stream is developed further, the direction is upward — source/sink callbacks so it can wrap a UART or file, plus EOF and error state — not more buffering features.

**A `lygroup` container (append-only batch abstraction) was built and then removed** at the owner's request. Its struct was field-for-field identical to `LyVector` and its `_reserve_for`/`_item_address` were literal duplicates. If a batch abstraction is wanted again, note that its only irreplaceable value over vector was type-level restriction (no `set`/`remove`, `const` returns, a callback that cannot tell members apart), not storage behavior.

## API And Ownership

- Match the existing create/init and delete/destroy lifecycle for each container.
- **`*_create_static` is currently uncallable from outside the library.** `LyArray_t`, `LyQueue_t`, `LyStack_t`, `LyStream_t`, and `LyPool_t` are incomplete types in the public header, so an external caller cannot write `LyStack_t s;` — it fails with `storage size isn't known`. Only `LyList_t` and `lytable` have their structs exposed. Resolving this needs a design decision (expose the structs, add a `*_sizeof()` accessor, or introduce opaque storage types) and affects every container, so raise it rather than fixing one container in isolation.
- Containers with a static constructor carry an `is_static` flag, and their `delete` only unbinds caller-provided storage instead of freeing it. This applies to array, queue, stack, stream, and pool. Preserve this when touching cleanup paths.
- `lylist_destroy` frees nodes but **not** the `list` handle itself, even though `lylist_create` mallocs it. This is intentional and documented: `struct LyList` is public, so a caller may stack-allocate it and freeing would crash. Heap-allocated lists therefore leak the handle by design — the caller frees it.
- List, pool, table, and bucket APIs retain external `void *` pointers; do not assume ownership of caller-owned business objects. The table copies keys but does not free values.
- Sets require caller-provided hash and equality callbacks. The callback types `Hash_t` and `Equals_t` are declared without a container prefix and sit in the global namespace.
- Assertions are enabled by default. `LYDSS_ASSERT` may enter a debugger breakpoint/infinite loop on failure, so do not assume invalid input produces a normal error return. Public functions generally assert on null handles and validate indices with a normal `false`/`NULL` return.
- Growth-capable containers (`vector`) guard `len * item_size` against `ULONG_MAX` overflow before allocating. `realloc` failure leaves the original buffer intact, so the handle stays usable. Keep both properties.

## Testing Status

Only these test functions are wired into a runner and actually execute:

- `simple()` — via `LyDSS_SimpleTest`
- `pool_basic()`, `array_basic()`, `vector_basic()`, `stream_basic()` — via `LyDSS_AllBasicTest`

These files compile but never run, and several are not functional:

- `list_basic.c`, `set_basic.c`, `stack_basic.c` — have working functions, simply not called by any runner
- `bucket_basic.c` — function body is empty
- `table_basic.c` — file header only, defines no function
- `queue_basic.c`, `stack_base_type.c` — entire body is inside a block comment and targets a removed older API (`queue_create`, `stack_push_multi`). Reviving them requires a rewrite, not just uncommenting.

When writing print-based tests, **do not call a mutating function and read the mutated variable in the same `printf` argument list.** GCC evaluates arguments right-to-left, so the variable is read before the call executes and the test silently reports stale values. Assign to a local on a separate statement first. This mistake was made twice in this repository's history.

## Code Style

The authoritative style spec is [`references/c-style.md`](.agents/skills/c-code-review/references/c-style.md) inside the `.agents/skills/c-code-review` git submodule, with a matching `.clang-format` in that submodule's `assets/`. Key points as applied here: four-space indent and no tabs; `if`/`for`/`while()` open braces on the same line while `switch` and `while(1)` break first; plain functions break before `{`; Doxygen `/** */` for file and function headers with `///` acceptable in `.c` bodies and `///<` for trailing comments; file-private static helpers in this codebase carry a leading underscore (`_memdup`, `_item_address`, `_reserve_for`), following the precedent in `lybucket.c`.

## Change Guidance

- Make small, local changes that preserve public APIs unless the task explicitly requires an API change.
- Read the matching implementation, public declarations, and nearest test before editing. Add a focused regression or smoke test when behavior changes.
- When evaluating a new container, first clarify its element model, core operations, ownership, capacity rules, and deletion behavior; then compare that contract against the table above. Check specifically whether the proposed struct is field-for-field equivalent to an existing one — that happened with group versus vector.
- Do not use the legacy `examples/RedPandaIDE/makefile.win` as the primary build entry point. It is kept in sync by hand and lists only a subset of sources.
- Verify with `-Wall -Wextra` after edits. `src/` is currently warning-free under those flags; keep it that way. When a failure is caused by existing test wiring, report it separately rather than masking it with unrelated changes.

## Known Duplication (Audited, Unresolved)

An audit of struct layouts and core abstractions found the following. None of it has been acted on except the group removal.

- **pool, bucket, and table are three implementations of a name-to-pointer map.** `lypool_post`/`get`/`clear`, `lybucket_put`/`get`/`erase`, and `lytable_set`/`get`/`remove` are the same operation set. Pool is the weakest of the three: linear scan, fixed capacity, and it silently discards on overflow. Its only distinguishing traits are zero extra allocation and not copying the key.
- **set and table are both chained hash tables** with isomorphic layouts (bucket array plus singly-linked nodes). They differ in which strengths they have: set is generic over element type via caller callbacks but never rehashes; table is string-keyed only but grows at load factor 0.75.
- **`set` and `table` use `size` with opposite meanings.** In `LySet`, `size` is the bucket count and `count` is the element count. In `lytable`, `capacity` is the bucket count and `size` is the element count. This is an easy source of error when reading or merging the two.
- **`lybucket.c`'s header comment claims a "list + table" core, but no hashing exists.** `lybucket_get` walks the doubly-linked list doing `strcmp`. It is currently a named linked list; the comment describes an unimplemented design.
- `LyBucket.count` is `uint8_t`, capping a bucket at 255 entries while the API suggests no such limit. The field width is only half the problem — see Verified Defects for the search truncation it causes.

## Verified Defects (Unresolved)

Every entry below was confirmed by reading the cited line. None has been fixed. Line numbers drift — re-grep before editing.

### Embedded portability blockers

These prevent the library from building or running on a real embedded target, so they outrank the cosmetic items.

- **`inc/lydss_api.h:29` hardcodes `__asm("int3")`** inside `LYDSS_ASSERT`. That is an x86-only instruction; an ARM or RISC-V build fails to compile. The trap action needs to be a replaceable macro with an architecture-neutral default.
- **`inc/lydss_cfg.h:16` includes `<stdio.h>` unconditionally** and `LYDSS_PRINTF` is hardwired to `printf`. A bare-metal target without stdio cannot use the library at all, even with assertions disabled. Both the output channel and the include need to be conditional on the caller not having supplied their own.
- **`<malloc.h>` in three implementations** — `src/lyqueue.c:19`, `src/lyset.c:18`, `src/lystack.c:18`. It is not a standard header and most embedded toolchains do not ship it. All three files already include `<stdlib.h>` (lines 14, 13, 13 respectively), which declares everything they use, so these three lines are pure redundancy and can be deleted with no replacement.
- **`src/lytable.c:77` uses floating point to test the load factor**: `(double)t->size / t->capacity >= LOAD_FACTOR`, with `LOAD_FACTOR 0.75` at line 17. On an FPU-less target this links in the soft-float runtime for one comparison. **The obvious integer rewrite is wrong on this target:** `size * 4 >= capacity * 3` overflows 32-bit `size_t`, the comparison becomes permanently false, and the table then *silently stops growing* — a performance cliff with no error. Use a division form such as `size >= capacity - capacity / 4`, which is exact here because the bucket count starts at 16 and only ever doubles, so it is always a multiple of 4.

### Missing parameter validation

- **`src/lyqueue.c:39` and `:67`, `src/lystack.c:33` and `:59` assert `item_size` but never `len`.** With `len == 0` the queue's `% queue->length` is integer division by zero (undefined behavior); the stack degrades more gently, with `push` always returning `false`. `src/lyarray.c:35` and `:62` *do* assert `len` — the three containers are inconsistent, and array is the one that is right.
- **`src/lyset.c:37` asserts `elem_size` but not the bucket count.** `size == 0` makes `% set->size` divide by zero.
- **`lypool_create` asserts nothing about `size`.** Same class of hole.

### Ownership and correctness

- **A node returned by `lybucket_remove` can never be freed.** `lybucket_remove` (`src/lybucket.c:192`) unlinks via `lybucket_remove_node`, clears the node's own `prev`/`next`, and returns it *without* freeing. `lybucket_erase` (`:235`) is the only code that frees a node and its meta copy, and it does so on a node it removed itself. So the documented "remove but do not free" path has no exit — the caller holds memory with no API to release it. A `lybucket_free_node`-style function is missing. Note also that `lybucket_remove_node` on its own leaves the node's `prev`/`next` dangling, because the clearing happens one level up in `lybucket_remove`.
- **`src/lybucket.c:126` bounds the search with `i < bucket->count`.** The bound is redundant — the list already terminates on `NULL` — and it turns into a truncation bug as soon as `count` saturates, since `count` is `uint8_t` (`:48`). Past 255 entries `lybucket_get` stops finding things. Removing the counter from the loop condition fixes the search independently of widening the field.
- **`lybucket_patch_meta` frees before it allocates.** `src/lybucket.c:176` frees the old `meta.data`, then `:181`/`:182` call `_memdup`. If that allocation fails the old metadata is already gone. Allocate first, then free.
- **`_memdup` traps on a legitimate input.** It asserts a non-zero `size` (`src/lybucket.c:383`), but `put`/`post`/`insert` reach it whenever `meta.type != NULL` and `meta.size == 0` (`:95`, `:285`, `:337`). A zero-size copy should be treated as "no metadata", not as a programming error.
- **`src/lystream.c:31` does not guard its allocation size.** `calloc(1, sizeof(LyStream_t) + size * sizeof(cell_t))` can wrap for large `size`, and the single-allocation layout means the wrap yields a short buffer rather than a failure.
- **Overflow-guard strength is inconsistent.** `lyvector` explicitly checks `len * item_size` against `ULONG_MAX` (a settled requirement, see API And Ownership), while `lyarray`, `lystack`, and `lyqueue` rely on `calloc`'s own internal check. Pick one standard and apply it to all four.
- **`LYLIST_FOREACH` (`inc/lydss_api.h:375`) has no safe variant.** Deleting the current node inside the loop is undefined behavior, because the macro reads `node->next` after the body has run. A variant that caches the successor before the body is the usual fix.
- **`lypool` handles duplicate names three different ways.** `lypool_post` (`src/lypool.c:111`) fills the first free slot without checking for an existing entry of the same name; `lypool_get` (`:127`) returns the first match; `lypool_clear` (`:144`) clears *every* matching slot, since its loop has no early exit. Whatever the intended rule is, these three should agree on it.
- **`src/lyset.c` narrows the hash index in two of three places.** `lyset_add` holds it in `size_t` (`:62`) while `lyset_remove` (`:93`) and `lyset_contains` (`:180`) use `unsigned`. Harmless on this 32-bit target, silently wrong on a 64-bit one.

### API surface inconsistencies

- **`lypool_post` returns `void` and drops silently when full** (`src/lypool.c:106`, with the `TODO` at `:118`). The caller cannot detect the failure. Returning `bool` is purely additive — code that ignores the result still compiles.
- **`lypool_get` and `lypool_clear` carry stale `// TODO: 找不到？` comments** at `:133` and `:151`, although returning `NULL` and doing nothing are already well-defined answers. These are noise, not open questions.
- **`lyset_init` breaks the `_create` convention** every other container follows.
- **`cell_t`, `Hash_t`, and `Equals_t` sit in the global namespace with no `Ly` prefix.** `cell_t` in particular is a name likely to collide with application code.
- **`lystream_clean` is spelled differently from the `clear` used by queue, stack, vector, and pool.**
- **`lytable_set` returns `int` (0 / -1)** while the rest of the library signals failure with `bool` or `NULL`.

## Known Repository Limitations

- `CMakePresets.json` is absent and gitignored, so preset-based build instructions cannot work as written.
- CMake declares no public include directories; sources reach the header by relative path. An external consumer cannot `#include <lydss_api.h>` without additional configuration.
- `examples/RedPandaIDE/RedPandaIDE.layout` is a Red Panda editor cache still holding another machine's absolute paths (`C:/Users/wzyde/...`). It is tracked because the `.gitignore` rules for it point at `dev/RedPandaIDE/*` while the files actually live in `examples/RedPandaIDE/`. Fixing the ignore paths would untrack these caches.
- `lypool_create_static` has an implementation in `src/lypool.c` but its declaration in the public header is commented out.
- `.cache/` is not listed in `.gitignore`, so the clangd index sits in the working tree as untracked noise. Adding `.cache/` is a one-line fix.
- The README is a placeholder; rely on the headers, CMake files, implementations, and tests for authoritative behavior.
