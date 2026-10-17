param([Parameter(Mandatory)][string]$BuildDirectory)
$ErrorActionPreference='Stop'
$repo=Split-Path -Parent $PSScriptRoot
$output=Join-Path $repo 'loader\bypass-amidewin520'
if(Test-Path -LiteralPath $output) {throw 'Refusing to overwrite an existing 5.20 package'}
$source=Join-Path $repo 'loader'
$expected=@{
    'loader.exe'='8730BEBE0F4B72D42D4DC6D6FD31A2F2193DD928ED971DDB353EEA775C79285C'
    'loaderbypass.exe'='B39D96667DFECCE5D11E33D9DAB6744DCDE09F93C097047E676417307ECC3FBB'
    'libcurl-x64.dll'='FD07F399C9555F5D89DC34BD2882F0BC17F4631E5B637FD48CB646D7FB49FA9E'
    'libsodium.dll'='F656AEB789BFC3A2AC587FAE1D7CFE278BBE8CE73F28DA73B4D08282EA8F9FE3'
}
foreach($name in $expected.Keys) {
    if((Get-FileHash -LiteralPath (Join-Path $source $name)).Hash -ne $expected[$name]) {throw "Original package identity changed: $name"}
}
$results=Get-Content -LiteralPath (Join-Path $BuildDirectory 'test-results.jsonl') | ForEach-Object {$_ | ConvertFrom-Json}
if($results[-1].passed -ne 17 -or @($results | Where-Object {$_.passed -eq $false}).Count) {throw '17 successful fixture checks required'}
New-Item -ItemType Directory -Path (Join-Path $output 'Offline520') | Out-Null
foreach($name in $expected.Keys) {Copy-Item -LiteralPath (Join-Path $source $name) -Destination (Join-Path $output $name)}
foreach($name in @('loader-AMIDEWIN520.exe','wininet.dll')) {
    Copy-Item -LiteralPath (Join-Path $BuildDirectory "package\$name") -Destination (Join-Path $output $name)
}
Copy-Item -LiteralPath (Join-Path $BuildDirectory 'package\Offline520\ks_driver.zip') -Destination (Join-Path $output 'Offline520\ks_driver.zip')
Copy-Item -LiteralPath (Join-Path $PSScriptRoot 'OFFLINE520_README.md') -Destination (Join-Path $output 'README.md')
Copy-Item -LiteralPath (Join-Path $PSScriptRoot 'OFFLINE520.gitignore') -Destination (Join-Path $output '.gitignore')
$manifest=@(Get-ChildItem -LiteralPath $output -Recurse -File | Sort-Object FullName | ForEach-Object {
    [pscustomobject]@{Path=[IO.Path]::GetRelativePath($output,$_.FullName).Replace('\','/'); Bytes=$_.Length; SHA256=(Get-FileHash -LiteralPath $_.FullName).Hash}
})
$manifest | ForEach-Object {$_.SHA256+'  '+$_.Path} | Set-Content -LiteralPath (Join-Path $output 'SHA256SUMS.txt') -Encoding ascii
$manifest | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath (Join-Path $BuildDirectory 'package-manifest.json') -Encoding utf8
Write-Output "New test package: $output"
Write-Output 'Original loader and 5.27 packages untouched. Main application/hardware execution not performed.'
