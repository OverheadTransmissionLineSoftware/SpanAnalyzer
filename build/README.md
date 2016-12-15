## Build Dependencies
Build the OTLS-Models libraries.
* See the `external` directory for instructions.

Build the wxWidgets GUI libraries.
* See the `external` directory for instructions.

## Linux
Code::Blocks is the primary build system for Linux. It can also be used for
building on Windows, although the project files are not currently configured
for this. For more information, see the [website](http://codeblocks.org).

To build SpanAnalyzer open the `build/codeblocks/SpanAnalyzer.cbp` file and
select the build configuration (debug/release).

## Windows
Visual Studio is the primary build system for Windows. The project file is
set up for use with the latest community edition version. For more information,
see the 
[website](https://www.visualstudio.com/en-us/products/visual-studio-community-vs.aspx).

To build the SpanAnalyzer open the `build/msvc/SpanAnalyzer.sln` file and
select the build configuration (Debug/Release). Only configurations for the x64
platform are supported.
