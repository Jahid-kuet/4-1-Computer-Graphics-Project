# run.ps1 - Easy launcher for Bangladeshi Village 3D Scene
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
if (-not $ScriptDir) { $ScriptDir = Get-Location }
Set-Location $ScriptDir

$ExePath = Join-Path $ScriptDir "x64\Debug\Hitlar.exe"

if (-not (Test-Path $ExePath) -or ($args -contains "build")) {
    Write-Host "Building project with MSBuild..." -ForegroundColor Yellow
    $vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    $msbuild = & $vswhere -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe
    if ($msbuild) {
        & $msbuild Hitlar.vcxproj /p:Configuration=Debug /p:Platform=x64 /nologo /v:m
        if ($LASTEXITCODE -ne 0) {
            Write-Error "Build failed with exit code $LASTEXITCODE"
            exit $LASTEXITCODE
        }
        $cert = Get-ChildItem Cert:\CurrentUser\My -CodeSigningCert | Select-Object -First 1
        if ($cert) {
            Set-AuthenticodeSignature -FilePath $ExePath -Certificate $cert | Out-Null
        }
    } else {
        Write-Error "MSBuild not found. Please build inside Visual Studio."
        exit 1
    }
}

Write-Host "========================================================" -ForegroundColor Cyan
Write-Host "  Bangladeshi Rural Village 3D Scene (OpenGL 3.3)" -ForegroundColor Cyan
Write-Host "========================================================" -ForegroundColor Cyan
Write-Host "CONTROLS:"
Write-Host "  Left-Drag Mouse : Orbit Camera Around Object"
Write-Host "  Scroll Wheel    : Zoom In / Out"
Write-Host "  W / S / A / D   : Move Camera Freely"
Write-Host "  Key 'L'         : Toggle Lighting (0: Crisp Unlit 3D | 1: Moonlit Night | 2: Day Phong | 3: Flat)"
Write-Host "  Key '1'         : View 1 - Courtyard Gathering (Charpai, Elders, Children Reading, Hens)"
Write-Host "  Key '2'         : View 2 - River Shore, Landing Ghat, Moored Boat & Rowing Boatman"
Write-Host "  Key '3'         : View 3 - Historic Terracotta Village Mosque (Ancient Sultanate Masjid)"
Write-Host "  Key '4'         : View 4 - North Homestead (Dochala House, Cow Shed & Straw Stack)"
Write-Host "  Key '5'         : View 5 - South Homestead, Straw Stack & Terraced Paddy Fields"
Write-Host "  Key '6'         : View 6 - Rural Trees & Riverbank Reeds (Palms, Banana, Mango, Bamboo & Kashbon)"
Write-Host "  Key '7'         : View 7 - Village Animals (Flocks of Hens & River Ducks)"
Write-Host "  Key '8'         : View 8 - Broader View (Moonlit Village Overview matching Concept Artwork)"
Write-Host "  Key 'T'         : Toggle Terrain/Ground Visibility"
Write-Host "  Key 'C'         : Capture All Individual Objects to 'object_images/'"
Write-Host "  ESC             : Exit"
Write-Host "========================================================" -ForegroundColor Cyan
Write-Host "Signing binary and launching window..." -ForegroundColor Green

$cert = Get-ChildItem Cert:\CurrentUser\My -CodeSigningCert | Select-Object -First 1
if ($cert) {
    Set-AuthenticodeSignature -FilePath $ExePath -Certificate $cert | Out-Null
}

try {
    Start-Process -FilePath $ExePath -WorkingDirectory $ScriptDir
} catch {
    Write-Host "`n=========================================================================" -ForegroundColor Yellow
    Write-Host "[NOTE: Windows 11 Smart App Control / Device Guard]" -ForegroundColor Red
    Write-Host "Windows blocked the newly compiled binary from running in PowerShell." -ForegroundColor Yellow
    Write-Host "Two simple ways to run:" -ForegroundColor Cyan
    Write-Host "  1. Open 'Hitlar.slnx' in Visual Studio and press [Ctrl+F5] (Runs directly!)" -ForegroundColor White
    Write-Host "  2. Or turn Off Smart App Control in:" -ForegroundColor White
    Write-Host "     Windows Security > App & browser control > Smart App Control > Off" -ForegroundColor White
    Write-Host "=========================================================================`n" -ForegroundColor Yellow
}
