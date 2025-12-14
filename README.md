# HFT Exploration

High-frequency trading exploration project using shared memory for low-latency communication.

## Prerequisites

- **CMake** 3.15 or higher
- **Conan** 2.x (package manager)
- **C++20** compatible compiler (clang/gcc)

### Installing Conan

```bash
# Using pip (recommended)
pip install conan

# Or using homebrew (macOS)
brew install conan
```

## Building

The project uses Conan for all dependency management - no system packages required.

```bash
# Build Release (default)
./build.sh

# Clean build
./build.sh --clean
```

The build script will:
1. Detect your OS (macOS/Linux)
2. Install dependencies via Conan (fmt library)
3. Configure CMake with Conan toolchain
4. Build the project

## Project Structure

- `src/producer.cpp` - Producer process (creates shared memory)
- `src/consumer.cpp` - Consumer process (reads from shared memory)
- `include/common.h` - Shared definitions (ring buffer, Tick structure)
- `conanfile.txt` - Conan dependencies (fmt)

## Dependencies

All dependencies are managed via Conan:
- `fmt/10.1.1` - Fast formatting library

No system packages are required - Conan handles everything.

## Cross-Platform

Works on:
- macOS (Apple Silicon and Intel)
- Linux (x86_64 and ARM)

The build system automatically detects the platform and uses the appropriate Conan packages.

## Running

```bash
# Terminal 1: Start producer
./build/producer

# Terminal 2: Start consumer
./build/consumer
```

## IDE Setup

VS Code/Cursor is configured to:
- Use CMake Tools extension
- Auto-configure with Conan toolchain
- Support IntelliSense via `compile_commands.json`

The IDE will automatically detect your platform and use the correct IntelliSense mode.

