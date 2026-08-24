#!/usr/bin/env bash
set -e

# Build Box2D to asm.js (legacy JavaScript) using Emscripten with WASM=0
echo "=== Building Box2D targeting asm.js (legacy) ==="

if ! command -v emcmake &> /dev/null; then
    echo "Warning: Emscripten (emcmake / emcc) was not found in PATH."
    echo "Please install or activate the Emscripten SDK (emsdk):"
    echo "  source /path/to/emsdk/emsdk_env.sh"
    echo "Then re-run this script."
    exit 1
fi

rm -rf build-asmjs
mkdir -p build-asmjs
cd build-asmjs

# Configure with emcmake, forcing WASM=0 (asm.js output)
emcmake cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DBOX2D_BUILD_WASM=OFF \
    -DBOX2D_BUILD_UNIT_TESTS=OFF \
    -DBOX2D_BUILD_TESTBED=OFF \
    -DBOX2D_BUILD_DOCS=OFF \
    -DCMAKE_CXX_FLAGS="-O3"

# Build all libraries
cmake --build . --config Release

echo "=== asm.js build completed successfully in build-asmjs/bin ==="
