set -e

set +x

echo "--- Starting Trevia Server Build ---"


COMPILER_CMD="g++"
OUTPUT_FILE="./server"
SOURCE_FILES="server.cpp functions.cpp"

INCLUDE_DIRS="-I./libs -I./libs/nlohmann"
CXX_FLAGS="-std=c++17 -Wall"

LINK_LIBS="-pthread -lssl -lcrypto"

echo "1. Compiling source files..."

$COMPILER_CMD $CXX_FLAGS $SOURCE_FILES $INCLUDE_DIRS -o $OUTPUT_FILE $LINK_LIBS

echo "2. Build successful!"

echo "3. Starting server..."

$OUTPUT_FILE