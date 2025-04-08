#!/bin/bash

# Exit on any error
set -e

echo "===== Fiber Inspector Setup ====="
echo "Running in: $(pwd)"

# Check if we're in a Docker container
if [ -f /.dockerenv ]; then
    echo "Detected Docker environment"
    # Check Qt packages
    echo "Checking Qt packages..."
    ./check_qt.sh
    
    # Check OpenGL packages
    echo "Checking OpenGL packages..."
    ./check_opengl.sh
fi

# Create build directory
echo "Creating build directory..."
mkdir -p build
cd build

# Configure with CMake
echo "Configuring with CMake..."
# Force Qt6 usage with the USE_QT6 option
cmake -DCMAKE_BUILD_TYPE=Debug -DUSE_QT6=ON ..

# Build the project
echo "Building the project..."
make -j$(nproc)

echo "Build completed successfully!"
echo "Run the application with: ./FiberInspector" 