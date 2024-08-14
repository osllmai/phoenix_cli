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

# Enable logging option
option(LOCAL_DEBUG "Enable logging" ON)

# Define project-related names
set(LIBRARY_NAME Lib)
set(EXECUTABLE_NAME phoenix)

# CMake module path and submodules
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/")
include(AddGitSubmodule)
include(FetchContent)

# Fetch dependencies
if (LOCAL_DEBUG)
    message(STATUS "Logging is active")
    FetchContent_Declare(
            spdlog
            GIT_REPOSITORY https://github.com/gabime/spdlog.git
            GIT_TAG v1.14.1
            GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(spdlog)
else ()
    message(STATUS "Logging is inactive")
endif ()

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
option(BUILD_MACOS_BUNDLE "Build as macOS application bundle" OFF)

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


# macOS Application Bundle Configuration
if (APPLE)
    set(MACOSX_BUNDLE TRUE)
    set(MACOSX_BUNDLE_ICON_FILE "icon.icns")
    set(APP_ICON_MACOSX ${CMAKE_SOURCE_DIR}/resources/${MACOSX_BUNDLE_ICON_FILE})
    # Set application bundle properties
    set_target_properties(${EXECUTABLE_NAME} PROPERTIES
            MACOSX_BUNDLE TRUE
            MACOSX_BUNDLE_GUI_IDENTIFIER "com.yourcompany.${PROJECT_NAME}"
            MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}"
            MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
            MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
            MACOSX_BUNDLE_ICON_FILE "icon.icns"
    )


    # Ensure the icon file is copied to the correct location in the app bundle
    set_source_files_properties(${APP_ICON_MACOSX} PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources"
    )

    # Add the icon to the target sources
    target_sources(${EXECUTABLE_NAME} PRIVATE ${APP_ICON_MACOSX})

    set(DMG_BACKGROUND_IMAGE "${CMAKE_SOURCE_DIR}/resources/background.png")
    set(CPACK_DMG_BACKGROUND_IMAGE ${DMG_BACKGROUND_IMAGE})

    # Copy background image to the .background folder
    set(DMG_BACKGROUND_IMAGE "${CMAKE_SOURCE_DIR}/resources/background.png")
    install(FILES ${DMG_BACKGROUND_IMAGE} DESTINATION .background COMPONENT ${COMPONENT_NAME_MAIN})

    # Ensure the icon is included in the DMG
    install(FILES ${APP_ICON_MACOSX} DESTINATION . COMPONENT ${COMPONENT_NAME_MAIN})

    # Set DMG properties
    set(CPACK_DMG_BACKGROUND_IMAGE ${DMG_BACKGROUND_IMAGE})
    set(CPACK_DMG_DS_STORE_SETUP_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/create_ds_store.applescript")
    set(CPACK_DMG_DISABLE_APPLICATIONS_SYMLINK OFF)

    set_target_properties(${EXECUTABLE_NAME} PROPERTIES
            MACOSX_BUNDLE TRUE
            MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/resources/Info.plist
            RESOURCE ${APP_ICON_MACOSX}
            MACOSX_BUNDLE_ICON_FILE ${MACOSX_BUNDLE_ICON_FILE}
            MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}"
            MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
            MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
            MACOSX_BUNDLE_GUI_IDENTIFIER "com.yourcompany.${PROJECT_NAME}"
            MACOSX_BUNDLE_COPYRIGHT "© 2024 Your Company Name. All rights reserved."
    )

    # Install the app bundle
    install(TARGETS ${EXECUTABLE_NAME}
            BUNDLE DESTINATION . COMPONENT ${COMPONENT_NAME_MAIN}
            RUNTIME DESTINATION bin COMPONENT ${COMPONENT_NAME_MAIN}
    )
else ()
    install(TARGETS ${EXECUTABLE_NAME}
            RUNTIME DESTINATION bin COMPONENT ${COMPONENT_NAME_MAIN}
    )
endif ()

# Install configuration
set(COMPONENT_NAME_MAIN ${PROJECT_NAME})

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install CACHE PATH "..." FORCE)
endif ()

# Install the llmodel library
install(
        TARGETS llmodel
        LIBRARY DESTINATION lib COMPONENT ${COMPONENT_NAME_MAIN}  # .so/.dylib
        RUNTIME DESTINATION bin COMPONENT ${COMPONENT_NAME_MAIN}  # .dll
)

# Install specific model implementations based on platform and configuration
set(MODEL_IMPL_TARGETS llamamodel-mainline-cpu llamamodel-mainline-cpu-avxonly)
if (APPLE)
    list(APPEND MODEL_IMPL_TARGETS llamamodel-mainline-metal)
endif ()

install(
        TARGETS ${MODEL_IMPL_TARGETS}
        LIBRARY DESTINATION lib COMPONENT ${COMPONENT_NAME_MAIN}  # .so/.dylib
        RUNTIME DESTINATION lib COMPONENT ${COMPONENT_NAME_MAIN}  # .dll
)

# macOS code signing (optional)
if (APPLE AND GPT4ALL_SIGN_INSTALL)
    include(SignMacOSBinaries)
    install_sign_osx(${EXECUTABLE_NAME})
    install_sign_osx(llmodel)
    foreach (tgt ${MODEL_IMPL_TARGETS})
        install_sign_osx(${tgt})
    endforeach ()
endif ()

# Platform-specific packaging
if (APPLE AND BUILD_MACOS_BUNDLE)
    set(CPACK_GENERATOR "DragNDrop")
    set(CPACK_DMG_VOLUME_NAME "${PROJECT_NAME}-${VERSION}")
    set(CPACK_DMG_BACKGROUND_IMAGE "${CMAKE_SOURCE_DIR}/resources/background.png")
    set(CPACK_BUNDLE_NAME ${PROJECT_NAME})

    set(CPACK_BUNDLE_ICON "${CMAKE_SOURCE_DIR}/resources/icon.icns")
    set(CPACK_DMG_FORMAT "UDZO")
    set(CPACK_DMG_DS_STORE_SETUP_SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/create_ds_store.applescript")
    configure_file("${CMAKE_SOURCE_DIR}/scripts/create_ds_store.applescript.in"
            "${CMAKE_CURRENT_BINARY_DIR}/create_ds_store.applescript" @ONLY)
    set(CPACK_DMG_DISABLE_APPLICATIONS_SYMLINK FALSE)
elseif (WIN32)
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
    set(CPACK_NSIS_MODIFY_PATH ON)
    set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/resources/icon.ico")
    set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/resources/icon.ico")
else ()
    set(CPACK_GENERATOR "TGZ;DEB")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Your Name <your.email@example.com>")
    set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.17), libstdc++6 (>= 4.8)")
endif ()

# Common CPack settings
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})
set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${VERSION}-${CMAKE_SYSTEM_NAME}")
set(CPACK_PACKAGE_CONTACT "youremail@example.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Installer for ${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "YourCompany")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${PROJECT_NAME}")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://example.com")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE.txt")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")

# Set the output directory for the DMG file
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_BINARY_DIR}/package")


# Debug output
message(STATUS "PROJECT_NAME: ${PROJECT_NAME}")
message(STATUS "VERSION: ${VERSION}")
message(STATUS "CPACK_DMG_VOLUME_NAME: ${CPACK_DMG_VOLUME_NAME}")
message(STATUS "CPACK_PACKAGE_FILE_NAME: ${CPACK_PACKAGE_FILE_NAME}")
message(STATUS "MACOSX_BUNDLE_ICON_FILE: ${MACOSX_BUNDLE_ICON_FILE}")
message(STATUS "APP_ICON_MACOSX: ${APP_ICON_MACOSX}")
message(STATUS "DMG_BACKGROUND_IMAGE: ${DMG_BACKGROUND_IMAGE}")

# Include CPack to generate installers
include(CPack)
