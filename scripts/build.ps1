# Build script for BummerGram
# Usage: .\scripts\build.ps1 [-Configuration Release] [-Architecture x64]

param(
    [ValidateSet("Debug", "Release")]
    [string]$Configuration = "Release",
    
    [ValidateSet("x64", "x86")]
    [string]$Architecture = "x64",
    
    [switch]$Clean,
    [switch]$Rebuild
)

$ErrorActionPreference = "Stop"

# Configuration
$ProjectRoot = Split-Path -Parent $PSScriptRoot
$BuildDir = Join-Path $ProjectRoot "build"
$QtVersion = "6.5.3"
$VsVersion = "Visual Studio 16 2019"

# Paths
$QtBase = "C:\Qt"
$QtDir = "$QtBase\$QtVersion\$QtVersion\msvc2019_64"
$QtBin = "$QtDir\bin"
$VcpkgRoot = Join-Path $ProjectRoot "vcpkg"
$VcpkgToolchain = "$VcpkgRoot\scripts\buildsystems\vcpkg.cmake"

# Colors for output
function Write-Step { param([string]$Message) Write-Host "[BUILD] $Message" -ForegroundColor Cyan }
function Write-Success { param([string]$Message) Write-Host "[OK] $Message" -ForegroundColor Green }
function Write-Warn { param([string]$Message) Write-Host "[WARN] $Message" -ForegroundColor Yellow }
function Write-Fail { param([string]$Message) Write-Host "[FAIL] $Message" -ForegroundColor Red }

Write-Host "====================================" -ForegroundColor Magenta
Write-Host "  BummerGram Build Script" -ForegroundColor Magenta
Write-Host "  Configuration: $Configuration" -ForegroundColor Magenta
Write-Host "  Architecture: $Architecture" -ForegroundColor Magenta
Write-Host "====================================" -ForegroundColor Magenta
Write-Host ""

# Check prerequisites
Write-Step "Checking prerequisites..."

# Check CMake
$cmake = Get-Command cmake -ErrorAction SilentlyContinue
if (-not $cmake) {
    Write-Fail "CMake not found. Please install CMake 3.20+"
    exit 1
}
Write-Success "CMake found: $($cmake.Source)"

# Check Qt
if (-not (Test-Path $QtBin)) {
    Write-Warn "Qt not found at $QtBin"
    Write-Host "  Please install Qt $QtVersion with MSVC 2019 64-bit kit"
    Write-Host "  Download from: https://www.qt.io/download-qt-installer"
    
    $manualConfig = Read-Host "Continue without Qt PATH? (y/n)"
    if ($manualConfig -ne 'y') {
        exit 1
    }
} else {
    Write-Success "Qt found at $QtDir"
}

# Check vcpkg
if (-not (Test-Path $VcpkgToolchain)) {
    Write-Warn "vcpkg toolchain not found at $VcpkgToolchain"
    Write-Host "  Installing vcpkg dependencies manually..."
    
    if (Test-Path $VcpkgRoot) {
        Write-Step "Running vcpkg integrate..."
        & "$VcpkgRoot\vcpkg" integrate install
    }
} else {
    Write-Success "vcpkg toolchain found"
}

# Clean if requested
if ($Clean -and (Test-Path $BuildDir)) {
    Write-Step "Cleaning build directory..."
    Remove-Item -Recurse -Force $BuildDir
}

# Create build directory
if (-not (Test-Path $BuildDir)) {
    New-Item -ItemType Directory -Path $BuildDir | Out-Null
}

# Configure
Write-Step "Configuring CMake..."
Set-Location $ProjectRoot

$cmakeArgs = @(
    "-S", ".",
    "-B", "build",
    "-G", $VsVersion,
    "-A", $Architecture,
    "-DCMAKE_BUILD_TYPE=$Configuration",
    "-DCMAKE_PREFIX_PATH=$QtDir"
)

if (Test-Path $VcpkgToolchain) {
    $cmakeArgs += "-DCMAKE_TOOLCHAIN_FILE=$VcpkgToolchain"
    $cmakeArgs += "-DVCPKG_TARGET_TRIPLET=$($Architecture.ToLower())-windows"
}

Write-Host "CMake arguments:" -ForegroundColor Gray
$cmakeArgs | ForEach-Object { Write-Host "  $_" -ForegroundColor Gray }

& cmake @cmakeArgs
if ($LASTEXITCODE -ne 0) {
    Write-Fail "CMake configuration failed"
    exit 1
}

Write-Success "CMake configuration complete"

# Build
Write-Step "Building BummerGram..."

if ($Rebuild) {
    & cmake --build $BuildDir --clean-first --config $Configuration --parallel
} else {
    & cmake --build $BuildDir --config $Configuration --parallel
}

if ($LASTEXITCODE -ne 0) {
    Write-Fail "Build failed"
    exit 1
}

Write-Success "Build complete"

# Locate output
$exePath = "$BuildDir\$Configuration\BummerGram.exe"
if (Test-Path $exePath) {
    Write-Host ""
    Write-Host "====================================" -ForegroundColor Green
    Write-Success "BummerGram built successfully!"
    Write-Host "  Executable: $exePath" -ForegroundColor White
    Write-Host "====================================" -ForegroundColor Green
    
    $run = Read-Host "Run BummerGram now? (y/n)"
    if ($run -eq 'y') {
        & $exePath
    }
} else {
    Write-Warn "Executable not found at expected location"
    Get-ChildItem $BuildDir -Filter "*.exe" -Recurse | ForEach-Object {
        Write-Host "  Found: $($_.FullName)" -ForegroundColor Yellow
    }
}
