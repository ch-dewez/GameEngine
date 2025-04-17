#!/bin/bash

# Get the directory where the script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
# Get the root directory (one level up from scripts)
ROOT_DIR="$SCRIPT_DIR/.."

# Default configuration
CONFIG="debug"

# Process command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --config=*)
        CONFIG="${1#*=}"
        shift
        ;;
        *)
        echo "Unknown option: $1"
        exit 1
        ;;
    esac
done

# Run the game
echo "Running the game in $CONFIG configuration..."
"$ROOT_DIR/build/bin/Game"

if [ $? -ne 0 ]; then
    echo "Game execution failed!"
    exit 1
fi
