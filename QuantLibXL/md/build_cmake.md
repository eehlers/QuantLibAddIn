# Build QuantLibXL from a git clone using cmake

This document explains how to build the QuantLibXL Excel add-in (the `.xll`
file) from a **git clone**, using the **cmake** build, **on its own** as a
standalone subproject. It is the cmake counterpart of [build_vs.md](build_vs.md)
(the hand-maintained Visual Studio solution build); both use the same source
files and produce the same XLL.

A standalone QuantLibXL build consumes **prebuilt** QuantLib, ObjectHandler and
QuantLibAddin libraries (it does not rebuild them); you build those three
prerequisites first, then point this build at them. This mirrors the project
dependency chain `gensrc <- ObjectHandler <- QuantLibAddin <- QuantLibXL`, with
QuantLib supplying the analytics underneath QuantLibAddin.

There are two ways to build the cmake projects in this repository:

1. **Each subproject on its own** (this document) - build QuantLib,
   ObjectHandler and QuantLibAddin first, then QuantLibXL against them. Use this
   when you want to build or test just the QuantLibXL Excel layer.
2. **The whole stack from the repository root** - a single configure builds
   QuantLib, ObjectHandler, QuantLibAddin and QuantLibXL together. See the root
   [../../build_cmake.md](../../build_cmake.md).

For other options:

- To download a compiled QuantLibXL XLL, see
  <https://www.quantlib.org/quantlibxl/installation.html>.
- To compile various flavors of QuantLibAddin (including the QuantLibXL XLL) from
  an official release of source code (zip files / tarballs), see
  <https://www.quantlib.org/quantlibaddin/tutorials.html>.

---

## 1 What gets built

A standalone cmake build produces one **Excel add-in** - a Windows DLL with an
`.xll` extension:

| Target | Kind | Description |
|---|---|---|
| `QuantLibXL` | shared library (`.xll`) | the QuantLibXL Excel add-in |

The XLL is **Windows-only**. Building `QuantLibXL` links the imported,
**prebuilt** prerequisite libraries (QuantLibAddin's `QuantLibObjects`,
ObjectHandler's `ohxllib` / `xlsdk`, and QuantLib) - the standalone build does
**not** rebuild any of them; it consumes them as imported targets located
through cmake's `find_package` (section 3).

QuantLibXL depends on **Boost** and on three other projects in this repository:
**QuantLib**, **ObjectHandler** and **QuantLibAddin**. You build those three
first (section 5), then configure QuantLibXL to find them (section 6).

---

## 2 Choose a build - Basic or Full

Both builds produce the same `.xll`; they differ only in whether the
auto-generated source code is regenerated. The choice is a single cmake cache
variable, `RUN_GENSRC`, set at configure time.

| Build | Configure with | Description |
|---|---|---|
| Basic | (default - `RUN_GENSRC` is `OFF`) | Compiles the auto-generated source files that are already present in the clone. Does **not** require Python. Use this for normal compilation. |
| Full  | `-DRUN_GENSRC=ON` | Runs gensrc first to (re)generate all auto-generated source files, then compiles. Requires Python 3. Use this on a fresh clone, or after you have changed the gensrc metadata (the XML files under `QuantLibAddin\gensrc\metadata`). |

On a **fresh clone the first build must be a Full build** (`-DRUN_GENSRC=ON`) so
that the generated Excel sources (`qlxl\functions\*.cpp` and
`qlxl\register\register_*.cpp`) are created. Otherwise cmake fails at configure
time with `Cannot find source file` for one of those `.cpp` files (see section
9). After one Full build the generated files exist, and subsequent Basic builds
compile them without needing Python.

> The standalone QuantLibXL Full build regenerates only the **Excel** (`qlxl`)
> sources - it targets gensrc's Excel addin (`-x`). It does **not** regenerate
> the upstream QuantLibAddin (`qlo` / `Addins\Cpp`) or ObjectHandler sources;
> those belong to those projects' own builds (see
> [../../QuantLibAddin/md/build_cmake.md](../../QuantLibAddin/md/build_cmake.md)
> and [../../ObjectHandler/md/build_cmake.md](../../ObjectHandler/md/build_cmake.md)),
> and you build them as prebuilt prerequisites in section 5.

---

## 3 How the prerequisites are located

A standalone QuantLibXL build imports the prebuilt prerequisite libraries using
cmake package configs that each prerequisite writes into **its own build tree**
(no install step is required):

- QuantLib's build tree contains `cmake\QuantLibConfig.cmake`, providing the
  imported target `QuantLib::QuantLib`.
- ObjectHandler's build tree contains `cmake\ObjectHandlerConfig.cmake`,
  providing the imported targets `ObjectHandler::ohxllib` / `ObjectHandler::xlsdk`
  / `ObjectHandler::ohlib` (written only when ObjectHandler is configured
  **standalone**).
- QuantLibAddin's build tree contains `cmake\QuantLibAddinConfig.cmake`,
  providing the imported target `QuantLibAddin::QuantLibObjects` (written only
  when QuantLibAddin is configured **standalone**).

QuantLibXL's `CMakeLists.txt` calls `find_package(ObjectHandler CONFIG)` and
`find_package(QuantLibAddin CONFIG)`. (QuantLib is pulled in automatically as a
transitive dependency of QuantLibAddin, so you do not call `find_package` for it
directly - but its build directory still has to be findable, see below.) You
tell cmake where those build trees are with **`CMAKE_PREFIX_PATH`** (a list of
the three build directories) at configure time - section 6 shows the exact
command.

> **The prerequisites must match this build.** QuantLib, ObjectHandler,
> QuantLibAddin and QuantLibXL are separate build trees, so their libraries only
> link together if all of them were built with the **same** Visual Studio
> toolset, the **same** CRT (static `/MT` vs dynamic `/MD`) and the **same**
> configuration (Release vs Debug). Use the same preset name throughout. Mixing
> them produces `LNK2038` runtime-mismatch errors at the link stage (see section
> 9).

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

QuantLib, ObjectHandler, QuantLibAddin and QuantLibXL all depend on Boost. You
need the **compiled** Boost libraries, not just the headers (the build links the
Boost `filesystem` and `serialization` components). Building Boost is outside the
scope of this document; these instructions assume you already have a Boost build
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

Like the other cmake subproject builds, the standalone QuantLibXL build has a
**single** Boost configuration point: you supply the include and library
directories once, in `CMakeUserPresets.json` (section 6.1). Use the **same**
Boost build (and the matching runtime variant) for QuantLib, ObjectHandler,
QuantLibAddin and QuantLibXL.

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

`gensrc`, `ObjectHandler`, `QuantLibAddin` and `QuantLibXL` are all contained in
this single repository. Clone it:

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
  QuantLibAddin\
  QuantLibXL\          (the subproject this document builds)
```

> The gensrc generator that the QuantLibXL Full build runs lives in the
> top-level `gensrc\` directory and writes the Excel sources into
> `QuantLibXL\qlxl`, reading the addin metadata from `QuantLibAddin\gensrc`.
> Both of those directories are part of the main clone above, so a standalone
> QuantLibXL Full build needs the full repository layout, not just the
> `QuantLibXL` folder.

### 5.2 Build QuantLib

Configure and build QuantLib into its own build tree, using the **same** toolset
and CRT you intend to use for QuantLibXL. For a VS 2026 static-CRT Release build:

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

### 5.4 Build QuantLibAddin

Build QuantLibAddin **standalone** (so it writes its package config) with the
matching preset, pointing it at the QuantLib and ObjectHandler build trees.
Follow [../../QuantLibAddin/md/build_cmake.md](../../QuantLibAddin/md/build_cmake.md);
in brief, for the same VS 2026 static preset (run from the **`QuantLibAddin`
subproject** directory, `QuantLibAddin\QuantLibAddin`):

```powershell
cd QuantLibAddin\QuantLibAddin
# Fresh clone: the first build must be Full so the qlo / Addins\Cpp sources are generated.
cmake --preset windows-vs2026-x64-static `
      "-DCMAKE_PREFIX_PATH=../QuantLib/build/windows-vs2026-x64-static;../ObjectHandler/build/windows-vs2026-x64-static" `
      -DRUN_GENSRC=ON
cmake --build build\windows-vs2026-x64-static --config Release --target QuantLibObjects
```

QuantLibAddin writes `QuantLibAddinConfig.cmake` under
`QuantLibAddin\build\windows-vs2026-x64-static\cmake\`.

> Build all three prerequisites with the **same** preset (toolset + CRT) and the
> **same** configuration you will use for QuantLibXL in section 6 - see the
> matching note in section 3.

---

## 6 Configure QuantLibXL

### 6.1 Configure Boost paths

Boost paths are **not** baked into the shared `CMakePresets.json`. Instead each
user supplies them in a `CMakeUserPresets.json` file in the **`QuantLibXL`
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
  libraries you point at, and the runtime you used for QuantLib, ObjectHandler
  and QuantLibAddin (see the `LNK2038` note in section 9).
- You only need the presets you intend to use - delete the others from your
  `CMakeUserPresets.json` if you build with just one compiler.

If Boost is installed somewhere cmake can find automatically (for example via a
`BOOST_ROOT` environment variable or vcpkg), `CMakeUserPresets.json` can be
omitted entirely and the shared presets will work as-is.

### 6.2 Configure

Open a **Developer PowerShell / Command Prompt** (or any shell where `cmake` is
on the `PATH`), change to the **`QuantLibXL` subproject directory** - the folder
that contains `CMakePresets.json` (i.e. `QuantLibAddin\QuantLibXL`) - and run
cmake with the preset you want, telling it where the three prebuilt prerequisites
are via `CMAKE_PREFIX_PATH`.

On a **fresh clone**, or whenever you want to (re)generate the `qlxl` sources,
add `-DRUN_GENSRC=ON` to configure a **Full** build, which runs gensrc's Excel
target first (this requires Python 3 - see section 4.3):

```powershell
cd QuantLibAddin\QuantLibXL

# Full build: regenerate the auto-generated qlxl sources, then configure
cmake --preset windows-vs2026-x64-static `
      "-DCMAKE_PREFIX_PATH=../QuantLib/build/windows-vs2026-x64-static;../ObjectHandler/build/windows-vs2026-x64-static;../QuantLibAddin/build/windows-vs2026-x64-static" `
      -DRUN_GENSRC=ON
```

`CMAKE_PREFIX_PATH` is a semicolon-separated list of the **three prerequisite
build directories** (the ones that contain a `cmake\*Config.cmake`, from sections
5.2, 5.3 and 5.4). All three are listed - even though QuantLibXL only calls
`find_package` for ObjectHandler and QuantLibAddin - because QuantLibAddin's
config in turn re-finds QuantLib and ObjectHandler, so their build directories
must be on the path too. Relative paths are resolved against the QuantLibXL
subproject directory; absolute paths work too. The preset's `binaryDir` puts the
QuantLibXL build under `build\<preset>\` automatically.

gensrc runs once, during this configure step, and writes the generated Excel
sources into the `QuantLibXL\qlxl` source tree.

Once the `qlxl` sources exist, drop `-DRUN_GENSRC=ON` to configure a **Basic**
build (section 2) - the default - which skips gensrc:

```powershell
cmake --preset windows-vs2026-x64-static `
      "-DCMAKE_PREFIX_PATH=../QuantLib/build/windows-vs2026-x64-static;../ObjectHandler/build/windows-vs2026-x64-static;../QuantLibAddin/build/windows-vs2026-x64-static"
```

After a successful configure you can build (section 7); unless you change the
gensrc metadata, you only need a Full configure once.

> Use the **same** preset name (`windows-vs2026-x64-static` above) you used to
> build the three prerequisites, and point `CMAKE_PREFIX_PATH` at the build
> directories of that same preset. Run `cmake --preset` from the
> `QuantLibAddin\QuantLibXL` folder so cmake finds these presets.

> QuantLibXL is also built automatically as part of the whole-stack root build,
> which uses the repository-root presets and needs no `CMAKE_PREFIX_PATH`. If you
> are building the whole stack, follow
> [../../build_cmake.md](../../build_cmake.md) and you do not need
> the standalone configure above.

---

## 7 Build

The Visual Studio generator is multi-config, so the **same** build directory
produces either Release or Debug. The simplest way to build is with a **build
preset**, which selects the configuration for you and writes to the configure
preset's `build\<preset>\` directory:

```powershell
# Release  (build preset names end in -release / -debug)
cmake --build --preset windows-vs2026-x64-static-release --target QuantLibXL

# Debug
cmake --build --preset windows-vs2026-x64-static-debug --target QuantLibXL
```

Equivalently, point `cmake --build` at the preset's build directory and choose
the configuration with `--config`:

```powershell
# Release static  ->  QuantLibXL-v145-x64-mt-s-1_42_0.xll
cmake --build build\windows-vs2026-x64-static --config Release --target QuantLibXL

# Debug static    ->  QuantLibXL-v145-x64-mt-sgd-1_42_0.xll
cmake --build build\windows-vs2026-x64-static --config Debug   --target QuantLibXL
```

`QuantLibXL` is the only library target in a standalone build, so you may omit
`--target QuantLibXL` and build the default target set instead.

For the VS 2022 presets the toolset tag in the XLL name is `v143` instead of
`v145`; for the dynamic presets the runtime tag is `-mt` / `-mt-gd` instead of
`-mt-s` / `-mt-sgd` (see section 8).

---

## 8 Output

On success the add-in is written under the preset's build directory
(`build\<preset>\xll\`), keeping it separate from the Visual Studio build's
output (`QuantLibXL\xll\`) so the two builds do not overwrite each other. The
file name encodes the platform toolset, the platform, the runtime variant and
the version:

| Preset | Config | Output file (under `build\<preset>\xll\`) |
|---|---|---|
| `windows-vs2026-x64-static`  | Release | `QuantLibXL-v145-x64-mt-s-1_42_0.xll` |
| `windows-vs2026-x64-static`  | Debug   | `QuantLibXL-v145-x64-mt-sgd-1_42_0.xll` |
| `windows-vs2026-x64-dynamic` | Release | `QuantLibXL-v145-x64-mt-1_42_0.xll` |
| `windows-vs2026-x64-dynamic` | Debug   | `QuantLibXL-v145-x64-mt-gd-1_42_0.xll` |
| `windows-vs2022-x64-static`  | Release | `QuantLibXL-v143-x64-mt-s-1_42_0.xll` |
| `windows-vs2022-x64-static`  | Debug   | `QuantLibXL-v143-x64-mt-sgd-1_42_0.xll` |
| `windows-vs2022-x64-dynamic` | Release | `QuantLibXL-v143-x64-mt-1_42_0.xll` |
| `windows-vs2022-x64-dynamic` | Debug   | `QuantLibXL-v143-x64-mt-gd-1_42_0.xll` |

The static-runtime (`-mt-s` / `-mt-sgd`) XLL is self-contained and does not
require the Visual C++ runtime to be installed on the target machine, which is
usually what you want for distribution.

---

## 9 Troubleshooting

**CMake error: `Could not find a package configuration file provided by
"ObjectHandler"`** (or `"QuantLibAddin"`, or `"QuantLib"`) at configure time.
cmake cannot find a prerequisite's build-tree package config. Either you have not
built that prerequisite yet (sections 5.2 - 5.4), or `CMAKE_PREFIX_PATH` does not
point at its build directory. `CMAKE_PREFIX_PATH` must list the directory that
contains `cmake\QuantLibConfig.cmake` / `cmake\ObjectHandlerConfig.cmake` /
`cmake\QuantLibAddinConfig.cmake` - that is the prerequisite's `build\<preset>`
directory, **not** its source folder. Remember that all **three** build
directories must be listed, because QuantLibAddin's config re-finds QuantLib and
ObjectHandler. Note that ObjectHandler and QuantLibAddin write their configs only
when built **standalone** (sections 5.3 and 5.4).

**CMake error: `Cannot find source file`** for one of the
`qlxl\functions\*.cpp` or `qlxl\register\register_*.cpp` files at configure time.
The auto-generated Excel sources are not present - this is a **fresh clone** and
you ran a Basic build. Configure a **Full** build once with `-DRUN_GENSRC=ON`
(sections 2 and 6.2) to generate them; this needs Python 3.

**CMake error: `Could NOT find Boost`** at configure time. `BOOST_INCLUDEDIR`
or `BOOST_LIBRARYDIR` is missing or wrong in your `CMakeUserPresets.json`, or
your Boost build is missing the `filesystem` / `serialization` components.
Re-check section 6.1: `BOOST_INCLUDEDIR` must point at the folder that contains
the `boost\` sub-directory, and `BOOST_LIBRARYDIR` at the folder that contains
the compiled `.lib` files.

**CMake error: `No such preset in ...` / `Could not read presets`** when you run
`cmake --preset`. Either you are not in the QuantLibXL subproject directory (run
cmake from `QuantLibAddin\QuantLibXL`, the folder that contains
`CMakePresets.json`), or the preset name is not defined in your
`CMakeUserPresets.json`. Check section 6.1.

**`error C1083: Cannot open include file: 'boost/config.hpp'`** during
compilation. `BOOST_INCLUDEDIR` does not point at the directory that contains
the `boost\` header sub-folder. Fix it in `CMakeUserPresets.json` and
re-configure.

**`error LNK1104: cannot open file 'libboost_...lib'`** (or `...QuantLib...lib`
/ `...ObjectHandler...lib` / `...QuantLibObjects...lib`) at the link stage. A
prerequisite library directory is wrong, or the static/dynamic runtime variant
your preset asked for is not present. Make sure each prerequisite was built with
the preset you chose and that `BOOST_LIBRARYDIR` provides the matching runtime
Boost libraries, then re-configure.

**`error LNK2038: mismatch detected for 'RuntimeLibrary'`** (for example
`value 'MT_StaticRelease' doesn't match value 'MD_DynamicRelease'`), usually
followed by `error LNK1169: one or more multiply defined symbols found`, at the
link stage. One or more of the prebuilt prerequisites (QuantLib, ObjectHandler,
QuantLibAddin or Boost) was built against a different C runtime than QuantLibXL
selected. The `dynamic` presets compile `/MD` (dynamic CRT); the `static`
presets compile `/MT` (static CRT), and **all** of the prerequisites, Boost and
QuantLibXL must agree. Rebuild the mismatched prerequisite with the **same**
preset (toolset + CRT) and configuration, or switch QuantLibXL to the preset that
matches what you already built (see section 3).

(The Boost `bind.hpp` "declaring the Bind placeholders ... is deprecated" lines
that may appear earlier in the log are harmless deprecation *messages*, not the
cause of this failure.)

**Python errors during a Full build**, or you expected regeneration but the
generated files were not refreshed. gensrc only runs when you configure with
`-DRUN_GENSRC=ON` (section 6.2); install Python 3 and ensure `python --version`
works. Remember the Full build also needs the full repository layout (the
top-level `gensrc\` and `QuantLibAddin\gensrc` directories), not just the
`QuantLibXL` folder.

---

## 10 Load the add-in in Excel

1. Open Excel.
2. Go to **File > Options > Add-ins**.
3. Set **Manage** to **Excel Add-ins** and click **Go**.
4. Click **Browse**, navigate to the `.xll` produced in section 8 (under
   `build\<preset>\xll\`), and click **OK**.

Alternatively, drag and drop the `.xll` onto an open Excel window.

---

## 11 Load a sample workbook

With the add-in loaded, open one of the sample workbooks to confirm the
functions are available and to see the add-in in action. The best ones to start
with are:

```
QuantLibXL\StandaloneExamples\Analytics\YieldCurveBootstrapping.xlsx
QuantLibXL\StandaloneExamples\Analytics\InterestRateDerivatives.xlsx
```

Open one of these workbooks in the same Excel instance that has the add-in
loaded, then recalculate (`Ctrl+Alt+F9`) to evaluate the QuantLib functions. If
the functions evaluate without `#NAME?` errors, the standalone QuantLibXL build
is good.
