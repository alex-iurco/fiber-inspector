# Debug configuration options
# Include this file to enable debug symbols and tools

# Set build type to Debug
set(CMAKE_BUILD_TYPE Debug)

# Add debug symbols
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

# Enable additional compiler warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

message(STATUS "Debug build configured with symbols") 