param(
    [Parameter(Mandatory)][string]$BuildDirectory,
    [Parameter(Mandatory)][string]$LocalPackage
)
$ErrorActionPreference='Stop'
$build=[IO.Path]::GetFullPath($BuildDirectory)
if((Get-FileHash -LiteralPath $LocalPackage).Hash -ne '24DD330F14BE62CDED3D1837124014FBFF2ABDF6D209BD4893C931064B50604D') {throw 'Unexpected local ZIP'}
foreach($folder in @('fixture','package','corrupt-local')) {
    $assets=Join-Path $build "$folder\Offline520"
    if(Test-Path -LiteralPath $assets) {throw 'Use a fresh build for the fixture setup'}
    New-Item -ItemType Directory -Path $assets | Out-Null
    Copy-Item -LiteralPath $LocalPackage -Destination (Join-Path $assets 'ks_driver.zip')
}
New-Item -ItemType Directory -Path (Join-Path $build 'missing-local') | Out-Null
foreach($folder in @('missing-local','corrupt-local')) {
    Copy-Item -LiteralPath (Join-Path $build 'fixture\wininet.dll') -Destination (Join-Path $build "$folder\wininet.dll")
}
# Deliberately corrupt only our disposable fixture copy, keeping its size.
$corrupt=Join-Path $build 'corrupt-local\Offline520\ks_driver.zip'
$bytes=[IO.File]::ReadAllBytes($corrupt)
$bytes[80]=$bytes[80] -bxor 1
[IO.File]::WriteAllBytes($corrupt,$bytes)
& python "$PSScriptRoot\TestOffline520.py" $build | Tee-Object -FilePath (Join-Path $build 'test-results.jsonl')
if($LASTEXITCODE) {throw 'Offline package fixture failed'}
