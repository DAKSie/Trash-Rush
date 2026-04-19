param(
    [ValidateSet('Debug', 'Release')]
    [string]$Configuration = 'Debug'
)

$root = $PSScriptRoot
$binDir = Join-Path $root 'bin'
$entrySource = Join-Path $root 'main.cpp'
$sourceDir = Join-Path $root 'src'
$output = Join-Path $binDir 'main.exe'
$compiler = 'C:\msys64\ucrt64\bin\g++.exe'
$includeDir = 'C:/msys64/ucrt64/include'
$libraryDir = 'C:/msys64/ucrt64/lib'
$runtimeSource = 'C:\msys64\ucrt64\bin'

$sources = @($entrySource)
if (Test-Path -LiteralPath $sourceDir) {
    $sources += Get-ChildItem -LiteralPath $sourceDir -Filter '*.cpp' -File -Recurse | ForEach-Object { $_.FullName }
}

New-Item -ItemType Directory -Force -Path $binDir | Out-Null
Get-ChildItem -LiteralPath $binDir -Force | Where-Object { $_.Name -ne '.gitkeep' } | Remove-Item -Recurse -Force -ErrorAction SilentlyContinue

$arguments = @()
$arguments += $sources
$arguments += @(
    '-o',
    $output,
    '-std=c++14',
    '-static-libstdc++',
    '-static-libgcc',
    '-I',
    $includeDir,
    '-L',
    $libraryDir,
    '-lraylib',
    '-lopengl32',
    '-lgdi32',
    '-lwinmm'
)

if ($Configuration -eq 'Debug') {
    $arguments += @('-g', '-O0')
} else {
    $arguments += @('-s', '-O1')
}

& $compiler @arguments

if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

Copy-Item -LiteralPath (Join-Path $runtimeSource 'libraylib.dll') -Destination $binDir -Force
Copy-Item -LiteralPath (Join-Path $runtimeSource 'glfw3.dll') -Destination $binDir -Force
Copy-Item -LiteralPath (Join-Path $runtimeSource 'libgcc_s_seh-1.dll') -Destination $binDir -Force
if (Test-Path -LiteralPath (Join-Path $runtimeSource 'libwinpthread-1.dll')) {
    Copy-Item -LiteralPath (Join-Path $runtimeSource 'libwinpthread-1.dll') -Destination $binDir -Force
}
Copy-Item -LiteralPath (Join-Path $root 'assets') -Destination $binDir -Recurse -Force
Copy-Item -LiteralPath (Join-Path $root 'Score.txt') -Destination (Join-Path $binDir 'Score.txt') -Force
