#!/bin/bash

echo "==== OpenGL Package Check ===="
echo "Checking for OpenGL packages:"
dpkg -l | grep -E 'libgl|mesa|glvnd' | sort

echo -e "\n==== OpenGL Libraries ===="
echo "Looking for OpenGL libraries:"
find /usr/lib -name "libGL*.so*" | sort
find /usr/lib -name "libOpenGL*.so*" | sort

echo -e "\n==== OpenGL Headers ===="
echo "Looking for OpenGL headers:"
find /usr/include -name "GL" | sort
find /usr/include -name "gl*.h" | sort | head -10

echo -e "\n==== pkg-config OpenGL ===="
pkg-config --modversion gl 2>/dev/null || echo "OpenGL not found in pkg-config" 