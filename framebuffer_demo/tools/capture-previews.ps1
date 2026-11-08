$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot
& (Join-Path $root 'run-simulator.ps1') -NoRun
$binary = Join-Path $root 'TouchGFX/build/Release/bin/Application.exe'
$process = Start-Process -FilePath $binary -ArgumentList '--capture-previews' -WorkingDirectory (Split-Path $binary) -WindowStyle Hidden -PassThru
if (!$process.WaitForExit(30000)) {
    Stop-Process -Id $process.Id
    throw 'Preview capture timed out.'
}
if ($process.ExitCode -ne 0) { throw 'Preview capture failed.' }
python (Join-Path $PSScriptRoot 'export_previews.py')
if ($LASTEXITCODE -ne 0) { throw 'Preview export failed. Install Pillow with python -m pip install Pillow.' }
