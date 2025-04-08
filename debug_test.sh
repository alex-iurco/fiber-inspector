#!/bin/bash

echo "===== Starting TestCoreFunctionality in Debug Mode ====="

# Ensure we're in the build directory
cd build || { echo "Build directory not found. Please run rebuild.sh first."; exit 1; }

# Kill any running gdbserver instances
pkill -f gdbserver || true

# Start the test with gdbserver
echo "Starting gdbserver on port 1234..."
QT_QPA_PLATFORM=offscreen gdbserver :1234 ./TestCoreFunctionality

echo "Debug session ended." 