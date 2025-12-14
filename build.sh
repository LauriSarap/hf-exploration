#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

BUILD_DIR="build"
BUILD_TYPE="Release" # "Release" for speed (-O3), "Debug" for symbols

# 1. Clean build folder if requested (./build.sh --clean)
if [ "$1" == "--clean" ]; then
    echo "Cleaning build directory..."
    rm -rf build
fi

# 2. Setup Build Directory
if [ ! -d "$BUILD_DIR" ]; then
    mkdir "$BUILD_DIR"
fi

# Remove stale CMakeCache.txt if it exists (from previous projects)
if [ -f "$BUILD_DIR/CMakeCache.txt" ]; then
    echo "Removing stale CMakeCache.txt..."
    rm -f "$BUILD_DIR/CMakeCache.txt"
    rm -rf "$BUILD_DIR/CMakeFiles"
fi

# 3. Detect OS (Just for logging)
echo "🔍 Checking System..."
if [[ "$OSTYPE" == "darwin"* ]]; then
    echo "Detected macOS (Apple Silicon/Intel)"
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    echo "Detected Linux"
else
    echo "Unknown OS: $OSTYPE. Proceeding anyway..."
fi

# 4. Install Dependencies (Conan)
# Conan handles all dependencies - no system packages needed
# It automatically detects OS and architecture
echo "Running Conan..."
conan install . --build=missing -s build_type=$BUILD_TYPE

# 5. Configure CMake
# Conan with cmake_layout generates toolchain in <BuildType>/generators/
# This works the same on both Mac and Linux
echo "Configuring CMake..."
cd build

# Find the toolchain file (should be in Release/generators/ or Debug/generators/)
TOOLCHAIN_FILE="${BUILD_TYPE}/generators/conan_toolchain.cmake"
if [ ! -f "$TOOLCHAIN_FILE" ]; then
    echo "Error: Conan toolchain not found at $TOOLCHAIN_FILE"
    echo "Make sure Conan installed successfully."
    exit 1
fi

cmake .. -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN_FILE" -DCMAKE_BUILD_TYPE=$BUILD_TYPE

# 6. Build
echo "Compiling..."
cmake --build .

echo "Build Complete! Run:"
echo "  ./build/producer"
echo "  ./build/consumer"