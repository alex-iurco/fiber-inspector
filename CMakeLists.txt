cmake_minimum_required(VERSION 3.14)

project(FiberInspector VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt packages
find_package(Qt6 COMPONENTS Widgets Core Gui QUIET)
if (NOT Qt6_FOUND)
    message(STATUS "Qt6 not found, trying to find Qt5")
    find_package(Qt5 COMPONENTS Widgets Core Gui REQUIRED)
    set(QT_VERSION_MAJOR 5)
else()
    set(QT_VERSION_MAJOR 6)
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

# Create executable
add_executable(FiberInspector
    ${SOURCES}
    ${HEADERS}
    ${UI_FILES}
    ${RESOURCES}
)

# Link libraries
if(QT_VERSION_MAJOR EQUAL 6)
    target_link_libraries(FiberInspector PRIVATE
        Qt6::Widgets
        Qt6::Core
        Qt6::Gui
        ${OpenCV_LIBS}
    )
else()
    target_link_libraries(FiberInspector PRIVATE
        Qt5::Widgets
        Qt5::Core
        Qt5::Gui
        ${OpenCV_LIBS}
    )
endif()

# Install
install(TARGETS FiberInspector DESTINATION bin)

# Output build information
message(STATUS "CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "CMAKE_CXX_COMPILER_VERSION: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "OpenCV libraries: ${OpenCV_LIBS}")

# Check if we're running in Docker
if(EXISTS "/.dockerenv")
    message(STATUS "Running inside Docker container")
endif() 