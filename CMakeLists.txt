cmake_minimum_required(VERSION 3.30)
project(overlayBFV LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ensure VCPKG_ROOT is set
if(NOT DEFINED ENV{VCPKG_ROOT})
    message(FATAL_ERROR "VCPKG_ROOT environment variable is not set. Please set it before running CMake.")
endif()

# Use vcpkg toolchain only if not already defined
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
endif()

# Define the package search path for vcpkg
set(CMAKE_PREFIX_PATH "$ENV{VCPKG_ROOT}/installed/x64-mingw-static")

# Find required libraries via vcpkg
find_package(fmt REQUIRED CONFIG)
find_package(spdlog REQUIRED CONFIG)
find_package(imgui REQUIRED CONFIG)

# Define the executable target
add_executable(overlayBFV
        src/main.cpp
        src/overlay/OverlayWindow.cpp
        src/input/IPCClient.cpp
        include/input/HotkeyManager.h
        src/input/HotkeyManager.cpp
        include/core/Constants.h
        include/overlay/OverlayWindow.h
        src/screenshot/ScreenCapturer.cpp
        include/screenshot/ScreenCapturer.h
        src/screenshot/ImageProcessor.cpp
        include/screenshot/ImageProcessor.h
        src/screenshot/ImageSaver.cpp
        include/screenshot/ImageSaver.h
        src/screenshot/ScreenshotManager.cpp
        include/screenshot/ScreenshotManager.h
)

# Include directories for headers
target_include_directories(overlayBFV PRIVATE include)

# Link the required libraries
target_link_libraries(overlayBFV PRIVATE
        spdlog::spdlog
        imgui::imgui
)
