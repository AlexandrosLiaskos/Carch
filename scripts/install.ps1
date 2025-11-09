# Installation script for Carch IDL Compiler (Windows/PowerShell)

param(
    [string]$Prefix = "$env:ProgramFiles\Carch",
    [switch]$SkipTests,
    [switch]$NoTools
)

Write-Host "=== Carch IDL Compiler Installation ===" -ForegroundColor Cyan
Write-Host "Install prefix: $Prefix"
Write-Host ""

# Check dependencies
Write-Host "Checking dependencies..." -ForegroundColor Yellow

$cmake = Get-Command cmake -ErrorAction SilentlyContinue
if (-not $cmake) {
    Write-Host "Error: CMake not found. Please install CMake 3.15 or later." -ForegroundColor Red
    Write-Host "Download from: https://cmake.org/download/"
    exit 1
}

$vsWhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (-not (Test-Path $vsWhere)) {
    Write-Host "Error: Visual Studio not found. Please install Visual Studio 2019 or later." -ForegroundColor Red
    Write-Host "Download from: https://visualstudio.microsoft.com/"
    exit 1
}

Write-Host "✓ Dependencies OK" -ForegroundColor Green
Write-Host ""

# Create build directory
Write-Host "Configuring build..." -ForegroundColor Yellow
if (Test-Path build) {
    Remove-Item -Recurse -Force build
}
New-Item -ItemType Directory -Path build | Out-Null
Set-Location build

# Configure with CMake
$buildTools = if ($NoTools) { "OFF" } else { "ON" }

cmake .. `
    -DCMAKE_BUILD_TYPE=Release `
    -DCMAKE_INSTALL_PREFIX="$Prefix" `
    -DBUILD_TESTS=ON `
    -DBUILD_TOOLS="$buildTools"

if ($LASTEXITCODE -ne 0) {
    Write-Host "Configuration failed" -ForegroundColor Red
    exit 1
}

Write-Host ""

# Build
Write-Host "Building..." -ForegroundColor Yellow
Write-Host "Installing Carch 0.0.1 (development release)" -ForegroundColor Yellow
cmake --build . --config Release

if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed" -ForegroundColor Red
    exit 1
}

Write-Host ""

# Run tests
if (-not $SkipTests) {
    Write-Host "Running tests..." -ForegroundColor Yellow
    ctest --output-on-failure -C Release
    
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Tests failed" -ForegroundColor Red
        exit 1
    }
    
    Write-Host ""
}

# Install
Write-Host "Installing to $Prefix..." -ForegroundColor Yellow
cmake --install . --config Release

if ($LASTEXITCODE -ne 0) {
    Write-Host "Installation failed" -ForegroundColor Red
    exit 1
}

Set-Location ..

Write-Host ""
Write-Host "=== Installation Complete ===" -ForegroundColor Green
Write-Host ""
Write-Host "Carch has been installed to: $Prefix\bin\carch.exe"
Write-Host ""

# Check if in PATH
$carch = Get-Command carch -ErrorAction SilentlyContinue
if ($carch) {
    Write-Host "✓ carch is in your PATH" -ForegroundColor Green
} else {
    Write-Host "⚠ $Prefix\bin is not in your PATH" -ForegroundColor Yellow
    Write-Host "  Add it manually or run:"
    Write-Host "  `$env:Path += ';$Prefix\bin'"
    Write-Host ""
    Write-Host "  For permanent change, add to System Environment Variables"
}

Write-Host ""
Write-Host "Get started with: carch --help"
Write-Host "Examples available in: examples\"
Write-Host "Documentation: docs\"
