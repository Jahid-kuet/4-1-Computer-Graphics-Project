@echo off
setlocal
cd /d "%~dp0"

echo ========================================================
echo   Bangladeshi Rural Village 3D Scene (OpenGL 3.3)
echo ========================================================

echo Ensuring build is up to date...
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
echo   Left-Drag Mouse : Orbit Camera
echo   Scroll Wheel    : Zoom In / Out
echo   W / S / A / D   : Move Camera (Forward / Backward / Left / Right)
echo   Key 'L'         : Cycle Lighting (Night -> Sunset -> Day)
echo   Key '1'         : View 1 - Courtyard Gathering
echo   Key '2'         : View 2 - River Shore ^& Moored Boat
echo   Key '3'         : View 3 - Full Village Overview
echo   Key '4'         : View 4 - Rural Homestead
echo   Key '5'         : View 5 - Lush Paddy Fields (Dhan Khet)
echo   ESC             : Exit
echo ========================================================
echo.
echo Launching 3D Village Scene Window...
start "" "x64\Debug\Hitlar.exe"
