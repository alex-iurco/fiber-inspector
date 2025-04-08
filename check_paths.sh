#!/bin/bash

echo "===== Path Checks ====="
echo "Current directory: $(pwd)"
echo "Current user: $(whoami)"
echo "Current user home: $HOME"

echo -e "\n===== Directory Structure ====="
ls -la

echo -e "\n===== CMake Version ====="
cmake --version

echo -e "\n===== Environment Variables ====="
env | sort

echo -e "\n===== Mount Points ====="
mount | grep app

echo -e "\n===== Build Directory Status ====="
if [ -d "build" ]; then
  echo "Build directory exists."
  echo "Contents:"
  ls -la build/
  if [ -f "build/CMakeCache.txt" ]; then
    echo -e "\nCMakeCache.txt exists. Showing first 10 lines:"
    head -10 build/CMakeCache.txt
  else
    echo "CMakeCache.txt does not exist."
  fi
else
  echo "Build directory does not exist."
fi 