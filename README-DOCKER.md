# Docker Development Environment for Fiber Inspector

This document provides instructions for using the Docker development environment to build and run the Fiber Inspector application.

## Prerequisites

- Docker installed and running
- Docker Compose installed (typically comes with Docker Desktop)
- For GUI applications: X11 server (XQuartz on macOS, native X11 on Linux)

## Getting Started

### 1. Build the Docker Image

```bash
# From the project root directory
docker-compose build
```

### 2. Start the Development Container

```bash
# From the project root directory
docker-compose up -d
```

### 3. Enter the Container Shell

```bash
docker-compose exec dev bash
```

### 4. Build the Project

Once inside the container, you can build the project using the provided setup script:

```bash
./setup.sh
```

## Running the GUI Application

### On Linux:

1. Allow local X server connections:
   ```bash
   xhost +local:docker
   ```

2. Uncomment the X11 socket mount in `docker-compose.yml`:
   ```yaml
   volumes:
     - ./:/home/developer/FiberInspector
     - /tmp/.X11-unix:/tmp/.X11-unix
   ```

3. Restart the container:
   ```bash
   docker-compose down
   docker-compose up -d
   ```

### On macOS:

1. Install and start XQuartz:
   ```bash
   brew install --cask xquartz
   open -a XQuartz
   ```

2. In XQuartz preferences, go to the "Security" tab and check "Allow connections from network clients"

3. Quit and restart XQuartz

4. Allow connections from your local machine:
   ```bash
   xhost +localhost
   ```

5. Find your host IP address:
   ```bash
   ip=$(ifconfig en0 | grep inet | awk '$1=="inet" {print $2}')
   echo $ip
   ```

6. Set the DISPLAY variable when starting the container:
   ```bash
   DISPLAY=$ip:0 docker-compose up -d
   ```

## Stopping the Container

When you're done working, you can stop the container:

```bash
docker-compose down
```

## Troubleshooting

### GUI Applications Not Displaying

- Check that your X server is running and accepting connections
- Verify the DISPLAY environment variable is set correctly
- Try running a simple X application like `xeyes` to test the X connection

### Build Errors

- Make sure all dependencies are installed in the Dockerfile
- Check CMake configuration errors in the build output

### Container Permission Issues

- The container runs as a non-root user (developer) with sudo access
- If you encounter permission issues, you might need to adjust the USER_UID and USER_GID in the Dockerfile to match your host user's IDs 