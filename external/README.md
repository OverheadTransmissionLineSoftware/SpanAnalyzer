## Clone Submodule
Clone the external repositories.
```
git submodule update --init
```

## Models
This repository contains the engineering calculation libraries. See the
`external/Models/build` subdirectory for build instructions.

## wxWidgets
This repository contains the GUI framework libraries, which are highly
configurable. Detailed build instructions below.

### Linux
Install the required packages. On Ubuntu:
```
sudo apt-get update
sudo apt-get install libgtk2.0-dev
```

Change to the wxWidgets repository.
```
cd external/wxWidgets
```

#### Debug
Create a build directory.
```
mkdir build-debug
cd build-debug
```

Configure the library build.
```
../configure --disable-shared --enable-stl --enable-debug_gdb
```

Modify the build configuration to use wxStreams, not std::streams. This is
required so the application can load the xml based documents.
```
nano lib/wx/include/<unique>/wx/setup.h
     #define wxUSE_STD_IOSTREAM 0
```

Build the libraries.
```
make
cd ..
```

#### Release
Create a build directory.
```
mkdir build-release
cd build-release
```

Configure the release library build.
```
../configure --disable-shared --enable-stl --disable-debug
```

Modify the build configuration to use wxStreams, not std::streams. This is
required so the application can load the xml based documents.
```
nano lib/wx/include/<unique>/wx/setup.h
     #define wxUSE_STD_IOSTREAM 0
```

Build the libraries.
```
make
cd ..
```

### Windows
Configure the library build.
* Copy `include/wx/msw/setup0.h` to `include/wx/msw/setup.h`
* Modify `include/wx/msw/setup.h`. This is required so the application can load
  the xml based documents.
 * `#define wxUSE_STD_IOSTREAM 0`

Build the libraries.
* Open the Visual Studio project solution.
```
build/msw/wx_vc14.sln
```
* Select the x64 platform.
* Build the Debug and Release configurations using the GUI. Do not build the
  DLL Debug or DLL Release configurations.
