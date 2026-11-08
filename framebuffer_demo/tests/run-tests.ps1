[CmdletBinding()]
param(
    [switch]$Simulator,
    [ValidateSet('Debug', 'Release')]
    [string]$Configuration = 'Debug'
)
$ErrorActionPreference = 'Stop'
$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
$msbuild = & $vswhere -latest -version '[17.0,18.0)' -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -find 'MSBuild\**\Bin\MSBuild.exe' | Select-Object -First 1
if (!$msbuild) { throw 'Visual Studio 2022 C++ tools were not found.' }
& $msbuild (Join-Path $PSScriptRoot 'native_tests.vcxproj') /m /p:Configuration=Debug /p:Platform=Win32 /verbosity:minimal /nologo
if ($LASTEXITCODE -ne 0) { throw 'Native regression tests failed to build.' }
& (Join-Path $PSScriptRoot '../TouchGFX/build/tests/native_tests.exe')
if ($LASTEXITCODE -ne 0) { throw 'Native regression tests failed.' }

if ($Simulator) {
    $root = Split-Path $PSScriptRoot
    $output = Join-Path $root "TouchGFX/build/selftest/$Configuration/bin/"
    $objects = Join-Path $root "TouchGFX/build/selftest/$Configuration/obj/"
    & $msbuild (Join-Path $root 'TouchGFX/simulator/msvs/Application.vcxproj') /m "/p:Configuration=$Configuration" /p:Platform=Win32 "/p:OutDir=$output" "/p:IntDir=$objects" /verbosity:minimal /nologo
    if ($LASTEXITCODE -ne 0) { throw 'Simulator self-test failed to build.' }
    $process = Start-Process -FilePath (Join-Path $output 'Application.exe') -ArgumentList '--self-test' -WorkingDirectory $output -WindowStyle Hidden -PassThru
    if (!$process.WaitForExit(30000)) {
        Stop-Process -Id $process.Id
        throw 'Simulator self-test timed out.'
    }
    $report = Join-Path $output 'self-test.log'
    if (Test-Path -LiteralPath $report) { Get-Content -LiteralPath $report }
    if ($process.ExitCode -ne 0) { throw "Simulator self-test failed (exit code $($process.ExitCode))." }
}
