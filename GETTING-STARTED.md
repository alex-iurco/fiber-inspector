# Getting Started with Fiber Inspector

This document provides a comprehensive guide to setting up and running the Fiber Inspector project.

## Project Overview

The Fiber Inspector application is designed to showcase C++/Qt programming skills for fiber inspection microscopes. It includes:

- A modern Qt GUI for image display and manipulation
- OpenCV-based image processing for fiber analysis
- Linux system integration features

## Development Options

You have two main options for developing this project:

### Option 1: Native Development

Requirements:
- C++17 compatible compiler
- Qt 6.x (or Qt 5.x with our updated CMakeLists.txt)
- OpenCV 4.x
- CMake 3.14+
- Linux environment (preferred, though partial functionality will work on macOS/Windows)

Installation on Ubuntu/Debian:
```bash
sudo apt update
sudo apt install build-essential cmake qt6-base-dev libopencv-dev
```

Building:
```bash
mkdir build && cd build
cmake ..
make
```

### Option 2: Docker Development Environment (Recommended)

This is the recommended approach as it ensures all dependencies are correctly configured in an isolated environment.

Requirements:
- Docker
- Docker Compose
- XQuartz (for macOS) or X11 (for Linux) if running GUI applications

Steps:

1. Start Docker Desktop or the Docker daemon:
   - On macOS/Windows: Open Docker Desktop
   - On Linux: `sudo systemctl start docker`

2. Build the Docker image:
   ```bash
   docker-compose build
   ```

3. Start the container:
   ```bash
   docker-compose up -d
   ```

4. Enter the container:
   ```bash
   docker-compose exec dev bash
   ```

5. Build the project inside the container:
   ```bash
   ./setup.sh
   ```

6. Run the application (inside the container):
   ```bash
   cd build
   ./FiberInspector
   ```

## Project Structure

- **include/**: Header files
  - **mainwindow.h**: Main window class
  - **imageprocessor.h**: Image processing functionality
  - **fiberanalyzer.h**: Fiber analysis algorithms
  - **resultsmanager.h**: Result management and export

- **src/**: Source files
  - **main.cpp**: Application entry point
  - **mainwindow.cpp**: Main window implementation
  - **imageprocessor.cpp**: Image processing implementation
  - **fiberanalyzer.cpp**: Fiber analysis implementation
  - **resultsmanager.cpp**: Result management implementation
  - **mainwindow.ui**: Qt Designer UI file

- **resources/**: Application resources
  - **icons/**: Application icons
  - **sample_images/**: Sample fiber images

- **Docker files**:
  - **Dockerfile**: Docker container definition
  - **docker-compose.yml**: Docker Compose configuration
  - **setup.sh**: Build script for inside the container

## Key Components

### MainWindow

The main application window that provides:
- Menu/toolbars for user interactions
- Image display area with scrolling/zooming
- Control panels for filters and adjustments
- Status bar with progress reporting

### ImageProcessor

Provides image processing capabilities:
- Loading and saving images
- Applying various filters (threshold, edge detection, etc.)
- Brightness/contrast adjustments
- OpenCV integration with Qt

### FiberAnalyzer

Core analysis functionality:
- Detecting fiber center and measurements
- Identifying defects in fibers
- Classifying defect types and severity
- Generating analysis reports

### ResultsManager

Handles analysis results:
- Saving/loading analysis data
- Exporting reports in various formats
- Session management
- Backup and device integration

## Next Steps

After setting up the environment:

1. Explore the codebase to understand the structure
2. Run the application and test basic functionality
3. Consider adding more sophisticated fiber analysis algorithms
4. Implement additional filters or detection methods
5. Add unit tests for key components

For additional help or questions, please refer to the README files or create an issue on our GitHub repository. 