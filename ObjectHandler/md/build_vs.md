
# Build ObjectHandler from a git clone using Visual Studio solution files

This document explains how to build ObjectHandler - the object repository and
Excel-add-in framework that QuantLibAddin and QuantLibXL are built on - from a
**git clone**, using the hand-maintained Visual Studio solution file.

ObjectHandler is normally built as a prerequisite of QuantLibXL. If your goal is
the QuantLibXL Excel add-in, follow [../../QuantLibXL/md/build_vs.md](../../QuantLibXL/md/build_vs.md) instead - it
builds ObjectHandler for you as part of the dependency chain. Use this document
when you want to build or test ObjectHandler on its own.

For other options:

- To download a compiled QuantLibXL XLL, see
  <https://www.quantlib.org/quantlibxl/installation.html>.
- To compile various flavors of QuantLibAddin (including the QuantLibXL XLL) from
  an official release of source code (zip files / tarballs), see
  <https://www.quantlib.org/quantlibaddin/tutorials.html>.

---

## 1 The solution file

One solution file is provided:

| Solution file | Description |
|---|---|
| `ObjectHandler\ObjectHandler.sln` | Builds the ObjectHandler libraries (`ohlib`, `ohxllib`, `xlsdk`), the `ohxll` demonstration add-in, and the bundled C++ / Excel examples. |

The solution includes the `ohgensrc` code-generation project, which runs gensrc
(a Python 3 script) to regenerate auto-generated source before compilation. See
section 2.3 for the Python prerequisite.

---

## 2 Prerequisites

### 2.1 Visual Studio

Any recent version of Visual Studio with the **"Desktop development with C++"**
workload installed will work. The platform toolset is selected automatically by
`ObjectHandler\toolset.props` from the `VisualStudioVersion` of the Visual Studio
instance that opens the solution:

| Visual Studio | `VisualStudioVersion` | Platform toolset |
|---|---|---|
| VS 2017 (v15) | 15.0 | v141 |
| VS 2019 (v16) | 16.0 | v142 |
| VS 2022 (v17) | 17.0 | v143 |
| VS 2026 (v18) | 18.0 | v145 |

No action is required: open the solution in any of these versions and the
correct toolset is selected for you. `ObjectHandler\toolset.props` is part of
the clone, so there is nothing to configure.

### 2.2 Boost

ObjectHandler depends on Boost. You need the **compiled** Boost libraries, not
just the headers. Building Boost is outside the scope of this document; these
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

How Boost is wired into the build is described in section 4.

> **Boost library / toolset compatibility.** ObjectHandler and Boost use Boost's
> *auto-linking*: the name of the `.lib` it asks for encodes a compiler toolset
> (e.g. `libboost_*-vc143-mt-s-x64-1_83.lib`). The toolset tag in the library
> name must be one that your Boost build provides. Boost binaries are generally
> forward-compatible across adjacent MSVC toolsets (for example a `vc143` build
> links successfully from VS 2026 / v145), but if you ever see an
> `LNK1104: cannot open file 'libboost_...'` error it means the auto-linked
> library name does not exist in your `AdditionalLibraryDirectories`; provide a
> Boost build whose toolset tag matches, or add the correct directory.

### 2.3 Python 3

The `ohgensrc` project runs gensrc, which is a Python 3 script. Install Python 3
and make sure `python` is on the `PATH` (i.e. `python --version` works from a
command prompt). See <https://www.python.org/>.

If `.py` files are associated with the Python executable, gensrc builds with no
further configuration. Otherwise, edit this file...

```
ObjectHandler\gensrc\Makefile.vc
```

...and set the `PYTHON` variable to the full path of the Python executable, e.g:

```
PYTHON=C:\Program Files\Python312\python.exe
```

---

## 3 Acquire the source code

The solution file refers to the prerequisite projects using **relative paths**,
so the directory layout matters. After cloning you must end up with this layout
(`QuantLibAddin` is the repository root - you may name the outer folder anything
you like):

```
QuantLibAddin\
  gensrc\         (code-generation framework)
  ObjectHandler\
```

### 3.1 Clone the main repository

`ObjectHandler` is contained in the main repository. Clone it:

```
git clone <repo-host>/<owner>/QuantLibAddIn.git
```

This creates the `QuantLibAddin` folder containing `ObjectHandler` (among the
other sub-projects). After this step the layout in section 3 is in place.

---

## 4 Configure Boost (required)

ObjectHandler reads its Boost configuration from a single machine-local property
sheet, `ObjectHandler\boost.props`. This file is **not** part of the clone - you
create it by hand - and it is git-ignored. It lives under `ObjectHandler` on
purpose: QuantLibAddin and QuantLibXL already depend on ObjectHandler and share
this same sheet, so no project references the repository root.

If the file is missing the build fails with a compile error in the first project
that includes a Boost header, for example:

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
  compiled `.lib` files. Boost auto-linking selects the correct `.lib` from that
  directory, so you do not list individual libraries. The path is split per
  platform; fill in the `Win32` block only if you build Win32.
- `<LanguageStandard>stdcpp17</LanguageStandard>` is required: the Boost headers
  used by ObjectHandler need C++17, and the ObjectHandler projects do not set it
  themselves.

---

## 5 Build ObjectHandler

1. Open `ObjectHandler\ObjectHandler.sln` in Visual Studio.
2. On the toolbar, choose the **solution configuration** and **platform**. The
   solution provides these configurations for **x64** and **Win32**:

   | Configuration | Runtime | Library produced |
   |---|---|---|
   | `Release` | dynamic (`/MD`) | release libraries, dynamic CRT |
   | `Debug` | dynamic (`/MDd`) | debug libraries, dynamic CRT |
   | `Release (static runtime)` | static (`/MT`) | release libraries, static CRT |
   | `Debug (static runtime)` | static (`/MTd`) | debug libraries, static CRT |

   Choose the configuration that matches how you intend to consume the
   libraries. When ObjectHandler is built as part of QuantLibXL, the
   static-runtime x64 configurations are the usual choice for distribution.
3. Select **Build > Build Solution** (`Ctrl+Shift+B`).

The `ohgensrc` project runs first and regenerates the source code; the remaining
projects then compile in dependency order.

> **What the static-runtime configurations build.** The
> `Release (static runtime)` and `Debug (static runtime)` configurations build
> the static libraries (`ohlib`, `xlsdk`, `ohxllib`), the static demonstration
> add-in, and the static example (`ExampleXllStatic`, `ExampleCpp`,
> `ExampleObjects`). They deliberately do **not** build the dynamic-runtime
> projects `ohxll`, `ExampleXllDynamic1` and `ExampleXllDynamic2`: those exist
> only for an experimental dynamic build and require the dynamic-runtime
> libraries, so they are excluded from the static-runtime configurations. To
> build them, select one of the dynamic configurations (`Release` or `Debug`)
> instead.
>
> The `docs` project is also part of the solution but builds the API
> documentation with Doxygen. If `doxygen.exe` is not installed (or not
> reachable), that project fails with `'doxygen.exe' is not recognized` and a
> popup asking which application should open `.py` files - see section 7. The
> documentation is not needed to build or use the libraries; you can build the
> specific project you need (for example `ExampleCpp`) instead of the whole
> solution, or simply ignore the `docs` failure.

---

## 6 Output

On success the ObjectHandler libraries are written to `ObjectHandler\lib\`, the
demonstration add-in to `ObjectHandler\xll\`, and the example XLLs to
`ObjectHandler\Examples\xl\xll\`. File names encode the platform toolset, the
platform, the runtime variant and the version. The table below shows the names
for a **VS 2026 (v145)**, **Release (static runtime)**, **x64** build; the
toolset tag reflects whichever Visual Studio version you built with (e.g. `v143`
for VS 2022):

| Component | Output file |
|---|---|
| ObjectHandler core library | `ObjectHandler\lib\ObjectHandler-v145-x64-mt-s-1_42_0.lib` |
| ObjectHandler XLL library | `ObjectHandler\lib\ObjectHandler-xllib-v145-x64-mt-s-1_42_0.lib` |
| Excel SDK library | `ObjectHandler\lib\xlsdk-v145-x64-mt-s.lib` |
| Example add-in | `ObjectHandler\Examples\xl\xll\ExampleXllStatic-v145-x64-mt-s-1_42_0.xll` |

---

## 7 Troubleshooting

**`error C1083: Cannot open include file: 'boost/config.hpp'`**.
`ObjectHandler\boost.props` is missing or its include path is wrong, so the
Boost headers are not on the include path. Re-check section 4: the file must be
named `boost.props`, sit in the `ObjectHandler` directory, and its
`AdditionalIncludeDirectories` must point at the folder that contains the
`boost\` sub-directory.

**`error LNK1104: cannot open file 'libboost_...lib'`** at the link stage. The
Boost library directory is missing or wrong, or the auto-linked library name
(which encodes the toolset, e.g. `vc143`) is not present in that directory. Fix
`AdditionalLibraryDirectories` in `ObjectHandler\boost.props`, or provide a Boost
build whose toolset matches - see section 2.2.

**`error LNK2038: mismatch detected for 'RuntimeLibrary'`** (followed by many
`LNK2005 ... already defined` errors) when building a static-runtime
configuration. This came from the dynamic-runtime project `ohxll` being built
under a `... (static runtime)` solution configuration, so it linked the static
`xlsdk` library with the dynamic CRT. The `ohxll`, `ExampleXllDynamic1` and
`ExampleXllDynamic2` projects are now excluded from the static-runtime
configurations (see section 5); build them under a dynamic configuration
(`Release` or `Debug`) instead.

**`'doxygen.exe' is not recognized`**, optionally preceded by a popup asking
which application should open `.py` files. This is the `docs` project, which
builds the API documentation with Doxygen. Doxygen is a separate tool and is not
required to build or use the libraries. Either install Doxygen and put
`doxygen.exe` on the `PATH`, or just ignore the `docs` failure (or build only the
projects you need, e.g. `ExampleCpp`). The `.py` popup appears because the `docs`
makefile runs a helper script by filename; associating `.py` files with the
Python launcher, or installing Doxygen so the project completes, stops it.

---

## 8 Verify the build

To confirm the ObjectHandler add-in framework is working, load the example
add-in produced in section 6:

1. Open Excel.
2. Go to **File > Options > Add-ins**.
3. Set **Manage** to **Excel Add-ins** and click **Go**.
4. Click **Browse**, navigate to the `ExampleXllStatic-...xll` produced in
   section 6, and click **OK**.

The example exposes a small set of demonstration functions (prefixed `oh`) that
exercise the object repository. If they evaluate without error, the build is
good and ObjectHandler is ready to be consumed by QuantLibAddin and QuantLibXL.

