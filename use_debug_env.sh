#!/bin/bash

echo "===== Starting Debug Development Environment ====="

# Stop any running containers
docker-compose down

# Start the debug environment
docker-compose -f docker-compose.debug.yml up -d

# Enter the container
docker-compose -f docker-compose.debug.yml exec dev bash

echo "Debug environment stopped." 