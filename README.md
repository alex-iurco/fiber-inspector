# Fiber Inspector

A Qt/OpenCV application for optical fiber inspection and analysis.

## Overview

Fiber Inspector is a desktop application designed to analyze optical fiber images to detect defects, measure core-cladding ratios, and assess fiber quality. It provides powerful image processing capabilities and automated analysis for fiber optic technicians and quality control specialists.

## Features

- **Image Processing**: Apply various filters (grayscale, threshold, edge detection, etc.) to enhance fiber images
- **Fiber Analysis**: Automated detection of fiber core and cladding with measurements
- **Defect Detection**: Identify scratches, chips, cracks, and contamination in fiber endfaces
- **Result Management**: Save analysis results and generate reports in JSON and PDF formats
- **Modern UI**: Qt-based user interface with intuitive controls and visualization

## Technology Stack

- **C++17**: Core programming language
- **Qt6**: UI framework and cross-platform support
- **OpenCV 4.5+**: Computer vision and image processing
- **CMake**: Build system
- **Docker**: Development and testing environment

## Requirements

- C++17 compatible compiler (GCC 9+, Clang 10+, or MSVC 19.20+)
- Qt 6.0+ (Qt 5.15+ also supported)
- OpenCV 4.5+
- CMake 3.14+

## Building from Source

### Using Docker (Recommended)

```bash
# Clone the repository
git clone https://github.com/alex-iurco/fiber-inspector.git
cd fiber-inspector

# Build and run with Docker
docker-compose up -d
docker-compose exec dev bash

# Inside Docker container
cd /app
./rebuild.sh
cd build
./FiberInspector
```

### Manual Build

```bash
# Prerequisites: CMake, Qt6, OpenCV 4.5+

# Clone the repository
git clone https://github.com/alex-iurco/fiber-inspector.git
cd fiber-inspector

# Create build directory
mkdir -p build
cd build

# Configure and build
cmake ..
make -j$(nproc)

# Run the application
./FiberInspector
```

## Testing

Run the automated tests to verify core functionality:

```bash
cd fiber-inspector
./run_test.sh
```

This will build and run both the main application and the core functionality test in offscreen mode.

## Directory Structure

- `include/`: Header files
- `src/`: Source files
- `resources/`: Application resources (icons, sample images)
- `build/`: Build output directory (created during build)
- `test_core_functionality.cpp`: Core functionality test

## Key Components

- `mainwindow.cpp`: Main application window and UI
- `imageprocessor.cpp`: Image loading, processing, and filters
- `fiberanalyzer.cpp`: Fiber detection and analysis algorithms
- `resultsmanager.cpp`: Results storage and report generation

## License

Copyright (c) 2023. All rights reserved.

## Contact

- Project Repository: https://github.com/alex-iurco/fiber-inspector 