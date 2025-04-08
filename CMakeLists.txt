cmake_minimum_required(VERSION 3.14)

project(FiberInspector VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Option to enable debug mode
option(ENABLE_DEBUG "Enable debug configuration with symbols" OFF)

if(ENABLE_DEBUG)
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/DebugOptions.cmake)
else()
    # Default to Release mode without debug symbols
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Release)
    endif()
endif()

# Check if running in Docker
if(EXISTS "/.dockerenv")
    message(STATUS "Running inside Docker container")
    # Explicitly set the Qt6 CMAKE_PREFIX_PATH for Docker environment
    set(CMAKE_PREFIX_PATH "/usr/lib/x86_64-linux-gnu/cmake")
    message(STATUS "Setting Qt6 prefix path for Docker: ${CMAKE_PREFIX_PATH}")
endif()

# Find OpenGL first (needed for Qt6)
find_package(OpenGL QUIET)
if(OpenGL_FOUND)
    message(STATUS "OpenGL found")
else()
    message(STATUS "OpenGL not found - some Qt6 features may be disabled")
endif()

# Find Qt packages with less strict requirements
option(USE_QT6 "Prefer Qt6 over Qt5 if available" ON)
if(USE_QT6)
    find_package(Qt6 COMPONENTS Core QUIET)
    if(Qt6Core_FOUND)
        message(STATUS "Found Qt6Core, will use Qt6")
        find_package(Qt6 COMPONENTS Widgets Gui REQUIRED)
        set(QT_VERSION_MAJOR 6)
    else()
        message(STATUS "Qt6Core not found, trying Qt5")
        find_package(Qt5 COMPONENTS Core Widgets Gui REQUIRED)
        set(QT_VERSION_MAJOR 5)
    endif()
else()
    find_package(Qt5 COMPONENTS Core Widgets Gui REQUIRED)
    set(QT_VERSION_MAJOR 5)
endif()

message(STATUS "Using Qt${QT_VERSION_MAJOR}")

# Find OpenCV
find_package(OpenCV REQUIRED)
message(STATUS "OpenCV version: ${OpenCV_VERSION}")

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${OpenCV_INCLUDE_DIRS}
)

# Source files
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/imageprocessor.cpp
    src/fiberanalyzer.cpp
    src/resultsmanager.cpp
)

# Header files
set(HEADERS
    include/mainwindow.h
    include/imageprocessor.h
    include/fiberanalyzer.h
    include/resultsmanager.h
)

# UI files
set(UI_FILES
    src/mainwindow.ui
)

# Resources
set(RESOURCES
    resources/resources.qrc
)

# Create main executable
add_executable(FiberInspector
    ${SOURCES}
    ${HEADERS}
    ${UI_FILES}
    ${RESOURCES}
)

# Link libraries for main executable
if(QT_VERSION_MAJOR EQUAL 6)
    target_link_libraries(FiberInspector PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        ${OpenCV_LIBS}
    )
    if(OpenGL_FOUND)
        target_link_libraries(FiberInspector PRIVATE OpenGL::GL)
    endif()
else()
    target_link_libraries(FiberInspector PRIVATE
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        ${OpenCV_LIBS}
    )
endif()

# Create test executable
add_executable(TestCoreFunctionality
    test_core_functionality.cpp
    src/imageprocessor.cpp
    src/fiberanalyzer.cpp
    src/resultsmanager.cpp
    include/imageprocessor.h
    include/fiberanalyzer.h
    include/resultsmanager.h
)

# Link libraries for test executable (no UI dependencies)
if(QT_VERSION_MAJOR EQUAL 6)
    target_link_libraries(TestCoreFunctionality PRIVATE
        Qt6::Core
        Qt6::Gui
        ${OpenCV_LIBS}
    )
else()
    target_link_libraries(TestCoreFunctionality PRIVATE
        Qt5::Core
        Qt5::Gui
        ${OpenCV_LIBS}
    )
endif()

# Install
install(TARGETS FiberInspector DESTINATION bin)
install(TARGETS TestCoreFunctionality DESTINATION bin)

# Output build information
message(STATUS "CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "CMAKE_CXX_COMPILER_VERSION: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "OpenCV libraries: ${OpenCV_LIBS}")

# Print CMake search paths for debugging
message(STATUS "CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")
message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
get_directory_property(_vars DIRECTORY ${CMAKE_SOURCE_DIR} INCLUDE_DIRECTORIES)
message(STATUS "INCLUDE_DIRECTORIES: ${_vars}")
get_directory_property(_vars COMPILE_DEFINITIONS)
message(STATUS "COMPILE_DEFINITIONS: ${_vars}") 