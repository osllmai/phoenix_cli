# app/CMakeLists.txt

set(EXE_SOURCE
        "main.cc"
        "tray_icon_manager.cc"
        "tray_icon_manager.h"
        "resources.qrc"
)

# Add WIN32 for Windows GUI applications to prevent console window
if(WIN32)
    add_executable(${EXECUTABLE_NAME} WIN32 ${EXE_SOURCE})
else()
    add_executable(${EXECUTABLE_NAME} ${EXE_SOURCE})
endif()

# Link against Qt6::Widgets and other necessary libraries
target_link_libraries(${EXECUTABLE_NAME} PUBLIC
        ${LIBRARY_NAME}
        Qt6::Widgets
        Qt6::Core
        Qt6::Gui
        Qt6::Network
)

# Ensure CMake processes Qt resources
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
