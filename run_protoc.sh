#!/bin/bash

# Check if a .proto file was provided as an argument
if [ -z "$1" ]; then
    echo "Usage: $0 path/to/yourfile.proto [output_directory]"
    exit 1
fi

PROTO_FILE="$1"
PROTO_BASENAME=$(basename "$PROTO_FILE" .proto)
DEFAULT_OUTPUT_DIR=$(dirname "$PROTO_FILE")/$PROTO_BASENAME

# Check if an output directory was provided as the second argument
if [ -n "$2" ]; then
    OUTPUT_DIR="$2/$PROTO_BASENAME"
else
    OUTPUT_DIR="$DEFAULT_OUTPUT_DIR"
fi

# Create the output directory if it does not exist
mkdir -p "$OUTPUT_DIR"

# Find the first directory in libdeps
LIBDEPS_DIR=$(find .pio/libdeps -maxdepth 1 -type d -name "esp32" | head -n 2 | tail -n 1)

if [ -z "$LIBDEPS_DIR" ]; then
    echo "No directory found in libdeps"
    exit 1
fi

# Define the path to protoc-gen-nanopb
NANOPB_PLUGIN="$LIBDEPS_DIR/Nanopb/generator/protoc-gen-nanopb"

# Check if protoc-gen-nanopb exists
if [ ! -f "$NANOPB_PLUGIN" ]; then
    echo "protoc-gen-nanopb not found in $NANOPB_PLUGIN"
    exit 1
fi

# Run the protoc command with the specified plugin and output directory
protoc --plugin=protoc-gen-nanopb=$NANOPB_PLUGIN --nanopb_out=$OUTPUT_DIR $PROTO_FILE

if [ $? -eq 0 ]; then
    echo "Protobuf files generated successfully in $OUTPUT_DIR."
else
    echo "Failed to generate protobuf files."
    exit 1
fi
