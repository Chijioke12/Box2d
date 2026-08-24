@echo off
rem Build Box2D targeting legacy asm.js using Emscripten (WASM=0)
echo === Building Box2D targeting asm.js (legacy) ===

where emcmake >nul 2>nul
if %errorlevel% neq 0 (
    echo Emscripten (emcmake / emcc) was not found in PATH.
    echo Please activate the Emscripten SDK:
    echo   emsdk_env.bat
    exit /b 1
)

if exist build-asmjs rmdir /s /q build-asmjs
mkdir build-asmjs
cd build-asmjs

call emcmake cmake .. -DCMAKE_BUILD_TYPE=Release -DBOX2D_BUILD_WASM=OFF -DBOX2D_BUILD_UNIT_TESTS=OFF -DBOX2D_BUILD_TESTBED=OFF -DBOX2D_BUILD_DOCS=OFF -DCMAKE_CXX_FLAGS="-O3"
call cmake --build . --config Release

echo === asm.js build completed successfully in build-asmjs/bin ===
cd ..
