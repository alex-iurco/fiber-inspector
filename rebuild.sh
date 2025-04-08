#!/bin/bash

echo "===== Rebuilding Fiber Inspector ====="
echo "Current directory: $(pwd)"

# Remove old build directory
echo "Removing old build directory..."
rm -rf build

# Create fresh build directory
echo "Creating new build directory..."
mkdir -p build
cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake -DCMAKE_BUILD_TYPE=Debug -DUSE_QT6=ON ..

# Build the project
echo "Building the project..."
make -j$(nproc)

echo "Build completed!"
echo "Run the application with: ./FiberInspector" 