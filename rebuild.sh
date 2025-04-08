#!/bin/bash

# Check if debug mode is requested
if [ "$1" == "debug" ]; then
    DEBUG_FLAG="-DENABLE_DEBUG=ON"
    echo "===== Rebuilding Fiber Inspector (Debug Mode) ====="
else
    DEBUG_FLAG=""
    echo "===== Rebuilding Fiber Inspector (Production Mode) ====="
fi

# Print current directory for debugging
echo "Current directory: $(pwd)"

# Remove old build directory
echo "Removing old build directory..."
rm -rf build

# Create new build directory
echo "Creating new build directory..."
mkdir -p build
cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake $DEBUG_FLAG ..

# Build the project
echo "Building the project..."
make -j$(nproc)

echo "Build completed!"
echo "Run the application with: ./FiberInspector" 