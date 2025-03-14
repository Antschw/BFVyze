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

# Téléchargement et extraction de Python embarqué via ExternalProject_Add
include(ExternalProject)
set(PYTHON_EMBED_URL "https://www.python.org/ftp/python/3.13.2/python-3.13.2-embed-amd64.zip")
# Remplacez la valeur ci-dessous par le hash SHA256 officiel de l'archive
set(PYTHON_EMBED_HASH "SHA256=1e803610b140cbf69dfa2ceaaeb39651bef75a239c381289e827c30862a27b93")

ExternalProject_Add(PythonEmbed
        URL ${PYTHON_EMBED_URL}
        URL_HASH ${PYTHON_EMBED_HASH}
        DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        LOG_DOWNLOAD ON
)

# Définir le dossier cible pour l'environnement Python embarqué
set(PYTHON_EMBED_DIR ${CMAKE_BINARY_DIR}/python-3.13.2-embed-amd64)

# Commande pour extraire l'archive dans PYTHON_EMBED_DIR
add_custom_command(
        OUTPUT ${PYTHON_EMBED_DIR}/pythonw.exe
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PYTHON_EMBED_DIR}
        COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_BINARY_DIR}/downloads/python-3.13.2-embed-amd64.zip
        WORKING_DIRECTORY ${PYTHON_EMBED_DIR}
        DEPENDS PythonEmbed
)

add_custom_target(ExtractPython ALL DEPENDS ${PYTHON_EMBED_DIR}/pythonw.exe)

# Copier les fichiers modifiés dans l'environnement Python
add_custom_command(TARGET ExtractPython POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/python/res/python313._pth
        ${PYTHON_EMBED_DIR}/python313._pth
        COMMENT "Mise à jour du fichier python313._pth"
)
add_custom_command(TARGET ExtractPython POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/python/res/get-pip.py
        ${PYTHON_EMBED_DIR}/get-pip.py
        COMMENT "Mise à jour de get-pip.py"
)
add_custom_command(TARGET ExtractPython POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/python/requirements.txt
        ${PYTHON_EMBED_DIR}/requirements.txt
        COMMENT "Mise à jour du requirements.txt"
)

# Installer pip via get-pip.py
add_custom_command(TARGET ExtractPython POST_BUILD
        COMMAND ${PYTHON_EMBED_DIR}/python.exe ${PYTHON_EMBED_DIR}/get-pip.py --no-warn-script-location
        COMMENT "Installation de pip via get-pip.py"
)

# Installer les dépendances listées dans requirements.txt
add_custom_command(TARGET ExtractPython POST_BUILD
        COMMAND ${PYTHON_EMBED_DIR}/python.exe -m pip install -r ${PYTHON_EMBED_DIR}/requirements.txt
        COMMENT "Installation des dépendances depuis requirements.txt"
)

# --- Configuration de l'exécutable BFVyze ---

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
        src/ipc/PythonBackendController.cpp
        include/ipc/PythonBackendController.h
)

file(COPY ${CMAKE_SOURCE_DIR}/python/src/
        DESTINATION ${CMAKE_BINARY_DIR}/python-3.13.2-embed-amd64/Scripts)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_target_properties(BFVyze PROPERTIES WIN32_EXECUTABLE NO)
else()
    set_target_properties(BFVyze PROPERTIES WIN32_EXECUTABLE YES)
endif()

# Utilisation du chemin absolu pour le dossier include
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

# Dépendance : l'exécutable BFVyze ne sera construit qu'après l'extraction/configuration de Python
add_dependencies(BFVyze ExtractPython)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Available configs: ${CMAKE_CONFIGURATION_TYPES}")
