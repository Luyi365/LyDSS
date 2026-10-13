# LyDSS Agent Guide

## 核心宗旨

**功能与代码保持极简：只实现主流场景确实需要的能力，绝不为极端个例或少数使用场景引入可能损害整体性能、代码体积或稳定性的代码。** 本文件其余条目与之冲突时以本宗旨为准；Architecture Decisions 里已定的事若按本宗旨需要推翻，先提出来，不要直接改。

落到具体判断：

- **新增前先问是否属于主流场景。** 少数场景的需求，让调用者在库外用现有接口组合实现，不进库。
- **「损害」按代价计**：热路径上多出的分支或运行开销、增加的代码体积与内存占用、扩大公开 API、引入新的所有权或生命周期规则、增加需要同步的位置，任何一项都算。
- **删优于修，修优于加。** 已有功能若只服务少数场景，又带着缺陷或维护负担，删除优先于修复。
- **反方向同样适用**：问题若影响的是**多数**场景（例如主流嵌入式目标编译不过），修复它正是本宗旨的要求，不受「少改动」约束。

**开发要求：面向嵌入式，跨平台、无依赖，合理拥抱新特性。**

- **针对嵌入式平台优化，必须能在 Keil 的 MicroLIB 下运行。** 库只能用 MicroLIB 提供的那部分标准库；某个标准函数 MicroLIB 没有，就不用它。
- **跨平台、无依赖。** 只依赖 C 标准头文件，不用编译器扩展、不用 POSIX、不依赖任何第三方库或操作系统。
- **不特意向旧的 C 版本做兼容**，不为 C99 及更早的编译器写兼容代码；已有的这类写法属于应清理的旧东西。
- **合理拥抱新特性**：主流嵌入式工具链都已支持的新特性就用；还没普及的（主要是 C23 的大部分内容）先不用。判断标准是目标工具链的实际支持，而不是标准发布了没有。

由此落到代码上的约束：

- **语言基线是 C11**，用 C17 构建（C17 只是 C11 的缺陷修订，没有新特性）。Arm Compiler 6 默认就是 `gnu11`。C11 的语言特性尽管用，**C23 的 `nullptr`、`constexpr`、`[[...]]` 属性、`enum : type`、`typeof` 暂不使用**。
- **C11 的特性优先用关键字形式**（`_Static_assert`、`_Alignof`），不用要包含 `<assert.h>` 才有的 `static_assert` 宏——MicroLIB 不支持 `assert()`，没必要把这个头文件拉进来。
- **MicroLIB 不是完整的 ISO C 库**：没有 `assert()`/`abort()`/`exit()` 这类与操作系统交互的函数，也没有宽字符函数；堆函数要求工程配置了堆区。库目前只调用 `malloc`/`calloc`/`realloc`/`free`/`memcpy`/`memset`/`strcmp`/`strcpy`/`strlen`，外加断言里的 `printf`（可经 `LYDSS_PRINTF` 替换或去掉）。**新增库函数调用前先确认 MicroLIB 提供它**；C23 新进标准库的 `strdup`、`<stdckdint.h>`、`unreachable()` 不能用。
- **长度、下标、成员大小一律用 `size_t`**，它是标准规定的对象大小类型。曾经一半容器用 `unsigned long`，已统一。
- **函数无参数写 `(void)`**，不写 `()`：后者在 C11 是过时的无原型声明，C23 已删除。库与用例目前都已遵守。
- CMake 固定 `C_STANDARD 17` 并关闭扩展（`-std=c17`，不是 `gnu17`），误用 GNU 扩展会直接编译失败。全部源文件另在 `gcc -std=c11 -pedantic-errors` 下零诊断；用 clang 以 `-std=c11 -pedantic-errors`、只声明 MicroLIB 也有的库函数的桩头文件，对 Cortex-M0/M3/M4F、RV32、AArch64、x86_64 Linux/Windows 做过语法检查，全部零诊断。保持这些状态。

## 外部规则（先读，不在本文件重复）

以下规则由 `.agents/` 下的子模块提供，是各自领域的唯一判定基准。**本文件只记录这些规则覆盖不到的、项目特有的决定与状态。**

| 领域 | 读这里 |
|---|---|
| C 代码风格（缩进、大括号、命名、缩写、文件与函数顺序、注释语法） | [`.agents/skills/c-code-review/references/c-style.md`](.agents/skills/c-code-review/references/c-style.md)，审查流程见同目录 `SKILL.md`，格式化配置见 `assets/.clang-format` |
| 注释内容（写什么、不写什么、精简程度） | [`.agents/rules/code-comment.md`](.agents/rules/code-comment.md) |
| Git 配置、子模块、行尾与 `.gitattributes` | [`.agents/skills/git-doctor/SKILL.md`](.agents/skills/git-doctor/SKILL.md) |
| `.editorconfig` | [`.agents/skills/editorconfig-doctor/SKILL.md`](.agents/skills/editorconfig-doctor/SKILL.md) |

分工：`c-style.md` 决定注释的**语法与位置**，code-comment 决定**能写什么内容**；重叠处语法从 `c-style.md`。规则之间冲突时，按各 skill 自身的优先级条款处理，不要自行发明裁决。

改完代码后用 `c-code-review` skill 审一遍改动的文件。若结论与本文件「项目特有的风格决定」冲突，以本文件为准；若你新引入一处有意的偏离，当场补记到那一节。

## Project Shape

- `Ly` 是作者 Luyi（Luyi365）的缩写，**不是 "Lyi"**；`LyDSS` = Luyi DataStore Struct。拼写恒为 `LyDSS`，`LyDDS` 是已清除的历史笔误，不要重新引入。
- 每个容器在 `src/` 下编译成一个独立静态库：array、bucket、list、pingpong、pool、queue、set、stack、stream、table、vector。**十一个库互不链接。**
- 公开声明集中在 [`inc/lydss_api.h`](inc/lydss_api.h)；配置、断言与功能开关在 [`inc/lydss_cfg.h`](inc/lydss_cfg.h)；容器间共用的内部底座在 [`inc/lydss_priv.h`](inc/lydss_priv.h)。
- 实现放在对应的 `src/ly<名称>.c`，用例放在 `test/<名称>_basic.c`。
- 命名：函数 `ly<容器>_<操作>`，类型 `Ly<Container>_t`（`lytable` 是遗留的全小写例外）。断言与配置宏前缀 `LYDSS_`。
- 源文件用相对路径包含公开头文件（`#include "../inc/lydss_api.h"`），CMake 没有配置任何 include 目录。

## Build And Test

**`CMakePresets.json` 不存在且被 gitignore**，所以 `cmake --preset GCC` 之类的命令一定失败。用显式的生成器与编译器：

```powershell
cmake -S . -B out/build/verify -G Ninja -DCMAKE_C_COMPILER=gcc -DCMAKE_C_FLAGS="-Wall -Wextra"
cmake --build out/build/verify
ctest --test-dir out/build/verify --output-on-failure
```

`out/` 被 gitignore，可作为临时目录，用完清理。改完代码必须在 `-Wall -Wextra` 下验证；`src/` 目前零告警，保持这个状态。

本机工具链：

- `gcc` 13.2 位于 `D:\Program Files\mingw-w64-gcc-13.2-stable-r40\bin`，目标是 **i686-w64-mingw32（32 位）**，`size_t` 为 4 字节。句柄尺寸是目标相关的，换 ABI 要重新测量而不是照抄。用例打印 `size_t` 用 `%zu`；在严格 `-std=c17` 下 MinGW 会切到符合标准的 printf，`%zu` 输出正确。
- **本机 gcc 13.2 不认识 `-std=c23`**，只有预览版 `-std=c2x`。实测 C23 的语言特性它都支持（`static_assert` 关键字、`bool`/`nullptr`/`constexpr`/`typeof`/`alignof` 关键字、`[[nodiscard]]`、`enum : uint8_t`、空初始化器 `{}`、标准 `strdup`），但**缺两项库设施：`<stdckdint.h>` 与 `unreachable()`**，两者从 GCC 14 起提供。本机 clang 20.1 以 `-std=c23` 全部支持。
- `clang` 与 `ninja` 也在 PATH 上。[`.vscode/tasks.json`](.vscode/tasks.json) 引用的 `C:\Program Files\mingw64\bin\gcc.exe` **不存在**，该任务已失效。
- Windows 上偶发一次无诊断的链接失败（文件锁，常见于 `libLyQueue.a`）。**先重跑一次构建再排查**，第二次通常就过。

CTest 跑两个可执行目标：`LyDSS_SimpleTest` 与 `LyDSS_AllBasicTest`，分别由 [`examples/run_simple_test.c`](examples/run_simple_test.c) 和 [`examples/run_all_basic_test.c`](examples/run_all_basic_test.c) 驱动。两个运行器都以 `status |=` 累积返回值，任一用例返回非 0 都会让 CTest 失败。`LyDSS_AllBasicTest` 通过 `file(GLOB_RECURSE SOURCES_TEST test/*.c)` 编译 `test/` 下**所有**文件，但只有运行器显式调用的函数才会执行。

## Feature Gating

每个容器都能单独裁剪。[`inc/lydss_cfg.h`](inc/lydss_cfg.h) **按模块分节**（不按功能分节），排列顺序与 `lydss_api.h` 的分节一致；每节放该模块的全部配置：`LYDSS_<模块>_ENABLED`，以及适用时的 `LYDSS_<模块>_STATIC_ENABLED` 与模块专属参数（如流的 `LYDSS_STREAM_CELL_TYPE`）。新增配置项放进所属模块的节里，不要另开功能节。非 0 启用、`0` 裁剪。默认除桶外全为 1；**桶默认为 0**，验证时记得另跑一次 `-DLYDSS_BUCKET_ENABLED=1`，否则它根本不参与编译。

所有开关（含 `LYDSS_ASSERT_ENABLED`）都包在 `#ifndef` 里，因此命令行上的 `-DLYDSS_SET_ENABLED=0` 优先于头文件默认值。**这是配置本库的推荐方式**，不要为了改配置去编辑头文件。

一个容器的门控出现在**四处，必须同步**：

1. `inc/lydss_cfg.h` 的 `#ifndef`/`#define` 默认值。
2. `inc/lydss_api.h` 的声明块，紧贴 `/* ---- 名称 ---- */` 分隔线内侧。
3. `src/ly<名称>.c` 的实现。
4. `test/<名称>_basic.c` 的函数体，以及 `examples/run_all_basic_test.c` 的调用点。

只改其中一处会导致裁剪后编译或链接失败。新增容器时四处一起加，并至少验证「全开」和「关掉该容器」两种配置。

`.c` 文件先包含 `"../inc/lydss_api.h"`，再开 `#if`，标准库头文件放在保护区内——门控必须先可见才能判断，而这也正好符合 `c-style.md` 要求的 include 顺序。被裁剪的容器因此连 `<stdlib.h>` 都不引入。

**CMake 刻意不参与门控**：十一个库目标始终声明、始终链接，被关掉的容器仍然编译，只是产出一个没有函数的目标文件。想让目标本身消失需要在 CMake 里再加一套镜像这些宏的 option，那会把配置真相拆成两份，没有做。

唯一的例外是 `test/simple.c` 与 `examples/run_simple_test.c`，见 Testing Status。

## Internal Base Layer

[`inc/lydss_priv.h`](inc/lydss_priv.h) 存放容器间共用的底层操作。它与公开头文件同处 `inc/`（本项目只保留一个头文件目录），靠**文件名里的 `priv` 而非位置**标记私有。这在当前安全，仅因为 CMake 没有声明任何公开 include 目录。**若将来声明了，只能导出 `lydss_api.h` 与 `lydss_cfg.h`**，否则底座会静默变成对外承诺。

两条判据决定什么能进底座：

**有句柄和生命周期的是容器，不是原语。** 原语是无状态操作。所以 `LyArray` 不能当队列和栈的底座——尽管字段对得上，它有句柄、有 `is_static`、有自己的断言与边界检查，组合它等于嵌套生命周期并重复付一次校验。同理 `LyVector` 不是原语，只有它的增长策略可能是，而那策略是 vector 专属的，留在 `lyvector.c`。

**底座只能是头文件，全部 `static inline` 函数或宏。** 这保证：十一个库仍然互不链接，十一个开关可自由组合；没有跨翻译单元的调用需要内联（CMake 未开 LTO，跨库调用本就内联不了）；底座保留随时重构的自由。底座**不做参数断言**，调用方进入前已校验，重复校验是嵌入式热路径上的纯开销。

**不建 `lydss_priv.c`。** 共享 `.c` 要么编进每个库（应用同时链接两个容器时符号重复定义），要么单独成库（正是本设计要避免的跨库依赖）。由此得出一条规则：**某段共用逻辑大到不适合内联时，说明这两个容器本该合并，而不是需要一个共享 `.c`。**

### `LyStore_t`

定长连续存储的描述符（`buffer` / `length` / `item_size`），由 array、vector、stack、queue **按值嵌入**，成员名统一为 `store`；pingpong 嵌入两份，成员名为 `store[2]`。按值嵌入不产生间接层也不产生链接依赖——**结构体共用便宜，函数组合才贵**，不要混为一谈。

**刻意不含 `is_static`**：那是所有权标志而非存储描述，放进来会让永不支持静态构造的 vector 白占一个字节加对齐填充。排除之后四个容器的句柄尺寸与接入前逐字节相同。

vector 把 `capacity` 映射到 `store.length`，没有独立的容量字段。这不违反「vector 不得依赖 lyarray」的决定：共用的描述符不归任何容器所有，`realloc` 原地扩张也未受影响；倍增策略仍在 `lyvector.c`，只有「重分配并更新容量」下沉为 `lystore_realloc`。

**stream 与 pool 不接入。** stream 的单元固定为 `cell_t`，`item_size` 是纯浪费，且缓冲区与句柄一次分配、销毁路径不同；pool 的成员是定型数组，按下标直接访问。两者不接入是正确的，**不要为了对称硬塞**。

生命周期辅助（`lystore_bind` / `alloc` / `realloc` / `free` / `unbind` / `address`）收在描述符上，因为 `create`/`create_static`/`delete` 三件套在 array、stack、queue 里几乎逐字重复——那是真正的逻辑重复。

**一处刻意的行为改变**：`lystore_unbind` 清空全部三个字段，统一了原先 array 清 `length` 而 stack/queue 不清的不一致。副作用是对已销毁的静态容器再查询，`is_full()` 会返回 `true`、长度与成员大小返回 0。仅在未定义用法下可观察。

### 命名与使用约束

名字描述**操作什么**，不描述在哪一层——层由文件标识。`LyStore_t` / `lystore_*` 是存储描述符，`lyring_next` 是环形游标，`LYDLIST_*` / `LYSLIST_*` 是侵入式链表宏。曾经统一用 `lybase_` 前缀，逼出了 `LYBASE_DLIST_UNLINK` 这种冗余复合名，已废弃。

链表宏要求节点类型的自指针成员命名为 `prev` / `next`，`head` / `tail` 须为左值，且**参数会被多次求值，不要传带副作用的表达式**。`LyListNode` 是公开类型，布局不能改成嵌入基节点，所以用宏操作命名字段（对比 BSD `sys/queue.h`），避免破坏性 API 变更。

底座改动若涉及接链或生命周期，**不要只依赖 ctest**（覆盖面有限）。用 `git worktree` 取出上一个良好提交，对同一份探针源码分别编译两版并 diff 输出——`gcc -I<tree>/src -I<tree>/inc` 即可。

## Container Contracts

| 容器 | 成员模型 | 所有权 | 容量 | 静态构造 |
|---|---|---|---|---|
| array | 定长值拷贝，`length` 个槽位自始存在 | 拷贝值 | 创建时固定 | 有 |
| vector | 定长值拷贝，`[0, size)` 有效 | 拷贝值 | 2 倍增长，`realloc` | 无（增长依赖 realloc） |
| pingpong | 定长值拷贝，两块等长缓冲轮换 | 拷贝值 | 固定，两块等长 | 有 |
| queue | 定长值拷贝，环形 FIFO | 拷贝值 | 固定 | 有 |
| stack | 定长值拷贝，LIFO | 拷贝值 | 固定 | 有 |
| stream | `cell_t` 单元（默认 `unsigned char`，由 `LYDSS_STREAM_CELL_TYPE` 配置），环形 FIFO | 拷贝单元 | 固定 | 有 |
| set | 定长值拷贝，链式哈希 | 拷贝值，比较回调由调用者提供 | 槽位固定，不 rehash | 无 |
| table | 拷贝键字符串，存值指针 | **不释放值** | 负载 0.75 时增长 | 无 |
| pool | 存 `name` 与对象指针，线性扫描 | 两者都不拥有 | 固定，满时 `post` 返回 `false` | 有 |
| bucket | 存 `name` 与项指针，外加拷贝的 `LyBucketMeta_t` | 只拥有元数据副本 | 按节点增长，无上限 | 无 |
| list | 每节点存 `void *data` | 拥有节点，不拥有数据 | 无界 | 无 |

四处边界约定的差异要留意：

- **array** 索引对 `length` 判定，每个槽位自始可读写，没有「已用数量」概念。`lyarray_create` 用 `calloc` 清零，`lyarray_create_static` 刻意不动调用者内存。
- **vector** 索引对 `size` 判定，`size` 之外的空闲容量不可寻址也不初始化。
- **stream** 的 `in`/`out` 返回实际传输的单元数，可以少于请求量；**queue** 的 `send`/`receive` 是全有或全无并返回 `bool`。这个差异是刻意的。
- **pingpong** 不提供按下标访问，只按块交付：写入侧对填充块、读取侧对就绪块，两侧各有独立计数。`lypingpong_swap` 在就绪块未 `release` 时返回 `false`，这是防止覆盖未消费数据的唯一屏障，不要当成冗余检查删掉。

## Architecture Decisions

已经定过的事，没有新信息不要重新讨论。

**`lyarray` 是定长存储原语，不是序列容器。** 它的用途是给 C 原生数组补上句柄携带的长度与成员大小。它刻意不跟踪已用数量，也没有 `push`/`pop`/`insert`/`remove`——那是上层的序列语义。

**`lyarray` 原本打算做 queue 和 stack 的存储基座**，字段层面的证据也支持（`LyStack` = `LyArray` + `top`，`LyQueue` = `LyArray` + 双游标 + 计数）。但**替换未执行且被明确推迟**，不要擅自把 queue/stack 改接到 array 上。

**`lyvector` 刻意独立，不得依赖 `lyarray`。** 早期版本曾把 vector 组合在 array 之上，作者要求它独立。代价是增长逻辑重复一份，换来 `realloc` 的原地扩张能力。

**`lystream` 与 `lyqueue` 必须分开。** 两者的结构与游标运算几乎相同，因为都建立在环形缓冲上（业界常见的共享底座，参见 Linux `kfifo`），但处在不同抽象层级：队列是装类型化定长成员的容器，流是搬运无类型字节的通道。部分传输契约是流侧的签名特征。曾有人提议给 queue 加 `send_multi`/`receive_multi` 并废弃 stream，该提议**已被判定为错误并撤回**。若继续发展 stream，方向是向上——source/sink 回调以便包装 UART 或文件，加上 EOF 与错误状态——而不是增加缓冲功能。

**`lypingpong` 必须建在定长存储上，不能建在 `lyvector` 上。** 乒乓的主场景是把缓冲基址预先配置给 DMA，而 `realloc` 随时可能迁移缓冲区——这不是性能退化而是内存损坏。另外两块缓冲必须等长，而「满」在乒乓语义里是正常节拍而非扩容信号，若满了就扩容则永远不会发生交换，结构失去意义。它嵌入两份 `LyStore_t`，与其他容器一样零库间依赖。

**`lypingpong` 当前是单上下文实现，不含任何同步措施。** 它真正的价值在跨执行上下文（中断或 DMA 写、主循环读），那需要 `volatile` 字段与临界区，而本库十一个容器都是单线程假设。按作者决定先与其他容器保持一致，并发留给调用者保护。要补并发支持时，`swap` 与 `release` 是需要保护的两个点，可能还要在 `lydss_cfg.h` 里加关中断钩子。

**`lygroup`（只追加的批量抽象）曾被建成又移除。** 它的结构与 `LyVector` 逐字段相同，两个静态辅助函数也是 vector 的字面复制品。若还想要批量抽象，注意它相对 vector 唯一不可替代的价值是类型层面的限制（没有 `set`/`remove`、`const` 返回、无法区分成员的回调），而不是存储行为。

**静态构造采用 FreeRTOS `StaticQueue_t` 的做法。** 句柄是不完整类型，库外写不出 `LyStack_t s;`，所以每个支持静态构造的容器在公开头文件里另有一个不透明存储类型 `Ly<容器>Static_t { void *opaque[N]; }`，`create_static` 接收它并返回句柄（就是这块存储本身）。要点：

- **不用 `ly<容器>_sizeof()`。** 运行期返回的大小没法用来声明 `static` 存储，无堆用户恰恰需要编译期常量；能 `malloc` 的话直接用 `create` 即可。曾经按这个方向提议过，已判定为错误。
- **`N` 按句柄的成员数取指针字数**，`bool`/`uint8_t` 合计算一个字（当前 array 4、stack 5、queue 7、stream 6、pool 3、pingpong 9）。这在 ILP32、LP64、LLP64 上都成立，因为 `size_t` 不宽于指针。**给句柄结构体加成员时必须同步加大 `N`**，否则 `.c` 里的检查会让编译失败——这是有意的，不要为了过编译去删检查。
- 每个支持静态构造的 `.c` 紧跟结构体定义有一处 C11 `_Static_assert`，同时检查**尺寸**（`sizeof`）与**对齐**（`_Alignof`）。对齐那半是必要的：把存储类型写成 `unsigned char opaque[...]` 尺寸照样够，却会因对齐不足编译失败，这已实测确认。曾经用过负长度数组的 C99 写法 `LYDSS_STATIC_ASSERT`，已按开发要求删除。
- **存储类型与真实结构体之间的指针转换在 C 标准意义上碰到了严格别名规则**，`-Wstrict-aliasing=1` 会提示，默认告警级别下不提示。它安全的前提是库从不通过 `opaque` 访问这块内存、调用者也不访问；FreeRTOS 依赖同样的前提。不要试图用 `memcpy` 或联合体"修正"它——那会改变公开 API，而不会让行为更安全。
- **`LyPoolItem_t` 因此改为公开布局**：静态池的槽位数组要由调用者声明为 `LyPoolItem_t items[N]`，不完整类型做不到。它只有两个指针成员，公开的代价很小。
- 六个 `create_static` 的签名由此从 `void f(Ly<容器>_t *, ...)` 改为 `Ly<容器>_t *f(Ly<容器>Static_t *, ...)`。改前这些函数从库外根本调不到，所以没有调用者被破坏。

**按核心宗旨删除过的接口，不要加回：**

- **`lylist_swap`。** 场景少且已确认会破坏链表：它不接收 `list`，交换头尾节点后 `head`/`tail` 失效；交换相邻节点会形成自环。修好它需要改签名，不如删。
- **`lyset_relation` 与 `LySetRelation` 枚举。** O(n·m)，要维护五态枚举；需要的调用者用 `lyset_contains` 两层循环在库外即可实现。
- **`lypingpong_is_ready`。** 与 `lypingpong_ready_count() > 0` 完全等价。

## API And Ownership

- 沿用各容器既有的 create/init 与 delete/destroy 生命周期。
- 静态构造的用法与约束见 Architecture Decisions 里 FreeRTOS 做法一条。`test/stack_basic.c` 用静态构造创建第一个栈，是 CTest 里唯一覆盖这条路径的用例，保留它。
- 支持静态构造的容器带 `is_static` 标志，其 `delete` 只解绑而不释放调用者内存。适用于 array、queue、stack、stream、pool、pingpong，改动清理路径时要保留这个性质。
- **静态构造按容器各自受 `LYDSS_<容器>_STATIC_ENABLED` 控制**（默认 1，池、数组、乒乓缓冲、栈、队列、流各一个）。曾做成全局的 `LYDSS_STATIC_ENABLED`，作者要求改为逐容器设置，不要改回。关闭后头文件不再声明该容器的 `Ly<容器>Static_t` 与 `create_static`，句柄去掉 `is_static`、`delete` 去掉静态分支。每个 `.c` 里有五处同一开关：`is_static` 成员、`_Static_assert`、`create` 里的 `is_static = false`、整个 `create_static`、`delete` 的静态分支；改动静态构造时五处一起看。关闭后句柄在 i686 上各省 4 字节，乒乓缓冲除外（`is_static` 原本与 `fill` 共用一个对齐字）。
- **`cell_t` 由 `LYDSS_STREAM_CELL_TYPE` 决定**，默认 `unsigned char`。实现只对单元做赋值与 `sizeof`，所以任意可赋值的完整类型都可用，结构体也已实测可行。`test/stream_basic.c` 逐字符转换填充数据，所以在整数单元类型下都能编译；单元换成结构体时这个用例编译不过，属预期。
- `lylist_destroy` 释放节点但**不释放 `list` 句柄本身**，即使 `lylist_create` 用了 malloc。这是刻意的：`struct LyList` 是公开类型，调用者可能栈分配它。堆上创建的句柄按设计由调用者自己 `free`。
- list、pool、table、bucket 保留外部 `void *` 指针，不要假定拥有调用者的业务对象。table 拷贝键但不释放值。
- set 的**哈希可为 NULL**（回退到 `set_hash_default`，按字节 FNV-1a，与 `lytable_create(NULL)` 一致），但**比较回调必须提供且无法有默认实现**：`Equals_t` 是 `bool (const void *a, const void *b)`，不带长度参数，库无从知道该比较多少字节。要修得改 `Equals_t` 签名，属破坏性 API 变更，未做。
- 断言默认开启。`LYDSS_ASSERT` 失败会进入陷阱且不再返回，**不要指望非法输入会得到正常的错误返回**。公开函数一般对空句柄断言，而对下标越界用 `false`/`NULL` 返回。
- 断言失败后在死循环里反复执行 `LYDSS_TRAP()`，它在 `lydss_cfg.h` 里以 `#ifndef` 定义、**默认为空**。默认值必须保持架构中立：曾经硬编码的 `__asm("int3")` 只有 x86 认识，ARM/RISC-V 编译不过。想停进调试器由构建系统覆盖，如 `-D'LYDSS_TRAP()=__builtin_trap()'`，不要在库里按架构加 `#if` 分支。
- 断言的输出走 `LYDSS_PRINTF`，同样以 `#ifndef` 定义在 `lydss_cfg.h`，默认是 `printf`，**`<stdio.h>` 只在采用默认值时才包含**。MicroLIB 下 `printf` 需要工程重定向 `fputc`；把它定义为空（`-D'LYDSS_PRINTF(...)='`），stdio 不再被引入，各断言的表达式字符串也一并从目标文件消失（实测 `lyarray.o` 从 2961 降到 1368 字节）。断言本身和陷阱仍然生效。
- 分配前对 `成员数量 × 成员大小` 的乘法溢出统一由 `lystore_bytes` 拦截（以 `SIZE_MAX` 为界）。`realloc` 失败时原缓冲区保持不变、句柄仍可用，这两个性质都要保留。

## Testing Status

十一个容器都有接入并实际执行的用例（桶默认关闭，打开开关后才执行）。`LyDSS_AllBasicTest` 按容器规范顺序调用，每个都在自己的开关后面，并在调用前打印一行横幅说明将要展示的特性。`LyDSS_SimpleTest` 单独跑 `simple()`。

每个用例刻意保持很小——创建、写入、读取、销毁，外加**一处体现该容器特性**的调用。它们仍是**打印式冒烟测试**：通过只意味着「没崩且返回 0」，不代表输出被校验过。

**不要把它们扩成穷举式的 API 覆盖**，也不要逐个函数讲解——曾经那样做过，作者要求退回。每个函数体保持 10–20 行。

**打印用中文，且只看输出就能复原运行过程**：每行写明做了什么、输入是什么、得到什么，如 `再入队 5：失败，队列已满`。不打印裸数值或 `key = 1` 式的代号；布尔结果译成「成功 / 失败，原因」「找到 / 未找到」；不加解释性废话。

`test/simple.c` 与 `examples/run_simple_test.c` 是作者的草稿本，随时重写，**刻意不加任何门控**。后果：`simple.c` 当前用到的那个容器必须保持启用，否则两个可执行目标都编译失败（`simple.c` 既被 `LyDSS_SimpleTest` 显式列出，又通过 `test/*.c` 的 glob 进入 `LyDSS_AllBasicTest`）。这是「全部关闭」配置唯一构建不过的原因，其余开关组合都正常。**不要用加回门控的方式"修"它。**

写打印式用例时，**不要在同一个 `printf` 参数表里既调用改变状态的函数、又读取被它改变的值**。GCC 从右往左求值，会读到旧值或让前一个调用失效。拆成独立语句先存进局部变量。这个错误在本仓库历史上犯过三次。

## 项目特有的风格决定

以下是 `c-style.md` 未覆盖、或本项目刻意偏离的部分。**`c-style.md` 本身已有的规则不在此重复。**

**容器顺序固定为：池、数组、动态数组、乒乓缓冲、栈、队列、链表、流、表、集合、桶。** `inc/lydss_api.h` 的分节、`examples/run_all_basic_test.c` 的调用、`CMakeLists.txt` 的库声明与链接列表都按这个顺序，便于对照阅读。顺序是从最简单的存储原语向外排到复合容器，新容器按抽象层级插入而非追加到末尾。`c-style.md` 的顺序一节只管文件内部的元素排列，不覆盖容器之间的顺序，所以这是本地约定。

**文件私有的静态辅助函数不带前导下划线**，命名为 `<容器>_<动词>_<名词>`（公开函数结构去掉 `ly` 前缀）。当前全部：`lyvector.c` 的 `vector_reserve_capacity`/`vector_resize_buffer`、`lybucket.c` 的 `bucket_dup_memory`、`lyset.c` 的 `set_hash_default`、`lytable.c` 的 `table_hash_default`/`table_resize_slots`；其余源文件没有静态辅助函数。

`c-style.md` 把 `_id_do_thing_sub()` 留给句柄注册接口函数（元方法），`_id_do_register()` 留给寄存器操作。普通文件私有辅助函数两个角色都不是，**不得借用这个标记**。早期代码曾用 `_memdup` 之类的命名，那是错的，已全部改正。

**以下三处刻意偏离 `c-style.md`，不要"修正"：**

- **局部变量用完整单词而非首字母缩写。** 规范把局部变量列在首字母缩写场景，但代码写 `array`、`vector`、`new_len`、`hash`，可读性优先。`idx` 与 `cur` 是实际在用的截断形式。新代码跟随周围文件。
- **查询函数省略动词。** 规范的结构是 标识 + 动词 + 名词（即 `lyarray_get_length`），而 API 提供的是 `lyarray_length`、`lyvector_size`、`lyset_count`。改名是破坏性 API 变更且无行为收益，新增访问器沿用这个无动词形式，不要引入第二种风格。
- **枚举成员名在前缀之后全大写**（`LyBucketMetaType_NULL`），而规范示例是帕斯卡。保持，新增枚举照此书写。

`/* ---- 名称 ---- */` 分节线保留。它早于 code-comment 规则存在，且文件顺序约定依赖它——按该规则自身的优先级 1（项目既有强制规范优先）裁决。`src/lypool.c` 的作者横幅与各文件的 MIT 许可文本是作者本人内容，不要删改。

**注意 `-Wcomment`**：块注释内出现 `/*` 序列会告警。在 `/** */` 里写 `test/*.c` 这样的通配符会破坏零告警状态。

## Working Agreement

- **本文件是项目规则的唯一来源**（`.agents/` 覆盖的领域除外）。没有 spec 目录：单人探索型项目，作者选择一份持久文档而不是按特性拆分的需求/设计/任务产物。不要重建 `.kiro/specs/` 或引用它。
- 开发是对话式、增量的。会话中产生新的规则、约束或决定时，**当场追加到本文件**，不要推迟，也不要塞进一次性的计划文档。
- **记录决定时连同理由一起记。** Architecture Decisions 一节存在的原因就是这些问题已经定过一次；没有理由它们会被重新翻出来。
- `out/` 与 `build/` 属于构建工具链，只做输出目录——**绝不要往里面写源文件、探针或临时代码**。需要持久保留的探针源码放进仓库正式位置（构建期检查放 `cmake/`，运行期用例放 `test/`）。
- `.cache/clangd/` 是 clangd 的符号索引，自动生成、可安全删除，不属于项目。
- `.kiro/` 下有 skill 的第二份副本，被 gitignore，属工具而非项目内容，不要动。

## Known Duplication（已审计，未处理）

一次针对结构体布局与核心抽象的审计发现以下重复。除移除 `lygroup` 外都未处理。

- **pool、bucket、table 是同一件事（名字到指针的映射）的三种实现。** `lypool_post`/`get`/`clear`、`lybucket_put`/`get`/`erase`、`lytable_set`/`get`/`remove` 是同一套操作。pool 最弱：线性扫描、容量固定；它的位置是「无堆的名字注册表」——静态构造可用之后这个位置成立，所以保留。bucket 曾按核心宗旨被提议删除，作者决定保留。
- **set 与 table 都是链式哈希表**，布局同构（槽位数组 + 单链节点）。各有对方缺的能力：set 借回调泛型化但从不 rehash，table 只支持字符串键但会在负载 0.75 时增长。
- **set 与 table 的 `size` 含义相反。** `LySet` 里 `size` 是槽位数、`count` 是元素数；`lytable` 里 `capacity` 是槽位数、`size` 是元素数。读或合并两者时极易出错。
- **`lybucket.c` 的文件头注释声称核心是「list + table」，但代码里没有任何哈希。** `lybucket_get` 沿双向链表做 `strcmp`。它目前是个命名链表，注释描述的是未实现的设计。

## Verified Defects（未解决）

每条都经过读码确认。**行号会漂移，编辑前先重新检索**；定位优先用函数名。

### 嵌入式移植阻塞项（均已修复）

记在这里以免被改回：

- 陷阱动作已改为可覆盖的 `LYDSS_TRAP()`，断言输出已改为可覆盖的 `LYDSS_PRINTF`（见 API And Ownership）。
- `lytable` 的负载判定已改为整数形式 `size >= capacity - capacity / 4`。**不要改写成 `size * 4 >= capacity * 3`**：32 位上乘积溢出、条件永假，表会静默停止扩容。槽位数从 16 起倍增，恒为 4 的倍数，所以除法形式精确等于 3/4。
- `lytable` 扩容前检查 `capacity <= SIZE_MAX / 2`，再翻倍会溢出时停止扩容，只让冲突链变长。不加这个检查，回绕后的槽位数会让取模除零。实际需要 2^31 个槽位才触发，所以只做了读码确认，没有运行期用例。

### 参数校验缺失

- **`lyqueue_create` 与 `lystack_create` 断言了 `item_size` 但从不断言 `len`**，而 `lyarray_create`/`create_static` 断言了——三者不一致，array 是对的。后果并非除零（`send` 在 `count >= length` 时提前返回，取模不可达），而是更安静：零长度的队列或栈会构造成功，然后永久同时为空且为满，所有 `send`/`push`/`receive`/`pop` 都返回 `false`，`is_empty()` 与 `is_full()` 同时为 `true`，没有任何东西报告参数有问题。
- **`lyset_init` 断言了 `elem_size` 但没有校验槽位数——这一条是真的除零。** 与队列和栈不同，`lyset_add`/`remove`/`contains` 计算 `% set->size` 之前没有任何守卫，`lyset_init(0, ...)` 能成功，第一次操作就是未定义行为。
- **`lypool_create` 对 `size` 什么都不校验。** 同类漏洞：零容量的池创建成功、不接受任何对象，`lypool_post` 每次都返回 `false`。

### 所有权与正确性

- **`lybucket_patch_meta` 先释放再分配。** 先 `free` 旧的 `meta.data` 再调用 `bucket_dup_memory`，分配失败时旧元数据已经丢了。应当先分配成功再释放旧的。
- **`bucket_dup_memory` 对合法输入触发断言。** 它断言 `size` 非零，但 `put`/`post`/`insert` 在 `meta.type != NULL` 而 `meta.size == 0` 时就会走到这里。零长度的拷贝应当被视作「无元数据」，而不是编程错误。
- **`LYLIST_FOREACH` 没有安全变体。** 在循环体内删除当前节点是未定义行为，因为宏在循环体执行完之后才读 `node->next`。常规修法是提供一个先缓存后继再执行循环体的变体。
- **`lypool` 对重名有三种互不一致的处理。** `lypool_post` 填第一个空槽而不查重，`lypool_get` 返回第一个匹配，`lypool_clear` 清掉**所有**匹配（循环没有提前退出）。无论意图是哪种，这三者应当一致。

### API 一致性

- **`lyset_init` 违反了其他容器都遵守的 `_create` 命名惯例。**
- **`cell_t`、`Hash_t`、`Equals_t` 位于全局命名空间且没有 `Ly` 前缀。** `cell_t` 尤其容易与应用代码撞名。`LyTableHash_t` 是同类回调 typedef 的正确写法。此外 `Hash_t` 与 `Equals_t` 被 typedef 成函数**类型**而非函数指针，所以每次使用都要写 `Hash_t *hash`。
- **`lystream_clean` 的拼写与 queue/stack/vector/pool 用的 `clear` 不一致。**
- **`lytable_set` 返回 `int`（0 / -1）**，而库内其余部分用 `bool` 或 `NULL` 表示失败。两种形式都被 `c-style.md` 认可（逻辑返回值 vs 错误处理返回值），所以这是待统一的一致性选择，不是风格违规。定下一种并记在这里。

## Known Repository Limitations

- `CMakePresets.json` 缺失且被 gitignore，基于 preset 的构建说明无法照着执行。
- CMake 没有声明公开的 include 目录，源文件靠相对路径找头文件。**外部消费者无法 `#include <lydss_api.h>`。** 若要修，必须只导出 `lydss_api.h` 与 `lydss_cfg.h`（见 Internal Base Layer）。
- **根目录没有 `.clang-format`**，所以格式并未被自动强制。唯一的副本在 skill 的 `assets/` 里。复制一份到根目录即可让 clang-format 自动生效；在那之前缩进与大括号靠手工维持。`.editorconfig` 与 `.gitattributes` 已在根目录就位。
- `examples/RedPandaIDE/RedPandaIDE.layout` 等是 Red Panda 的编辑器缓存，仍带着另一台机器的绝对路径。它们被跟踪是因为 `.gitignore` 的对应规则指向 `dev/RedPandaIDE/*`，而文件实际在 `examples/RedPandaIDE/`。修正忽略路径即可让这些缓存脱离版本控制。
- 不要把 `examples/RedPandaIDE/makefile.win` 当作主构建入口。它靠手工同步，且只列了一部分源文件。
- [`README.md`](README.md) 已不是占位文件。它逐个说明容器的适用场景与特征，并有一张 19 个特征列的对比表（外加容器名列）加 15 条脚注，每个格子都是读实现得出的，脚注刻意记录了影响选型的已知缺陷。**它还没有克隆与编译章节**，作者暂时搁置。行为变化时**当场同步 README 的表**——错的表比没有表更有害。精确签名仍以头文件和实现为准。
