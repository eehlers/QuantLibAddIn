# Build ObjectHandler from a git clone using cmake

This document explains how to build ObjectHandler - the object repository and
Excel-add-in framework that QuantLibAddin and QuantLibXL are built on - from a
**git clone**, using the **cmake** build. It is the cmake counterpart of
[build_vs.md](build_vs.md) (the hand-maintained Visual Studio solution build);
both use the same source files and produce the same libraries.

ObjectHandler is normally built as a prerequisite of QuantLibXL. If your goal is
the QuantLibXL Excel add-in, follow
[../../QuantLibXL/md/build_cmake.md](../../QuantLibXL/md/build_cmake.md)
instead - it builds ObjectHandler for you as part of the dependency chain. Use
this document when you want to build or test ObjectHandler on its own.

To build the whole stack - QuantLib, ObjectHandler, QuantLibAddin and
QuantLibXL - in a single configure from the repository root, see
[../../build_cmake.md](../../build_cmake.md). This HOWTO covers the common case:
building the ObjectHandler libraries standalone from a clone.

---

## 1 What gets built

A standalone cmake build produces three **static libraries**:

| Target | Kind | Description |
|---|---|---|
| `ohlib` | static library | ObjectHandler core (no Excel dependency) |
| `xlsdk` | static library | Excel SDK wrapper (Windows only) |
| `ohxllib` | static library | ObjectHandler Excel layer (Windows only) |

These are the libraries consumed by QuantLibAddin and QuantLibXL. The supplied
presets also enable `BUILD_EXAMPLES` (section 5), so the build additionally
produces the example program that ships with ObjectHandler:

| Target | Kind | Description |
|---|---|---|
| `ExampleObjects` | static library | Sample business objects (Account, Customer) used by the example |
| `ExampleCpp` | **executable** | Console program that exercises the ObjectHandler repository |

> The Excel example add-ins under `Examples\xl` (and the demonstration `.xll`)
> are **not** built by this cmake build yet; the XLL build is owned by
> QuantLibXL. The hand-maintained Visual Studio solution
> ([build_vs.md](build_vs.md)) still builds them.

ObjectHandler depends only on **Boost** and on its own upstream build settings
(`gensrc\cmake\commonSettings.cmake`); it requires no other project in the
repository. The standalone build described here therefore needs nothing beyond
this repository and a Boost build (plus Python 3 for the one-time Full build
described in section 2).

---

## 2 Choose a build - Basic or Full

The Windows Excel layer (`ohxllib`) is compiled from source files that are
**auto-generated** by gensrc and are **not** committed to the repository, so on
a fresh clone they are not present yet. The build can generate them for you. The
choice is a single cmake cache variable, `RUN_GENSRC`, set at configure time.

| Build | Configure with | Description |
|---|---|---|
| Basic | (default - `RUN_GENSRC` is `OFF`) | Compiles the auto-generated source files that are already present in the clone. Does **not** require Python. Use this for normal compilation. |
| Full  | `-DRUN_GENSRC=ON` | Runs gensrc first to (re)generate the auto-generated source files, then compiles. Requires Python 3. Use this on a fresh clone, or after you have changed the gensrc metadata (the XML files under `ObjectHandler\gensrc\metadata`). |

On a **fresh clone the first build must be a Full build** (`-DRUN_GENSRC=ON`) so
that the generated `ohxl` sources are created. Otherwise cmake fails at configure
time with `Cannot find source file` for one of the `ohxl\functions\*.cpp` or
`ohxl\register\*.cpp` files (see section 9). After one Full build the generated
files exist, and subsequent Basic builds compile them without needing Python.

> The core library `ohlib` has **no** auto-generated sources, so it builds from a
> fresh clone with the default Basic build. The Linux build (which builds only
> `ohlib`, not the Excel layer) therefore does not need the Full build at all;
> only the Windows `ohxllib` does.

---

## 3 Prerequisites

### 3.1 CMake and a C++ compiler

- **CMake 3.15 or later.** CMake ships with the **"C++ CMake tools for
  Windows"** component of the Visual Studio **"Desktop development with C++"**
  workload, so if you installed that workload you already have it. Check with
  `cmake --version`.
- **Visual Studio 2026 or 2022** with the **"Desktop development with C++"**
  workload. This build is **x64 only**.

The platform toolset is selected automatically by the generator named in the
preset you choose (section 5); no manual toolset configuration is required:

| Visual Studio | Generator (from the preset) | Platform toolset |
|---|---|---|
| VS 2022 (v17) | `Visual Studio 17 2022` | v143 |
| VS 2026 (v18) | `Visual Studio 18 2026` | v145 |

### 3.2 Boost

ObjectHandler depends on Boost. You need the **compiled** Boost libraries, not
just the headers (the build links the Boost `filesystem` and `serialization`
components). Building Boost is outside the scope of this document; these
instructions assume you already have a Boost build available.

The build requires **Boost 1.58 or later**. It was tested using **Boost 1.83**.

For purposes of this HOWTO, it is assumed that you have installed Boost to:

```
C:\repos\boost_1_83_0
```

with headers under `C:\repos\boost_1_83_0` (i.e. the folder that contains the
`boost\` sub-directory) and the compiled libraries under
`C:\repos\boost_1_83_0\stage\lib`. Modify that path as necessary for your own
environment.

Like the QuantLibAddin and QuantLibXL cmake builds, ObjectHandler has a
**single** Boost configuration point: you supply the include and library
directories once, in a `CMakeUserPresets.json` file, and cmake applies them to
the build. This is described in section 5.

### 3.3 Python 3 (Full build only)

The **Full** build (`-DRUN_GENSRC=ON`) runs gensrc, which is a Python 3 script.
Install Python 3 and make sure `python` is on the `PATH` (i.e. `python
--version` works from a command prompt). See <https://www.python.org/>. When
`RUN_GENSRC` is on, cmake locates the interpreter itself via `find_package`.

The **Basic** build does not use Python or gensrc, so you can skip this section
for a Basic build. Remember, though, that a fresh clone needs **one** Full build
to create the generated `ohxl` sources (section 2).

---

## 4 Acquire the source code

`ObjectHandler` is contained in the main repository, alongside its upstream
`gensrc` build settings. Clone it:

```
git clone <repo-host>/<owner>/QuantLibAddIn.git
```

This creates the `QuantLibAddin` folder (you may name the outer folder anything
you like) containing `ObjectHandler` and `gensrc`, among the other
sub-projects. No other repository is required to build ObjectHandler.

---

## 5 Configure Boost (required)

Like the rest of the cmake build, ObjectHandler uses a **preset** to bundle the
generator, architecture and runtime selection into a single named profile, so
that configuring is a one-line `cmake --preset` command (section 6). Boost paths
are machine-specific and are **not** baked into the shared `CMakePresets.json`;
instead you supply them in a `CMakeUserPresets.json` file in the `ObjectHandler`
directory (next to `CMakePresets.json`). That file is listed in `.gitignore` and
is never committed, so your local paths stay out of the repository.

`ObjectHandler\CMakePresets.json` defines hidden **base** presets (generator,
architecture and runtime selection); your `CMakeUserPresets.json` defines the
presets you actually use, each **inheriting** a base preset and adding your Boost
paths. Create `ObjectHandler\CMakeUserPresets.json` by copying the template below
and adjusting the two Boost paths:

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
  libraries you point at (see the troubleshooting note about `LNK2038`).
- The base presets enable `BUILD_EXAMPLES`, so every preset above also builds
  the `ExampleObjects` library and the `ExampleCpp` example executable (section
  1). To build only the libraries, add `"BUILD_EXAMPLES": false` to a preset's
  `cacheVariables`.
- You only need the presets you intend to use - delete the others from your
  `CMakeUserPresets.json` if you build with just one compiler.

If Boost is installed somewhere cmake can find automatically (for example via a
`BOOST_ROOT` environment variable or vcpkg), `CMakeUserPresets.json` can be
omitted entirely and the shared presets will work as-is.

---

## 6 Configure

Open a **Developer PowerShell / Command Prompt** (or any shell where `cmake` is
on the `PATH`), change to the **`ObjectHandler` directory** - the folder that
contains `CMakePresets.json` - and run cmake with the preset you want. The
preset's `binaryDir` puts the build under `build\<preset>\` automatically.

On a **fresh clone**, or whenever you want to (re)generate the `ohxl` sources,
add `-DRUN_GENSRC=ON` to configure a **Full** build, which runs gensrc first
(this requires Python 3 - see section 3.3):

```powershell
cd ObjectHandler

# VS 2026, static CRT  (recommended for distribution)
cmake --preset windows-vs2026-x64-static -DRUN_GENSRC=ON

# VS 2026, dynamic CRT
cmake --preset windows-vs2026-x64-dynamic -DRUN_GENSRC=ON

# VS 2022, static CRT
cmake --preset windows-vs2022-x64-static -DRUN_GENSRC=ON

# VS 2022, dynamic CRT
cmake --preset windows-vs2022-x64-dynamic -DRUN_GENSRC=ON
```

gensrc runs once, during this configure step, and writes the generated sources
into the source tree.

Once the `ohxl` sources exist, drop `-DRUN_GENSRC=ON` to configure a **Basic**
build (section 2) - the default - which skips gensrc:

```powershell
cmake --preset windows-vs2026-x64-static
```

After a successful configure you can build (section 7); unless you change the
gensrc metadata, you only need a Full configure once.

> Unlike the QuantLibAddin and QuantLibXL presets (which live at the repository
> root and configure the whole add-in stack), the ObjectHandler presets live in
> the `ObjectHandler` directory and configure ObjectHandler **on its own** - no
> QuantLib clone is required. Run `cmake --preset` from the `ObjectHandler`
> folder so cmake finds these presets.

> ObjectHandler is also built automatically as part of the QuantLibAddin and
> QuantLibXL cmake builds, which use the repository-root presets. If you are
> building the whole stack, follow
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
cmake --build --preset windows-vs2026-x64-static-release

# Debug
cmake --build --preset windows-vs2026-x64-static-debug
```

Equivalently, point `cmake --build` at the preset's build directory and choose
the configuration with `--config`:

```powershell
cmake --build build\windows-vs2026-x64-static --config Release
cmake --build build\windows-vs2026-x64-static --config Debug
```

To build a single library, add `--target ohlib` (or `xlsdk` / `ohxllib`).

Because the presets enable `BUILD_EXAMPLES`, the build also produces
`ExampleObjects` and the `ExampleCpp` executable. To build just the example
program, add `--target ExampleCpp`.

---

## 8 Output

The build artifacts are written under the preset's build directory
(`build\<preset>\`), in a per-configuration sub-folder. For the
`windows-vs2026-x64-static` preset:

| Config | Output files |
|---|---|
| Release | `build\windows-vs2026-x64-static\Release\ohlib.lib`, `xlsdk.lib`, `ohxllib.lib` |
| Debug   | `build\windows-vs2026-x64-static\Debug\ohlib.lib`, `xlsdk.lib`, `ohxllib.lib` |

These are **intermediate** libraries: they are consumed by QuantLibAddin and
QuantLibXL (which link them into the `.xll`), so unlike those projects the
ObjectHandler cmake build does not apply a toolset/version tag to the file
names. `xlsdk` and `ohxllib` are produced only on Windows.

The example executable is written to the `examples` sub-folder of the build
directory and, like the other client executables in the repository, **is** tagged
with the toolset, platform and runtime:

| Preset | Config | Output file (under `build\<preset>\examples\`) |
|---|---|---|
| `windows-vs2026-x64-static`  | Release | `ExampleCpp-v145-x64-mt-s-1_42_0.exe` |
| `windows-vs2026-x64-static`  | Debug   | `ExampleCpp-v145-x64-mt-sgd-1_42_0.exe` |
| `windows-vs2026-x64-dynamic` | Release | `ExampleCpp-v145-x64-mt-1_42_0.exe` |
| `windows-vs2026-x64-dynamic` | Debug   | `ExampleCpp-v145-x64-mt-gd-1_42_0.exe` |
| `windows-vs2022-x64-static`  | Release | `ExampleCpp-v143-x64-mt-s-1_42_0.exe` |
| `windows-vs2022-x64-static`  | Debug   | `ExampleCpp-v143-x64-mt-sgd-1_42_0.exe` |

The `ExampleObjects.lib` helper library is written next to the other libraries
in the per-config sub-folder.

---

## 9 Troubleshooting

**CMake error: `Cannot find source file`** at configure time, naming a file
under `ohxl\functions\` or `ohxl\register\` (for example
`ohxl/register/register_all.cpp`). These sources are auto-generated by gensrc
and are not present on a fresh clone. Configure a **Full** build once to
add `-DRUN_GENSRC=ON` (section 6) - which also requires Python 3
(section 3.3). After one Full configure the files exist and subsequent Basic
builds work.

**CMake error: `Could NOT find Boost`** at configure time. `BOOST_INCLUDEDIR`
or `BOOST_LIBRARYDIR` is missing or wrong in your `CMakeUserPresets.json`, or
your Boost build is missing the `filesystem` / `serialization` components.
`BOOST_INCLUDEDIR` must point at the folder that contains the `boost\`
sub-directory, and `BOOST_LIBRARYDIR` at the folder that contains the compiled
`.lib` files (section 5).

**`error C1083: Cannot open include file: 'boost/config.hpp'`** during
compilation. `BOOST_INCLUDEDIR` does not point at the directory that contains
the `boost\` header sub-folder. Fix it in `CMakeUserPresets.json` and
re-configure.

**`error LNK2038: mismatch detected for 'RuntimeLibrary'`** (for example
`value 'MT_StaticRelease' doesn't match value 'MD_DynamicRelease'`), usually
when ObjectHandler is linked into a downstream project. The C runtime selected
by the preset must match the runtime of the Boost libraries you pointed at. A
Boost build tags the dynamic-runtime variant `...-mt-x64-...` and the
static-runtime variant `...-mt-s-x64-...`. Use a `dynamic` preset with `-mt`
Boost libraries, or a `static` preset with `-mt-s` Boost libraries, and make
sure the same choice is used across every project you link together.

**`error LNK1104: cannot open file 'libboost_...lib'`** at the link stage of a
downstream project. The Boost library directory is wrong, or the static/dynamic
runtime variant is not present in `BOOST_LIBRARYDIR`. Point `BOOST_LIBRARYDIR`
at a Boost build that provides the matching runtime libraries, then
re-configure.

---

## 10 Verify the build

Confirm the build by the presence of the `.lib` files listed in section 8:

```powershell
Get-ChildItem build\windows-vs2026-x64-static\Release\*.lib
```

If `ohlib.lib`, `xlsdk.lib` and `ohxllib.lib` are present, the build is good and
ObjectHandler is ready to be consumed by QuantLibAddin and QuantLibXL.

You can also functionally exercise the libraries by running the example program,
which stores, retrieves, serializes and deletes objects in the repository. Run
the executable produced in section 8, for example (VS 2026, Release static):

```powershell
build\windows-vs2026-x64-static\examples\ExampleCpp-v145-x64-mt-s-1_42_0.exe
```

It finishes with `End example program` and exit code `0`, and writes a
`example.log` and an `account.xml` in the current directory. (The remaining
functional testing of the Excel layer is performed by QuantLibXL, which links
`ohxllib` into the `.xll`.)

