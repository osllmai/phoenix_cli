cmake_minimum_required(VERSION 3.5)

# Project name and version
set(PROJECT_NAME phoenix)
project(${PROJECT_NAME}
        VERSION 0.0.3
        LANGUAGES C CXX)

# Set standards
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define project-related names
set(LIBRARY_NAME Lib)
set(EXECUTABLE_NAME phoenix_cli)

# CMake module path and submodules
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/")
include(AddGitSubmodule)
include(FetchContent)

# Configure vcpkg (add this near the top of the file)
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
            CACHE STRING "")
endif()
# Find Boost (add this after the vcpkg configuration)
find_package(Boost 1.85 REQUIRED COMPONENTS system filesystem)


FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(nlohmann_json)

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
        add_definitions(-DOLD_MACOS)
    endif ()
endif ()

# Versioning
set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_PATCH 3)
set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# Determine if this is a standalone build
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(LLAMA_STANDALONE ON)
else ()
    set(LLAMA_STANDALONE OFF)
endif ()

# Processor options
option(AVX2 "Enable AVX2" ON)
option(AVX512 "Enable AVX512" OFF)

option(LLAMA_AVX "llama: Enable AVX" ON)
option(LLAMA_AVX2 "llama: Enable AVX2" ${AVX2})
option(LLAMA_AVX512 "llama: Enable AVX512" ${AVX512})
option(LLAMA_AVX512_VBMI "llama: Enable AVX512-VBMI" ${AVX512})
option(LLAMA_AVX512_VNNI "llama: Enable AVX512-VNNI" ${AVX512})
option(LLAMA_FMA "llama: Enable FMA" ${AVX2})

# Sanitizers
if (GGML_SANITIZE_THREAD)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=thread")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
endif ()

if (GGML_SANITIZE_ADDRESS)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
endif ()

if (GGML_SANITIZE_UNDEFINED)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=undefined")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
endif ()

if (AVX512)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mavx512vl")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx512vl")
endif ()

# Find Threads package
find_package(Threads REQUIRED)

# Linker flags for different platforms
if (APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-search_paths_first -lSystem")
elseif (UNIX)
    if (NOT ${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static")
    endif ()
elseif (WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static")
endif ()

# Generate a header file with the version number
configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.h.in"
        "${CMAKE_CURRENT_BINARY_DIR}/config.h"
)

# Include the binary directory for the generated header file
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# Add subdirectories
add_subdirectory(phoenix)
add_subdirectory(src)
add_subdirectory(app)


# -------- install --------#
include(InstallRequiredSystemLibraries)

# Determine the system
if (APPLE)
    set(CPACK_SYSTEM_NAME "macOS")
elseif (WIN32)
    set(CPACK_SYSTEM_NAME "Windows")
elseif (UNIX AND NOT APPLE)
    set(CPACK_SYSTEM_NAME "Linux")
endif ()

if(WIN32)
    set(VULKAN_SDK $ENV{VULKAN_SDK})
    if(VULKAN_SDK)
        set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "${VULKAN_SDK}")
    endif()
    # Find Vulkan package
    find_package(Vulkan REQUIRED)
endif()


# Common settings
set(CPACK_PACKAGE_NAME "Phoenix")
set(CPACK_PACKAGE_VERSION ${VERSION})
set(CPACK_PACKAGE_VENDOR "Your Company Name")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Phoenix Application")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "Phoenix")

# macOS specific configurations
if (APPLE)
    set(CPACK_GENERATOR "DragNDrop")
    set(CPACK_DMG_FORMAT "UDZO")
    set(CPACK_DMG_VOLUME_NAME "PhoenixInstaller")
    set(CPACK_DMG_BACKGROUND_IMAGE "${PROJECT_SOURCE_DIR}/resources/background.png")
    set(CPACK_DMG_ICON "${PROJECT_SOURCE_DIR}/resources/icon.icns")
    set(MACOSX_BUNDLE_EXECUTABLE_NAME ${EXECUTABLE_NAME})
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.yourcompany.phoenix")
    set(MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME})
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${VERSION})
    set(MACOSX_BUNDLE_BUNDLE_VERSION ${VERSION})
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

    # Configure and install the update_path script
    configure_file(
            "${CMAKE_SOURCE_DIR}/scripts/update_path.sh.in"
            "${CMAKE_CURRENT_BINARY_DIR}/update_path.sh"
            @ONLY
    )
    install(PROGRAMS "${CMAKE_CURRENT_BINARY_DIR}/update_path.sh" DESTINATION "$ENV{HOME}/Phoenix")

    # Windows specific configurations
elseif (WIN32)
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_NSIS_PACKAGE_NAME "Phoenix")
    set(CPACK_NSIS_DISPLAY_NAME "Phoenix")
    set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\${EXECUTABLE_NAME}.exe")
    set(CPACK_NSIS_HELP_LINK "http://www.yourcompany.com")
    set(CPACK_NSIS_URL_INFO_ABOUT "http://www.yourcompany.com")
    set(CPACK_NSIS_CONTACT "your.email@example.com")
    set(CPACK_NSIS_MODIFY_PATH ON)

    # Set the icon for the installer
    set(CPACK_NSIS_MUI_ICON "${PROJECT_SOURCE_DIR}/resources/phoenix.ico")
    set(CPACK_NSIS_MUI_UNIICON "${PROJECT_SOURCE_DIR}/resources/phoenix.ico")

    # Create a desktop shortcut
    set(CPACK_NSIS_CREATE_ICONS_EXTRA
            "CreateShortCut '$DESKTOP\\\\Phoenix.lnk' '$INSTDIR\\\\bin\\\\${EXECUTABLE_NAME}.exe'")
    set(CPACK_NSIS_DELETE_ICONS_EXTRA
            "Delete '$DESKTOP\\\\Phoenix.lnk'")

    # Add/Remove programs info
    set(CPACK_NSIS_DISPLAY_NAME "Phoenix")
    set(CPACK_NSIS_PACKAGE_NAME "Phoenix")
    set(CPACK_NSIS_UNINSTALL_NAME "Uninstall Phoenix")

    # Install the icon
    install(FILES ${PROJECT_SOURCE_DIR}/resources/phoenix.ico DESTINATION bin)
    # Install the main executable
    install(FILES ${CMAKE_BINARY_DIR}/bin/Debug/${EXECUTABLE_NAME}.exe
            DESTINATION bin
            PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
            GROUP_EXECUTE GROUP_READ
            WORLD_EXECUTE WORLD_READ
    )
    install(DIRECTORY ${CMAKE_BINARY_DIR}/bin/Debug/ DESTINATION bin)



    # Install any additional DLLs or resources
    # install(FILES ${PROJECT_SOURCE_DIR}/path/to/additional/dll DESTINATION bin)

    # Linux specific configurations
elseif (UNIX AND NOT APPLE)
    set(CPACK_GENERATOR "DEB;RPM")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Your Name <your.email@example.com>")
    set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.19), libstdc++6 (>= 4.8)")
    set(CPACK_RPM_PACKAGE_LICENSE "Your License")
    set(CPACK_RPM_PACKAGE_GROUP "Applications/Productivity")
    set(CPACK_RPM_PACKAGE_REQUIRES "libstdc++ >= 4.8")

    # Install the main executable
    install(TARGETS ${EXECUTABLE_NAME} DESTINATION bin)

    # Install desktop file and icon for Linux
    install(FILES "${PROJECT_SOURCE_DIR}/resources/phoenix.desktop" DESTINATION share/applications)
    install(FILES "${PROJECT_SOURCE_DIR}/resources/phoenix.png" DESTINATION share/icons/hicolor/256x256/apps)
endif ()

# Common installation for all platforms
install(FILES ${PROJECT_SOURCE_DIR}/LICENSE.txt DESTINATION share/doc/phoenix)
install(FILES ${PROJECT_SOURCE_DIR}/README.md DESTINATION share/doc/phoenix)

include(CPack)