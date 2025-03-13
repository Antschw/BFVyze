cmake_minimum_required(VERSION 3.15)
project(BFVyze LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Configs" FORCE)
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()

if(NOT DEFINED ENV{VCPKG_ROOT})
    message(FATAL_ERROR "VCPKG_ROOT environment variable is not set. Please set it before running CMake.")
endif()

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
endif()

set(CMAKE_PREFIX_PATH "$ENV{VCPKG_ROOT}/installed/x64-mingw-static")
list(APPEND CMAKE_PREFIX_PATH "$ENV{VCPKG_ROOT}/installed/x64-mingw-static")
list(APPEND CMAKE_LIBRARY_PATH "$ENV{VCPKG_ROOT}/installed/x64-mingw-static/lib")
list(APPEND CMAKE_INCLUDE_PATH "$ENV{VCPKG_ROOT}/installed/x64-mingw-static/include")

find_package(fmt REQUIRED CONFIG)
find_package(spdlog REQUIRED CONFIG)
find_package(imgui REQUIRED CONFIG)
find_package(cppzmq REQUIRED CONFIG)
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glad REQUIRED CONFIG)
find_package(nlohmann_json REQUIRED CONFIG)

include_directories(third_party/imgui)
include_directories(third_party/imgui/backends)

add_executable(BFVyze WIN32
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
        src/core/CheaterCountManager.cpp

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
        include/core/CheaterCountManager.h

        # Third Party
        third_party/imgui/imgui.cpp
        third_party/imgui/imgui_draw.cpp
        third_party/imgui/imgui_tables.cpp
        third_party/imgui/imgui_widgets.cpp
        third_party/imgui/backends/imgui_impl_glfw.cpp
        third_party/imgui/backends/imgui_impl_opengl3.cpp

        resource.rc
        include/global/GlobalState.h
        src/global/GlobalState.cpp
)

# Copier l'environnement Python embeddable
file(COPY ${CMAKE_SOURCE_DIR}/python/python-3.13.2-embed-amd64
        DESTINATION ${CMAKE_BINARY_DIR})

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/python-3.13.2-embed-amd64/scripts)
file(COPY ${CMAKE_SOURCE_DIR}/python/src/
        DESTINATION ${CMAKE_BINARY_DIR}/python-3.13.2-embed-amd64/scripts)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_target_properties(BFVyze PROPERTIES WIN32_EXECUTABLE NO)
else()
    set_target_properties(BFVyze PROPERTIES WIN32_EXECUTABLE YES)
endif()

# Ici on utilise le chemin absolu pour le dossier include
target_include_directories(BFVyze PRIVATE ${CMAKE_SOURCE_DIR}/include)

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

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Available configs: ${CMAKE_CONFIGURATION_TYPES}")
