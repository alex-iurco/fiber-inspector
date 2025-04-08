#!/bin/bash

# Create directory structure
mkdir -p resources/icons resources/sample_images

# Base64-encoded minimal 1x1 transparent PNG
TINY_PNG_BASE64="iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAQAAAC1HAwCAAAAC0lEQVR42mNkYAAAAAYAAjCB0C8AAAAASUVORK5CYII="

# Function to decode base64 string to a PNG file
create_png() {
  local filename=$1
  echo "Creating $filename..."
  echo $TINY_PNG_BASE64 | base64 -d > $filename
  if [ -f "$filename" ]; then
    echo "✅ Created $filename ($(stat -f%z $filename) bytes)"
  else
    echo "❌ Failed to create $filename"
  fi
}

# Create icon files
echo "==== Creating icon files ===="
create_png "resources/icons/open.png"
create_png "resources/icons/save.png"
create_png "resources/icons/analyze.png"
create_png "resources/icons/settings.png"

# Create sample image files
echo -e "\n==== Creating sample image files ===="
create_png "resources/sample_images/fiber1.png"
create_png "resources/sample_images/fiber2.png"

echo -e "\nAll resources created!" 