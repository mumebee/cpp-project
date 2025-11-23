@echo off
echo Compiling...
g++ -o server src/server.cpp src/functions.cpp -I./libs -std=c++17 -lws2_32
if %errorlevel% == 0 (
    echo Build successful!
    echo Starting server...
    server.exe
) else (
    echo Build failed!
    pause
)