# BummerGram Build Guide

This guide covers building BummerGram from source on Windows.

## Prerequisites

### Required Software

1. **Visual Studio 2019 or later** with C++ toolset
   - Download: https://visualstudio.microsoft.com/downloads/
   - Required workloads: "Desktop development with C++"

2. **CMake 3.20+**
   - Download: https://cmake.org/download/
   - Add CMake to PATH

3. **Qt 6.5.x** (LTS)
   - Download: https://www.qt.io/download-qt-installer
   - Components needed:
     - Qt 6.5.3
     - Qt Creator (optional, for IDE)
     - MSVC 2019 64-bit kit
     - Additional libraries: qtimageformats, qttools

4. **vcpkg** (for dependencies)
   ```powershell
   git clone https://github.com/Microsoft/vcpkg.git
   .\vcpkg\bootstrap-vcpkg.bat
   ```

5. **Lua 5.4** (via vcpkg)
   ```powershell
   .\vcpkg install lua:x64-windows
   ```

## Quick Build

### Using the build script (Recommended)

```powershell
cd BummerGram
.\scripts\build.ps1
```

### Manual Build

```powershell
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. ^
    -G "Visual Studio 16 2019" ^
    -A x64 ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_TOOLCHAIN_FILE=..\vcpkg\scripts\buildsystems\vcpkg.cmake ^
    -DVCPKG_TARGET_TRIPLET=x64-windows

# Build
cmake --build . --config Release --parallel
```

## Detailed Build Process

### Step 1: Clone and Setup

```powershell
git clone https://github.com/your-repo/BummerGram.git
cd BummerGram
```

### Step 2: Configure Qt

```powershell
# Set Qt path (adjust for your installation)
$env:QtDir = "C:\Qt\6.5.3\6.5.3\msvc2019_64"
$env:PATH = "$env:QtDir\bin;$env:PATH"
```

### Step 3: Install Dependencies

```powershell
# Using vcpkg
cd ..
.\vcpkg install lua:x64-windows sqlite3:x64-windows zlib:x64-windows
cd BummerGram
```

### Step 4: Configure

```powershell
mkdir build
cd build

cmake .. `
    -G "Visual Studio 16 2019" `
    -A x64 `
    -DCMAKE_BUILD_TYPE=Release `
    -DCMAKE_TOOLCHAIN_FILE=..\vcpkg\scripts\buildsystems\vcpkg.cmake
```

### Step 5: Build

```powershell
cmake --build . --config Release
```

### Step 6: Run

```powershell
.\Release\BummerGram.exe
```

## Build Troubleshooting

### Qt Not Found

```
CMake Error: CMake was unable to find a Qt installation.
```

**Solution:** Set QT_DIR or use Qt Online Installer to install Qt components.

### vcpkg Integration Fails

```powershell
# Re-integrate
.\vcpkg integrate install
```

### MSVC Runtime Errors

Ensure Visual Studio is properly configured:
```powershell
# In Developer Command Prompt
vcvarsall.bat x64
```

### Plugin Loading Fails

Make sure the `plugins` folder exists next to the executable with required Qt plugins:
- platforms/qwindows.dll
- styles/qwindowsvistastyle.dll
- imageformats/qgif.dll, qjpeg.dll, qsvg.dll
- audio/qaudio.dll

## Build Outputs

After successful build:

```
build/
├── Release/
│   ├── BummerGram.exe          # Main executable
│   └── (dependencies...)
├── bummergram/
│   └── (object files...)
└── CMakeFiles/
```

## Deployment

### Creating Portable Package

```powershell
# Create deploy directory
mkdir deploy
cd deploy

# Copy executable
cp ../build/Release/BummerGram.exe .

# Copy Qt plugins
xcopy /E /I "C:\Qt\6.5.3\6.5.3\msvc2019_64\plugins" .\plugins\

# Copy vcpkg DLLs
# (Use windeployqt or manual copy)
```

### Using windeployqt

```powershell
# After build
cd build\Release
C:\Qt\6.5.3\6.5.3\msvc2019_64\bin\windeployqt.exe BummerGram.exe
```

## Development Workflow

### Building with Qt Creator

1. Open `CMakeLists.txt` in Qt Creator
2. Configure kit for MSVC 2019 64-bit
3. Set CMAKE_TOOLCHAIN_FILE to vcpkg.cmake
4. Build and run

### Running Tests

```powershell
cd build
ctest -C Release --output-on-failure
```

## CI/CD

GitHub Actions workflow is configured in `.github/workflows/windows-build.yml`.

Triggers:
- Push to main/master/develop
- Pull requests
- Version tags (v*)
- Manual workflow dispatch

## License

This software is released under GPLv3. See LICENSE file for details.
