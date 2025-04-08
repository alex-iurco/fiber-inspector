#!/bin/bash

echo "===== Checking Required Source Files ====="

# List of files we need
FILES=(
  "src/main.cpp"
  "src/mainwindow.cpp"
  "src/imageprocessor.cpp"
  "src/fiberanalyzer.cpp"
  "src/resultsmanager.cpp"
  "include/mainwindow.h"
  "include/imageprocessor.h"
  "include/fiberanalyzer.h"
  "include/resultsmanager.h"
  "src/mainwindow.ui"
  "resources/resources.qrc"
)

# Check each file
for file in "${FILES[@]}"; do
  if [ -f "$file" ]; then
    echo "✅ $file exists"
  else
    echo "❌ $file is missing"
  fi
done

# Create directories if they don't exist
echo -e "\n===== Creating Missing Directories ====="
mkdir -p src include resources/icons resources/sample_images
echo "Created necessary directories" 