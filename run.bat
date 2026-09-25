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
echo   SPACE           : Toggle Cinematic Fly-Through Village Tour
echo   Key 'J'         : Toggle Directional Light (Moonlight/Sunlight ON/OFF)
echo   Key 'H'         : Cycle 6 Point Lights (Normal / Bright / Amber / OFF)
echo   Key 'P'         : Pump Tubewell (Interactive Water Flow into Kolshi)
echo   Key 'B'         : Toggle Summer Breeze (Tree Foliage Wind Sway)
echo   Key '[' / ']'   : Decrease / Increase Animation Speed (0.25x - 3.0x)
echo   Key 'K'         : Pause / Resume Continuous Village Animations
echo   Key 'L'         : Toggle Lighting (0: Moonlit Night ^| 1: Day ^| 2: Unlit Facets ^| 3: Flat)
echo   Key 'X'         : Toggle Texture Mode (0: Solid ^| 1: Procedural Detailing ^| 2: GPU Texture Maps)
echo   Key 'V'         : View V - Curved Objects (Bamboo Footbridge ^& Parametric Bézier Vase)
echo   Key '1'         : View 1 - Courtyard Gathering (Charpai, Elders, Children, Hens)
echo   Key '2'         : View 2 - River Shore, Landing Ghat, Moored Boat ^& Rowing Boatman
echo   Key '3'         : View 3 - Historic Terracotta Village Mosque (Ancient Sultanate Masjid)
echo   Key '4'         : View 4 - North Homestead (Dochala, Cow Shed ^& Straw Stack)
echo   Key '5'         : View 5 - South Homestead, Straw Stack ^& Terraced Paddy Fields
echo   Key '6'         : View 6 - Rural Trees ^& Riverbank Reeds
echo   Key '7'         : View 7 - Village Animals (Hens ^& Ducks)
echo   Key '8'         : View 8 - Broader View (Moonlit Village Overview matching Concept Artwork)
echo   Key '9'         : View 9 - Hand-Pump Tubewell ^& Clay Cooking Kitchen
echo   Key '0'         : View 0 - Thatched Cow Shed ^& Resting Deshi Cow
echo   Key 'T'         : Toggle Terrain/Ground Visibility
echo   Key 'C'         : Capture All 30 Individual Objects to 'object_images/'
echo   ESC             : Exit
echo ========================================================
echo.
echo Launching 3D Village Scene Window...
start "" "x64\Debug\Hitlar.exe"
if errorlevel 1 (
    echo.
    echo =========================================================================
    echo [NOTE: Windows 11 Smart App Control / Device Guard]
    echo If Windows blocked the binary, choose either:
    echo   1. Open Hitlar.slnx in Visual Studio and press [Ctrl+F5] (Runs directly!)
    echo   2. Or turn Off Smart App Control in:
    echo      Windows Security ^> App ^& browser control ^> Smart App Control ^> Off
    echo =========================================================================
)

