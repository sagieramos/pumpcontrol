#!/bin/bash

if [[ "$1" == "--fix" ]]; then
    echo "Fixing code format..."
    SOURCE_DIR="./"

    if ! command -v clang-format &> /dev/null; then
        echo "Clang-Format is not installed. Please install it first."
        exit 1
    fi

    find "$SOURCE_DIR" \( -name .pio -o -name .vscode -o -name node_modules \) -prune -o -type f \( -name "*.cpp" -o -name "*.h" \) -exec clang-format -i {} \;

    echo "Code formatting complete."
else
    echo "Checking for errors..."
    SOURCE_DIR="./"

    if ! command -v clang-format &> /dev/null; then
        echo "Clang-Format is not installed. Please install it first."
        exit 1
    fi

    error_found=false

    find "$SOURCE_DIR" \( -name .pio -o -name .vscode -o -name node_modules \) -prune -o -type f \( -name "*.cpp" -o -name "*.h" \) -exec sh -c 'if clang-format -output-replacements-xml "$0" | grep -q "<replacement "; then echo "Errors found in $0"; error_found=true; fi' {} \;

    if [ "$error_found" = true ]; then
        echo "Linting found errors."
        exit 1
    else
        echo "Linting complete. No errors found."
    fi
fi