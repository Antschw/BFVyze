cmake_minimum_required(VERSION 3.30)
project(BFVyze LANGUAGES CXX)

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
find_package(cppzmq REQUIRED CONFIG)
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glad REQUIRED CONFIG)
find_package(nlohmann_json REQUIRED CONFIG)


list(APPEND CMAKE_PREFIX_PATH "$ENV{VCPKG_ROOT}/installed/x64-mingw-static")
list(APPEND CMAKE_LIBRARY_PATH "$ENV{VCPKG_ROOT}/installed/x64-mingw-static/lib")
list(APPEND CMAKE_INCLUDE_PATH "$ENV{VCPKG_ROOT}/installed/x64-mingw-static/include")

# Inclure ImGui
include_directories(third_party/imgui)
include_directories(third_party/imgui/backends)

# Define the executable target
add_executable(BFVyze
        # Sources
        src/main.cpp
        src/overlay/OverlayWindow.cpp
        src/input/HotkeyManager.cpp
        src/ipc/IPCManager.cpp
        src/ipc/ZMQClient.cpp
        src/ipc/ZMQMessageFormatter.cpp
        src/ipc/ZMQMessageParser.cpp
        src/screenshot/ScreenshotCapturer.cpp
        src/screenshot/ScreenshotOrchestrator.cpp
        src/screenshot/ScreenshotProcessor.cpp
        src/screenshot/ScreenshotSaver.cpp
        src/overlay/OverlayController.cpp

        # Headers
        include/input/HotkeyManager.h
        include/core/Constants.h
        include/overlay/OverlayWindow.h
        include/ipc/IPCManager.h
        include/ipc/ZMQClient.h
        include/ipc/ZMQMessageFormatter.h
        include/ipc/ZMQMessageParser.h
        include/screenshot/ScreenshotCapturer.h
        include/screenshot/ScreenshotOrchestrator.h
        include/screenshot/ScreenshotProcessor.h
        include/screenshot/ScreenshotSaver.h
        include/overlay/OverlayController.h

        # Third Party
        third_party/imgui/imgui.cpp
        third_party/imgui/imgui_draw.cpp
        third_party/imgui/imgui_tables.cpp
        third_party/imgui/imgui_widgets.cpp
        third_party/imgui/backends/imgui_impl_glfw.cpp
        third_party/imgui/backends/imgui_impl_opengl3.cpp
        include/core/CheaterCountManager.h
        src/core/CheaterCountManager.cpp
)

# Include directories for headers
target_include_directories(BFVyze PRIVATE include)

# Link the required libraries
target_link_libraries(BFVyze PRIVATE
        glad::glad
        spdlog::spdlog
        imgui::imgui
        cppzmq
        glfw
        OpenGL::GL
        nlohmann_json::nlohmann_json
        ${CMAKE_DL_LIBS}
)

