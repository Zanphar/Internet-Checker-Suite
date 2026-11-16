@echo off
rem Build Internet Connection Utility with MinGW-w64 g++.
rem Produces a single self-contained exe with the icon embedded as a resource.
setlocal
cd /d "%~dp0"

windres -I res res\app.rc -O coff -o res\app.res
if errorlevel 1 (
    echo.
    echo Resource compilation failed.
    exit /b 1
)

g++ -std=c++17 -O2 -mwindows -DUNICODE -D_UNICODE -DNOMINMAX ^
    src\main.cpp src\ui.cpp src\doctor.cpp src\net.cpp res\app.res ^
    -I res ^
    -o InternetConnectionUtility.exe ^
    -lgdiplus -lwlanapi -liphlpapi -lwinhttp -ldwmapi ^
    -lws2_32 -lole32 -luuid -lshell32 -lgdi32 -luser32 ^
    -static-libgcc -static-libstdc++

if errorlevel 1 (
    echo.
    echo Build failed.
    exit /b 1
)
echo.
echo Built InternetConnectionUtility.exe
