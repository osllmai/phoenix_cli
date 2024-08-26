cmake_minimum_required(VERSION 3.18) # Ensure you are using a sufficiently new CMake version

# Set policy to avoid CMP0146 warning
cmake_policy(SET CMP0146 NEW)


# Check the platform and configure accordingly
if (APPLE)
    # Configuration for Apple (macOS)
elseif (UNIX)
    # Configuration for Unix/Linux
elseif (WIN32)
    enable_language(CUDA)
    set(CUDA_SEPARABLE_COMPILATION ON)
    set(CUDA_PROPAGATE_HOST_FLAGS OFF)

    add_definitions(-DGGML_USE_CUDA)

    # set(CURL_LIBRARY "C:/vcpkg/packages/curl_x64-windows/lib/libcurl.lib")
    # set(CURL_INCLUDE_DIR "C:/vcpkg/packages/curl_x64-windows/include")

    # set(SQLite3_LIBRARY "C:/vcpkg/packages/sqlite3_x64-windows/lib/sqlite3.lib")
    # set(SQLite3_INCLUDE_DIR "C:/vcpkg/packages/sqlite3_x64-windows/include")

    # set(Boost_SYSTEM_LIBRARY "C:/vcpkg/packages/boost-system_x64-windows/lib/boost_system-vc144-mt-x64-1_85.lib")
    # set(Boost_INCLUDE_DIR "C:/vcpkg/packages/boost-system_x64-windows/include")
    set(Boost_USE_STATIC_LIBS ON)
    set(Boost_USE_DEBUG_LIBS OFF)
    set(Boost_USE_RELEASE_LIBS ON)
    set(Boost_USE_MULTITHREADED ON)
    set(Boost_USE_STATIC_RUNTIME ON)



    # include_directories(${SQLite3_INCLUDE_DIR})

    # # CUDA Architecture settings
    # set(CMAKE_CUDA_ARCHITECTURES 75)
    # set_target_properties(${LIBRARY_NAME} PROPERTIES CUDA_ARCHITECTURES "75")
    # set_target_properties(${LIBRARY_NAME} PROPERTIES CUDA_RUNTIME_LIBRARY Static)
endif()

# Find packages globally (use the variables set above)
find_package(CURL REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(Boost 1.85 REQUIRED COMPONENTS system filesystem)


# Define sources and headers
set(LIBRARY_SOURCES
        "models_list.cc"
        "chat_manager.cc"
        "directory_manager.cc"
        "chat.cc"
        "commands_list.cc"
        "download_model.cc"
        "web_server.cc"
        "database_manager.cc"
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
        "web_server.h"
        "database_manager.h"
)

set(LIBRARY_INCLUDES
        "./"
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/phoenix
)

# Create the static library
add_library(${LIBRARY_NAME} STATIC
        ${LIBRARY_SOURCES}
        ${LIBRARY_HEADERS}
)

# Include directories for the library
target_include_directories(${LIBRARY_NAME} PUBLIC
        ${LIBRARY_INCLUDES}
        ${Boost_INCLUDE_DIRS}
        ${SQLite3_INCLUDE_DIR}
        ${CURL_INCLUDE_DIR}
)

if(WIN32)
    target_link_directories(${LIBRARY_NAME} PUBLIC ${CUDA_INCLUDE_DIRS})
endif()

# Link the appropriate libraries based on the platform
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
    endif()
    # Set CUDA architecture (adjust according to your GPU)
    set_target_properties(${LIBRARY_NAME} PROPERTIES CUDA_ARCHITECTURES "52;60;61;70;75;80;86")
    # Set CUDA runtime library (static or shared)
    set_target_properties(${LIBRARY_NAME} PROPERTIES CUDA_RUNTIME_LIBRARY Static)
endif()
