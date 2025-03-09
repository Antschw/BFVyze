# overlayBFV

## Overview
overlayBFV is a real-time overlay for Battlefield V, designed to analyze players on a server and display relevant statistics. The project is structured to be modular and extensible, focusing on safety and compliance with anti-cheat mechanisms.

## Features
### Implemented
- Basic project structure setup
- CMake configuration with vcpkg integration
- Initial ImGui window for overlay display

### In Development
- Player analysis system to retrieve and evaluate server statistics
- Display real-time player statistics (K/D, KPM, accuracy)
- Identify high K/D players for awareness
- Communication between overlay (C++) and backend (Python) using ZeroMQ

## Project Structure
```
overlayBFV/
│── cmake/               # Additional CMake configurations (if needed)
│── cmake-build-debug/   # Auto-generated build files (excluded from Git)
│── src/                 # C++ source files
│   ├── IPCClient.cpp
│   ├── OverlayWindow.cpp
│   ├── main.cpp
│── include/             # Header files
│   ├── IPCClient.h
│   ├── OverlayWindow.h
│── .gitignore           # Git ignore rules
│── CMakeLists.txt       # CMake build configuration
│── CMakePresets.json    # CMake presets for portability
│── README.md            # Project documentation
```

## Build Instructions

### Prerequisites
- C++20 Compiler (MinGW-w64 recommended)
- CMake (>= 3.30)
- vcpkg for package management
- Ninja build system

### Install Dependencies
Ensure `vcpkg` is installed and set up:
```powershell
git clone https://github.com/microsoft/vcpkg.git C:\path\to\vcpkg
cd C:\path\to\vcpkg
.\bootstrap-vcpkg.bat
```
Install required libraries:
```powershell
vcpkg install fmt:x64-mingw-static spdlog:x64-mingw-static imgui:x64-mingw-static
```
Set `VCPKG_ROOT` environment variable:
```powershell
[System.Environment]::SetEnvironmentVariable("VCPKG_ROOT", "C:\\path\\to\\vcpkg", "User")
```

### Configure the Project
```powershell
cmake --preset default
```

### Build the Project
```powershell
cmake --build cmake-build-debug
```

### Run the Overlay
```powershell
.\cmake-build-debug\overlayBFV.exe
```

## License
This project is private. Unauthorized distribution is not allowed.

