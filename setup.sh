#!/bin/bash

# Exit on any error
set -e

# Create build directory
mkdir -p build
cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake ..

# Build the project
echo "Building the project..."
make -j$(nproc)

echo "Build completed successfully!"
echo "Run the application with: ./FiberInspector" 