#!/bin/bash

echo "Cleaning up Docker resources..."

# Copy the minimal Dockerfile
cp Dockerfile.minimal Dockerfile

# Stop any running containers
echo "Stopping containers..."
docker-compose down

# Clean Docker build cache 
echo "Pruning Docker system..."
docker system prune -f

# Build with the minimal Dockerfile
echo "Building with minimal Dockerfile..."
docker-compose build --no-cache

echo "Done! Now run: docker-compose up -d" 