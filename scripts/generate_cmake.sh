#!/bin/bash

# Get the directory where the script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
ROOT_DIR="$SCRIPT_DIR/.."

# Create build directory
mkdir -p "$ROOT_DIR/build"
cd "$ROOT_DIR/build"

# Generate build files
cmake ..

echo "CMake project files generated successfully!"