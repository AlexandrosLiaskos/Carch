#!/bin/bash
# Installation script for Carch IDL Compiler (Unix/Linux/macOS)

set -e

# Default values
PREFIX="/usr/local"
SKIP_TESTS=false
BUILD_TOOLS=true

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --prefix)
            PREFIX="$2"
            shift 2
            ;;
        --skip-tests)
            SKIP_TESTS=true
            shift
            ;;
        --no-tools)
            BUILD_TOOLS=false
            shift
            ;;
        *)
            echo "Unknown option: $1"
            echo "Usage: $0 [--prefix PATH] [--skip-tests] [--no-tools]"
            exit 1
            ;;
    esac
done

echo "=== Carch IDL Compiler Installation ==="
echo "Install prefix: $PREFIX"
echo

# Check dependencies
echo "Checking dependencies..."

if ! command -v cmake &> /dev/null; then
    echo "Error: CMake not found. Please install CMake 3.15 or later."
    exit 1
fi

if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
    echo "Error: C++ compiler not found. Please install g++ or clang++."
    exit 1
fi

echo "✓ Dependencies OK"
echo

# Create build directory
echo "Configuring build..."
rm -rf build
mkdir -p build
cd build

# Configure with CMake
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="$PREFIX" \
    -DBUILD_TESTS=ON \
    -DBUILD_TOOLS="$BUILD_TOOLS"

echo

# Build
echo "Building..."
echo "Installing Carch 0.0.1 (development release)"
cmake --build . --config Release -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 2)

echo

# Run tests
if [ "$SKIP_TESTS" = false ]; then
    echo "Running tests..."
    ctest --output-on-failure
    echo
fi

# Install
echo "Installing to $PREFIX..."
if [ "$PREFIX" = "/usr/local" ] || [ "$PREFIX" = "/usr" ]; then
    sudo cmake --install .
else
    cmake --install .
fi

echo
echo "=== Installation Complete ==="
echo
echo "Carch has been installed to: $PREFIX/bin/carch"
echo

# Check if in PATH
if command -v carch &> /dev/null; then
    echo "✓ carch is in your PATH"
    carch --version
else
    echo "⚠ $PREFIX/bin is not in your PATH"
    echo "  Add it with: export PATH=\"$PREFIX/bin:\$PATH\""
    echo "  Or add to ~/.bashrc or ~/.zshrc for permanence"
fi

echo
echo "Get started with: carch --help"
echo "Examples available in: examples/"
echo "Documentation: docs/"
