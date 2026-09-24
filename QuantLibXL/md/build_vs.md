# Build QuantLibXL from a git clone using Visual Studio solution files

This document explains how to build the QuantLibXL Excel add-in (the `.xll`
file) and its prerequisites from a **git clone**, using the hand-maintained
Visual Studio solution files.

For other options:

- To download a compiled QuantLibXL XLL, see
  <https://www.quantlib.org/quantlibxl/installation.html>.
- To compile various flavors of QuantLibAddin (including the QuantLibXL XLL) from
  an official release of source code (zip files / tarballs), see
  <https://www.quantlib.org/quantlibaddin/tutorials.html>.

---

## 1 Choose a build - Basic or Full

Two solution files are provided. Both produce the same `.xll`; they differ only
in whether the auto-generated source code is regenerated.

| Build | Solution file | Description |
|---|---|---|
| Basic | `QuantLibXL\QuantLibXL_basic.sln` | Compiles the auto-generated source files that are already present in the clone. Does **not** require Python or gensrc. Use this for normal compilation. |
| Full  | `QuantLibXL\QuantLibXL_full.sln`  | Runs gensrc first to regenerate all auto-generated source files, then compiles. Requires Python 3. Use this only if you have changed the gensrc metadata (the XML files under `ObjectHandler\gensrc\metadata` or `QuantLibAddin\gensrc\metadata`). |

Unless you are editing the add-in's metadata, use the **Basic** build.

---

## 2 Prerequisites

### 2.1 Visual Studio

Any recent version of Visual Studio with the **"Desktop development with C++"**
workload installed will work. The platform toolset is selected automatically by
`QuantLib\QuantLib.props` from the `VisualStudioVersion` of the Visual Studio
instance that opens the solution:

| Visual Studio | `VisualStudioVersion` | Platform toolset |
|---|---|---|
| VS 2017 (v15) | 15.0 | v141 |
| VS 2019 (v16) | 16.0 | v142 |
| VS 2022 (v17) | 17.0 | v143 |
| VS 2026 (v18) | 18.0 | v145 |

No action is required: open the solution in any of these versions and the
correct toolset is selected for you. See the Boost note in section 2.2 about
keeping the Boost libraries and the toolset in step.

### 2.2 Boost

QuantLib, ObjectHandler, QuantLibAddin and QuantLibXL all depend on Boost. You
need the **compiled** Boost libraries, not just the headers. Building Boost is
outside the scope of this document; these instructions assume you already have
a Boost build available.

The build requires **Boost 1.58 or later**. It was tested using **Boost 1.83**.

For purposes of this HOWTO, it is assumed that you have installed Boost to:

```
C:\repos\boost_1_83_0
```

with headers under `C:\repos\boost_1_83_0` (i.e. the folder that contains the
`boost\` sub-directory) and the compiled libraries under
`C:\repos\boost_1_83_0\stage\lib`. Modify that path as necessary for your own
environment.

How Boost is wired into the build is described in section 4 - this is the step
most likely to trip you up, so read it carefully.

> **Boost library / toolset compatibility.** QuantLib and Boost use Boost's
> *auto-linking*: the name of the `.lib` it asks for encodes a compiler
> toolset (e.g. `libboost_*-vc143-mt-s-x64-1_83.lib`). The toolset tag in the
> library name must be one that your Boost build provides. Boost binaries are
> generally forward-compatible across adjacent MSVC toolsets (for example a
> `vc143` build links successfully from VS 2026 / v145), but if you ever see an
> `LNK1104: cannot open file 'libboost_...'` error it means the auto-linked
> library name does not exist in your `AdditionalLibraryDirectories`; provide a
> Boost build whose toolset tag matches, or add the correct directory.

### 2.3 Python 3 (Full build only)

The **Full** build runs gensrc, which is a Python 3 script. Install Python 3
and make sure `python` is on the `PATH` (i.e. `python --version` works from a
command prompt). See <https://www.python.org/>.

If `.py` files are associated with the Python executable, gensrc builds with no
further configuration. Otherwise, edit both of these files...

```
ObjectHandler\gensrc\Makefile.vc
QuantLibAddin\gensrc\Makefile.vc
```

...and set the `PYTHON` variable to the full path of the Python executable, e.g:

```
PYTHON=C:\Program Files\Python312\python.exe
```

The **Basic** build does not use Python or gensrc, so you can skip this section
for a Basic build.

---

## 3 Acquire the source code

The solution files refer to the prerequisite projects using **relative paths**,
so the directory layout matters. After cloning you must end up with this
layout (`QuantLibAddin` is the repository root - you may name the outer folder
anything you like):

```
QuantLibAddin\
  gensrc\         (required for the Full build only)
  ObjectHandler\
  QuantLib\       (cloned separately - see below)
  QuantLibAddin\
  QuantLibXL\
```

### 3.1 Clone the main repository

`gensrc`, `ObjectHandler`, `QuantLibAddin` and `QuantLibXL` are all contained in
a single repository. Clone it:

```
git clone <repo-host>/<owner>/QuantLibAddIn
```

This creates the `QuantLibAddin` folder containing the four sub-projects above.

### 3.2 Clone QuantLib into the working tree

QuantLib is maintained as a **separate** repository and is deliberately
excluded from the main repository (it is listed in `.gitignore`). You must
clone it yourself into a sub-folder named exactly `QuantLib` inside the working
tree you just cloned:

```
cd QuantLibAddin
git clone <repo-host>/<owner>/QuantLib QuantLib
```

The folder name `QuantLib` **is case sensitive** and must be spelled exactly as
shown - `QuantLib`, not `quantlib` or `QUANTLIB`, and with no version suffix -
because the solution files reference `..\QuantLib` explicitly.  So if you are
cloning a repo named anything other than 'QuantLib', then you need that final
argument in order to create the subdirectory name correctly.

After this step the layout in section 3 should be in place.

---

## 4 Configure Boost (required)

There are **two** machine-local Boost configuration points, because the build
is really two layers: QuantLib (the lowest layer) configures Boost for itself,
and ObjectHandler / QuantLibAddin / QuantLibXL share a separate sheet. Neither
file is part of the clone - you create both by hand - and both are git-ignored.

| Layer | File you create | Used by |
|---|---|---|
| QuantLib | `QuantLib\MSVC\quantlib.x64.user.props` and `quantlib.Win32.user.props` | QuantLib only |
| Add-in stack | `ObjectHandler\boost.props` | ObjectHandler, QuantLibAddin, QuantLibXL |

The add-in sheet lives under `ObjectHandler` (not at the repository root) on
purpose: QuantLibAddin and QuantLibXL already depend on ObjectHandler, so
importing `..\ObjectHandler\boost.props` introduces no new cross-project
dependency. No project references the repository root, and QuantLib - which
sits below ObjectHandler - never references it either.

### 4.1 Configure Boost for QuantLib

`QuantLib.vcxproj` imports two per-platform property sheets **unconditionally**:

```
QuantLib\MSVC\quantlib.Win32.user.props
QuantLib\MSVC\quantlib.x64.user.props
```

The `MSVC` folder is not part of the QuantLib clone, so if the files are absent
the build stops immediately - before any compilation - with:

```
error MSB4019: The imported project "...\QuantLib\MSVC\quantlib.x64.user.props"
was not found.
```

Create the `QuantLib\MSVC` folder and both files, pointing them at your Boost
build. `quantlib.x64.user.props` (used for both **x64** configurations):

```xml
<?xml version="1.0" encoding="utf-8"?>
<Project DefaultTargets="Build" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <PropertyGroup>
    <IncludePath>C:\repos\boost_1_83_0;$(IncludePath)</IncludePath>
    <LibraryPath>C:\repos\boost_1_83_0\stage\lib;$(LibraryPath)</LibraryPath>
  </PropertyGroup>
</Project>
```

`quantlib.Win32.user.props` (used for both **Win32** configurations - point
`LibraryPath` at your 32-bit Boost libraries):

```xml
<?xml version="1.0" encoding="utf-8"?>
<Project DefaultTargets="Build" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <PropertyGroup>
    <IncludePath>C:\repos\boost_1_83_0;$(IncludePath)</IncludePath>
    <LibraryPath>C:\path\to\boost\win32\lib;$(LibraryPath)</LibraryPath>
  </PropertyGroup>
</Project>
```

`IncludePath` points at the directory that **contains** the `boost\` header
sub-folder; `LibraryPath` points at the directory that contains the compiled
`.lib` files. Even if you build x64 only, create `quantlib.Win32.user.props`
too (with any valid content) - it is imported unconditionally, so its absence
also triggers the MSB4019 error above.

### 4.2 Configure Boost for ObjectHandler, QuantLibAddin and QuantLibXL

These three projects do **not** import the QuantLib `MSVC` sheets. They each
import `ObjectHandler\boost.props` instead (the import is already present in the
project files, using a relative path such as `..\ObjectHandler\boost.props`).
If this file is missing the build fails with a compile error in the first
project that includes a Boost header, for example:

```
error C1083: Cannot open include file: 'boost/config.hpp': No such file or directory
```

Create `ObjectHandler\boost.props`, adjusting the paths to your Boost build:

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

Notes:

- `AdditionalIncludeDirectories` points at the directory that **contains** the
  `boost\` header sub-folder (so that `#include <boost/config.hpp>` resolves).
- `AdditionalLibraryDirectories` points at the directory that contains the
  compiled `.lib` files. Boost auto-linking selects the correct `.lib` from
  that directory, so you do not list individual libraries. The path is split
  per platform; fill in the `Win32` block only if you build Win32.
- `<LanguageStandard>stdcpp17</LanguageStandard>` is required: the QuantLib
  headers need C++17, and the ObjectHandler / QuantLibAddin / QuantLibXL
  projects do not set it themselves.

> **Tip.** The Boost paths in `ObjectHandler\boost.props` and in the QuantLib
> `MSVC` sheets normally point at the same Boost build. Keeping them in step
> avoids mixing two different Boost versions into one XLL.

---

## 5 Build QuantLibXL

1. Open the solution in Visual Studio:
   - **Basic** build: `QuantLibXL\QuantLibXL_basic.sln`
   - **Full** build: `QuantLibXL\QuantLibXL_full.sln`
2. On the toolbar, choose the **solution configuration** and **platform**. Both
   solutions provide these configurations for **x64** and **Win32**:

   | Configuration | Runtime | Add-in produced |
   |---|---|---|
   | `Release` | dynamic (`/MD`) | release XLL, dynamic CRT |
   | `Debug` | dynamic (`/MDd`) | debug XLL, dynamic CRT |
   | `Release (static runtime)` | static (`/MT`) | release XLL, static CRT |
   | `Debug (static runtime)` | static (`/MTd`) | debug XLL, static CRT |

   The static-runtime configurations produce a self-contained XLL that does not
   require the Visual C++ runtime to be installed on the target machine, which
   is usually what you want for distribution. For a first build, select
   **`Release (static runtime)`** and **`x64`**, and make sure your Boost build
   provides the matching static-runtime x64 libraries.
3. Select **Build > Build Solution** (`Ctrl+Shift+B`).

For the **Full** build the gensrc projects (`ohgensrc`, `qlgensrc`) run first
and regenerate the source code; the remaining projects then compile in
dependency order. For the **Basic** build the gensrc projects are absent and
compilation starts immediately.

---

## 6 Output

On success the add-in is written to `QuantLibXL\xll\`. The file name encodes the
platform toolset, the platform, the runtime variant and the version. The table
below shows the names for a **VS 2026 (v145)** build; the toolset tag reflects
whichever Visual Studio version you built with (e.g. `v143` for VS 2022):

| Configuration | Platform | Output file |
|---|---|---|
| `Release (static runtime)` | x64 | `QuantLibXL-v145-x64-mt-s-1_42_0.xll` |
| `Debug (static runtime)`   | x64 | `QuantLibXL-v145-x64-mt-sgd-1_42_0.xll` |
| `Release`                  | x64 | `QuantLibXL-v145-x64-mt-1_42_0.xll` |
| `Debug`                    | x64 | `QuantLibXL-v145-x64-mt-gd-1_42_0.xll` |
| `Release (static runtime)` | Win32 | `QuantLibXL-v145-mt-s-1_42_0.xll` |
| `Debug (static runtime)`   | Win32 | `QuantLibXL-v145-mt-sgd-1_42_0.xll` |
| `Release`                  | Win32 | `QuantLibXL-v145-mt-1_42_0.xll` |
| `Debug`                    | Win32 | `QuantLibXL-v145-mt-gd-1_42_0.xll` |

---

## 7 Troubleshooting

**`error C1083: Cannot open include file: 'boost/config.hpp'`** in the
ObjectHandler, QuantLibAddin or QuantLibXL projects (while QuantLib itself
compiles). `ObjectHandler\boost.props` is missing or its include path is wrong,
so the Boost headers are not on the include path for those projects. Re-check
section 4.2: the file must be named `boost.props`, sit in the `ObjectHandler`
directory, and its `AdditionalIncludeDirectories` must point at the folder that
contains the `boost\` sub-directory.

**`error MSB4019: The imported project "...\QuantLib\MSVC\quantlib.x64.user.props"
was not found`** before compilation starts. The QuantLib Boost sheets are
missing. Create them as described in section 4.1.

**`error LNK1104: cannot open file 'libboost_...lib'`** at the link stage. The
Boost library directory is missing or wrong, or the auto-linked library name
(which encodes the toolset, e.g. `vc143`) is not present in that directory. Fix
`AdditionalLibraryDirectories` in `ObjectHandler\boost.props` (and the
`LibraryPath` in the QuantLib `MSVC` sheets), or provide a Boost build whose
toolset matches - see section 2.2.

## 8 Load the add-in in Excel

1. Open Excel.
2. Go to **File > Options > Add-ins**.
3. Set **Manage** to **Excel Add-ins** and click **Go**.
4. Click **Browse**, navigate to the `.xll` produced in section 6, and click
   **OK**.

Alternatively, drag and drop the `.xll` onto an open Excel window.

## 9 Load a sample workbook

With the add-in loaded, open one of the sample workbooks to confirm the
functions are available and to see the add-in in action. The best ones to start
with are:

```
QuantLibXL\StandaloneExamples\Analytics\YieldCurveBootstrapping.xlsx
QuantLibXL\StandaloneExamples\Analytics\InterestRateDerivatives.xlsx
```

Open one of these workbooks in the same Excel instance that has the add-in
loaded, then recalculate (`Ctrl+Alt+F9`) to evaluate the QuantLib functions.

