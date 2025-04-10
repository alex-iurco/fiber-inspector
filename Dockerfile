FROM ubuntu:22.04

# Avoid interactive prompts
ENV DEBIAN_FRONTEND=noninteractive

# Install only essential dependencies for Qt and OpenCV
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libopencv-dev \
    qt6-base-dev \
    libgl1-mesa-dev \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Set up the working directory
WORKDIR /app

# Default command to keep the container running
CMD ["bash"] 