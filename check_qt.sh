#!/bin/bash

echo "==== Qt6 Package Check ===="
echo "Checking for Qt6 packages:"
dpkg -l | grep -E 'qt6|libqt6' | sort

echo -e "\n==== Qt6 CMake Modules ===="
echo "Looking for Qt6 CMake config files:"
find /usr -name "Qt6Config.cmake" 2>/dev/null
find /usr -name "Qt6CoreConfig.cmake" 2>/dev/null
find /usr -name "Qt6WidgetsConfig.cmake" 2>/dev/null
find /usr -name "Qt6GuiConfig.cmake" 2>/dev/null

echo -e "\n==== CMake Module Path ===="
echo "Available CMake modules:"
find /usr/lib/*/cmake -type d -name "Qt6*" | sort

echo -e "\n==== Qt6 Libraries ===="
echo "Looking for Qt6 libraries:"
find /usr/lib -name "libQt6*.so*" | head -10

echo -e "\n==== Environment Variables ===="
echo "QT_BASE_DIR: $QT_BASE_DIR"
echo "PATH: $PATH"

echo -e "\n==== OpenCV Check ===="
echo "OpenCV libraries:"
find /usr/lib -name "libopencv_*.so*" | head -10 