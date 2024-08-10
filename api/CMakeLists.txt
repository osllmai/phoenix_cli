# Define the source and header files for the library
set(NETWORK_LIBRARY_HEADERS
        "web_server.h"
)

set(NETWORK_LIBRARY_SOURCES
        "web_server.cc"
)

# Define the include directories for the library
set(NETWORK_LIBRARY_INCLUDES
        "./"
        ${PROJECT_SOURCE_DIR}/phoenix
        ${PROJECT_SOURCE_DIR}/src
)

find_package(Boost 1.70 REQUIRED COMPONENTS system)

# Add the library
add_library(${NETWORK_LIBRARY_NAME} STATIC ${NETWORK_LIBRARY_HEADERS} ${NETWORK_LIBRARY_SOURCES})

# Link boost library
target_link_libraries(${NETWORK_LIBRARY_NAME} PUBLIC
        nlohmann_json::nlohmann_json
        Boost::system)

# Set the include directories for the library
target_include_directories(${NETWORK_LIBRARY_NAME} PUBLIC
        ${NETWORK_LIBRARY_INCLUDES}
        ${Boost_INCLUDE_DIRS}
        ${LIBRARY_NAME})
