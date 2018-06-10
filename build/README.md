## Build Dependencies
Build the OTLS-Models libraries.
* See the `external` directory for instructions.

Build the wxWidgets GUI libraries.
* See the `external` directory for instructions.

## CMake
CMake is used to generate makefiles.

For the debug configuration:
```
mkdir <repository_root>/build-debug
cd <repository_root>/build-debug
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug ../build/cmake
cmake --build .
```

For the release configuration:
```
mkdir <repository_root>/build-release
cd <repository_root>/build-release
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ../build/cmake
cmake --build .
```

## Linux
Code::Blocks is the primary IDE for Linux. These files are manually maintained
even though CMake can generate them. Codeblocks can also be used for
building on Windows, although the project files are not currently configured
for this. For more information, see the [website](http://codeblocks.org).

To build SpanAnalyzer open the `build/codeblocks/SpanAnalyzer.cbp` file and
select the build configuration (debug/release).

## Windows
Visual Studio is the primary IDE for Windows. These files are manually maintained
even though CMake can generate them. The project files are set up for use with
the latest community edition version. For more information, see the
[website](https://www.visualstudio.com/en-us/products/visual-studio-community-vs.aspx).

To build the SpanAnalyzer open the `build/msvc/SpanAnalyzer.sln` file and
select the build configuration (Debug/Release). Only configurations for the x64
platform are supported.
