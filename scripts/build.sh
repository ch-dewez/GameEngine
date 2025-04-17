#!/bin/bash

# Get the directory where the script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
ROOT_DIR="$SCRIPT_DIR/.."

# Default configuration
CONFIG="Debug"
RUN_AFTER_BUILD=false

# Process command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --config)
            CONFIG="$2"
            shift 2
            ;;
        --run)
            RUN_AFTER_BUILD=true
            shift
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Build the project
cd "$ROOT_DIR/build"
cmake --build . --config "$CONFIG"

if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo "Build completed successfully!"

# Run the game if --run was specified
if [ "$RUN_AFTER_BUILD" = true ]; then
    echo "Running the game..."
    "$SCRIPT_DIR/Run.sh" "--config=$CONFIG"
fi
