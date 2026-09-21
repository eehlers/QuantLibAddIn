# Building QuantLibXL with CMake

This document describes the cmake build for QuantLibXL.  It exists
alongside the hand-maintained Visual Studio solution files documented in
[README_vs_build.md](README_vs_build.md); both builds use the same source files and produce
the same XLL output.

---

## 1  Overview

| Property | Value |
|----------|-------|
| Platform | x64 and Win32 (x86) |
| Compilers | VS 2026 (v145 toolset), VS 2022 (v143 toolset) |
| Configurations | Release static, Debug static, Release dynamic, Debug dynamic |
| XLL output | `build\<preset>\xll\QuantLibXL-<toolset>[-x64]-mt[-s|-gd|-sgd]-1_42_0.xll` |

The platform tag `-x64` is present only for x64 builds; Win32 builds omit it
(e.g. `QuantLibXL-v145-mt-s-1_42_0.xll`), matching the hand-maintained Win32
solution output name.  Win32 requires a Boost built for x86 (see section 3).

The XLL is written into the cmake binary directory (`build\<preset>\xll\`),
which keeps it separate from the output of the hand-maintained solution
files (`QuantLibXL\xll\`).

---

## 2  Prerequisites

The cmake build has the same prerequisites as the hand-maintained build
(see [README_vs_build.md](README_vs_build.md) sections 2.1 and 2.2) plus:

- **CMake 3.15+** - included with the "C++ CMake tools" component of the
  VS "Desktop development with C++" workload.

---

## 3  Boost paths

Boost paths are not baked into the shared CMakePresets.json.  Instead each
user supplies them in a CMakeUserPresets.json file in the repo root.  That
file is listed in .gitignore and is never committed.

Create CMakeUserPresets.json by copying the template below and adjusting
the paths to match your local Boost installation:

```json
{
    "version": 6,
    "configurePresets": [
        {
            "name": "windows-vs2026-x64-dynamic",
            "displayName": "VS 2026 x64 — dynamic CRT (Release+Debug)",
            "inherits": "windows-vs2026-x64-dynamic-base",
            "cacheVariables": {
                "BOOST_INCLUDEDIR": "C:/path/to/boost/include",
                "BOOST_LIBRARYDIR": "C:/path/to/boost/lib-md"
            }
        },
        {
            "name": "windows-vs2026-x64-static",
            "displayName": "VS 2026 x64 — static CRT (Release+Debug)",
            "inherits": "windows-vs2026-x64-static-base",
            "cacheVariables": {
                "BOOST_INCLUDEDIR": "C:/path/to/boost/include",
                "BOOST_LIBRARYDIR": "C:/path/to/boost/lib-mt"
            }
        },
        {
            "name": "windows-vs2026-Win32-static",
            "displayName": "VS 2026 Win32 — static CRT (Release+Debug)",
            "inherits": "windows-vs2026-Win32-static-base",
            "cacheVariables": {
                "BOOST_INCLUDEDIR": "C:/path/to/boost/include",
                "BOOST_LIBRARYDIR": "C:/path/to/boost/lib-mt-x86"
            }
        },
        {
            "name": "windows-vs2022-x64-dynamic",
            "displayName": "VS 2022 x64 — dynamic CRT (Release+Debug)",
            "inherits": "windows-vs2022-x64-dynamic-base",
            "cacheVariables": {
                "BOOST_INCLUDEDIR": "C:/path/to/boost/include",
                "BOOST_LIBRARYDIR": "C:/path/to/boost/lib-md"
            }
        },
        {
            "name": "windows-vs2022-x64-static",
            "displayName": "VS 2022 x64 — static CRT (Release+Debug)",
            "inherits": "windows-vs2022-x64-static-base",
            "cacheVariables": {
                "BOOST_INCLUDEDIR": "C:/path/to/boost/include",
                "BOOST_LIBRARYDIR": "C:/path/to/boost/lib-mt"
            }
        }
    ],
    "buildPresets": [
        { "name": "windows-vs2026-x64-dynamic-release", "configurePreset": "windows-vs2026-x64-dynamic", "configuration": "Release" },
        { "name": "windows-vs2026-x64-dynamic-debug",   "configurePreset": "windows-vs2026-x64-dynamic", "configuration": "Debug"   },
        { "name": "windows-vs2026-x64-static-release",  "configurePreset": "windows-vs2026-x64-static",  "configuration": "Release" },
        { "name": "windows-vs2026-x64-static-debug",    "configurePreset": "windows-vs2026-x64-static",  "configuration": "Debug"   },
        { "name": "windows-vs2026-Win32-static-release","configurePreset": "windows-vs2026-Win32-static","configuration": "Release" },
        { "name": "windows-vs2026-Win32-static-debug",  "configurePreset": "windows-vs2026-Win32-static","configuration": "Debug"   },
        { "name": "windows-vs2022-x64-dynamic-release", "configurePreset": "windows-vs2022-x64-dynamic", "configuration": "Release" },
        { "name": "windows-vs2022-x64-dynamic-debug",   "configurePreset": "windows-vs2022-x64-dynamic", "configuration": "Debug"   },
        { "name": "windows-vs2022-x64-static-release",  "configurePreset": "windows-vs2022-x64-static",  "configuration": "Release" },
        { "name": "windows-vs2022-x64-static-debug",    "configurePreset": "windows-vs2022-x64-static",  "configuration": "Debug"   }
    ]
}
```

If Boost is installed in a location that cmake can find automatically
(e.g. via BOOST_ROOT or vcpkg), CMakeUserPresets.json can simply be
omitted and the shared presets will work as-is.

---

## 4  Presets

Configure presets are provided per compiler x platform x CRT combination:

| Configure preset              | Compiler | Platform | CRT   |
|-------------------------------|----------|----------|-------|
| `windows-vs2026-x64-dynamic`  | VS 2026  | x64      | `/MD` |
| `windows-vs2026-x64-static`   | VS 2026  | x64      | `/MT` |
| `windows-vs2026-Win32-static` | VS 2026  | Win32    | `/MT` |
| `windows-vs2022-x64-dynamic`  | VS 2022  | x64      | `/MD` |
| `windows-vs2022-x64-static`   | VS 2022  | x64      | `/MT` |

The shared CMakePresets.json also defines the hidden base presets
`windows-vs2026-Win32-base` and `windows-vs2026-Win32-static-base` that the
Win32 user preset inherits from.  Each configure preset has a corresponding pair
of build presets with `-release` and `-debug` suffixes.

---

## 5  Configure

After creating CMakeUserPresets.json (see section 3), run cmake from the root of
your clone. In the commands below, replace `<repo>` with the full path of that
clone (the folder that contains `QuantLibXL`):

```powershell
# VS 2026, static CRT
cmake --preset windows-vs2026-x64-static -S <repo> -B <repo>\build\windows-vs2026-x64-static

# VS 2026, dynamic CRT
cmake --preset windows-vs2026-x64-dynamic -S <repo> -B <repo>\build\windows-vs2026-x64-dynamic

# VS 2022, static CRT
cmake --preset windows-vs2022-x64-static -S <repo> -B <repo>\build\windows-vs2022-x64-static

# VS 2022, dynamic CRT
cmake --preset windows-vs2022-x64-dynamic -S <repo> -B <repo>\build\windows-vs2022-x64-dynamic

# VS 2026, Win32 (x86), static CRT
cmake --preset windows-vs2026-Win32-static -S <repo> -B <repo>\build\windows-vs2026-Win32-static
```

---

## 6  Build

Pass `--config` and `--target QuantLibXL` to build a specific configuration.
Examples using the VS 2026 static preset:

```powershell
# Release static  ->  QuantLibXL-v145-x64-mt-s-1_42_0.xll
cmake --build <repo>\build\windows-vs2026-x64-static --config Release --target QuantLibXL

# Debug static    ->  QuantLibXL-v145-x64-mt-sgd-1_42_0.xll
cmake --build <repo>\build\windows-vs2026-x64-static --config Debug --target QuantLibXL
```

Examples using the VS 2022 static preset:

```powershell
# Release static  ->  QuantLibXL-v143-x64-mt-s-1_42_0.xll
cmake --build <repo>\build\windows-vs2022-x64-static --config Release --target QuantLibXL

# Debug static    ->  QuantLibXL-v143-x64-mt-sgd-1_42_0.xll
cmake --build <repo>\build\windows-vs2022-x64-static --config Debug --target QuantLibXL
```

Example using the VS 2026 Win32 static preset (note the name has no `-x64` tag):

```powershell
# Release static  ->  QuantLibXL-v145-mt-s-1_42_0.xll
cmake --build <repo>\build\windows-vs2026-Win32-static --config Release --target QuantLibXL
```

---

## 7  Output locations

| Preset                     | Config  | XLL filename |
|----------------------------|---------|--------------|
| windows-vs2026-x64-static  | Release | `build\windows-vs2026-x64-static\xll\QuantLibXL-v145-x64-mt-s-1_42_0.xll` |
| windows-vs2026-x64-static  | Debug   | `build\windows-vs2026-x64-static\xll\QuantLibXL-v145-x64-mt-sgd-1_42_0.xll` |
| windows-vs2026-x64-dynamic | Release | `build\windows-vs2026-x64-dynamic\xll\QuantLibXL-v145-x64-mt-1_42_0.xll` |
| windows-vs2026-x64-dynamic | Debug   | `build\windows-vs2026-x64-dynamic\xll\QuantLibXL-v145-x64-mt-gd-1_42_0.xll` |
| windows-vs2022-x64-static  | Release | `build\windows-vs2022-x64-static\xll\QuantLibXL-v143-x64-mt-s-1_42_0.xll` |
| windows-vs2022-x64-static  | Debug   | `build\windows-vs2022-x64-static\xll\QuantLibXL-v143-x64-mt-sgd-1_42_0.xll` |
| windows-vs2022-x64-dynamic | Release | `build\windows-vs2022-x64-dynamic\xll\QuantLibXL-v143-x64-mt-1_42_0.xll` |
| windows-vs2022-x64-dynamic | Debug   | `build\windows-vs2022-x64-dynamic\xll\QuantLibXL-v143-x64-mt-gd-1_42_0.xll` |
| windows-vs2026-Win32-static| Release | `build\windows-vs2026-Win32-static\xll\QuantLibXL-v145-mt-s-1_42_0.xll` |
| windows-vs2026-Win32-static| Debug   | `build\windows-vs2026-Win32-static\xll\QuantLibXL-v145-mt-sgd-1_42_0.xll` |

---

## 8  cmake file layout

```
CMakeLists.txt              <- root: wires together all subprojects
CMakePresets.json           <- configure and build presets
gensrc\
  CMakeLists.txt            <- optional gensrc code-generation step
  cmake\
    commonSettings.cmake    <- MSVC compile options, CRT selection (generic)
    DocsCommon.cmake        <- shared docs helper (tool discovery + target helpers)
    RunDoxygen.cmake        <- build-time driver that runs doxygen for one project
  Docs\CMakeLists.txt       <- gensrc-docs target
ObjectHandler\
  CMakeLists.txt            <- builds xlsdk, ohlib, ohxllib static libs
  Docs\CMakeLists.txt       <- ObjectHandler-docs target
QuantLibAddin\
  CMakeLists.txt            <- builds QuantLibObjects static lib
  Docs\CMakeLists.txt       <- QuantLibAddin-docs target
QuantLibXL\
  CMakeLists.txt            <- builds the XLL
  Docs\CMakeLists.txt       <- QuantLibXL-docs target
QuantLib\
  CMakeLists.txt            <- upstream QuantLib cmake (unchanged)
```

The shared cmake helpers live under `gensrc\cmake\` because gensrc is the root of
the project dependency chain (`gensrc <- ObjectHandler <- QuantLibAddin <-
QuantLibXL`): every project may depend on gensrc, so generic build code placed
there is visible to all of them without any of them having to reference a project
further down the chain.  In particular, ObjectHandler can be configured and built
standalone (see section 10) without any reference to QuantLibAddin, QuantLib or
QuantLibXL.

Each subproject keeps its own documentation build in its `Docs\CMakeLists.txt`,
so a project's docs are part of that standalone project (just like its code) and
can be built on their own.  The shared docs pieces are `DocsCommon.cmake` (tool
discovery and the `add_doxygen_docs` / `add_gensrc_docs` helpers) and
`RunDoxygen.cmake` (the per-project driver), both under `gensrc\cmake\`.

### Standalone subproject builds

Because every subproject's `CMakeLists.txt` guards its root-only logic with
`if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)`, a subproject can be
configured on its own, without the projects further down the dependency chain.
For example, ObjectHandler (which depends only on its upstream gensrc and on
Boost) can be built standalone — with no reference to QuantLibAddin, QuantLib or
QuantLibXL:

```powershell
# Just the ObjectHandler libraries
cmake -S ObjectHandler -B build\objecthandler-only ^
      -DBOOST_INCLUDEDIR=C:/path/to/boost/include ^
      -DBOOST_LIBRARYDIR=C:/path/to/boost/lib-md
cmake --build build\objecthandler-only --config Release

# ObjectHandler plus its documentation (needs Python + Doxygen, not Boost)
cmake -S ObjectHandler -B build\objecthandler-docs -DBUILD_DOCS=ON
cmake --build build\objecthandler-docs --target docs
```

This mirrors Build A, where `ObjectHandler\ObjectHandler.sln` builds a
standalone ObjectHandler.

---

## 9  Design notes

### Auto-link suppression

The ObjectHandler and QuantLibAddin sources include Boost-style auto-link
headers (`oh/auto_link.hpp`, `qlo/auto_link.hpp`, `xlsdk/auto_link.hpp`)
that emit `#pragma comment(lib, ...)` directives referencing tagged library
names (e.g. `QuantLibObjects-v145-x64-mt-s-1_42_0.lib`).  These conflict
with the cmake-managed link step.

Each auto_link header has been guarded with a `#ifndef` macro:

| Header                | Guard macro            | Defined by (PUBLIC, on) |
|-----------------------|------------------------|-------------------------|
| `oh/auto_link.hpp`    | `OH_NO_AUTO_LINK`      | `ohlib`                 |
| `qlo/auto_link.hpp`   | `QLADDIN_NO_AUTO_LINK` | `QuantLibObjects`       |
| `xlsdk/auto_link.hpp` | `XLSDK_NO_AUTO_LINK`   | `xlsdk`                 |

Each macro is defined as a **PUBLIC compile definition on the library that owns
the header** (via `target_compile_definitions`), so it is inherited by every
consumer of that library through the normal link graph and never leaks the other
projects' names into a project's build.  In particular, the ObjectHandler build
defines only `OH_NO_AUTO_LINK` and `XLSDK_NO_AUTO_LINK`; it has no knowledge of
`QLADDIN_NO_AUTO_LINK` (that belongs to QuantLibAddin), respecting the project
hierarchy.  The hand-maintained solution files do not define these macros, so
their behaviour is unchanged.

### QuantLib tagged layout

QuantLib's cmake `QL_TAGGED_LAYOUT` option, when ON, sets
`CMAKE_RELEASE_POSTFIX` and `CMAKE_DEBUG_POSTFIX` globally, which would
apply the tagged suffix to every cmake target in the build.  To avoid
this, `QL_TAGGED_LAYOUT` is set to OFF in the root CMakeLists.txt and the
postfix is applied only to `ql_library` via `set_target_properties`.

### XLL output name

The XLL `OUTPUT_NAME` uses cmake generator expressions to select the
correct runtime tag per configuration:

```
QuantLibXL-<toolset>[-x64]-<runtime-tag>-1_42_0.xll
```

`<toolset>` is derived from `MSVC_TOOLSET_VERSION` at configure time
(e.g. `v145` for VS 2026, `v143` for VS 2022).  The `-x64` platform tag is
added only for 64-bit builds (`CMAKE_SIZEOF_VOID_P EQUAL 8`) and omitted for
Win32, matching the hand-maintained solution's Win32 output name.
`<runtime-tag>` is `-mt-s` / `-mt-sgd` (static CRT) or `-mt` / `-mt-gd`
(dynamic CRT) depending on `MSVC_LINK_DYNAMIC_RUNTIME`.

---

## 10  Documentation (Doxygen)

The cmake build can also build the HTML documentation for gensrc,
ObjectHandler, QuantLibAddin and QuantLibXL, replicating the legacy automake
`make docs` target.  This works on both Windows and Linux/WSL.

### Prerequisites

- **Doxygen** (the `doxygen` executable on `PATH`).
- **Graphviz** (the `dot` executable on `PATH`) — optional, but enables the
  inheritance / collaboration diagrams.  When `dot` is not found the docs
  still build, just without the graphs.
- **Python 3** — used to run the doxyfile preprocessor and `gensrc`.

A C++ compiler and Boost are **not** required to build the documentation.

### Building the docs

There are two ways to drive the documentation build.

**A. Standalone (recommended when you only want the docs)** — each project's
`Docs` directory is itself a tiny standalone cmake project that needs only
Python, Doxygen and dot, so it works even on a machine that cannot compile the
libraries (no Boost / no C++ toolchain).  Configure the `Docs` directory of the
project you want and build its `docs` target:

```powershell
cmake -S ObjectHandler\Docs -B build\docs-objecthandler
cmake --build build\docs-objecthandler --target docs
```

The same pattern works for `gensrc\Docs`, `QuantLibAddin\Docs` and
`QuantLibXL\Docs`.  (The QuantLibXL *library* is Windows-only, but its
documentation builds on any platform.)

**B. As part of the main build** — add `-DBUILD_DOCS=ON` at configure
time, then build the aggregate `docs` target (or an individual project target):

```powershell
cmake --preset windows-vs2026-x64-static -DBUILD_DOCS=ON
cmake --build build\windows-vs2026-x64-static --target docs
```

On Linux/WSL the equivalent standalone build is:

```bash
cmake -S ObjectHandler/Docs -B build/docs-objecthandler
cmake --build build/docs-objecthandler --target docs
```

### Targets

In the main build (option B) the aggregate `docs` target and all four
per-project targets are available:

| Target               | Builds |
|----------------------|--------|
| `docs`               | all four projects below (aggregate) |
| `gensrc-docs`        | gensrc HTML docs |
| `ObjectHandler-docs` | ObjectHandler HTML docs |
| `QuantLibAddin-docs` | QuantLibAddin HTML docs |
| `QuantLibXL-docs`    | QuantLibXL HTML docs |

In a standalone `Docs` build (option A) the project exposes its own
`<project>-docs` target plus a convenience `docs` target that builds it.

Because this is a *pristine* checkout, the `Docs/auto.pages` Doxygen inputs for
ObjectHandler and QuantLibAddin do not exist yet.  Each project that needs them
defines its own gensrc step — `ObjectHandler-docs-gensrc` and
`QuantLibAddin-docs-gensrc` — which runs `gensrc.py -d` and is wired as a
prerequisite of that project's doc target.  QuantLibXL reuses
`QuantLibAddin-docs-gensrc` (it consumes the same QuantLibAddin auto.pages), so
a single `--target docs` produces everything from a clean tree.

### Output locations

The generated HTML is written under the build tree, keeping the source tree
clean.  In the main build (option B) all four appear under one build directory:

```
build\<preset>\gensrc\Docs\gensrc-docs\html\index.html
build\<preset>\ObjectHandler\Docs\ObjectHandler-docs\html\index.html
build\<preset>\QuantLibAddin\Docs\QuantLibAddin-docs\html\index.html
build\<preset>\QuantLibXL\Docs\QuantLibXL-docs\html\index.html
```

In a standalone build (option A) the output is under that project's build
directory, e.g. `build\docs-objecthandler\ObjectHandler-docs\html\index.html`.
A per-project `doxywarnings.txt` is written next to each `html` directory.

### Design notes

- Each subproject owns its documentation build in its own
  `<project>\Docs\CMakeLists.txt`, consistent with the repository convention
  that every subdirectory is a self-contained project.  Two pieces are shared
  under `gensrc\cmake\` (gensrc is the upstream of every project, so generic
  helpers placed there are visible to all without creating a downstream
  dependency):
  - **`gensrc\cmake\DocsCommon.cmake`** discovers the tools (Python 3, Doxygen,
    optional dot) once and provides the `add_doxygen_docs` and
    `add_gensrc_docs` helper functions used by each `Docs\CMakeLists.txt`.
  - **`gensrc\cmake\RunDoxygen.cmake`** is a `cmake -P` script invoked once per
    project.  It runs `ObjectHandler/Docs/preprocess_doxyfile.py` (which sets
    `STRIP_FROM_PATH` and, on posix, `HAVE_DOT=YES` / `GENERATE_HTMLHELP=NO`),
    appends a small block of overrides that redirect `OUTPUT_DIRECTORY` into the
    build tree, runs doxygen with the working directory set to the project's
    `Docs` directory (the `.doxy` files use relative INPUT / header paths), and
    finally copies `tabs.css`, `ql.css` and the `images` into the html output —
    exactly the steps the old `Makefile.am` performed.
- The older `.doxy` files predate Doxygen 1.9, so Doxygen emits a number of
  "obsolete tag" warnings; these are harmless.  `WARN_AS_ERROR` is `NO`, so
  content warnings do not fail the build.

---

## 11  Continuous integration (GitHub Actions)

The workflow `.github/workflows/build-xll.yml` builds the **static-CRT Release**
XLL for **both x64 and Win32** on a GitHub-hosted `windows-latest` runner,
bundles them with the example spreadsheets into a versioned package, and can
optionally publish that package as a **GitHub Release**.

| Property | Value |
|----------|-------|
| Trigger | `workflow_dispatch` (manual) only |
| Runner | `windows-latest` (Visual Studio 2026, toolset **v145**) |
| Variants | static-CRT Release, `x64` and `Win32` (matrix) |
| Inputs | `quantlib_ref` (default `master`), `boost_version` (default `1.89.0`), `arch` (`both`/`x64`/`Win32`, default `both`), `make_release` (default `false`), `release_tag` (default `v1.42.0`) |
| Output | per-arch XLL artifacts, a `QuantLibXL-<version>` package artifact, and (optionally) a GitHub Release |

The runner uses VS 2026, so the CI XLLs are tagged `v145`
(`QuantLibXL-v145-x64-mt-s-1_42_0.xll` and `QuantLibXL-v145-mt-s-1_42_0.xll`),
matching a local VS 2026 build.

### Building Boost under VS 2026 (v145)

`windows-latest` maps to the VS 2026 (v145) image, which Boost 1.89's build
tooling does not yet know about, so the workflow drives it explicitly:

1. `bootstrap.bat msvc` builds the `b2` engine using `cl.exe` directly (its
   auto-detection only recognises up to `vc143` and otherwise fails with
   "Unknown toolset: vcunk").
2. A generated `user-config.jam` registers the toolset under the **known**
   version `14.3`, but points both the compiler path **and** the `<setup>`
   script at the real v145 `cl.exe` and `vcvarsall.bat` (located via
   `vswhere`).  Without the explicit `<setup>`, `b2` cannot build the
   `msvc-setup` target and silently skips every object file.

The resulting Boost libraries carry a `-vc143-` filename tag (a label only —
they are genuinely compiled by the v145 compiler).  The XLL itself is compiled
by CMake with the real v145 toolset, so the `v145` artifact name is accurate.

### Package and release

After both build legs succeed, a `package` job assembles a versioned tree and
uploads it as the `QuantLibXL-<version>` artifact (itself a zip).  It extracts
to a single rooted folder:

```
QuantLibXL-1.42.0/
  Addins/
    x64/    QuantLibXL-v145-x64-mt-s-1_42_0.xll
    Win32/  QuantLibXL-v145-mt-s-1_42_0.xll
  Examples/ InterestRateDerivatives.xlsx, YieldCurveBootstrapping.xlsx
```

The x64 and Win32 XLLs are placed in separate `Addins/` subfolders so end users
must consciously pick the one matching their Excel bitness.

When the **`make_release`** input is checked, the same job zips that tree into
`QuantLibXL-<version>.zip` and publishes a **GitHub Release** with
`softprops/action-gh-release`.  The release step:

- requires `arch=both` (so the package contains both XLLs) and a `release_tag`;
- **creates the tag itself** from the commit the workflow ran on — you do not
  push a tag manually;
- attaches the zip as a publicly downloadable asset that never expires.

Standard release flow: merge the CI branch into the default branch, then on
**Actions → Build QuantLibXL XLL → Run workflow** choose **Use workflow from:
`main`**, tick `make_release`, set `release_tag` (e.g. `v1.42.0`) and run.  The
release is cut from `main`, and the tag marks the exact commit that produced the
artifacts.  Re-running with the same tag updates the existing release.

Publishing a release requires the workflow's `contents: write` permission (set
in the workflow) and the repository's **Settings → Actions → General → Workflow
permissions** to allow read/write.

### Build steps

The workflow is self-contained and needs no `CMakeUserPresets.json`.  Each
matrix leg:

1. **Clones QuantLib** from `https://github.com/<owner>/QuantLib` where
   `<owner>` is the owner of the repository running the workflow, so the
   QuantLib fork is taken from the same owner as this repository.  The sources
   are placed in the camel-case `QuantLib\` folder the build expects (QuantLib
   is `.gitignore`'d and never committed here).
2. **Builds Boost** (from source, only the required components, static runtime)
   for the leg's architecture — x64 staged to `boost\stage\lib`, Win32 staged
   to `boost\stage32\lib` and built from an x86 developer environment.  The
   result is cached with `actions/cache` keyed on version + architecture, so the
   slow Boost build happens only on the first run.
3. **Configures and builds** with `-DRUN_GENSRC=ON` (a Full build; the
   generated sources are `.gitignore`'d) passing `BOOST_INCLUDEDIR` /
   `BOOST_LIBRARYDIR` on the cmake command line, then builds the `QuantLibXL`
   target in `Release`.
4. **Verifies and uploads** the expected XLL, failing the job if it is missing.
