@echo off
setlocal
cd /d "%~dp0"
where cmake >nul 2>nul
if errorlevel 1 (
 echo CMake fehlt. Visual Studio 2022 mit Desktopentwicklung mit C++ und CMake installieren.
 pause
 exit /b 1
)
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
if errorlevel 1 goto fail
cmake --build build --config Release --target PhaseOrbit_VST3 EngineTests --parallel
if errorlevel 1 goto fail
ctest --test-dir build -C Release --output-on-failure
if errorlevel 1 goto fail
 echo.
 echo Fertig: build\PhaseOrbit_artefacts\Release\VST3\Phase Orbit.vst3
 echo Den gesamten Ordner Phase Orbit.vst3 nach C:\Program Files\Common Files\VST3 kopieren.
 pause
 exit /b 0
:fail
 echo BUILD FEHLGESCHLAGEN. Bitte die Fehlermeldung weitergeben.
 pause
 exit /b 1
