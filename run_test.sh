#!/bin/bash

echo "===== Testing FiberInspector ====="

# Navigate to build directory
cd build || { echo "Build directory not found"; exit 1; }

# First, rebuild the project
echo "Rebuilding project..."
cmake ..
make -j$(nproc)

# Test main application in offscreen mode
echo -e "\nTesting main application..."
QT_QPA_PLATFORM=offscreen ./FiberInspector -v

# Test core functionality
echo -e "\nTesting core functionality..."
QT_QPA_PLATFORM=offscreen ./TestCoreFunctionality

echo -e "\n===== Tests Complete =====" 