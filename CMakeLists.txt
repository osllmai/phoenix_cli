cmake_minimum_required(VERSION 3.16)

# Project metadata
set(APP_VERSION_MAJOR 0)
set(APP_VERSION_MINOR 0)
set(APP_VERSION_PATCH 5)
set(APP_VERSION_BASE "${APP_VERSION_MAJOR}.${APP_VERSION_MINOR}.${APP_VERSION_PATCH}")
set(APP_VERSION "${APP_VERSION_BASE}-dev0")


project(phoenix
        VERSION ${APP_VERSION_BASE}
        LANGUAGES C CXX
)

# CMake standards
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Library and Executable names
set(LIBRARY_NAME phoenix_lib)
set(EXECUTABLE_NAME phoenix)

# CMake module path and submodules
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/Modules")

include(AddGitSubmodule)
include(FetchContent)

# Vcpkg integration
if (DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
            CACHE STRING "")
endif ()

# Dependencies
find_package(Boost 1.85 REQUIRED COMPONENTS system filesystem)

# Fetch ASIO
FetchContent_Declare(
        asio
        GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
        GIT_TAG asio-1-31-0
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(asio)

# Fetch Crow
FetchContent_Declare(
        crow
        GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
        GIT_TAG v1.0+5
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(crow)

# Fetch jwt-cpp
FetchContent_Declare(
        jwt-cpp
        GIT_REPOSITORY https://github.com/Thalhammer/jwt-cpp.git
        GIT_TAG v0.7.0
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(jwt-cpp)

# Fetch sqlite_modern_cpp
FetchContent_Declare(
        sqlite_modern_cpp
        GIT_REPOSITORY https://github.com/SqliteModernCpp/sqlite_modern_cpp.git
        GIT_TAG v3.2
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(sqlite_modern_cpp)

# Fetch nlohmann_json
FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(nlohmann_json)

# Find Qt6
find_package(Qt6 6.5 REQUIRED COMPONENTS
        Core
        Widgets
        Network
        DBus
        HttpServer
        LinguistTools
        Pdf
        Quick
        QuickDialogs2
        Sql
        Svg
)

# Enable automatic processing of Qt-specific files
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# macOS specific configurations
if (APPLE)
    option(OLD_MACOS "Using old macOS" OFF)
    option(BUILD_UNIVERSAL "Build a Universal binary on macOS" ON)

    if (BUILD_UNIVERSAL AND NOT OLD_MACOS)
        set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "" FORCE)
    else ()
        set(CMAKE_OSX_ARCHITECTURES "${CMAKE_HOST_SYSTEM_PROCESSOR}" CACHE STRING "" FORCE)
    endif ()

    if (OLD_MACOS)
        add_compile_definitions(OLD_MACOS)
    endif ()
endif ()

# Project versioning
set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_PATCH 5)
set(PROJECT_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

# Compile options
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# Standalone build check
set(LLAMA_STANDALONE OFF)
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(LLAMA_STANDALONE ON)
endif ()

# Processor options
option(AVX2 "Enable AVX2" ON)
option(AVX512 "Enable AVX512" OFF)
option(LLAMA_AVX "Enable AVX" ON)
option(LLAMA_AVX2 "Enable AVX2" ${AVX2})
option(LLAMA_AVX512 "Enable AVX512" ${AVX512})
option(LLAMA_AVX512_VBMI "Enable AVX512-VBMI" ${AVX512})
option(LLAMA_AVX512_VNNI "Enable AVX512-VNNI" ${AVX512})
option(LLAMA_FMA "Enable FMA" ${AVX2})

# Sanitizers
if (GGML_SANITIZE_THREAD)
    add_compile_options(-fsanitize=thread)
endif ()

if (GGML_SANITIZE_ADDRESS)
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
endif ()

if (GGML_SANITIZE_UNDEFINED)
    add_compile_options(-fsanitize=undefined)
endif ()

if (AVX512)
    add_compile_options(-mavx512vl)
endif ()

# Find threads package
find_package(Threads REQUIRED)

# Platform-specific linker flags
if (APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-search_paths_first -lSystem")
elseif (UNIX)
    if (NOT CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static")
    endif ()
elseif (WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static")
endif ()

# Include the binary directory for generated header
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# Generate header file with version number
configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.h.in"
        "${CMAKE_CURRENT_BINARY_DIR}/config.h"
)

set(CMAKE_FIND_PACKAGE_TARGETS_GLOBAL ON)

if (QT_KNOWN_POLICY_QTP0004)
    qt_policy(SET QTP0004 NEW)  # generate extra qmldir files on Qt 6.8+
endif ()

# Get the Qt6Core target properties
get_target_property(Qt6Core_INCLUDE_DIRS Qt6::Core INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(Qt6Core_LIBRARY_RELEASE Qt6::Core LOCATION_RELEASE)

# Find the qmake binary
find_program(QMAKE_EXECUTABLE NAMES qmake qmake6 PATHS ${Qt6Core_INCLUDE_DIRS}/../.. NO_DEFAULT_PATH)

# Get the Qt 6 root directory
get_filename_component(Qt6_ROOT_DIR "${Qt6Core_LIBRARY_RELEASE}" DIRECTORY)
get_filename_component(Qt6_ROOT_DIR "${Qt6_ROOT_DIR}/.." ABSOLUTE)

message(STATUS "qmake binary: ${QMAKE_EXECUTABLE}")
message(STATUS "Qt 6 root directory: ${Qt6_ROOT_DIR}")

# Add subdirectories
add_subdirectory(phoenix)
add_subdirectory(src)
add_subdirectory(app)

find_package(Python3 3.12 QUIET COMPONENTS Interpreter)


macro(REPORT_MISSING_SIGNING_CONTEXT)
    message(FATAL_ERROR [=[
        Signing requested but no identity configured.
        Please set the correct env variable or provide the MAC_SIGNING_IDENTITY argument on the command line
        ]=])
endmacro()

if (APPLE)
    set_target_properties(phoenix PROPERTIES
            MACOSX_BUNDLE TRUE
            MACOSX_BUNDLE_GUI_IDENTIFIER phoenix
            MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
            MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
            OUTPUT_NAME phoenix
    )
    add_dependencies(phoenix ggml-metal)
endif ()


option(GPT4ALL_SIGN_INSTALL "Sign installed binaries and installers (requires signing identities)" OFF)

if (APPLE AND GPT4ALL_SIGN_INSTALL)
    if (NOT MAC_SIGNING_IDENTITY)
        if (NOT DEFINED ENV{MAC_SIGNING_CERT_NAME})
            REPORT_MISSING_SIGNING_CONTEXT()
        endif ()
        set(MAC_SIGNING_IDENTITY $ENV{MAC_SIGNING_CERT_NAME})
    endif ()
    if (NOT MAC_SIGNING_TID)
        if (NOT DEFINED ENV{MAC_NOTARIZATION_TID})
            REPORT_MISSING_SIGNING_CONTEXT()
        endif ()
        set(MAC_SIGNING_TID $ENV{MAC_NOTARIZATION_TID})
    endif ()

    # Setup MacOS signing for individual binaries
    set_target_properties(phoenix PROPERTIES
            XCODE_ATTRIBUTE_CODE_SIGN_STYLE "Manual"
            XCODE_ATTRIBUTE_DEVELOPMENT_TEAM ${MAC_SIGNING_TID}
            XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ${MAC_SIGNING_IDENTITY}
            XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED True
            XCODE_ATTRIBUTE_OTHER_CODE_SIGN_FLAGS "--timestamp=http://timestamp.apple.com/ts01  --options=runtime,library"
    )
endif ()

# -- install --

set(COMPONENT_NAME_MAIN ${PROJECT_NAME})

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install CACHE PATH "..." FORCE)
endif ()

set_target_properties(phoenix PROPERTIES
        INSTALL_RPATH "@executable_path/../lib"
)

install(TARGETS phoenix DESTINATION bin COMPONENT ${COMPONENT_NAME_MAIN})

install(TARGETS phoenix_lib DESTINATION bin COMPONENT ${COMPONENT_NAME_MAIN})

if (APPLE)
    # Use relative paths for DESTINATION
    set(APP_BUNDLE_PATH "bin/phoenix.app")
    set(MODEL_IMPL_TARGETS
            llamamodel-mainline-cpu
            llamamodel-mainline-cpu-avxonly
            llamamodel-mainline-metal
    )
    install(
            TARGETS llmodel
            LIBRARY DESTINATION "${APP_BUNDLE_PATH}/Contents/lib" COMPONENT ${COMPONENT_NAME_MAIN}
            RUNTIME DESTINATION "${APP_BUNDLE_PATH}/Contents/MacOS" COMPONENT ${COMPONENT_NAME_MAIN}
    )
    install(
            TARGETS ${MODEL_IMPL_TARGETS}
            LIBRARY DESTINATION "${APP_BUNDLE_PATH}/Contents/lib" COMPONENT ${COMPONENT_NAME_MAIN}
            RUNTIME DESTINATION "${APP_BUNDLE_PATH}/Contents/MacOS" COMPONENT ${COMPONENT_NAME_MAIN}
    )
else ()
    install(
            TARGETS llmodel
            LIBRARY DESTINATION lib COMPONENT ${COMPONENT_NAME_MAIN}
            RUNTIME DESTINATION bin COMPONENT ${COMPONENT_NAME_MAIN}
    )
    install(
            TARGETS ${MODEL_IMPL_TARGETS}
            LIBRARY DESTINATION lib COMPONENT ${COMPONENT_NAME_MAIN}
            RUNTIME DESTINATION bin COMPONENT ${COMPONENT_NAME_MAIN}
    )
endif ()


if (APPLE AND GPT4ALL_SIGN_INSTALL)
    include(SignMacOSBinaries)
    install_sign_osx(phoenix_lib)
    install_sign_osx(llmodel)
    foreach (tgt ${MODEL_IMPL_TARGETS})
        install_sign_osx(${tgt})
    endforeach ()
endif ()

if (WIN32 AND GPT4ALL_SIGN_INSTALL)
    include(SignWindowsBinaries)
    sign_target_windows(chat)
    sign_target_windows(llmodel)
    foreach (tgt ${MODEL_IMPL_TARGETS})
        sign_target_windows(${tgt})
    endforeach ()
endif ()

set(CPACK_GENERATOR "IFW")
set(CPACK_VERBATIM_VARIABLES YES)
set(CPACK_IFW_VERBOSE ON)

if (${CMAKE_SYSTEM_NAME} MATCHES Linux)
    find_program(LINUXDEPLOYQT linuxdeployqt HINTS "$ENV{HOME}/dev/linuxdeployqt/build/tools/linuxdeployqt" "$ENV{HOME}/project/linuxdeployqt/bin")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/deploy-qt-linux.cmake.in"
            "${CMAKE_BINARY_DIR}/cmake/deploy-qt-linux.cmake" @ONLY)
    set(CPACK_PRE_BUILD_SCRIPTS ${CMAKE_BINARY_DIR}/cmake/deploy-qt-linux.cmake)
    set(CPACK_IFW_ROOT "~/Qt/Tools/QtInstallerFramework/4.6")
    set(CPACK_PACKAGE_FILE_NAME "${COMPONENT_NAME_MAIN}-installer-linux")
    set(CPACK_IFW_TARGET_DIRECTORY "@HomeDir@/${COMPONENT_NAME_MAIN}")
elseif (${CMAKE_SYSTEM_NAME} MATCHES Windows)
    find_program(WINDEPLOYQT windeployqt HINTS ${_qt_bin_dir})
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/deploy-qt-windows.cmake.in"
            "${CMAKE_BINARY_DIR}/cmake/deploy-qt-windows.cmake" @ONLY)
    set(CPACK_PRE_BUILD_SCRIPTS ${CMAKE_BINARY_DIR}/cmake/deploy-qt-windows.cmake)
    set(CPACK_IFW_ROOT "C:/Qt/Tools/QtInstallerFramework/4.6")
    set(CPACK_IFW_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/resources/gpt4all.ico")
    set(CPACK_PACKAGE_FILE_NAME "${COMPONENT_NAME_MAIN}-installer-win64")
    set(CPACK_IFW_TARGET_DIRECTORY "@HomeDir@\\${COMPONENT_NAME_MAIN}")
elseif (${CMAKE_SYSTEM_NAME} MATCHES Darwin)
    find_program(MACDEPLOYQT macdeployqt HINTS ${_qt_bin_dir})
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/deploy-qt-mac.cmake.in"
            "${CMAKE_BINARY_DIR}/cmake/deploy-qt-mac.cmake" @ONLY)
    set(CPACK_PRE_BUILD_SCRIPTS ${CMAKE_BINARY_DIR}/cmake/deploy-qt-mac.cmake)
    set(CPACK_IFW_ROOT "~/Qt/Tools/QtInstallerFramework/4.6")
    set(CPACK_IFW_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/resources/icon.icns")
    set(CPACK_PACKAGE_FILE_NAME "${COMPONENT_NAME_MAIN}-installer-darwin")
    set(CPACK_IFW_TARGET_DIRECTORY "@ApplicationsDir@/${COMPONENT_NAME_MAIN}")
    set(CPACK_BUNDLE_NAME ${COMPONENT_NAME_MAIN})
    set(CPACK_BUNDLE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/resources/icon.icns")
endif ()

set(CPACK_PACKAGE_INSTALL_DIRECTORY ${COMPONENT_NAME_MAIN})
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_HOMEPAGE_URL "https://www.nomic.ai/gpt4all")
set(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/resources/icon.icns")
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE)
set(CPACK_RESOURCE_FILE_README ${CMAKE_CURRENT_SOURCE_DIR}/README.md)
set(CPACK_PACKAGE_EXECUTABLES "phoenix")
set(CPACK_CREATE_DESKTOP_LINKS "phoenix")
set(CPACK_IFW_PACKAGE_NAME "phoenix")
set(CPACK_IFW_PACKAGE_TITLE "phoenix Installer")
set(CPACK_IFW_PACKAGE_PUBLISHER "OSLLM, Inc.")
set(CPACK_IFW_PRODUCT_URL "https://www.nomic.ai/gpt4all")
set(CPACK_IFW_PACKAGE_WIZARD_STYLE "Mac")
set(CPACK_IFW_PACKAGE_LOGO "${CMAKE_CURRENT_SOURCE_DIR}/resources/icon.icns")
set(CPACK_IFW_PACKAGE_WINDOW_ICON "${CMAKE_CURRENT_SOURCE_DIR}/resources/icon.icns")
set(CPACK_IFW_PACKAGE_WIZARD_SHOW_PAGE_LIST OFF)
set(CPACK_IFW_PACKAGE_CONTROL_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/cmake/installer_control.qs")

include(InstallRequiredSystemLibraries)
include(CPack)
include(CPackIFW)
if (GPT4ALL_OFFLINE_INSTALLER)
    cpack_add_component(${COMPONENT_NAME_MAIN})
else ()
    cpack_add_component(${COMPONENT_NAME_MAIN} DOWNLOADED)
endif ()
cpack_ifw_configure_component(${COMPONENT_NAME_MAIN} ESSENTIAL FORCED_INSTALLATION)
cpack_ifw_configure_component(${COMPONENT_NAME_MAIN} VERSION ${APP_VERSION})
cpack_ifw_configure_component(${COMPONENT_NAME_MAIN} LICENSES "MIT LICENSE" ${CPACK_RESOURCE_FILE_LICENSE})
cpack_ifw_configure_component(${COMPONENT_NAME_MAIN} SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/cmake/installer_component.qs")
