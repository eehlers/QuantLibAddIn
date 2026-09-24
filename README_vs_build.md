# Building QuantLibXL from Source Code (Visual Studio)

This document explains how to build the QuantLibXL Excel add-in and its
prerequisites from source code using the hand-maintained Visual Studio
solution files.

> **Scope.** This covers the hand-maintained Visual Studio solution files. For
> the cmake build see [`README_cmake_build.md`](README_cmake_build.md). The build was tested with VS 2026
> (v145 toolset) and VS 2022 (v143 toolset). Both **x64** and **Win32**
> (32-bit) platforms build successfully; x64 is the default used in practice
> (see section 5). Output XLLs are written to `QuantLibXL\xll\`. See sections
> 2.2 and 5 for full details.

---

## 1 Choose a Build - Basic or Full

Two solution files are provided:

| Build | Solution file | What it does |
|---|---|---|
| Basic | `QuantLibXL\QuantLibXL_basic.sln` | Compiles pre-existing auto-generated source files. Does not require Python or gensrc. |
| Full | `QuantLibXL\QuantLibXL_full.sln` | Runs gensrc first to regenerate all auto-generated source files, then compiles. Requires Python 3 and nmake. |

Use the Basic build for normal compilation. Use the Full build only if you
have changed the gensrc metadata (the XML files under
`ObjectHandler\gensrc\metadata` or `QuantLibAddin\gensrc\metadata`).

---

## 2 Prerequisites

### 2.1 Visual Studio

VS 2026 (v145 toolset) or VS 2022 (v143 toolset) is required. The
"Desktop development with C++" workload must be installed. The platform
toolset is selected automatically from QuantLib\QuantLib.props based on
the `VisualStudioVersion` environment variable.

| Visual Studio version | VisualStudioVersion | Toolset tag |
|---|---|---|
| VS 2026 (version 18) | 18.0 | v145 |
| VS 2022 (version 17) | 17.0 | v143 |

For the Full build, the **"Desktop development with C++"** workload must
include the **C++ CMake tools** component, which provides `nmake.exe` on
the PATH inside a Developer Command Prompt. Alternatively, nmake can be
found at:

```
C:\Program Files\Microsoft Visual Studio\<version>\Professional\VC\Tools\MSVC\<toolset>\bin\Hostx64\x64\nmake.exe
```

### 2.2 Boost

The build depends on the **compiled** Boost libraries, not just the headers.
Building Boost is outside the scope of this document; these instructions assume
you already have a Boost build available.

The build requires **Boost 1.58 or later**. It was tested using **Boost 1.83**.
For purposes of this HOWTO, it is assumed that you have installed Boost to:

```
C:\repos\boost_1_83_0
```

with headers under `C:\repos\boost_1_83_0` (the folder that contains the
`boost\` sub-directory) and the compiled libraries under
`C:\repos\boost_1_83_0\stage\lib`. Modify that path as necessary for your own
environment.

Boost is wired into the build through **two** machine-local property sheets,
one per layer. Neither is part of the clone — you create both by hand — and
both are git-ignored:

| Layer | File you create | Used by |
|---|---|---|
| QuantLib | `QuantLib\MSVC\quantlib.x64.user.props` and `quantlib.Win32.user.props` | QuantLib only |
| Add-in stack | `ObjectHandler\boost.props` | ObjectHandler, QuantLibAddin, QuantLibXL |

The add-in sheet lives under `ObjectHandler` (not at the repository root)
because QuantLibAddin and QuantLibXL already depend on ObjectHandler; importing
`..\ObjectHandler\boost.props` adds no new cross-project dependency, and no
project references the repository root.

`QuantLib.vcxproj` imports both `QuantLib\MSVC` sheets **unconditionally**, so
both must exist even if you build x64 only (an absent file triggers an `MSB4019`
error before compilation). `quantlib.x64.user.props` (used for both **x64**
configurations):

```xml
<?xml version="1.0" encoding="utf-8"?>
<Project DefaultTargets="Build" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <PropertyGroup>
    <IncludePath>C:\repos\boost_1_83_0;$(IncludePath)</IncludePath>
    <LibraryPath>C:\repos\boost_1_83_0\stage\lib;$(LibraryPath)</LibraryPath>
  </PropertyGroup>
</Project>
```

`quantlib.Win32.user.props` is the same, with `LibraryPath` pointing at your
32-bit Boost libraries (or any valid content if you build x64 only).

`ObjectHandler\boost.props` (used by ObjectHandler, QuantLibAddin and
QuantLibXL) also sets `<LanguageStandard>stdcpp17</LanguageStandard>`, which is
required by the QuantLib headers:

```xml
<?xml version="1.0" encoding="utf-8"?>
<Project xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemDefinitionGroup>
    <ClCompile>
      <AdditionalIncludeDirectories>C:\repos\boost_1_83_0;%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>
      <LanguageStandard>stdcpp17</LanguageStandard>
    </ClCompile>
  </ItemDefinitionGroup>
  <ItemDefinitionGroup Condition="'$(Platform)'=='x64'">
    <Link>
      <AdditionalLibraryDirectories>C:\repos\boost_1_83_0\stage\lib;%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>
    </Link>
  </ItemDefinitionGroup>
  <ItemDefinitionGroup Condition="'$(Platform)'=='Win32'">
    <Link>
      <AdditionalLibraryDirectories>C:\path\to\boost\win32\lib;%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>
    </Link>
  </ItemDefinitionGroup>
</Project>
```

> **Boost library / toolset compatibility.** QuantLib and Boost use Boost's
> *auto-linking*: the name of the `.lib` it asks for encodes a compiler toolset
> (e.g. `libboost_*-vc143-mt-s-x64-1_83.lib`). The toolset tag in the library
> name must be one that your Boost build provides. Boost binaries are generally
> forward-compatible across adjacent MSVC toolsets (for example a `vc143` build
> links successfully from VS 2026 / v145), but if you ever see an
> `LNK1104: cannot open file 'libboost_...'` error it means the auto-linked
> library name does not exist in your library directory; provide a Boost build
> whose toolset tag matches, or add the correct directory.

### 2.3 Python 3 (Full build only)

The Full build runs gensrc, a Python 3 script. Python must be installed and
`python` must be on the PATH (i.e. `python --version` works from a command
prompt). If `.py` files are not associated with the Python executable, edit
`ObjectHandler\gensrc\Makefile.vc` and `QuantLibAddin\gensrc\Makefile.vc`
and set the `PYTHON` variable to the full path of the Python executable:

```
PYTHON=C:\Program Files\Python312\python.exe
```

---

## 3 Repository Layout

The solution files use relative paths to locate dependent projects. The
expected layout after cloning this repository is:

```
QuantLibAddin\
  gensrc\               # code-generation framework
  ObjectHandler\        # object repository (also holds boost.props — see section 2.2)
  QuantLib\             # QuantLib C++ analytics library (cloned separately — see below)
  QuantLibAddin\        # QuantLib C++ wrapper
  QuantLibXL\           # Excel XLL
```

QuantLib is maintained as a **separate** repository and is deliberately excluded
from the main repository (it is listed in `.gitignore`). Clone it yourself into
a sub-folder named exactly `QuantLib` inside the working tree:

```
cd QuantLibAddin
git clone <repo-host>/<owner>/QuantLib QuantLib
```

The folder name `QuantLib` **is case sensitive** and must be spelled exactly as
shown — `QuantLib`, not `quantlib` or `QUANTLIB`, and with no version suffix —
because the solution files reference `..\QuantLib` explicitly. The explicit
`QuantLib` argument on the `git clone` command above ensures this; without it
git would create a folder named `quantlib` from the URL.

---

## 4 Build Steps

### Step 1 — Create the Boost property sheets

Create the two machine-local Boost property sheets described in section 2.2,
pointing them at your Boost build:

- `QuantLib\MSVC\quantlib.x64.user.props` and `quantlib.Win32.user.props`
  (used by QuantLib).
- `ObjectHandler\boost.props` (used by ObjectHandler, QuantLibAddin and
  QuantLibXL).

Both files are git-ignored and are not part of the clone.

### Step 2 — Open the solution

- **Basic build**: open `QuantLibXL\QuantLibXL_basic.sln` in Visual Studio.
- **Full build**: open `QuantLibXL\QuantLibXL_full.sln` in Visual Studio.

### Step 3 — Select configuration and platform

In the Visual Studio toolbar select the desired configuration and platform
(**x64** or **Win32**):

| Goal | Configuration | Runtime | Platform |
|---|---|---|---|
| Dynamic-runtime release XLL | **Release** | Dynamic (`/MD`) | **x64** or **Win32** |
| Dynamic-runtime debug XLL | **Debug** | Dynamic (`/MDd`) | **x64** or **Win32** |
| Static-runtime release XLL | **Release (static runtime)** | Static (`/MT`) | **x64** or **Win32** |
| Static-runtime debug XLL | **Debug (static runtime)** | Static (`/MTd`) | **x64** or **Win32** |

Use **x64** unless you specifically need a 32-bit add-in: the Win32
configurations build successfully but require 32-bit Boost libraries, and the
resulting 32-bit XLL can only be loaded by a 32-bit Excel (see section 5).

### Step 4 — Build

Choose **Build > Build Solution** (`Ctrl+Shift+B`).

For the Full build, the gensrc projects (`ohgensrc`, `qlgensrc`) run first,
invoking `nmake` and Python to regenerate source files. Compilation of all
other projects follows automatically in dependency order:
`QuantLib` → `xlsdk`, `apr`, `aprutil`, `log4cxx`, `ohxllib` →
`QuantLibObjects` → `QuantLibXLStatic`.

---

## 5 Output

On a successful build the XLL is written to `QuantLibXL\xll\`. The filename
encodes the toolset, platform, configuration and version:

| Configuration | Platform | Output filename |
|---|---|---|
| Release | x64 | `QuantLibXL-v145-x64-mt-1_42_0.xll` |
| Release (static runtime) | x64 | `QuantLibXL-v145-x64-mt-s-1_42_0.xll` |
| Debug | x64 | `QuantLibXL-v145-x64-mt-gd-1_42_0.xll` |
| Debug (static runtime) | x64 | `QuantLibXL-v145-x64-mt-sgd-1_42_0.xll` |
| Release | Win32 | `QuantLibXL-v145-mt-1_42_0.xll` |
| Release (static runtime) | Win32 | `QuantLibXL-v145-mt-s-1_42_0.xll` |
| Debug | Win32 | `QuantLibXL-v145-mt-gd-1_42_0.xll` |
| Debug (static runtime) | Win32 | `QuantLibXL-v145-mt-sgd-1_42_0.xll` |

The toolset tag (`v145`, `v143`, …) is determined automatically from the
Visual Studio version used to open the solution. Building with VS 2022
produces separate XLL files (v143-tagged) that coexist with the VS 2026
builds (v145-tagged) in the same xll\ directory.

The intermediate static libraries (QuantLib, QuantLibObjects,
ObjectHandler-xllib, xlsdk) do **not** include a toolset tag in their
filenames. A v143 build therefore overwrites the v145 intermediates and
vice versa. This is harmless provided both toolset versions are not built
in an interleaved fashion; build all configurations for one toolset before
switching to the other.

The basic and full builds share the same output filenames and overwrite each
other. Build full only when gensrc metadata has changed; use basic otherwise.

> **Win32 (32-bit) build.** Both solutions declare the four Win32
> configurations alongside the x64 ones (hence the Win32 rows in the table
> above). The Win32 build has been verified: the `Release (static runtime)`
> configuration of both the basic and full solutions builds successfully with
> VS 2026 (v145), producing a genuine 32-bit (PE32) XLL. x64 nonetheless
> remains the default this project uses in practice. Two caveats if you build
> Win32:
>
> - Point the `Win32` Boost property sheets (section 2.2) at 32-bit Boost
>   libraries (e.g. `libboost_*-vc143-mt-s-x32-1_83.lib`).
> - A 32-bit XLL can only be loaded by a 32-bit Excel; 64-bit Excel cannot load
>   it.
>
> This Win32 support applies to the **hand-maintained Visual Studio solution
> files only**. The cmake build is currently **x64 only** (see
> [`README_cmake_build.md`](README_cmake_build.md)); use the Visual Studio
> solutions if you need a 32-bit add-in.

---

## 5.1 Command-line builds (MSBuild)

To build from the command line without opening Visual Studio, set
`VisualStudioVersion` so that `QuantLib.props` selects the correct toolset,
and add `nmake.exe` to the PATH for Full builds:

For VS 2026 (v145):

```powershell
$env:VisualStudioVersion = "18.0"
$msbuild = "C:\Program Files\Microsoft Visual Studio\18\Professional\MSBuild\Current\Bin\MSBuild.exe"

&$msbuild "<repo>\QuantLibXL\QuantLibXL_basic.sln" `
    /p:Configuration="Release (static runtime)" /p:Platform=x64 /m /nologo
```

For VS 2022 (v143), use that installation's MSBuild and pass `VisualStudioVersion=17.0`
so that QuantLib.props selects the v143 toolset instead of v145:

```powershell
$env:VisualStudioVersion = "17.0"
$msbuild = "C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe"

&$msbuild "<repo>\QuantLibXL\QuantLibXL_basic.sln" `
    /p:Configuration="Release (static runtime)" /p:Platform=x64 `
    /p:VisualStudioVersion=17.0 /m /nologo
```

Replace `<repo>` with the full path of your clone (the folder that contains
`QuantLibXL`). Configuration names with spaces (e.g. `"Release (static
runtime)"`) must be quoted. The `/m` flag enables parallel compilation. As a
rough guide, incremental dynamic-runtime builds complete in about a minute,
while a clean static-runtime build can take far longer:

| Configuration | Basic | Full |
|---|---|---|
| Release / Debug | ~30 s (incremental) | ~1 min (incremental) |
| Release (static) / Debug (static) | ~20 min (clean) | ~40 min (clean) |

Static-runtime clean builds are slow because QuantLib itself must be
recompiled from scratch for the new runtime library setting.

---

## 6 Known Issues

### mspdbsrv PDB file lock

**Symptom:** A Debug-configuration build fails with `MSB3027: Could not copy
"...\build\...\Foo.pdb" to "...\lib\Foo.pdb" — being used by another
process`.

**Root cause:** `mspdbsrv.exe` (the MSVC PDB server, spawned by `cl.exe /MP`
to serialise concurrent writes from parallel compiler processes) keeps the PDB
file **open after compilation finishes**. It lingers for reuse by incremental
builds, holding an exclusive write handle on the PDB that lives in the build
intermediate directory (`IntDir`). MSBuild's post-build step tries to copy
that file to the output (`OutDir` / `lib\`), but mspdbsrv's handle prevents
it.

**Fix (applied):** The `<ProgramDataBaseFileName>` for all Debug configurations
in QuantLib.vcxproj and QuantLibObjects3.vcxproj has been set to
`$(OutDir)$(TargetName).pdb` — that is, the final `lib\` path — rather than
the intermediate `build\` directory. mspdbsrv writes the PDB directly to its
final destination; there is no post-build copy step, so the lock is
irrelevant.

This means the `lib\` PDB is held open by mspdbsrv while the build is
running, but it is already in the right place. Subsequent builds can
overwrite it once mspdbsrv has finished with it (which happens naturally
before the next compilation begins).

---

## 7 Loading the Add-in in Excel

1. Open Excel.
2. Go to **File > Options > Add-ins**.
3. At the bottom, set **Manage** to **Excel Add-ins** and click **Go**.
4. Click **Browse** and navigate to the `.xll` file produced above.
5. Click **OK**.

Alternatively, drag and drop the `.xll` file onto an open Excel workbook.

