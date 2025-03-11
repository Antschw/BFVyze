# BFVyze

## Overview
BFVyze is a real-time overlay for Battlefield V, designed to analyze players on a server and display relevant statistics. The project is structured to be modular and extensible, focusing on safety and compliance with anti-cheat mechanisms.

## Features
### Implemented
- Basic project structure setup
- CMake configuration with vcpkg integration

### In Development (first phase) - In game cheater detection
- OCR for identifying server id
- Shortcut manager
- Retrieve data from bfvhacker API and Gametool API
- Initial ImGui window for overlay display
- Communication between overlay (C++) and backend (Python) using ZeroMQ
  
### Planned features
- Display real-time player statistics (K/D, KPM, accuracy) during game session
- Identify high K/D, kpm players for awareness and players with a lot of hours
- Identify all the servers with cheater in the search server HUD

## Project Structure
```
BFVyze/
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
Set `VCPKG_ROOT` environment variable (Windows only):
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
.\cmake-build-debug\BFVyze.exe
```

## License
This project is private. Unauthorized distribution is not allowed.

