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
Write-Host "  Left-Drag Mouse : Orbit Camera"
Write-Host "  Scroll Wheel    : Zoom In / Out"
Write-Host "  Key 'L'         : Cycle Lighting (Night -> Sunset -> Day)"
Write-Host "  Key '1'         : View 1 - Courtyard Gathering"
Write-Host "  Key '2'         : View 2 - River Shore & Moored Boat
  Key '3'         : View 3 - Full Village Overview
  Key '4'         : View 4 - Rural Homestead
  ESC             : Exit
====================================================================" -ForegroundColor Cyan
Write-Host "Signing binary and launching window..." -ForegroundColor Green

$cert = Get-ChildItem Cert:\CurrentUser\My -CodeSigningCert | Select-Object -First 1
if ($cert) {
    Set-AuthenticodeSignature -FilePath $ExePath -Certificate $cert | Out-Null
}

Start-Process -FilePath $ExePath -WorkingDirectory $ScriptDir
