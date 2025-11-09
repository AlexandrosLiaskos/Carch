#!/bin/bash
# Comprehensive test runner for Carch

set -e

# Parse options
VERBOSE=false
SUITE=""
COVERAGE=false
UPDATE_GOLDEN=false

while [[ $# -gt 0 ]]; do
    case $1 in
        --verbose) VERBOSE=true; shift ;;
        --suite) SUITE="$2"; shift 2 ;;
        --coverage) COVERAGE=true; shift ;;
        --update-golden) UPDATE_GOLDEN=true; shift ;;
        *) echo "Unknown option: $1"; exit 1 ;;
    esac
done

# Build if needed
if [ ! -d "build" ]; then
    echo "Building project..."
    cmake -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON
    cmake --build build
fi

# Set environment
if [ "$UPDATE_GOLDEN" = true ]; then
    export UPDATE_GOLDEN=1
fi

# Run tests
cd build

if [ -n "$SUITE" ]; then
    echo "Running $SUITE..."
    ctest -R "$SUITE" --output-on-failure
else
    echo "Running all tests..."
    if [ "$VERBOSE" = true ]; then
        ctest --output-on-failure --verbose
    else
        ctest --output-on-failure
    fi
fi

# Coverage report
if [ "$COVERAGE" = true ]; then
    echo ""
    echo "Generating coverage report..."
    lcov --directory . --capture --output-file coverage.info
    lcov --remove coverage.info '/usr/*' --output-file coverage.info
    lcov --list coverage.info
fi

echo ""
echo "✓ Tests complete"
