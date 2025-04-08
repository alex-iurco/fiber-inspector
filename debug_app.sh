#!/bin/bash

echo "===== Starting FiberInspector in Debug Mode ====="

# Ensure we're in the build directory
cd build || { echo "Build directory not found. Please run rebuild.sh first."; exit 1; }

# Kill any running gdbserver instances
pkill -f gdbserver || true

# Start the application with gdbserver
echo "Starting gdbserver on port 1234..."
gdbserver :1234 ./FiberInspector $@

echo "Debug session ended." 