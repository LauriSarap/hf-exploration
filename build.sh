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
# This looks at your OS and downloads the correct version of fmt
echo "Running Conan..."
conan install . --build=missing build_type=$BUILD_TYPE

# 5. Configure CMake
# We use 'Release' for HFT speed (-O3)
echo "Configuring CMake..."
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE

# 6. Build
echo "Compiling..."
cmake --preset conan-release
cmake --build .

echo "Build Complete! Run:"
echo "  ./build/producer"
echo "  ./build/consumer"