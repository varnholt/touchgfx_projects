[CmdletBinding()]
param(
    [ValidateSet('Release', 'Debug')]
    [string]$Configuration = 'Release',
    [switch]$NoRun
)

$ErrorActionPreference = 'Stop'
if (!(Test-Path -LiteralPath (Join-Path $PSScriptRoot 'TouchGFX/generated/images/src/BitmapDatabase.cpp')) -or
    !(Test-Path -LiteralPath (Join-Path $PSScriptRoot 'Middlewares/ST/touchgfx/lib/win/msvs/libtouchgfx_v142.lib'))) {
    throw 'TouchGFX 4.17 framework or generated assets are missing. Follow the first-time setup in README.md, then run python tools/normalize_gui_filenames.py.'
}
$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
if (!(Test-Path -LiteralPath $vswhere)) {
    throw 'Install Visual Studio 2022 with Desktop development with C++ and a Windows 10/11 SDK.'
}
$msbuild = & $vswhere -latest -version '[17.0,18.0)' -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -find 'MSBuild\**\Bin\MSBuild.exe' | Select-Object -First 1
if (!$msbuild) {
    throw 'Visual Studio 2022 C++ tools were not found. Install Desktop development with C++ and a Windows 10/11 SDK.'
}

$project = Join-Path $PSScriptRoot 'TouchGFX/simulator/msvs/Application.vcxproj'
& $msbuild $project /m "/p:Configuration=$Configuration" /p:Platform=Win32 /verbosity:minimal /nologo
if ($LASTEXITCODE -ne 0) {
    throw "Simulator build failed (exit code $LASTEXITCODE). Close the simulator before rebuilding if its executable is locked."
}

$executable = Join-Path $PSScriptRoot "TouchGFX/build/$Configuration/bin/Application.exe"
if (!$NoRun) {
    # Do not inherit a hidden startup state from a background PowerShell host.
    $simulator = Start-Process -FilePath $executable -WorkingDirectory (Split-Path $executable) -WindowStyle Normal -PassThru
    Start-Sleep -Seconds 2
    $simulator.Refresh()
    if ($simulator.HasExited) {
        throw "Simulator exited during startup (exit code $($simulator.ExitCode))."
    }
    Write-Output "Simulator started (PID $($simulator.Id)): $executable"
}
