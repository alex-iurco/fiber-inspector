# Fiber Inspector

A Qt/C++ application for fiber-optic inspection, analysis, and defect detection.

## Overview

Fiber Inspector is a demonstration application that showcases C++/Qt programming skills combined with image processing capabilities specifically designed for fiber inspection microscopes. The application provides a user-friendly interface for loading, processing, analyzing, and saving fiber inspection images.

## Features

- **Qt-based GUI**: Modern interface with image display and control panels
- **Image Processing**: Multiple filters and adjustments using OpenCV
- **Fiber Analysis**: Algorithms to detect and classify fiber defects
- **Linux Integration**: Utilizes Linux-specific features for hardware integration
- **Results Management**: Save, load, and export analysis results

## Requirements

- C++17 compatible compiler
- Qt 6.x
- OpenCV 4.x
- CMake 3.14+
- Linux operating system (for full functionality)

## Building the Application

### Option 1: Native Build (Linux)

1. Make sure you have all dependencies installed:

```bash
# For Ubuntu/Debian
sudo apt update
sudo apt install build-essential cmake qt6-base-dev libopencv-dev
```

2. Clone the repository:

```bash
git clone https://github.com/yourusername/FiberInspector.git
cd FiberInspector
```

3. Create a build directory and compile:

```bash
mkdir build && cd build
cmake ..
make
```

4. Run the application:

```bash
./FiberInspector
```

### Option 2: Docker Development Environment (Cross-Platform)

We provide a Docker-based development environment that includes all required dependencies. This is the recommended approach, especially on non-Linux systems.

See [README-DOCKER.md](README-DOCKER.md) for detailed instructions on using the Docker environment.

Quick start:

```bash
# Build and start the container
docker-compose build
docker-compose up -d

# Enter the container
docker-compose exec dev bash

# Build the project inside the container
./setup.sh
```

## Project Structure

- **src/**: Source files
  - **main.cpp**: Application entry point
  - **mainwindow.cpp**: Main application window
  - **imageprocessor.cpp**: Image processing functionality
  - **fiberanalyzer.cpp**: Fiber analysis algorithms
  - **resultsmanager.cpp**: Results and report management
- **include/**: Header files
- **resources/**: Application resources (icons, sample images)
- **docs/**: Documentation
- **tests/**: Unit tests

## Usage

1. Launch the application
2. Use the "Open" button to load a fiber image
3. Apply filters or adjustments as needed
4. Click "Analyze Fiber" to perform defect detection
5. View the results and export reports if needed

## Linux System Integration

The application demonstrates Linux integration through:

- System resource checking (memory/disk/GPU)
- Process management for background tasks
- Access to system information via native Linux APIs
- Camera device access for live imaging

## License

MIT License

## Contact

Your Name - your.email@example.com

## Acknowledgments

- OpenCV for image processing capabilities
- Qt for the GUI framework 