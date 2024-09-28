cmake_minimum_required(VERSION 3.16)

# Project metadata
project(phoenix
        VERSION 0.0.5
        LANGUAGES C CXX
)

# CMake standards
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Library and Executable names
set(LIBRARY_NAME phoenix_lib)
set(EXECUTABLE_NAME phoenix_cli)

# CMake module path and submodules
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(AddGitSubmodule)
include(FetchContent)

# Vcpkg integration
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
            CACHE STRING "")
endif()

# Dependencies
find_package(Boost 1.85 REQUIRED COMPONENTS system filesystem)

# Fetch ASIO
FetchContent_Declare(
        asio
        GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
        GIT_TAG        asio-1-31-0
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(asio)

# Fetch Crow
FetchContent_Declare(
        crow
        GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
        GIT_TAG        v1.0+5
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(crow)

# Fetch jwt-cpp
FetchContent_Declare(
        jwt-cpp
        GIT_REPOSITORY https://github.com/Thalhammer/jwt-cpp.git
        GIT_TAG        v0.7.0
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(jwt-cpp)

# Fetch sqlite_modern_cpp
FetchContent_Declare(
        sqlite_modern_cpp
        GIT_REPOSITORY https://github.com/SqliteModernCpp/sqlite_modern_cpp.git
        GIT_TAG        v3.2
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
find_package(Qt6 COMPONENTS Widgets REQUIRED)

# Enable automatic processing of Qt-specific files
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


# macOS specific configurations
if(APPLE)
    option(OLD_MACOS "Using old macOS" OFF)
    option(BUILD_UNIVERSAL "Build a Universal binary on macOS" ON)

    if(BUILD_UNIVERSAL AND NOT OLD_MACOS)
        set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "" FORCE)
    else()
        set(CMAKE_OSX_ARCHITECTURES "${CMAKE_HOST_SYSTEM_PROCESSOR}" CACHE STRING "" FORCE)
    endif()

    if(OLD_MACOS)
        add_compile_definitions(OLD_MACOS)
    endif()
endif()

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
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(LLAMA_STANDALONE ON)
endif()

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
if(GGML_SANITIZE_THREAD)
    add_compile_options(-fsanitize=thread)
endif()

if(GGML_SANITIZE_ADDRESS)
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
endif()

if(GGML_SANITIZE_UNDEFINED)
    add_compile_options(-fsanitize=undefined)
endif()

if(AVX512)
    add_compile_options(-mavx512vl)
endif()

# Find threads package
find_package(Threads REQUIRED)

# Platform-specific linker flags
if(APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-search_paths_first -lSystem")
elseif(UNIX)
    if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static")
    endif()
elseif(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static")
endif()

# Generate header file with version number
configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.h.in"
        "${CMAKE_CURRENT_BINARY_DIR}/config.h"
)

# Include the binary directory for generated header
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# Add subdirectories
add_subdirectory(phoenix)
add_subdirectory(src)
add_subdirectory(app)

# Installation configuration
include(InstallRequiredSystemLibraries)

# System detection for packaging
if(APPLE)
    set(CPACK_SYSTEM_NAME "macOS")
elseif(WIN32)
    set(CPACK_SYSTEM_NAME "Windows")
elseif(UNIX AND NOT APPLE)
    set(CPACK_SYSTEM_NAME "Linux")
endif()

# Vulkan for Windows
if(WIN32)
    set(VULKAN_SDK $ENV{VULKAN_SDK})
    if(VULKAN_SDK)
        list(APPEND CMAKE_PREFIX_PATH "${VULKAN_SDK}")
    endif()
    find_package(Vulkan REQUIRED)
endif()

# Common packaging settings
set(CPACK_PACKAGE_NAME "Phoenix")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VENDOR "Your Company Name")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Phoenix Application")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "Phoenix")

# Platform-specific packaging configurations
if(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
    set(CPACK_DMG_FORMAT "UDZO")
    set(CPACK_DMG_VOLUME_NAME "PhoenixInstaller")
    set(CPACK_DMG_BACKGROUND_IMAGE "${PROJECT_SOURCE_DIR}/resources/background.png")
    set(CPACK_DMG_ICON "${PROJECT_SOURCE_DIR}/resources/icon.icns")
    set(MACOSX_BUNDLE_EXECUTABLE_NAME ${EXECUTABLE_NAME})
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.yourcompany.phoenix")
    set(MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME})
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION})
    set(MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION})
    set(MACOSX_BUNDLE_COPYRIGHT "Your Company © ${VERSION_YEAR}")

    # Disable DS_Store settings
    set(CPACK_DMG_DS_STORE_SETUP_SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/create_ds_store.applescript")
    configure_file("${CMAKE_SOURCE_DIR}/scripts/create_ds_store.applescript.in"
            "${CMAKE_CURRENT_BINARY_DIR}/create_ds_store.applescript" @ONLY)

    install(TARGETS ${EXECUTABLE_NAME} DESTINATION "Phoenix.app/Contents/MacOS")
    install(FILES ${PROJECT_SOURCE_DIR}/resources/background.png DESTINATION ".background")
    install(FILES ${PROJECT_SOURCE_DIR}/resources/Info.plist DESTINATION "Phoenix.app/Contents")
    install(FILES ${PROJECT_SOURCE_DIR}/resources/icon.icns DESTINATION "Phoenix.app/Contents/Resources")
    install(DIRECTORY ${CMAKE_BINARY_DIR}/bin/ DESTINATION "Phoenix.app/Contents/MacOS")

    configure_file(
            "${CMAKE_SOURCE_DIR}/scripts/update_path.sh.in"
            "${CMAKE_CURRENT_BINARY_DIR}/update_path.sh"
            @ONLY
    )
    install(PROGRAMS "${CMAKE_CURRENT_BINARY_DIR}/update_path.sh" DESTINATION "$ENV{HOME}/Phoenix")

elseif(WIN32)
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_NSIS_PACKAGE_NAME "Phoenix")
    set(CPACK_NSIS_DISPLAY_NAME "Phoenix")
    set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\${EXECUTABLE_NAME}.exe")
    set(CPACK_NSIS_HELP_LINK "http://www.yourcompany.com")
    set(CPACK_NSIS_URL_INFO_ABOUT "http://www.yourcompany.com")
    set(CPACK_NSIS_CONTACT "your.email@example.com")
    set(CPACK_NSIS_MODIFY_PATH ON)

    # Set installer icon
    set(CPACK_NSIS_MUI_ICON "${PROJECT_SOURCE_DIR}/resources/phoenix.ico")
    set(CPACK_NSIS_MUI_UNIICON "${PROJECT_SOURCE_DIR}/resources/phoenix.ico")

    # Create desktop shortcut
    set(CPACK_NSIS_CREATE_ICONS_EXTRA
            "CreateShortCut '$DESKTOP\\\\Phoenix.lnk' '$INSTDIR\\\\bin\\\\${EXECUTABLE_NAME}.exe'")
    set(CPACK_NSIS_DELETE_ICONS_EXTRA
            "Delete '$DESKTOP\\\\Phoenix.lnk'")

    # Add/Remove programs info
    set(CPACK_NSIS_DISPLAY_NAME "Phoenix")
    set(CPACK_NSIS_PACKAGE_NAME "Phoenix")
    set(CPACK_NSIS_UNINSTALL_NAME "Uninstall Phoenix")

    # Install resources and executable
    install(FILES ${PROJECT_SOURCE_DIR}/resources/phoenix.ico DESTINATION bin)
    install(TARGETS ${EXECUTABLE_NAME} DESTINATION bin)

    # Install additional DLLs or resources as needed
    # install(FILES ${PROJECT_SOURCE_DIR}/path/to/additional/dll DESTINATION bin)

elseif(UNIX AND NOT APPLE)
    set(CPACK_GENERATOR "DEB;RPM")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Your Name <your.email@example.com>")
    set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.19), libstdc++6 (>= 4.8)")
    set(CPACK_RPM_PACKAGE_LICENSE "Your License")
    set(CPACK_RPM_PACKAGE_GROUP "Applications/Productivity")
    set(CPACK_RPM_PACKAGE_REQUIRES "libstdc++ >= 4.8")

    # Install desktop file and icon for Linux
    install(TARGETS ${EXECUTABLE_NAME} DESTINATION bin)
    install(FILES "${PROJECT_SOURCE_DIR}/resources/phoenix.desktop" DESTINATION share/applications)
    install(FILES "${PROJECT_SOURCE_DIR}/resources/phoenix.png" DESTINATION share/icons/hicolor/256x256/apps)
endif()

# Common installation files
install(FILES ${PROJECT_SOURCE_DIR}/LICENSE.txt DESTINATION share/doc/phoenix)
install(FILES ${PROJECT_SOURCE_DIR}/README.md DESTINATION share/doc/phoenix)

# Include CPack for packaging
include(CPack)
