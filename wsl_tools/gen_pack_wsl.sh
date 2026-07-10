#!/usr/bin/env bash
# Simplified pack generation script without packchk validation.
# This creates a basic .pack file from the PDSC for local Windows/WSL use.

set -e

PDSC_FILE="fool_cat.letter_shell.pdsc"
OUTPUT_DIR="cmsis-pack"
BUILD_DIR="build"

echo "=========================================="
echo "Simplified CMSIS Pack Generation"
echo "=========================================="
echo ""

if ! command -v zip >/dev/null 2>&1; then
    echo "zip not found, installing..."
    if command -v apt-get >/dev/null 2>&1; then
        sudo apt-get update && sudo apt-get install -y zip
    elif command -v dnf >/dev/null 2>&1; then
        sudo dnf install -y zip
    elif command -v yum >/dev/null 2>&1; then
        sudo yum install -y zip
    else
        echo "Error: Cannot install zip automatically. Please install it manually."
        exit 1
    fi
    echo "zip installed successfully."
    echo ""
fi

if [ ! -f "$PDSC_FILE" ]; then
    echo "Error: $PDSC_FILE not found!"
    exit 1
fi

VERSION=$(grep -m1 '<release version=' "$PDSC_FILE" | sed 's/.*version="\([^"]*\)".*/\1/')
if [ -z "$VERSION" ]; then
    echo "Error: Could not extract version from PDSC file"
    exit 1
fi

VENDOR=$(grep -m1 '<vendor>' "$PDSC_FILE" | sed 's/.*<vendor>\(.*\)<\/vendor>.*/\1/')
NAME=$(grep -m1 '<name>' "$PDSC_FILE" | sed 's/.*<name>\(.*\)<\/name>.*/\1/')

PACK_NAME="${VENDOR}.${NAME}.${VERSION}.pack"

echo "Pack Information:"
echo "  Vendor:  $VENDOR"
echo "  Name:    $NAME"
echo "  Version: $VERSION"
echo "  Output:  $PACK_NAME"
echo ""

mkdir -p "$OUTPUT_DIR"
mkdir -p "$BUILD_DIR"

rm -rf "$BUILD_DIR"/*

echo "Copying files to build directory..."

cp "$PDSC_FILE" "$BUILD_DIR/"

for base_file in LICENSE README.md; do
    if [ -f "$base_file" ]; then
        cp "$base_file" "$BUILD_DIR/"
    fi
done

for pack_dir in src extensions doc templates; do
    if [ -d "$pack_dir" ]; then
        echo "Copying $pack_dir directory..."
        cp -r "$pack_dir" "$BUILD_DIR/"
    fi
done

echo ""
echo "Creating pack archive..."

cd "$BUILD_DIR"
zip -r "../$OUTPUT_DIR/$PACK_NAME" . -q
cd ..

echo ""
echo "=========================================="
echo "Pack generation completed!"
echo "=========================================="
echo ""
echo "Output: $OUTPUT_DIR/$PACK_NAME"
echo ""

if [ -f "$OUTPUT_DIR/$PACK_NAME" ]; then
    SIZE=$(du -h "$OUTPUT_DIR/$PACK_NAME" | cut -f1)
    echo "Pack size: $SIZE"
fi

echo ""
echo "Cleaning up temporary files..."
rm -rf "$BUILD_DIR"

exit 0
