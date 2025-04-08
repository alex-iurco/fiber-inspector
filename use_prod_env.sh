#!/bin/bash

echo "===== Starting Production Development Environment ====="

# Stop any running containers
docker-compose down

# Start the production environment
docker-compose -f docker-compose.prod.yml up -d

# Enter the container
docker-compose -f docker-compose.prod.yml exec dev bash

echo "Production environment stopped." 