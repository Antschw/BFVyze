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
├── .venv/                       # Local Python virtual environment (ignored by Git)
├── cmake-build-debug/           # Out-of-source build directory (ignored by Git)
├── include/                     # C++ header files
│   ├── core/
│   │   └── Constants.h
│   ├── ipc/
│   │   ├── IPCManager.h
│   │   ├── ZMQClient.h
│   │   ├── ZMQMessageFormatter.h
│   │   └── ZMQMessageParser.h
│   ├── input/
│   │   └── HotkeyManager.h
│   ├── overlay/
│   │   └── OverlayWindow.h
│   └── screenshot/
│       ├── ScreenshotCapturer.h
│       ├── ScreenshotOrchestrator.h
│       ├── ScreenshotProcessor.h
│       └── ScreenshotSaver.h
├── python/                      # Python backend
│   ├── main.py                  # Entry point for the Python application
│   ├── setup.py                 # Python package configuration
│   ├── requirements.txt         # Python dependencies
│   ├── api/
│   │   ├── __init__.py
│   │   ├── cheater_count_fetcher.py
│   │   └── server_id_resolver.py
│   ├── ocr/
│   │   ├── __init__.py
│   │   └── ocr_processor.py
│   └── zmq_comm/
│       ├── __init__.py
│       └── screenshot_receiver.py
├── src/                         # C++ source files
│   ├── main.cpp                 # Entry point for the C++ application
│   ├── input/
│   │   └── HotkeyManager.cpp
│   ├── ipc/
│   │   ├── IPCManager.cpp
│   │   ├── ZMQClient.cpp
│   │   ├── ZMQMessageFormatter.cpp
│   │   └── ZMQMessageParser.cpp
│   ├── overlay/
│   │   └── OverlayWindow.cpp
│   └── screenshot/
│       ├── ScreenshotCapturer.cpp
│       ├── ScreenshotOrchestrator.cpp
│       ├── ScreenshotProcessor.cpp
│       └── ScreenshotSaver.cpp
├── CMakeLists.txt               # Main CMake configuration
├── CMakePresets.json            # CMake presets for reproducible builds
└── README.md                    # Project documentation
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

### Run the Python Backend
```powershell
cd python
python main.py
```

## License
This project is private. Unauthorized distribution is not allowed.

