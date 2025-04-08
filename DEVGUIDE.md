# Fiber Inspector Development Guide

This guide explains how to set up and use different development environments for the Fiber Inspector project.

## Development Environments

The project supports two Docker-based development environments:

1. **Production Environment**: Minimal dependencies, optimized build
2. **Debug Environment**: Full development tools, debug symbols, and GDB/LLDB integration

## Using the Production Environment

The production environment is suitable for normal development, testing features, and building release versions.

```bash
# Start the production environment
./use_prod_env.sh

# Inside Docker container
./rebuild.sh
cd build && ./FiberInspector
```

## Using the Debug Environment

The debug environment adds debugging tools and integrates with Cursor/VSCode for debugging.

```bash
# Start the debug environment
./use_debug_env.sh

# Inside Docker container to build with debug symbols
./rebuild.sh debug
```

## Debugging with Cursor

To debug the application using Cursor/VSCode:

1. Start the debug Docker environment: `./use_debug_env.sh`
2. Inside the container, rebuild with debug symbols: `./rebuild.sh debug`
3. Start the application in debug mode: `./debug_app.sh`
4. In Cursor/VSCode, select the "Docker: Debug FiberInspector" debug configuration
5. Start debugging with F5 or the debug button
6. Set breakpoints in the code and inspect variables

For testing core functionality:

1. Start the debug environment
2. Build with debug symbols: `./rebuild.sh debug`
3. Start the test in debug mode: `./debug_test.sh`
4. In Cursor/VSCode, select the "Docker: Debug TestCoreFunctionality" debug configuration
5. Start debugging

## CMake Debug Options

You can directly control debug settings with CMake:

```bash
# Enable debug
cmake -DENABLE_DEBUG=ON ..

# Disable debug (default)
cmake -DENABLE_DEBUG=OFF ..
```

## Building Without Docker

If building directly on your host machine:

```bash
# Production build
mkdir -p build && cd build
cmake ..
make -j$(nproc)

# Debug build
mkdir -p build && cd build
cmake -DENABLE_DEBUG=ON ..
make -j$(nproc)
``` 