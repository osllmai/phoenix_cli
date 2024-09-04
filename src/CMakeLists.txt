cmake_minimum_required(VERSION 3.18)

# Set policy to avoid CMP0146 warning
cmake_policy(SET CMP0146 NEW)

if (APPLE)
    set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "Build architectures for Mac OS X" FORCE)
endif ()

# Platform-specific configurations
if (APPLE)
    # Configuration for Apple (macOS)
elseif (UNIX)
    # Configuration for Unix/Linux
elseif (WIN32)
    enable_language(CUDA)
    set(CUDA_SEPARABLE_COMPILATION ON)
    set(CUDA_PROPAGATE_HOST_FLAGS OFF)

    add_definitions(-DGGML_USE_CUDA)

    # Boost configuration
    set(Boost_USE_STATIC_LIBS ON)
    set(Boost_USE_DEBUG_LIBS OFF)
    set(Boost_USE_RELEASE_LIBS ON)
    set(Boost_USE_MULTITHREADED ON)
    set(Boost_USE_STATIC_RUNTIME ON)
endif ()

# Include directories for dependencies
include_directories(${OPENSSL_INCLUDE_DIR})

# Find required packages
find_package(CURL REQUIRED)
# Link SQLite3
find_package(SQLite3 REQUIRED)
find_package(Boost 1.85 REQUIRED COMPONENTS system filesystem)
# Link pthread (required by Crow)
find_package(Threads REQUIRED)
# OpenSSL is required by Crow
find_package(OpenSSL REQUIRED)

# Define source and header files
set(LIBRARY_SOURCES
        "models_list.cc"
        "chat_manager.cc"
        "directory_manager.cc"
        "chat.cc"
        "commands_list.cc"
        "download_model.cc"
        "database_manager.cc"
        "api/api.cc"
        "api/models/database.cc"
        "api/models/user.cc"
        "api/models/profile.cc"
        "api/models/workspace.cc"
        "api/models/folder.cc"
        "api/models/file.cc"
        "api/models/file_item.cc"
        "api/models/preset.cc"
        "api/controllers/auth_controller.cc"
        "api/controllers/workspace_controller.cc"
        "api/controllers/folder_controller.cc"
        "api/controllers/file_controller.cc"
        "api/controllers/file_item_controller.cc"
        "api/controllers/preset_controller.cc"
        "api/controllers/streaming_controller.cc"
        "api/controllers/profile_controller.cc"
        "api/routes/auth.cc"
        "api/routes/workspace.cc"
        "api/routes/profile.cc"
        "api/routes/folder.cc"
        "api/routes/file.cc"
        "api/routes/file_item.cc"
        "api/routes/preset.cc"
        "api/routes/streaming.cc"
)

set(LIBRARY_HEADERS
        "${PROJECT_SOURCE_DIR}/phoenix/llmodel.h"
        "commands_list.h"
        "models_list.h"
        "download_model.h"
        "header.h"
        "parse_json.h"
        "utils.h"
        "directory_manager.h"
        "chat_manager.h"
        "database_manager.h"
        "api/include/crow_all.h"
        "api/include/models/database.h"
        "api/include/models/user.h"
        "api/include/models/workspace.h"
        "api/include/models/folder.h"
        "api/include/models/file.h"
        "api/include/models/file_item.h"
        "api/include/models/profile.h"
        "api/include/models/preset.h"
        "api/include/controllers/auth_controller.h"
        "api/include/controllers/profile_controller.h"
        "api/include/controllers/streaming_controller.h"
        "api/include/controllers/folder_controller.h"
        "api/include/controllers/file_controller.h"
        "api/include/controllers/file_item_controller.h"
        "api/include/controllers/preset_controller.h"
        "api/include/controllers/workspace_controller.h"
        "api/include/routes/auth.h"
        "api/include/routes/profile.h"
        "api/include/routes/workspace.h"
        "api/include/routes/folder.h"
        "api/include/routes/file.h"
        "api/include/routes/file_item.h"
        "api/include/routes/preset.h"
        "api/include/routes/streaming.h"
)

# Include directories
set(LIBRARY_INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/phoenix
        ${Boost_INCLUDE_DIRS}
        ${SQLite3_INCLUDE_DIR}
        ${CURL_INCLUDE_DIR}
        ${asio_SOURCE_DIR}/asio/include
        ${sqlite_modern_cpp_SOURCE_DIR}/hdr
        ${jwt-cpp_SOURCE_DIR}/include
        ${sqlite_modern_cpp_SOURCE_DIR}/hdr
        ${nlohmann_json_SOURCE_DIR}/include

)

# Create the static library
add_library(${LIBRARY_NAME} STATIC ${LIBRARY_SOURCES} ${LIBRARY_HEADERS})

# Include directories for the library
target_include_directories(${LIBRARY_NAME} SYSTEM PUBLIC ${LIBRARY_INCLUDES})

# Link directories for CUDA on Windows
if (WIN32)
    target_link_directories(${LIBRARY_NAME} PUBLIC ${CUDA_INCLUDE_DIRS})
endif ()

# Link libraries based on the platform
if (APPLE)
    target_link_libraries(${LIBRARY_NAME} PUBLIC
            nlohmann_json::nlohmann_json
            CURL::libcurl
            SQLite::SQLite3
            llmodel
            Boost::system
            llamamodel-mainline-cpu
            llamamodel-mainline-cpu-avxonly
            llamamodel-mainline-metal
            Crow::Crow
            jwt-cpp
            OpenSSL::SSL
            OpenSSL::Crypto
            Threads::Threads
    )
elseif (UNIX)
    target_link_libraries(${LIBRARY_NAME} PUBLIC
            nlohmann_json::nlohmann_json
            CURL::libcurl
            SQLite::SQLite3
            llmodel
            Boost::system
            Boost::filesystem
    )
elseif (WIN32)
    find_package(Kompute QUIET)

    if (Kompute_FOUND)
        target_link_libraries(${LIBRARY_NAME} PUBLIC
                nlohmann_json::nlohmann_json
                llmodel
                llama-mainline-kompute
                llama-mainline-kompute-avxonly
                llamamodel-mainline-kompute
                llamamodel-mainline-kompute-avxonly
                llama-mainline-cuda
                llamamodel-mainline-cuda
                llama-mainline-cuda-avxonly
                llamamodel-mainline-kompute-avxonly
                ${CUDA_LIBRARIES}
                ${CURL_LIBRARY}
                Boost::system
                Boost::filesystem
                ${SQLite3_LIBRARY}
        )
    else ()
        target_link_libraries(${LIBRARY_NAME} PUBLIC
                nlohmann_json::nlohmann_json
                llmodel
                llama-mainline-kompute
                llama-mainline-kompute-avxonly
                llamamodel-mainline-kompute
                llamamodel-mainline-kompute-avxonly
                llama-mainline-cuda
                llamamodel-mainline-cuda
                llama-mainline-cuda-avxonly
                llamamodel-mainline-kompute-avxonly
                ${CUDA_LIBRARIES}
                ${CURL_LIBRARY}
                Boost::system
                Boost::filesystem
                ${SQLite3_LIBRARY}
        )
    endif ()

    # CUDA-specific properties
    set_target_properties(${LIBRARY_NAME} PROPERTIES
            CUDA_ARCHITECTURES "52;60;61;70;75;80;86"  # Adjust to your GPU
            CUDA_RUNTIME_LIBRARY Static
    )
endif ()
