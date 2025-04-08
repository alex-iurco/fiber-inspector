FROM ubuntu:22.04

# Avoid interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Set up timezone (needed for some package installations)
ENV TZ=UTC
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libopencv-dev \
    libopencv-contrib-dev \
    libopencv-core-dev \
    libopencv-imgproc-dev \
    pkg-config \
    qt6-base-dev \
    qt6-declarative-dev \
    qt6-tools-dev \
    libqt6opengl6-dev \
    libqt6widgets6-dev \
    libqt6gui6-dev \
    libqt6core6-dev \
    ninja-build \
    gdb \
    valgrind \
    wget \
    unzip \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Create a non-root user
ARG USERNAME=developer
ARG USER_UID=1000
ARG USER_GID=$USER_UID

RUN groupadd --gid $USER_GID $USERNAME \
    && useradd --uid $USER_UID --gid $USER_GID -m $USERNAME \
    && apt-get update \
    && apt-get install -y sudo \
    && echo $USERNAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME \
    && chmod 0440 /etc/sudoers.d/$USERNAME

# Set the default user
USER $USERNAME
WORKDIR /home/$USERNAME/FiberInspector

# Set up environment variables
ENV PATH="/home/$USERNAME/.local/bin:${PATH}"

# Display versions of installed components
RUN echo "Installed versions:" && \
    cmake --version && \
    g++ --version && \
    pkg-config --modversion opencv4 || echo "OpenCV version check failed" && \
    echo "Qt6 installed: $(dpkg -l | grep qt6-base-dev | wc -l)"

CMD ["/bin/bash"] 