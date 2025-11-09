#!/bin/bash
# Package Carch for distribution

set -e

VERSION=${1:-"1.0.0"}
PLATFORM=${2:-$(uname -s | tr '[:upper:]' '[:lower:]')}
OUTPUT_DIR=${3:-"dist"}

echo "=== Packaging Carch v$VERSION for $PLATFORM ==="

# Build in release mode
cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF -DBUILD_TOOLS=ON
cmake --build build --config Release

# Create package directory
PACKAGE_NAME="carch-$PLATFORM-$(uname -m)"
mkdir -p "$OUTPUT_DIR/$PACKAGE_NAME"

# Copy binaries
if [ "$PLATFORM" = "windows" ]; then
    cp build/Release/carch.exe "$OUTPUT_DIR/$PACKAGE_NAME/"
    cp build/tools/Release/*.exe "$OUTPUT_DIR/$PACKAGE_NAME/" 2>/dev/null || true
else
    cp build/carch "$OUTPUT_DIR/$PACKAGE_NAME/"
    strip "$OUTPUT_DIR/$PACKAGE_NAME/carch"
    cp build/tools/carch-* "$OUTPUT_DIR/$PACKAGE_NAME/" 2>/dev/null || true
fi

# Copy documentation and examples
cp LICENSE "$OUTPUT_DIR/$PACKAGE_NAME/"
cp README.md "$OUTPUT_DIR/$PACKAGE_NAME/"
cp -r examples "$OUTPUT_DIR/$PACKAGE_NAME/"
cp -r docs "$OUTPUT_DIR/$PACKAGE_NAME/"

# Create archive
cd "$OUTPUT_DIR"
if [ "$PLATFORM" = "windows" ]; then
    zip -r "$PACKAGE_NAME.zip" "$PACKAGE_NAME"
    sha256sum "$PACKAGE_NAME.zip" > "$PACKAGE_NAME.zip.sha256"
else
    tar -czf "$PACKAGE_NAME.tar.gz" "$PACKAGE_NAME"
    sha256sum "$PACKAGE_NAME.tar.gz" > "$PACKAGE_NAME.tar.gz.sha256"
fi

echo ""
echo "✓ Package created: $OUTPUT_DIR/$PACKAGE_NAME"
