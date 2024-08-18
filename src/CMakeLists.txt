if (APPLE)
elseif (UNIX)
elseif (WIN32)
    # Enable CUDA
    find_package(CUDA REQUIRED)
    set(CUDA_SEPARABLE_COMPILATION ON)
    set(CUDA_PROPAGATE_HOST_FLAGS OFF)

    # Define preprocessor macros for CUDA
    add_definitions(-DGGML_USE_CUDA)
endif ()

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
        ${PROJECT_SOURCE_DIR}/phoenix)


find_package(CURL REQUIRED)
find_package(Boost 1.70 REQUIRED COMPONENTS system)
find_package(SQLite3 REQUIRED)



add_library(${LIBRARY_NAME}
        STATIC
        ${LIBRARY_SOURCES}
        ${LIBRARY_HEADERS})

if (APPLE)
    add_dependencies(${LIBRARY_NAME} ggml-metal)
    message("ggml-metal added")
endif ()

target_include_directories(${LIBRARY_NAME} PUBLIC
        ${LIBRARY_INCLUDES}
        ${Boost_INCLUDE_DIRS}
)


if (APPLE)
    target_link_libraries(${LIBRARY_NAME} PUBLIC
            nlohmann_json::nlohmann_json
            CURL::libcurl
            SQLite::SQLite3
            llmodel
            Boost::system
            llamamodel-mainline-cpu
            llamamodel-mainline-cpu-avxonly
            llamamodel-mainline-metal)
elseif (UNIX)
    # Add UNIX-specific configurations here if needed
elseif (WIN32)
    target_link_libraries(${LIBRARY_NAME} PUBLIC
            nlohmann_json::nlohmann_json
            CURL::libcurl
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
    )

    # Set CUDA architecture (optional, adjust according to your GPU)
    set_target_properties(${LIBRARY_NAME} PROPERTIES CUDA_ARCHITECTURES "52;60;61;70;75;80;86")

    # Set CUDA runtime library (static or shared)
    set_target_properties(${LIBRARY_NAME} PROPERTIES CUDA_RUNTIME_LIBRARY Static)
endif ()
