if(APPLE)
elseif(UNIX)
elseif(WIN32)
        # Enable CUDA
        find_package(CUDA REQUIRED)
        set(CUDA_SEPARABLE_COMPILATION ON)
        set(CUDA_PROPAGATE_HOST_FLAGS OFF)

        # Include directories and link libraries for GPU
        include_directories(${CUDA_INCLUDE_DIRS})
        link_libraries(${CUDA_LIBRARIES})

        # Define preprocessor macros for CUDA
        add_definitions(-DGGML_USE_CUDA)
endif()

set(LIBRARY_SOURCES
        "commands_list.cc"
        "models_list.cc"
        "download_model.cc"
        "chat.cc"
        "chat_manager.cc"
        "directory_manager.cc")


set(LIBRARY_HEADERS
        "commands_list.h"
        "models_list.h"
        "download_model.h"
        "header.h"
        "parse_json.h"
        "utils.h"
        "directory_manager.h"
        "chat_manager.h"
        "${PROJECT_SOURCE_DIR}/phoenix/llmodel.h")

set(LIBRARY_INCLUDES
        "./"
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/phoenix)

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
)

find_package(CURL REQUIRED)

if (APPLE)
        target_link_libraries(${LIBRARY_NAME} PUBLIC
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        CURL::libcurl
        llmodel
        llamamodel-mainline-cpu
        llamamodel-mainline-cpu-avxonly
        llamamodel-mainline-metal)
elseif(UNIX)

elseif(WIN32)

        target_link_libraries(${LIBRARY_NAME} PUBLIC
                spdlog::spdlog
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
