if (WHISPER_SDL2)
    # talk-llama
    target_link_libraries(${LIBRARY_NAME} talk-llama.cpp
            llama.cpp
            llama-vocab.cpp
            llama-grammar.cpp
            llama-sampling.cpp
            unicode.cpp
            unicode-data.cpp)
    target_include_directories(${LIBRARY_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})

    if (WHISPER_CLBLAST)
        set(CLBLAST_LIBNAME clblast)
    endif ()
    target_link_libraries(${LIBRARY_NAME} PRIVATE whisper-common common-sdl whisper ${SDL2_LIBRARIES} ${CLBLAST_LIBNAME} ${CMAKE_THREAD_LIBS_INIT})

    if(WIN32)
        # It requires Windows 8.1 or later for PrefetchVirtualMemory
        target_compile_definitions(${LIBRARY_NAME} PRIVATE -D_WIN32_WINNT=0x0602)
    endif()

    include(DefaultTargetOptions)
endif ()
