# Build QuantLibAddin from a git clone using cmake

This document explains how to build QuantLibAddin - the QuantLib wrapper layer
(`QuantLibObjects`), its C++ add-in (`AddinCpp`) and the C++ demo client
(`QLADemo`) - from a **git clone**, using the **cmake** build, **on its own** as
a standalone subproject. It is the cmake counterpart of [build_vs.md](build_vs.md)
(the hand-maintained Visual Studio solution build); both use the same source
files and produce the same libraries.

A standalone QuantLibAddin build consumes **prebuilt** QuantLib and ObjectHandler
libraries (it does not rebuild them); you build those two prerequisites first,
then point this build at them. This mirrors the project dependency chain
`gensrc <- ObjectHandler <- QuantLibAddin <- QuantLibXL`.

There are two ways to build the cmake projects in this repository:

1. **Each subproject on its own** (this document) - build QuantLib and
   ObjectHandler first, then QuantLibAddin against them. Use this when you want
   to build or test just the QuantLibAddin layer (or its C++ client).
2. **The whole stack from the repository root** - a single configure builds
   QuantLib, ObjectHandler, QuantLibAddin and QuantLibXL together. See the root
   [../../build_cmake.md](../../build_cmake.md). If your goal is
   the QuantLibXL Excel add-in, follow
   [../../QuantLibXL/md/build_cmake.md](../../QuantLibXL/md/build_cmake.md).

For other options:

- To download a compiled QuantLibXL XLL, see
  <https://www.quantlib.org/quantlibxl/installation.html>.
- To compile various flavors of QuantLibAddin (including the QuantLibXL XLL) from
  an official release of source code (zip files / tarballs), see
  <https://www.quantlib.org/quantlibaddin/tutorials.html>.

---

## 1 What gets built

A standalone cmake build produces two **static libraries** and one
**executable**:

| Target | Kind | Description |
|---|---|---|
| `QuantLibObjects` | static library | the QuantLib/ObjectHandler wrapper layer |
| `AddinCpp` | static library | the C++ add-in layer (links `QuantLibObjects`) |
| `QLADemo` | executable | the C++ demo client (links `AddinCpp`) |

Building `QLADemo` pulls in everything it depends on - `AddinCpp`,
`QuantLibObjects`, and the imported QuantLib and ObjectHandler libraries - in
dependency order, so it is the simplest single target to build.

QuantLibAddin depends on **Boost** and, unlike ObjectHandler, on two other
projects in this repository: **QuantLib** and **ObjectHandler**. A standalone
QuantLibAddin build does **not** rebuild those two; it consumes them as
**prebuilt** libraries located through cmake's `find_package` (section 3). You
therefore build QuantLib and ObjectHandler first (section 5), then configure
QuantLibAddin to find them (section 6).

---

## 2 Choose a build - Basic or Full

Both builds produce the same libraries; they differ only in whether the
auto-generated source code is regenerated. The choice is a single cmake cache
variable, `RUN_GENSRC`, set at configure time.

| Build | Configure with | Description |
|---|---|---|
| Basic | (default - `RUN_GENSRC` is `OFF`) | Compiles the auto-generated source files that are already present in the clone. Does **not** require Python. Use this for normal compilation. |
| Full  | `-DRUN_GENSRC=ON` | Runs gensrc first to (re)generate all auto-generated source files, then compiles. Requires Python 3. Use this on a fresh clone, or after you have changed the gensrc metadata (the XML files under `QuantLibAddin\gensrc\metadata`). |

On a **fresh clone the first build must be a Full build** (`-DRUN_GENSRC=ON`) so
that the generated `qlo` sources (under `qlo\valueobjects`, `qlo\serialization`
and `qlo\enumerations\register`) and the `Addins\Cpp` sources are created.
Otherwise cmake fails at configure time with `Cannot find source file` for one
of those `.cpp` files (see section 9). After one Full build the generated files
exist, and subsequent Basic builds compile them without needing Python.

> The standalone QuantLibAddin Full build regenerates only the QuantLibAddin
> (`qlo` / `Addins\Cpp`) sources - the C++ add-in targets `-pvels`. It does
> **not** regenerate the ObjectHandler sources (that is ObjectHandler's own Full
> build, see [../../ObjectHandler/md/build_cmake.md](../../ObjectHandler/md/build_cmake.md))
> and it does **not** generate the Excel (`qlxl`) sources, which belong to the
> downstream QuantLibXL project.

---

## 3 How the prerequisites are located

A standalone QuantLibAddin build imports the prebuilt QuantLib and ObjectHandler
libraries using cmake package configs that each prerequisite writes into **its
own build tree** (no install step is required):

- QuantLib's build tree contains `cmake\QuantLibConfig.cmake`, providing the
  imported target `QuantLib::QuantLib`.
- ObjectHandler's build tree contains `cmake\ObjectHandlerConfig.cmake`,
  providing the imported target `ObjectHandler::ohlib` (written only when
  ObjectHandler is configured **standalone**).

QuantLibAddin's `CMakeLists.txt` calls `find_package(QuantLib CONFIG)` and
`find_package(ObjectHandler CONFIG)`. You tell cmake where those build trees are
with **`CMAKE_PREFIX_PATH`** (a list of the two build directories) at configure
time - section 6 shows the exact command.

> **The prerequisites must match this build.** QuantLib, ObjectHandler and
> QuantLibAddin are separate build trees, so their static libraries only link
> together if all three were built with the **same** Visual Studio toolset, the
> **same** CRT (static `/MT` vs dynamic `/MD`) and the **same** configuration
> (Release vs Debug). Use the same preset name for all three. Mixing them
> produces `LNK2038` runtime-mismatch errors at the link stage (see section 9).

---

## 4 Prerequisites

### 4.1 CMake and a C++ compiler

- **CMake 3.15 or later.** CMake ships with the **"C++ CMake tools for
  Windows"** component of the Visual Studio **"Desktop development with C++"**
  workload, so if you installed that workload you already have it. Check with
  `cmake --version`.
- **Visual Studio 2026 or 2022** with the **"Desktop development with C++"**
  workload. The cmake build is **x64 only**.

The platform toolset is selected automatically by the generator named in the
preset you choose (section 6); no manual toolset configuration is required:

| Visual Studio | Generator (from the preset) | Platform toolset |
|---|---|---|
| VS 2022 (v17) | `Visual Studio 17 2022` | v143 |
| VS 2026 (v18) | `Visual Studio 18 2026` | v145 |

### 4.2 Boost

QuantLib, ObjectHandler and QuantLibAddin all depend on Boost. You need the
**compiled** Boost libraries, not just the headers (the build links the Boost
`filesystem` and `serialization` components). Building Boost is outside the scope
of this document; these instructions assume you already have a Boost build
available.

The build requires **Boost 1.58 or later**. It was tested using **Boost 1.83**.

For purposes of this HOWTO, it is assumed that you have installed Boost to:

```
C:\repos\boost_1_83_0
```

with headers under `C:\repos\boost_1_83_0` (i.e. the folder that contains the
`boost\` sub-directory) and the compiled libraries under
`C:\repos\boost_1_83_0\stage\lib`. Modify that path as necessary for your own
environment.

Like the ObjectHandler cmake build, the standalone QuantLibAddin build has a
**single** Boost configuration point: you supply the include and library
directories once, in `CMakeUserPresets.json` (section 6.1). Use the **same**
Boost build (and the matching runtime variant) for QuantLib, ObjectHandler and
QuantLibAddin.

### 4.3 Python 3 (Full build only)

The **Full** build (`-DRUN_GENSRC=ON`) runs gensrc, which is a Python 3 script.
Install Python 3 and make sure `python` is on the `PATH` (i.e. `python
--version` works from a command prompt). See <https://www.python.org/>. When
`RUN_GENSRC` is on, cmake locates the interpreter itself via `find_package`.

The **Basic** build does not use Python or gensrc, so you can skip this section
for a Basic build.

---

## 5 Acquire and build the prerequisites

### 5.1 Clone the source code

`gensrc`, `ObjectHandler` and `QuantLibAddin` are all contained in this single
repository. Clone it:

```
git clone <repo-host>/<owner>/QuantLibAddIn.git
```

QuantLib is maintained as a **separate** repository and is deliberately excluded
from this one (it is listed in `.gitignore`), so clone it yourself into a
sub-folder named exactly `QuantLib` inside the working tree:

```
cd QuantLibAddin
git clone <repo-host>/<owner>/QuantLib QuantLib
```

The folder name `QuantLib` **is case sensitive**. The explicit `QuantLib`
argument at the end of the `git clone` command ensures the sources land in a
folder named exactly `QuantLib`, regardless of what the source repository is
named (some hosts or forks name it differently, e.g. `quantlib` or
`QuantLib-fork`).

After this step you have, side by side under the repository root:

```
QuantLibAddin\         (the repository root - the outer folder)
  gensrc\
  ObjectHandler\
  QuantLib\            (cloned separately, above)
  QuantLibAddin\       (the subproject this document builds)
```

### 5.2 Build QuantLib

Configure and build QuantLib into its own build tree, using the **same** toolset
and CRT you intend to use for QuantLibAddin. For a VS 2026 static-CRT Release
build:

```powershell
cd QuantLibAddin\QuantLib
cmake -S . -B build\windows-vs2026-x64-static -G "Visual Studio 18 2026" -A x64 ^
      -DMSVC_LINK_DYNAMIC_RUNTIME=OFF ^
      -DBOOST_INCLUDEDIR=C:/repos/boost_1_83_0 ^
      -DBOOST_LIBRARYDIR=C:/repos/boost_1_83_0/stage/lib ^
      -DQL_BUILD_EXAMPLES=OFF -DQL_BUILD_TEST_SUITE=OFF
cmake --build build\windows-vs2026-x64-static --config Release
```

Use `-DMSVC_LINK_DYNAMIC_RUNTIME=ON` instead for a dynamic-CRT build, and
`--config Debug` for Debug. QuantLib writes `QuantLibConfig.cmake` under
`QuantLib\build\windows-vs2026-x64-static\cmake\`.

### 5.3 Build ObjectHandler

Build ObjectHandler **standalone** (so it writes its package config) with the
matching preset. Follow
[../../ObjectHandler/md/build_cmake.md](../../ObjectHandler/md/build_cmake.md);
in brief, for the same VS 2026 static preset:

```powershell
cd QuantLibAddin\ObjectHandler
# Fresh clone: the first build must be Full so the ohxl sources are generated.
cmake --preset windows-vs2026-x64-static -DRUN_GENSRC=ON
cmake --build build\windows-vs2026-x64-static --config Release
```

ObjectHandler writes `ObjectHandlerConfig.cmake` under
`ObjectHandler\build\windows-vs2026-x64-static\cmake\`.

> Build both prerequisites with the **same** preset (toolset + CRT) and the
> **same** configuration you will use for QuantLibAddin in section 6 - see the
> matching note in section 3.

---

## 6 Configure QuantLibAddin

### 6.1 Configure Boost paths

Boost paths are **not** baked into the shared `CMakePresets.json`. Instead each
user supplies them in a `CMakeUserPresets.json` file in the **`QuantLibAddin`
subproject directory** (next to its `CMakePresets.json`). That file is listed in
`.gitignore` and is never committed, so your local paths stay out of the
repository.

`CMakePresets.json` defines hidden **base** presets (compiler, architecture and
runtime selection); your `CMakeUserPresets.json` defines the presets you
actually use, each **inheriting** a base preset and adding your Boost paths.
Create it by copying the template below and adjusting the two Boost paths:

```json
{
    "version": 6,
    "configurePresets": [
        {
            "name": "windows-vs2026-x64-static",
            "displayName": "VS 2026 x64 - static CRT",
            "inherits": "windows-vs2026-x64-static-base",
            "cacheVariables": {
                "BOOST_INCLUDEDIR": "C:/repos/boost_1_83_0",
                "BOOST_LIBRARYDIR": "C:/repos/boost_1_83_0/stage/lib"
            }
        },
        {
            "name": "windows-vs2026-x64-dynamic",
            "displayName": "VS 2026 x64 - dynamic CRT",
            "inherits": "windows-vs2026-x64-dynamic-base",
            "cacheVariables": {
                "BOOST_INCLUDEDIR": "C:/repos/boost_1_83_0",
                "BOOST_LIBRARYDIR": "C:/repos/boost_1_83_0/stage/lib"
            }
        },
        {
            "name": "windows-vs2022-x64-static",
            "displayName": "VS 2022 x64 - static CRT",
            "inherits": "windows-vs2022-x64-static-base",
            "cacheVariables": {
                "BOOST_INCLUDEDIR": "C:/repos/boost_1_83_0",
                "BOOST_LIBRARYDIR": "C:/repos/boost_1_83_0/stage/lib"
            }
        },
        {
            "name": "windows-vs2022-x64-dynamic",
            "displayName": "VS 2022 x64 - dynamic CRT",
            "inherits": "windows-vs2022-x64-dynamic-base",
            "cacheVariables": {
                "BOOST_INCLUDEDIR": "C:/repos/boost_1_83_0",
                "BOOST_LIBRARYDIR": "C:/repos/boost_1_83_0/stage/lib"
            }
        }
    ],
    "buildPresets": [
        { "name": "windows-vs2026-x64-static-release",  "configurePreset": "windows-vs2026-x64-static",  "configuration": "Release" },
        { "name": "windows-vs2026-x64-static-debug",    "configurePreset": "windows-vs2026-x64-static",  "configuration": "Debug"   },
        { "name": "windows-vs2026-x64-dynamic-release", "configurePreset": "windows-vs2026-x64-dynamic", "configuration": "Release" },
        { "name": "windows-vs2026-x64-dynamic-debug",   "configurePreset": "windows-vs2026-x64-dynamic", "configuration": "Debug"   },
        { "name": "windows-vs2022-x64-static-release",  "configurePreset": "windows-vs2022-x64-static",  "configuration": "Release" },
        { "name": "windows-vs2022-x64-static-debug",    "configurePreset": "windows-vs2022-x64-static",  "configuration": "Debug"   },
        { "name": "windows-vs2022-x64-dynamic-release", "configurePreset": "windows-vs2022-x64-dynamic", "configuration": "Release" },
        { "name": "windows-vs2022-x64-dynamic-debug",   "configurePreset": "windows-vs2022-x64-dynamic", "configuration": "Debug"   }
    ]
}
```

Notes:

- `BOOST_INCLUDEDIR` is the directory that **contains** the `boost\` header
  sub-folder (so that `#include <boost/config.hpp>` resolves).
- `BOOST_LIBRARYDIR` is the directory that contains the compiled `.lib` files.
  Boost auto-linking is disabled in this build, so cmake selects the correct
  libraries (`filesystem` and `serialization`) from that directory; you do not
  list individual libraries.
- Paths in JSON use **forward slashes** (or escaped `\\`). The `static` presets
  request the static-runtime Boost libraries (`/MT`) and the `dynamic` presets
  the dynamic-runtime ones (`/MD`); this **must** match the runtime of the Boost
  libraries you point at, and the runtime you used for QuantLib and
  ObjectHandler (see the `LNK2038` note in section 9).
- You only need the presets you intend to use - delete the others from your
  `CMakeUserPresets.json` if you build with just one compiler.

If Boost is installed somewhere cmake can find automatically (for example via a
`BOOST_ROOT` environment variable or vcpkg), `CMakeUserPresets.json` can be
omitted entirely and the shared presets will work as-is.

### 6.2 Configure

Open a **Developer PowerShell / Command Prompt** (or any shell where `cmake` is
on the `PATH`), change to the **`QuantLibAddin` subproject directory** - the
folder that contains `CMakePresets.json` (i.e. `QuantLibAddin\QuantLibAddin`) -
and run cmake with the preset you want, telling it where the two prebuilt
prerequisites are via `CMAKE_PREFIX_PATH`.

On a **fresh clone**, or whenever you want to (re)generate the `qlo` /
`Addins\Cpp` sources, add `-DRUN_GENSRC=ON` to configure a **Full** build, which
runs gensrc first (this requires Python 3 - see section 4.3):

```powershell
cd QuantLibAddin\QuantLibAddin

# Full build: regenerate the auto-generated qlo / Addins\Cpp sources, then configure
cmake --preset windows-vs2026-x64-static `
      "-DCMAKE_PREFIX_PATH=../QuantLib/build/windows-vs2026-x64-static;../ObjectHandler/build/windows-vs2026-x64-static" `
      -DRUN_GENSRC=ON
```

`CMAKE_PREFIX_PATH` is a semicolon-separated list of the **two prerequisite
build directories** (the ones that contain a `cmake\*Config.cmake`, from
sections 5.2 and 5.3). Relative paths are resolved against the QuantLibAddin
subproject directory; absolute paths work too. The preset's `binaryDir` puts the
QuantLibAddin build under `build\<preset>\` automatically.

gensrc runs once, during this configure step, and writes the generated sources
into the source tree.

Once the `qlo` / `Addins\Cpp` sources exist, drop `-DRUN_GENSRC=ON` to configure
a **Basic** build (section 2) - the default - which skips gensrc:

```powershell
cmake --preset windows-vs2026-x64-static `
      "-DCMAKE_PREFIX_PATH=../QuantLib/build/windows-vs2026-x64-static;../ObjectHandler/build/windows-vs2026-x64-static"
```

After a successful configure you can build (section 7); unless you change the
gensrc metadata, you only need a Full configure once.

> Use the **same** preset name (`windows-vs2026-x64-static` above) you used to
> build QuantLib and ObjectHandler, and point `CMAKE_PREFIX_PATH` at the build
> directories of that same preset. Run `cmake --preset` from the
> `QuantLibAddin\QuantLibAddin` folder so cmake finds these presets.

> QuantLibAddin is also built automatically as part of the whole-stack root
> build and the QuantLibXL cmake build, which use the repository-root presets
> and need no `CMAKE_PREFIX_PATH`. If you are building the whole stack, follow
> [../../build_cmake.md](../../build_cmake.md) or
> [../../QuantLibXL/md/build_cmake.md](../../QuantLibXL/md/build_cmake.md) and
> you do not need the standalone configure above.

---

## 7 Build

The Visual Studio generator is multi-config, so the **same** build directory
produces either Release or Debug. The simplest way to build is with a **build
preset**, which selects the configuration for you and writes to the configure
preset's `build\<preset>\` directory:

```powershell
# Release  (build preset names end in -release / -debug)
cmake --build --preset windows-vs2026-x64-static-release --target QLADemo

# Debug
cmake --build --preset windows-vs2026-x64-static-debug --target QLADemo
```

Equivalently, point `cmake --build` at the preset's build directory and choose
the configuration with `--config`:

```powershell
# Release static  ->  QLADemo-v145-x64-mt-s-1_42_0.exe
cmake --build build\windows-vs2026-x64-static --config Release --target QLADemo

# Debug static    ->  QLADemo-v145-x64-mt-sgd-1_42_0.exe
cmake --build build\windows-vs2026-x64-static --config Debug   --target QLADemo
```

Building `QLADemo` builds its prerequisites `AddinCpp` and `QuantLibObjects`
first. To build only the wrapper library, use `--target QuantLibObjects`. Omit
`--target` to build the default target set (all of the above).

For the VS 2022 presets the toolset tag in the `QLADemo` name is `v143` instead
of `v145`; for the dynamic presets the runtime tag is `-mt` / `-mt-gd` instead
of `-mt-s` / `-mt-sgd` (see section 8).

---

## 8 Output

The build artifacts are written under the preset's build directory
(`build\<preset>\`), keeping them separate from the Visual Studio build's output
(`QuantLibAddin\lib\` and `QuantLibAddin\Clients\Cpp\bin\`) so the two builds do
not overwrite each other.

The `QuantLibObjects` and `AddinCpp` static libraries are intermediate artifacts
produced under the build tree and linked into `QLADemo`. The demo client is the
named, runnable output; its file name encodes the platform toolset, the
platform, the runtime variant and the version, and it is written to
`build\<preset>\cpp\`:

| Preset | Config | Output file (under `build\<preset>\cpp\`) |
|---|---|---|
| `windows-vs2026-x64-static`  | Release | `QLADemo-v145-x64-mt-s-1_42_0.exe` |
| `windows-vs2026-x64-static`  | Debug   | `QLADemo-v145-x64-mt-sgd-1_42_0.exe` |
| `windows-vs2026-x64-dynamic` | Release | `QLADemo-v145-x64-mt-1_42_0.exe` |
| `windows-vs2026-x64-dynamic` | Debug   | `QLADemo-v145-x64-mt-gd-1_42_0.exe` |
| `windows-vs2022-x64-static`  | Release | `QLADemo-v143-x64-mt-s-1_42_0.exe` |
| `windows-vs2022-x64-static`  | Debug   | `QLADemo-v143-x64-mt-sgd-1_42_0.exe` |
| `windows-vs2022-x64-dynamic` | Release | `QLADemo-v143-x64-mt-1_42_0.exe` |
| `windows-vs2022-x64-dynamic` | Debug   | `QLADemo-v143-x64-mt-gd-1_42_0.exe` |

The static-runtime (`-mt-s` / `-mt-sgd`) executable is self-contained and does
not require the Visual C++ runtime to be installed on the target machine.

---

## 9 Troubleshooting

**CMake error: `Could not find a package configuration file provided by
"QuantLib"`** (or `"ObjectHandler"`) at configure time. cmake cannot find the
prerequisite's build-tree package config. Either you have not built that
prerequisite yet (sections 5.2 / 5.3), or `CMAKE_PREFIX_PATH` does not point at
its build directory. `CMAKE_PREFIX_PATH` must list the directory that contains
`cmake\QuantLibConfig.cmake` / `cmake\ObjectHandlerConfig.cmake` - that is the
prerequisite's `build\<preset>` directory, **not** its source folder. Note that
ObjectHandler writes `ObjectHandlerConfig.cmake` only when it is built
**standalone** (section 5.3).

**CMake error: `Cannot find source file`** for one of the
`qlo\valueobjects\vo_*.cpp`, `qlo\serialization\**` or `Addins\Cpp\*.cpp` files
at configure time. The auto-generated sources are not present - this is a
**fresh clone** and you ran a Basic build. Configure a **Full** build once with
`-DRUN_GENSRC=ON` (sections 2 and 6.2) to generate them; this needs Python 3.

**CMake error: `Could NOT find Boost`** at configure time. `BOOST_INCLUDEDIR`
or `BOOST_LIBRARYDIR` is missing or wrong in your `CMakeUserPresets.json`, or
your Boost build is missing the `filesystem` / `serialization` components.
Re-check section 6.1: `BOOST_INCLUDEDIR` must point at the folder that contains
the `boost\` sub-directory, and `BOOST_LIBRARYDIR` at the folder that contains
the compiled `.lib` files.

**CMake error: `No such preset in ...` / `Could not read presets`** when you run
`cmake --preset`. Either you are not in the QuantLibAddin subproject directory
(run cmake from `QuantLibAddin\QuantLibAddin`, the folder that contains
`CMakePresets.json`), or the preset name is not defined in your
`CMakeUserPresets.json`. Check section 6.1.

**`error C1083: Cannot open include file: 'boost/config.hpp'`** during
compilation. `BOOST_INCLUDEDIR` does not point at the directory that contains
the `boost\` header sub-folder. Fix it in `CMakeUserPresets.json` and
re-configure.

**`error LNK1104: cannot open file 'libboost_...lib'`** (or `...QuantLib...lib`
/ `...ObjectHandler...lib`) at the link stage. The Boost library directory is
wrong, or the static/dynamic runtime variant your preset asked for is not
present in `BOOST_LIBRARYDIR`. Make sure `BOOST_LIBRARYDIR` points at a Boost
build that provides the matching runtime libraries for the preset you chose,
then re-configure.

**`error LNK2038: mismatch detected for 'RuntimeLibrary'`** (for example
`value 'MT_StaticRelease' doesn't match value 'MD_DynamicRelease'`), usually
followed by `error LNK1169: one or more multiply defined symbols found`, at the
link stage. The prebuilt QuantLib, ObjectHandler and/or Boost were built against
a different C runtime than QuantLibAddin selected. The `dynamic` presets compile
`/MD` (dynamic CRT); the `static` presets compile `/MT` (static CRT), and **all**
of QuantLib, ObjectHandler, Boost and QuantLibAddin must agree. Rebuild the
mismatched prerequisite with the **same** preset (toolset + CRT) and
configuration, or switch QuantLibAddin to the preset that matches what you
already built (see section 3).

(The Boost `bind.hpp` "declaring the Bind placeholders ... is deprecated" lines
that may appear earlier in the log are harmless deprecation *messages*, not the
cause of this failure.)

**Python errors during a Full build**, or you expected regeneration but the
generated files were not refreshed. gensrc only runs when you configure with
`-DRUN_GENSRC=ON` (section 6.2); install Python 3 and ensure `python --version`
works.

---

## 10 Verify the build

`QuantLibObjects` is a static library, so the most direct way to confirm the
build is to run the bundled C++ client, `QLADemo`, which links against it and
exercises a cross-section of the wrapped QuantLib objects.

Run the executable produced in section 8, for example (VS 2026, Release static):

```powershell
build\windows-vs2026-x64-static\cpp\QLADemo-v145-x64-mt-s-1_42_0.exe
```

If it runs to completion and prints its results without error, the QuantLibAddin
layer is good and ready to be consumed by QuantLibXL.
