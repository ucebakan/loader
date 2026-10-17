param([Parameter(Mandatory)][string]$OutputDirectory)
$ErrorActionPreference='Stop'
$repo=Split-Path -Parent $PSScriptRoot
$source=Join-Path $repo 'loader\bypass-amidewin520'
$output=[IO.Path]::GetFullPath($OutputDirectory)
if(Test-Path -LiteralPath $output) {throw 'Use a new release evidence directory'}
$expected=@('.gitignore','libcurl-x64.dll','libsodium.dll','loader-AMIDEWIN520.exe','loader.exe','loaderbypass.exe','README.md','SHA256SUMS.txt','wininet.dll','Offline520/ks_driver.zip')
$files=@(Get-ChildItem -LiteralPath $source -Recurse -Force -File | ForEach-Object {
    [pscustomobject]@{Path=[IO.Path]::GetRelativePath($source,$_.FullName).Replace('\','/');Bytes=$_.Length;SHA256=(Get-FileHash -LiteralPath $_.FullName).Hash}
} | Sort-Object Path)
if($files.Count -ne $expected.Count -or @($files | Where-Object {$_.Path -notin $expected}).Count) {throw 'Unexpected package file; profiles/logs must not be published'}
foreach($line in (Get-Content -LiteralPath (Join-Path $source 'SHA256SUMS.txt'))) {
    $pair=$line -split '  ',2
    if($pair.Count -ne 2 -or (Get-FileHash -LiteralPath (Join-Path $source $pair[1])).Hash -ne $pair[0]) {throw 'Package checksums failed'}
}
if((Get-Item -LiteralPath (Join-Path $source 'loaderbypass.exe')).Length -ne 112938496) {throw 'Bundle must be the real EXE, not an LFS pointer'}
New-Item -ItemType Directory -Path $output | Out-Null
$zip=Join-Path $output 'loader-AMIDEWIN520-offline-test.zip'
$archive=[IO.Compression.ZipFile]::Open($zip,[IO.Compression.ZipArchiveMode]::Create)
try {
    foreach($file in $files) {
        [IO.Compression.ZipFileExtensions]::CreateEntryFromFile($archive,(Join-Path $source $file.Path),('loader/'+$file.Path),[IO.Compression.CompressionLevel]::Optimal) | Out-Null
    }
} finally {$archive.Dispose()}
$hash=(Get-FileHash -LiteralPath $zip).Hash
($hash+'  '+[IO.Path]::GetFileName($zip)) | Set-Content -LiteralPath ($zip+'.sha256') -Encoding ascii
$manifest=[pscustomobject]@{Source=$source;Zip=$zip;ZipSHA256=$hash;ZipBytes=(Get-Item -LiteralPath $zip).Length;ArchiveRoot='loader/';Files=$files}
$manifest | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $output 'release-manifest.json') -Encoding utf8
$manifest | Select-Object Source,Zip,ZipSHA256,ZipBytes | ConvertTo-Json
