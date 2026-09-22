
# QuantLibAddin

This repo contains source code and examples for the QuantLibXL, QuantLibAddin, ObjectHandler, and gensrc projects.

## Documentation

The four projects are documented at the links below:

https://www.quantlib.org/quantlibxl  
https://www.quantlib.org/quantlibaddin  
https://www.quantlib.org/objecthandler  
https://www.quantlib.org/gensrc

Compiled XLLs, example spreadsheets, and documentation are available for download at this link:

https://github.com/eehlers/QuantLibAddIn/releases/tag/v1.42.0

Below are instructions for building QuantLibXL (and other incarnations of QuantLibAddin) from source code.

NB: QuantLibAddin now depends not on the official QuantLib repo but on the OpenSourceRisk fork of QuantLib.

## Building from a git clone

There are two ways to build the projects:
1) Using Visual Studio solution files, which are maintained manually
2) Using cmake

### Building from Visual Studio solution files

Here are the instructions to build the various projects from Visual Studio solution files:

- QuantLibXL (including QuantLibAddin and ObjectHandler): [./QuantLibXL/md/build_vs.md](./QuantLibXL/md/build_vs.md)
- QuantLibAddin (including ObjectHandler): [./QuantLibAddin/md/build_vs.md](./QuantLibAddin/md/build_vs.md)
- ObjectHandler: [./ObjectHandler/md/build_vs.md](./ObjectHandler/md/build_vs.md)

### Building with cmake

There are two ways to build the projects with cmake:

1) Invoke the build from here in the root directory, which configures and builds the whole stack (QuantLib, ObjectHandler, QuantLibAddin, and QuantLibXL) together in a single step: [./build_cmake.md](./build_cmake.md)

2) Build the subprojects individually, as described in their respective documentation. Each subproject builds against the prebuilt prerequisites above it in the dependency chain (gensrc &lt;- ObjectHandler &lt;- QuantLibAddin &lt;- QuantLibXL):

   - ObjectHandler: [./ObjectHandler/md/build_cmake.md](./ObjectHandler/md/build_cmake.md)
   - QuantLibAddin (against prebuilt QuantLib and ObjectHandler): [./QuantLibAddin/md/build_cmake.md](./QuantLibAddin/md/build_cmake.md)
   - QuantLibXL (against prebuilt QuantLib, ObjectHandler, and QuantLibAddin): [./QuantLibXL/md/build_cmake.md](./QuantLibXL/md/build_cmake.md)

