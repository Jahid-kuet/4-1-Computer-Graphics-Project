@echo off
setlocal
cd /d "%~dp0"

echo ========================================================
echo   Bangladeshi Rural Village 3D Scene (OpenGL 3.3)
echo ========================================================

echo Ensuring build is up to date...
taskkill /F /IM Hitlar.exe >nul 2>&1
for /f "usebackq tokens=*" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe`) do (
    set "MSBUILD=%%i"
)
if not defined MSBUILD (
    echo [WARNING] MSBuild not found! Proceeding with existing binary.
    goto :run
)
"%MSBUILD%" Hitlar.vcxproj /p:Configuration=Debug /p:Platform=x64 /nologo /v:m
if errorlevel 1 (
    echo [ERROR] Build failed!
    pause
    exit /b 1
)

REM Sign with local code-signing cert to satisfy Windows 11 Smart App Control
powershell -NoProfile -ExecutionPolicy Bypass -Command "$c = Get-ChildItem Cert:\CurrentUser\My -CodeSigningCert | Select-Object -First 1; if ($c) { Set-AuthenticodeSignature -FilePath 'x64\Debug\Hitlar.exe' -Certificate $c | Out-Null }"

:run
echo.
echo CONTROLS:
echo   Left-Drag Mouse : Orbit Camera Around Object
echo   Scroll Wheel    : Zoom In / Out
echo   W / S / A / D   : Move Camera Freely
echo   Key '1'         : Inspect House (Chouchala ^& Clay Cooking Stove)
echo   Key '2'         : Inspect Boat (Dingi Nouka with Hanging Hariken)
echo   Key '3'         : Inspect Charpai, Seated Elder ^& Handmade Fan
echo   Key '4'         : Inspect Child Reading Book
echo   Key '5'         : Inspect Trees (1 Palm, 1 Banana, 1 Mango, 1 Bamboo)
echo   Key '6'         : Inspect Animals (1 Hen ^& 1 Duck)
echo   Key '7'         : Inspect Rice Tree (Dhan Gachh), Grass ^& Water Grass
echo   Key '8'         : Full Scene Overview
echo   Key 'T'         : Toggle Terrain/Ground Visibility
echo   Key 'L'         : Toggle Lighting (0: Unlit 3D ^| 1: Flat ^| 2: Night)
echo   ESC             : Exit
echo ========================================================
echo.
echo Launching 3D Village Scene Window...
start "" "x64\Debug\Hitlar.exe"
