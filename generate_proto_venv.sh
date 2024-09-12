#!/bin/bash

# Replace with your desired path for the virtual environment
VENV_PATH="."

# Step 1: Create a virtual environment
python3 -m venv $VENV_PATH

# Step 2: Activate the virtual environment
source $VENV_PATH/bin/activate

# Step 3: Install required packages within the virtual environment
pip install protobuf grpcio-tools

# Step 4: Run your script (replace with your actual script command)
# Example command to run your script
./run_protoc.sh protoc/msg1.proto

# Step 5: Deactivate the virtual environment (optional)
deactivate

