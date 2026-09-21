# LyDSS Agent Guide

## Project Shape

- LyDSS stands for **DataStore Struct**, prefixed with the author's handle. **`Ly` is short for Luyi (the author, Luyi365) — not "Lyi".** Do not expand it as `Lyi` in the README or anywhere else. The spelling is `LyDSS` everywhere; `LyDDS` is a historical typo that was fully removed. Do not reintroduce it.
- LyDSS is a C project that builds one static library per container in `src/`: array, bucket, list, pool, queue, set, stack, stream, table, and vector. All ten libraries are independent — no container links against another.
- Public declarations are centralized in [`inc/lydss_api.h`](inc/lydss_api.h); configuration, assertion behavior, and the per-container feature switches are in [`inc/lydss_cfg.h`](inc/lydss_cfg.h). See Feature Gating below.
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

## Feature Gating (Compile-Time)

Every container can be compiled out individually. [`inc/lydss_cfg.h`](inc/lydss_cfg.h) defines one switch per container — `LYDSS_POOL_ENABLED`, `LYDSS_ARRAY_ENABLED`, `LYDSS_VECTOR_ENABLED`, `LYDSS_STACK_ENABLED`, `LYDSS_QUEUE_ENABLED`, `LYDSS_LIST_ENABLED`, `LYDSS_STREAM_ENABLED`, `LYDSS_TABLE_ENABLED`, `LYDSS_SET_ENABLED`, `LYDSS_BUCKET_ENABLED` — listed in the same order as the `lydss_api.h` sections. Non-zero enables, `0` strips. All default to `1`.

Each switch, including `LYDSS_ASSERT_ENABLED`, is wrapped in `#ifndef`, so a `-DLYDSS_SET_ENABLED=0` on the command line wins over the header and the file does not need editing. That is the intended way to configure a build; the header default is the fallback.

A container's gate appears in **four** places, and all four must be kept in sync:

1. The `#ifndef`/`#define` default in `inc/lydss_cfg.h`.
2. The declaration block in `inc/lydss_api.h`, wrapped `#if LYDSS_X_ENABLED` … `#endif /* LYDSS_X_ENABLED */` immediately inside the `/* ---- 名称 ---- */` separator.
3. The implementation in `src/ly<name>.c`, wrapped the same way.
4. The matching `test/<name>_basic.c` body, plus the call site in `examples/run_all_basic_test.c`. (`test/simple.c` and `examples/run_simple_test.c` are gated on `LYDSS_STACK_ENABLED`, because `simple()` exercises the stack.)

Because the gate has to be visible before it can be tested, `.c` files now include `"../inc/lydss_api.h"` **first**, then open the `#if`, then include the standard library headers inside the guard. This is also the order the style spec asks for (本模块头文件 → 项目内 → 标准库), so the previous stdlib-first ordering was a violation that this change incidentally fixed. A disabled container therefore pulls in no `<stdlib.h>`/`<string.h>` either.

CMake is deliberately untouched: the ten library targets are always declared and always linked. A disabled container still compiles, but to an object with no functions in it. Verified with `nm --defined-only`: an enabled `libLyArray.a` exports 13 symbols, a disabled one exports only the three empty `.text`/`.data`/`.bss` section markers. If you want the targets themselves to disappear, that needs CMake options mirroring these macros — not done, and it would duplicate the source of truth.

Verified configurations (all with `-Wall -Wextra`, Ninja, gcc 13.2):

| Configuration | Result |
|---|---|
| all enabled | builds clean, 2/2 ctest pass |
| `ARRAY`/`STACK`/`SET`/`TABLE`/`BUCKET` off | builds clean, 2/2 pass, array test correctly skipped at runtime |
| all ten off | builds clean, 2/2 pass |
| all ten off **and** `LYDSS_ASSERT_ENABLED=0`, plus `-pedantic` | builds clean |
| `LYDSS_ASSERT_ENABLED=0`, containers on | builds clean, 2/2 pass |

Gating adds no new warnings even under `-pedantic`. The three `ISO C forbids an empty translation unit` warnings that `-pedantic` reports are pre-existing and independent of the switches: `test/queue_basic.c`, `test/stack_base_type.c` (whole body block-commented) and `test/table_basic.c` (no function). They appear identically with everything enabled.

When adding a container, add its switch to all four places above and re-run at least the all-enabled and the container-off builds.

## Internal Base Layer

[`inc/lydss_priv.h`](inc/lydss_priv.h) holds the operations shared between containers. It lives in `inc/` alongside the public headers because this project keeps **one** header directory; the `priv` in the name, not its location, marks it private. That is safe today only because CMake declares no public include directory at all — `inc/` is not currently an exported path. **If a public include directory is ever declared, export only `lydss_api.h` and `lydss_cfg.h`, or move this file to `inc/internal/`**, otherwise the base layer silently becomes an external promise. Two rules define what belongs there, and they were settled deliberately:

**A thing with a handle and a lifecycle is a container, not a primitive.** Primitives are stateless operations. This is why `LyArray` is *not* the base for queue and stack even though the fields line up — it has `LyArray_t *`, `is_static`, its own asserts and bounds checks. Composing it would mean nesting lifecycles and paying its validation twice. Likewise `LyVector` is not a primitive; only its growth policy could be, and that policy is vector-specific so it stays in `lyvector.c`.

**The base is header-only and not public.** Everything in it is a `static inline` function or a macro, and none of it may be re-declared in `lydss_api.h`. Three consequences, all of which were the point:

1. No cross-library dependency. The ten libraries still do not link against one another, so the ten feature switches stay freely combinable — `LYDSS_QUEUE_ENABLED=1` with `LYDSS_ARRAY_ENABLED=0` remains valid. Composing through a *linked* base would have turned the gating matrix into a dependency graph.
2. Nothing to inline across translation units. CMake declares no `INTERPROCEDURAL_OPTIMIZATION` and no `-flto`, so a cross-library call could not have been inlined; each element access would have cost a real call plus redundant asserts and a second bounds check.
3. Freedom to refactor. A public base would be frozen by compatibility.

The base deliberately performs **no parameter assertions**. Callers validate handles and bounds before entering it; re-checking is pure overhead on an embedded hot path.

### `LyStore_t` — the shared storage descriptor

```c
typedef struct LyStore {
    void *buffer;
    unsigned long length;
    unsigned long item_size;
} LyStore_t;
```

Embedded **by value** as a member named `store` in array, vector, stack, and queue. Embedding by value costs nothing: `q->store.buffer` resolves to the same offset arithmetic as the old `q->items`, and it creates no link dependency because only fields and inline primitives are touched. This is why struct sharing is cheap while *function* composition (queue calling `lyarray_set`) is not — do not confuse the two.

**`is_static` is deliberately excluded.** It is an ownership flag, not a storage description. Including it would give vector — which will never support static construction — a wasted byte plus padding, growing its handle from 16 to 20. With it excluded, all four handles are byte-identical to before. Measured on i686 after the change: `LyStore` 12, `LyArray` 16, `LyVector` 16, `LyStack` 20, `LyQueue` 28, `LyStream` 24, `LyPool` 12 — every one matching its pre-refactor size.

Vector maps `capacity` onto `store.length`; there is no separate capacity field, so `lyvector_capacity()` returns `vector->store.length`. This does **not** violate the settled "vector must not depend on `lyarray`" decision: the shared descriptor is owned by nobody, and `realloc`-based in-place growth is untouched. The growth *policy* (the 2× doubling in `vector_reserve_capacity`) stays in `lyvector.c`; only the "resize the buffer and update capacity" step moved to `lystore_realloc`.

**Stream and pool opt out.** Stream has no `item_size` — its unit is fixed at `cell_t` — so adopting the descriptor would waste 4 bytes (24 → 28), and its buffer is allocated together with its handle so the destroy path differs too. Pool's items are a typed array reached by plain subscript. Both opting out is correct; do not force them in for symmetry.

Lifecycle helpers that were duplicated across the four containers now live on the descriptor: `lystore_bind` (static construction), `lystore_alloc` (with the overflow guard folded in), `lystore_realloc`, `lystore_free`, `lystore_unbind`, and `lystore_address`. The `create`/`create_static`/`delete` triplet was near-identical in array, stack, and queue; that was real duplicated logic, not just arithmetic, which is why it was worth extracting.

**One deliberate behavior change:** `lystore_unbind` clears all three descriptor fields. Previously `lyarray_delete` zeroed `length` while `lystack_delete` and `lyqueue_delete` left it intact — an existing inconsistency. Now a released static container uniformly reports `length == 0` and `item_size == 0`, which also means `lystack_is_full()`/`lyqueue_is_full()` return `true` instead of `false` on a destroyed handle. This is only observable when querying an already-destroyed container, which is unsupported use. Verified to be the *only* behavioral difference: a comparison probe covering array, vector, stack, queue, and the static-construction paths produced 58 of 59 identical lines, the single diff being this post-unbind query.

**No `lydss_priv.c`.** A shared `.c` would have to be either compiled into every container library (duplicate symbols when an application links two containers) or built as its own library that the ten link against (the cross-library dependency this design exists to avoid). The rule that follows: **if a piece of shared logic is too large to inline comfortably, that is a signal the two containers should be merged, not that a shared `.c` is needed.**

### Naming

Names describe *what is operated on*, not which layer they sit in — the layer is identified by the file, not by a prefix. `LyStore_t` / `lystore_*` for the storage descriptor, `lyring_*` for the ring cursor, `LYDLIST_*` and `LYSLIST_*` for the intrusive link macros. An earlier revision put everything under a single `lybase_` / `LYBASE_` umbrella; that forced redundant compounds like `LYBASE_DLIST_UNLINK` and was replaced. `LyStore_t` was also preferred over `LyBase_t` because a type name should say what a thing *is*, not where it sits — and it echoes the project's own name, Data**Store** Struct. These sit alongside the public `ly<container>_*` and the config macros `LYDSS_*`; privacy is carried by the header they live in, not by the prefix. Note the style spec's rule that `inline` functions put `{` on the *same* line — the base header follows it, unlike the plain functions everywhere else in `src/`.

### What each container uses

| Container | Base facilities used |
|---|---|
| `lyarray.c` | `LyStore_t`, `lystore_bind`/`alloc`/`free`/`unbind`/`address` |
| `lyvector.c` | `LyStore_t`, `lystore_alloc`/`realloc`/`free`/`address` |
| `lystack.c` | `LyStore_t`, `lystore_bind`/`alloc`/`free`/`unbind`/`address` |
| `lyqueue.c` | `LyStore_t`, `lystore_bind`/`alloc`/`free`/`unbind`/`address`, `lyring_next` |
| `lystream.c` | `lyring_next`, `lystore_bytes` (no `LyStore_t` — see above) |
| `lylist.c` | all five `LYDLIST_*` macros |
| `lybucket.c` | `LYDLIST_LINK_FRONT`/`LINK_REAR`/`LINK_BEFORE`/`LINK_AFTER`/`UNLINK` |
| `lyset.c` | `LYSLIST_LINK_HEAD`, `LYSLIST_UNLINK` |
| `lytable.c` | `LYSLIST_LINK_HEAD`, `LYSLIST_UNLINK` |
| `lypool.c` | **none** — its items are a typed array accessed by plain subscript, and `calloc` already guards the product. Opting out is correct; do not wire it in for symmetry's sake. |

The intrusive-list macros require the participating node type to name its links `prev` and `next`, and they reference their arguments more than once, so do not pass expressions with side effects. `LyListNode` is public so its layout could not be changed to embed a base node type; macros over named fields are the standard C answer (compare BSD `sys/queue.h`) and avoid a breaking API change.

`lytable_remove` was converted from a `prev`-pointer walk to the `**pp_node` idiom so both hash containers unlink the same way.

### Measured cost

Compared `.text` per library at `-Os` against commit `3f1773f` (the last revision before the base layer). Net change across all ten: **+24 bytes total** — array +12, vector +4, stack +4, queue +8, stream +8, list +8, table −16, set 0, bucket −4, pool 0.

The increases are attributable to the newly added overflow guards, not to inlining or to `LyStore_t`: embedding the descriptor by value produces identical offset arithmetic, and the addressing primitive compiles to what was already there. Table and bucket shrank because real logic was deduplicated. Handle sizes did not change at all (see `LyStore_t` above).

One honest cost: `LYDLIST_UNLINK` clears the node's own `prev`/`next`, so `lylist_remove` performs two stores to memory it is about to `free`. That is the +8 bytes on list. It buys a macro that is safe to reuse when the node survives the unlink (which is exactly what `lybucket_remove` needs), and it fixes the dangling-pointer defect recorded below.

### Verification performed

ctest covers only pool, array, vector, and stream (see Testing Status), so most of this refactor is invisible to it. Two throwaway comparison programs were used instead, each built twice — once against the pre-refactor sources from a `git worktree` at the previous commit, once against the working tree — and their output diffed:

1. **Linking logic** (list, bucket, set, table): forward *and* backward traversal after every list mutation, insertion before the head and after the tail, draining to empty, removing the only node, bucket head/tail/middle erase, set duplicate detection across conflict chains, 64 table insertions spanning several rehashes. 54 lines, byte-identical.
2. **`LyStore_t` adoption** (array, vector, stack, queue): bounds behaviour, address stability, growth and shrink sequences, ring wrap-around under interleaved send/receive, plus the static-construction paths — reachable only by `#include`-ing the `.c` files directly, which is the one way to exercise `*_create_static` at all given the incomplete-type problem. 59 lines, one expected diff (the `lystore_unbind` change above).

Reproduce this rather than trusting ctest if you touch linking, the descriptor, or any lifecycle path. A `git worktree` at the last good commit plus `gcc -I<tree>/src -I<tree>/inc` on the same probe source is the whole technique.

## Working Agreement

- **This file is the single source of truth for project rules.** There is no spec directory: for a single-developer, exploratory project the owner chose one durable document over per-feature requirements/design/tasks artifacts. Do not recreate `.kiro/specs/` or reference it. `.kiro/` itself does exist again, but only as a second install location for the `c-code-review` skill (`.kiro/skills/c-code-review/`, mirroring `.agents/skills/c-code-review/`); it is gitignored. Leave it alone — it is tooling, not project content.
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

**A `lygroup` container (append-only batch abstraction) was built and then removed** at the owner's request. Its struct was field-for-field identical to `LyVector`, and its two static helpers were literal duplicates of vector's `vector_reserve_capacity`/`vector_locate_item` (both were then still named `_reserve_for`/`_item_address`). If a batch abstraction is wanted again, note that its only irreplaceable value over vector was type-level restriction (no `set`/`remove`, `const` returns, a callback that cannot tell members apart), not storage behavior.

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

The authoritative style spec is [`references/c-style.md`](.agents/skills/c-code-review/references/c-style.md) inside the `.agents/skills/c-code-review` git submodule, with a matching `.clang-format` in that submodule's `assets/`. That submodule also ships a `c-code-review` skill that produces a graded (违规 / 警告 / 通过) audit against the spec — use it instead of inventing style rules, and treat the spec as the only basis for a judgement.

That `.clang-format` is **not** copied to the repository root, so nothing is auto-formatted today (see Known Repository Limitations). And even when it is used, it covers only the mechanizable rules: brace placement for `switch`, `while(1)`, `inline`, and metamethods, plus every naming and file-ordering rule, has to be checked by reading.

### Conformant and settled

- Four-space indent, no tabs. Verified: `src/` and `inc/` contain zero tab characters.
- `if`/`else`/`for`/`while()`/`do` open the brace on the same line; `switch` and `while(1)` break first. Live examples: `src/lybucket.c:88` (`switch`), `inc/lydss_api.h:29` (`while(1)` inside `LYDSS_ASSERT`).
- Plain functions break before `{`. No `inline` function and no metamethod exists yet, so the same-line exception is currently unused.
- `/** */` Doxygen blocks for file headers and for the public function declarations in `inc/lydss_api.h`; `///` for function headers inside `.c` bodies; `///<` for trailing member comments.
- `.c` layout follows the spec's order exactly: file header comment, includes, macros, struct definition, static forward declarations, public functions, then static implementations behind a `/* --------------------------------- static --------------------------------- */` separator. `src/lyvector.c` is the cleanest reference. Include order is 本模块头文件 → 标准库, i.e. `"../inc/lydss_api.h"` first — see Feature Gating for why.
- **Container order in `inc/lydss_api.h` is fixed: pool, array, vector, stack, queue, list, stream, table, set, bucket.** Each container is a block introduced by a `/* ---- 名称 ---- */` separator. The order runs from the simplest storage primitive outward to the composite containers, and `examples/run_all_basic_test.c` calls its tests in the same order (including the commented-out entries) so the two files can be read side by side. A new container goes at the position its abstraction level implies, not at the end. The spec's 顺序 section does not cover inter-container order, so this is a local convention.
- Header guards are `文件名_H`: `LYDSS_API_H`, `LYDSS_CFG_H`. Never `__LYDSS_API_H__` — `_`/`__` prefixes are the compiler's reserved namespace.
- Types are `struct Ly<Name>` in Pascal case, exposed through a `typedef ... Ly<Name>_t` forward declaration in the header. Enum members carry the `枚举名_` prefix (`LySetRelation_NONE`, `LyBucketMetaType_COPY`).
- Abbreviation strength tracks scope as the spec requires: parameters use truncated forms (`buf`, `len`, `obj`), struct members are spelled out (`buffer`, `length`, `item_size`, `capacity`, `is_static`), function and macro names are never abbreviated.
- Return conventions are both legal under the spec, which defines two separate schemes: `bool`/`NULL` is a 逻辑返回值 (1 true / 0 false), while `int` 0-success / non-zero-error is an 错误处理返回值. So `lytable_set` returning `int` is a *consistency* complaint, not a style violation — see API surface inconsistencies below.

### File-private static helpers carry no leading underscore

Static helpers are named `<container>_<verb>_<noun>` — the public function structure minus the `ly` prefix. Current set:

| File | Static helpers |
|---|---|
| `lyarray.c` | `array_locate_item` |
| `lyvector.c` | `vector_reserve_capacity`, `vector_resize_buffer`, `vector_locate_item` |
| `lybucket.c` | `bucket_dup_memory` |
| `lytable.c` | `table_hash_default`, `table_resize_slots` |

`lyarray.c` and `lyvector.c` lost their `*_locate_item` helpers to the base layer, so `lylist.c`, `lypool.c`, `lyqueue.c`, `lyset.c`, `lystack.c`, `lystream.c` and now `lyarray.c` have no static helpers at all.

The shared base layer uses its own namespaces, `lystore_*` / `lyring_*` / `LYDLIST_*` / `LYSLIST_*` — see Internal Base Layer. Its `inline` functions put `{` on the same line, per the spec's exception for `inline`; plain functions in `src/` still break before `{`.

**Do not add a leading underscore.** The spec reserves `_id_do_thing_sub()` for handle-registration interface functions (metamethods, which may only be `static`) and `_id_do_register()` for register access. A plain file-private helper fills neither role and must not borrow the marker. An earlier revision of this codebase used `_memdup`/`_item_address`/`_reserve_for` and an earlier revision of *this document* recorded that as the house convention; both were wrong against the spec, and the code has since been renamed — `_memdup` is now `bucket_dup_memory`. Every entry below that once cited an underscored name has been updated.

### Accepted deviations

These differ from the spec and are kept deliberately. Do not "fix" them.

- **Local variables use whole words, not initials.** The spec files local variables under 首字母缩写; the code writes `array`, `vector`, `new_len`, `new_buffer`, `hash`. Readability was preferred over abbreviation strength. `idx` and `cur` are the truncated forms actually in use. New code should match the surrounding files, not the spec's letter on this point.
- **Query functions omit the verb.** The spec's structure is 标识 + 动词 + 名词, which would give `lyarray_get_length`. The API ships `lyarray_length`, `lyarray_item_size`, `lyvector_size`, `lyvector_capacity`, `lyset_count`. Renaming them is a breaking public API change with no behavioral gain. New accessors follow the established verbless form rather than introducing a second style.
- **Enum member names are upper-case after the prefix** (`LySetRelation_NONE`), where the spec's only example is Pascal (`UserInfo_BaseID`). Consistent across both enums in the header; keep it.

### Open style violations

- **Trailing comment form is inconsistent inside the public header.** `enum LySetRelation` members (`inc/lydss_api.h:508`–`:512`) use end-of-line `/* ... */`, while `struct LyBucketMeta` members (`:607`–`:609`) use the spec's `///<`. Convert the enum to `///<`.
- **`cell_t`, `Hash_t`, and `Equals_t` (`inc/lydss_api.h:385`, `:517`, `:518`) are unprefixed**, and `cell_t` is all lower case where the spec puts `typedef`'d types in Pascal case. `LyTableHash_t` (`:451`) shows the intended form. The collision risk is tracked separately under API surface inconsistencies.
- **`Hash_t` and `Equals_t` are typedef'd as function *types*, not function pointers**, which is why every use has to spell `Hash_t *hash`. 规范未覆盖 — flagged as a warning only, but it is the reason the set API reads oddly.

## Change Guidance

- Make small, local changes that preserve public APIs unless the task explicitly requires an API change.
- Read the matching implementation, public declarations, and nearest test before editing. Add a focused regression or smoke test when behavior changes.
- When evaluating a new container, first clarify its element model, core operations, ownership, capacity rules, and deletion behavior; then compare that contract against the table above. Check specifically whether the proposed struct is field-for-field equivalent to an existing one — that happened with group versus vector.
- Do not use the legacy `examples/RedPandaIDE/makefile.win` as the primary build entry point. It is kept in sync by hand and lists only a subset of sources.
- Verify with `-Wall -Wextra` after edits. `src/` is currently warning-free under those flags; keep it that way. When a failure is caused by existing test wiring, report it separately rather than masking it with unrelated changes.
- Run the `c-code-review` skill over the files you touched before calling a change done, and judge only against [`references/c-style.md`](.agents/skills/c-code-review/references/c-style.md). If the result conflicts with something already listed under Code Style → Accepted deviations, the accepted deviation wins. If you introduce a new intentional deviation, append it there in the same session instead of leaving it for the next audit to rediscover.

## Known Duplication (Audited, Unresolved)

An audit of struct layouts and core abstractions found the following. None of it has been acted on except the group removal.

- **pool, bucket, and table are three implementations of a name-to-pointer map.** `lypool_post`/`get`/`clear`, `lybucket_put`/`get`/`erase`, and `lytable_set`/`get`/`remove` are the same operation set. Pool is the weakest of the three: linear scan, fixed capacity, and it silently discards on overflow. Its only distinguishing traits are zero extra allocation and not copying the key.
- **set and table are both chained hash tables** with isomorphic layouts (bucket array plus singly-linked nodes). They differ in which strengths they have: set is generic over element type via caller callbacks but never rehashes; table is string-keyed only but grows at load factor 0.75.
- **`set` and `table` use `size` with opposite meanings.** In `LySet`, `size` is the bucket count and `count` is the element count. In `lytable`, `capacity` is the bucket count and `size` is the element count. This is an easy source of error when reading or merging the two.
- **`lybucket.c`'s header comment claims a "list + table" core, but no hashing exists.** `lybucket_get` walks the doubly-linked list doing `strcmp`. It is currently a named linked list; the comment describes an unimplemented design.
- `LyBucket.count` is `uint8_t`, capping a bucket at 255 entries while the API suggests no such limit. The field width is only half the problem — see Verified Defects for the search truncation it causes.

## Verified Defects (Unresolved)

Every entry below was confirmed by reading the cited line, and all line numbers were re-verified against the current tree during the style audit. Line numbers still drift — re-grep before editing.

Three entries that used to live here were verified fixed and removed. They are listed so nobody re-files them:

- **`<malloc.h>` in `lyqueue.c`/`lyset.c`/`lystack.c`** — gone. All three now include only `<stdlib.h>` (`:14`, `:13`, `:13`).
- **Hash-index narrowing in `lyset.c`** — fixed. `lyset_add` (`:63`), `lyset_remove` (`:98`), and `lyset_contains` (`:190`) all hold the index in `size_t idx` now.
- **Leading-underscore static helpers** — renamed. See Code Style.

Three more were resolved by the base-layer work (see Internal Base Layer):

- **Inconsistent overflow-guard strength** — unified. `lyarray`, `lyvector`, `lystack`, `lyqueue`, and `lystream` all now go through `lystore_bytes`, which rejects the multiplication before allocating. The guard is written against `SIZE_MAX` rather than `ULONG_MAX`: on this 32-bit target the two are identical, and `SIZE_MAX` cannot mis-judge on an ABI where `unsigned long` is narrower than `size_t`. This supersedes the earlier "keep the `ULONG_MAX` guards" note — the strength is preserved, the type is just correct now. `lyvector` still keeps its own `ULONG_MAX` check on the *doubling factor*, which is about the capacity count and not a byte product.
- **`lystream_create` allocating without a guard** — fixed. It now validates `size * sizeof(cell_t)` and separately checks that adding `sizeof(LyStream_t)` cannot wrap, so the single-allocation layout can no longer yield a short buffer.
- **`lybucket_remove_node` leaving `prev`/`next` dangling** — fixed by `LYDLIST_UNLINK`.

One defect was *found* while re-running the gating matrix after the refactor, and fixed:

- **`examples/run_simple_test.c` called `simple()` without gating it** while `test/simple.c` gates its body on `LYDSS_STACK_ENABLED`, so any build with `-DLYDSS_STACK_ENABLED=0` failed to link with `undefined reference to 'simple'`. The guard was present when the feature-gating work was verified but is absent in commit `3f1773f`, so it was lost somewhere between verification and commit. `run_all_basic_test.c` was unaffected. **Lesson: a gated test function and its call site must be changed together**; re-run the all-off configuration after touching either.

### Embedded portability blockers

These prevent the library from building or running on a real embedded target, so they outrank the cosmetic items.

- **`inc/lydss_api.h:31` hardcodes `__asm("int3")`** inside `LYDSS_ASSERT`. That is an x86-only instruction; an ARM or RISC-V build fails to compile. The trap action needs to be a replaceable macro with an architecture-neutral default.
- **`LYDSS_PRINTF` is still hardwired to `printf`** at `inc/lydss_api.h:23`. Half of this is now fixed: the `<stdio.h>` include in `inc/lydss_cfg.h` sits inside `#if LYDSS_ASSERT_ENABLED`, so a target that builds with `-DLYDSS_ASSERT_ENABLED=0` no longer drags in stdio (verified to build clean). What remains is that a target which *wants* assertions but has no `printf` cannot redirect the output — `LYDSS_PRINTF` needs the same `#ifndef` treatment as the other switches so a caller can supply their own sink.
- **`src/lytable.c:66` uses floating point to test the load factor**: `(double)table->size / table->capacity >= LOAD_FACTOR`, with `LOAD_FACTOR 0.75` at line 18. On an FPU-less target this links in the soft-float runtime for one comparison. **The obvious integer rewrite is wrong on this target:** `size * 4 >= capacity * 3` overflows 32-bit `size_t`, the comparison becomes permanently false, and the table then *silently stops growing* — a performance cliff with no error. Use a division form such as `size >= capacity - capacity / 4`, which is exact here because the bucket count starts at 16 and only ever doubles, so it is always a multiple of 4.

### Missing parameter validation

- **`src/lyqueue.c:34` and `:62`, `src/lystack.c:32` and `:58` assert `item_size` but never `len`.** `src/lyarray.c:35` and `:62` *do* assert `len` — the three containers are inconsistent, and array is the one that is right. **Correction to an earlier version of this entry:** it claimed `len == 0` makes the queue divide by zero at `% queue->length`. That is wrong. `lyqueue_send` returns early on `count >= length` (`:80`), and `receive`/`peek` return early on `count == 0`, so the modulo is unreachable when `length == 0`. The real consequence is quieter and applies to both containers: a zero-length queue or stack is constructed successfully and is then permanently both empty **and** full — every `send`/`push` returns `false`, every `receive`/`pop` returns `false`, and `is_empty()` and `is_full()` both return `true`. Nothing reports the bad argument. Re-verified by reading both files in full.
- **`src/lyset.c:36` asserts `elem_size` but not the bucket count — this one *is* a real divide by zero.** Unlike queue and stack, `lyset_add` (`:63`), `lyset_remove` (`:98`) and `lyset_contains` (`:190`) compute `% set->size` with no prior guard, so `lyset_init(0, ...)` succeeds and the first operation is undefined behavior.
- **`lypool_create` (`src/lypool.c:69`) asserts nothing about `size`.** Same class of hole; a zero-size pool is created successfully, accepts no objects, and `lypool_post` drops every one of them silently.

### Ownership and correctness

- **A node returned by `lybucket_remove` can never be freed.** `lybucket_remove` (`src/lybucket.c:188`) unlinks via `lybucket_remove_node` (`:205`), clears the node's own `prev`/`next`, and returns it *without* freeing. `lybucket_erase` (`:231`) is the only code that frees a node and its meta copy, and it does so on a node it removed itself. So the documented "remove but do not free" path has no exit — the caller holds memory with no API to release it. A `lybucket_free_node`-style function is missing. (The secondary half of this entry — `lybucket_remove_node` leaving `prev`/`next` dangling — is now fixed: `LYDLIST_UNLINK` clears them, so the node is self-consistent at whichever level it is unlinked.)
- **`src/lybucket.c:122` bounds the search with `i < bucket->count`.** The bound is redundant — the list already terminates on `NULL` — and it turns into a truncation bug as soon as `count` saturates, since `count` is `uint8_t` (`:45`). Past 255 entries `lybucket_get` stops finding things. Removing the counter from the loop condition fixes the search independently of widening the field.
- **`lybucket_patch_meta` (`src/lybucket.c:164`) frees before it allocates.** `:172` frees the old `meta.data`, then `:177`/`:178` call `bucket_dup_memory`. If that allocation fails the old metadata is already gone. Allocate first, then free.
- **`bucket_dup_memory` traps on a legitimate input.** It asserts a non-zero `size` (`src/lybucket.c:382`), but `put`/`post`/`insert` reach it whenever `meta.type != NULL` and `meta.size == 0` (`:91`, `:281`, `:333`). A zero-size copy should be treated as "no metadata", not as a programming error.

- **`LYLIST_FOREACH` (`inc/lydss_api.h:381`) has no safe variant.** Deleting the current node inside the loop is undefined behavior, because the macro reads `node->next` after the body has run. A variant that caches the successor before the body is the usual fix.
- **`lypool` handles duplicate names three different ways.** `lypool_post` (`src/lypool.c:107`) fills the first free slot without checking for an existing entry of the same name; `lypool_get` (`:123`) returns the first match; `lypool_clear` (`:140`) clears *every* matching slot, since its loop has no early exit. Whatever the intended rule is, these three should agree on it.

### API surface inconsistencies

- **`lypool_post` returns `void` and drops silently when full** (`src/lypool.c:107`, with the `TODO` at `:119`). The caller cannot detect the failure. Returning `bool` is purely additive — code that ignores the result still compiles.
- **`lypool_get` and `lypool_clear` carry stale `// TODO: 找不到？` comments** at `:134` and `:152`, although returning `NULL` and doing nothing are already well-defined answers. These are noise, not open questions.
- **`lyset_init` breaks the `_create` convention** every other container follows.
- **`cell_t`, `Hash_t`, and `Equals_t` sit in the global namespace with no `Ly` prefix** (`inc/lydss_api.h:385`, `:517`, `:518`). `cell_t` in particular is a name likely to collide with application code. `LyTableHash_t` (`:451`) is the same kind of callback typedef done right. See Code Style → Open style violations for the casing side of this.
- **`lystream_clean` is spelled differently from the `clear` used by queue, stack, vector, and pool.**
- **`lytable_set` returns `int` (0 / -1)** while the rest of the library signals failure with `bool` or `NULL`. Both forms are sanctioned by the style spec (逻辑返回值 vs 错误处理返回值), so this is a consistency choice to settle, not a style violation to fix. Pick one scheme library-wide and say so here.

## Known Repository Limitations

- `CMakePresets.json` is absent and gitignored, so preset-based build instructions cannot work as written.
- CMake declares no public include directories; sources reach the header by relative path. An external consumer cannot `#include <lydss_api.h>` without additional configuration.
- **No `.clang-format` at the repository root**, so formatting is not actually enforced. The only copies live in the skill's `assets/` (`.agents/skills/c-code-review/assets/.clang-format` and the `.kiro/` mirror). Copying one to the root would make `clang-format` pick it up automatically; until then, brace and indent rules are honored by hand.
- `examples/RedPandaIDE/RedPandaIDE.layout` is a Red Panda editor cache still holding another machine's absolute paths (`C:/Users/wzyde/...`). It is tracked because the `.gitignore` rules for it point at `dev/RedPandaIDE/*` while the files actually live in `examples/RedPandaIDE/`. Fixing the ignore paths would untrack these caches.
- `lypool_create_static` has an implementation in `src/lypool.c:91` but its declaration is commented out in the public header (`inc/lydss_api.h:64`).
- [`README.md`](README.md) is no longer a placeholder. It documents each container's applicability and characteristics plus two comparison tables (the owner's 14 axes, and a supplementary 6 covering capacity model, lookup cost, member ownership, address stability, required callbacks, and full/failure behavior). Every cell was derived by reading the implementation, and the footnotes deliberately record the known defects that affect usage choices (bucket's 255 cap, `lypool_post` dropping silently, set never rehashing, static constructors being uncallable from outside). **It has no clone/build section yet** — the owner deferred that. When behavior changes, update the README table in the same session; a wrong table there is worse than no table. The headers and implementations remain authoritative for exact signatures.
