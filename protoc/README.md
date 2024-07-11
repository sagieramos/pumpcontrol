# nanopb Protocol Buffers Processing Guide

This guide provides steps to process a Protocol Buffers (.proto) file using nanopb on Linux (Ubuntu).

## Prerequisites

1. **Clone nanopb Repository:**
   Clone the nanopb repository to your home directory (replace `$HOME` with your actual home directory path):

   ```bash
   git clone https://github.com/nanopb/nanopb $HOME
2. **Install Protocol Buffers Compiler (`protoc`):**
Ensure you have protoc (Protocol Buffers compiler) installed. You can install it using your package manager:

```sh
  sudo apt update
  sudo apt install protobuf-compiler
```
# Processing a .proto File
To generate code from your .proto file using nanopb, follow these steps:

1. Navigate to Your Project Directory:
Move to the directory containing your .proto file and where you want to generate the nanopb code.

```sh
cd /path/to/your/project
```
2. Compile the .proto File:
Use protoc with the nanopb generator plugin (protoc-gen-nanopb):

```sh
protoc --plugin=protoc-gen-nanopb=$HOME/nanopb/generator/protoc-gen-nanopb --nanopb_out=. your_file.proto
```
Replace your_file.proto with the name of your .proto file.

3 Generated Files:
After successful compilation, nanopb will generate .c and .h files based on your .proto definitions.

# Example Command
```sh
protoc --plugin=protoc-gen-nanopb=$HOME/nanopb/generator/protoc-gen-nanopb --nanopb_out=. your_file.proto
```
Replace your_file.proto with the actual name of your .proto file.

