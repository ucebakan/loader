param([Parameter(Mandatory)][string]$ManifestPath)
$ErrorActionPreference='Stop'
$repo=Split-Path -Parent $PSScriptRoot
Set-Location -LiteralPath $repo
$manifest=Get-Content -LiteralPath $ManifestPath -Raw | ConvertFrom-Json
$output=Split-Path -Parent ([IO.Path]::GetFullPath($ManifestPath))
$tag='loader-amidewin520-offline-test-2026-10-17'
$api='https://api.github.com/repos/ucebakan/loader'
$commit=(git rev-parse HEAD).Trim()
$remote=(git ls-remote origin refs/heads/main) -split '\s+'
if($LASTEXITCODE -or $remote[0] -ne $commit) {throw 'Push the exact source commit before publishing'}
foreach($file in $manifest.Files) {
    if((Get-FileHash -LiteralPath (Join-Path $manifest.Source $file.Path)).Hash -ne $file.SHA256) {throw 'Source package changed'}
}
if((Get-FileHash -LiteralPath $manifest.Zip).Hash -ne $manifest.ZipSHA256) {throw 'ZIP changed'}
$zipName=[IO.Path]::GetFileName($manifest.Zip)
$savedPrompt=$env:GIT_TERMINAL_PROMPT
try {
    $env:GIT_TERMINAL_PROMPT='0'
    $raw="protocol=https`nhost=github.com`npath=ucebakan/loader.git`n`n" | git credential fill
    if($LASTEXITCODE) {throw 'GitHub credential unavailable'}
    $credential=@{}
    foreach($line in $raw) {if($line -match '^([^=]+)=(.*)$') {$credential[$Matches[1]]=$Matches[2]}}
    if(-not $credential['password']) {throw 'GitHub authentication unavailable'}
    $headers=@{Authorization=('Bearer '+$credential['password']);Accept='application/vnd.github+json';'X-GitHub-Api-Version'='2022-11-28';'User-Agent'='loader-offline520-publisher'}
    $existing=Invoke-RestMethod -Uri ($api+'/releases?per_page=100') -Headers $headers
    if(@($existing | Where-Object tag_name -eq $tag).Count) {throw 'Matching release already exists; inspect before retrying, never replace blindly'}
    $notes=@"
AMIDEWIN 5.20 yerel paket yedegi - TEST / PRE-RELEASE.

Indirilecek dosya: $zipName (otomatik Source code arsivi degil).
ZIP kok klasoru: loader/
Baslangic: loader/loader-AMIDEWIN520.exe
Tum dosyalari birlikte tutun; paket gercek 112938496 baytlik loaderbypass.exe icerir, LFS isaretcisi degildir.
Depodaki konum: loader/bypass-amidewin520/

Mevcut paket indirmesi basariliysa o kullanilir. Indirme/HTTP/eksik ZIP hatasinda paketteki Offline520/ks_driver.zip (5.20.0042) kimlik dogrulamasi sonrasinda sunulur. Yerel yedek eksik veya degistirilmis ise hata gizlenmez.
Ana EXE ve ara baslatici degistirilmedi; 5.27 backend secicisi bu pakete eklenmedi. Mevcut 5.27 paketi ve release'leri korunur.

17 surucu calistirmayan fixture kontrolu gecti. Tam ana uygulamada indirme/cikarma entegrasyonu ve surucu/donanim uyumlulugu henuz canli dogrulanmadi. Uygulamanin C:\ks1 gibi ortak konumlara erisimi izole degildir. Bu yayin uctan uca test edilmis production surumu degildir.
Kisisel profiller ve calisma gunlukleri dahil edilmedi. ZIP'teki $($manifest.Files.Count) dosya yerel paketle SHA-256 olarak karsilastirilir; GitHub'dan geri indirilerek tekrar dogrulanir.

Kaynak commit: $commit
ZIP SHA-256: $($manifest.ZipSHA256)
"@
    $body=@{tag_name=$tag;target_commitish=$commit;name='Loader - AMIDEWIN 5.20 offline fallback (test)';body=$notes;draft=$true;prerelease=$true;make_latest='false'} | ConvertTo-Json
    $release=Invoke-RestMethod -Uri ($api+'/releases') -Method Post -Headers $headers -ContentType 'application/json' -Body $body
    [pscustomobject]@{ReleaseId=$release.id;Tag=$tag;Commit=$commit} | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $output 'draft-receipt.json') -Encoding utf8
    $upload=($release.upload_url -split '\{')[0]
    $zipAsset=$null
    foreach($asset in @($manifest.Zip,($manifest.Zip+'.sha256'))) {
        $name=[IO.Path]::GetFileName($asset)
        $type=if($name.EndsWith('.zip')) {'application/zip'} else {'text/plain'}
        $uploaded=Invoke-RestMethod -Uri ($upload+'?name='+[Uri]::EscapeDataString($name)) -Method Post -Headers $headers -ContentType $type -InFile $asset
        if($uploaded.state -ne 'uploaded' -or $uploaded.size -ne (Get-Item -LiteralPath $asset).Length) {throw 'Incomplete asset upload'}
        if($name -eq $zipName) {$zipAsset=$uploaded}
    }
    if(-not $zipAsset -or ($zipAsset.digest -and $zipAsset.digest -ne ('sha256:'+$manifest.ZipSHA256.ToLowerInvariant()))) {throw 'GitHub upload digest mismatch'}
    $download=Join-Path $output 'downloaded-from-github.zip'
    if(Test-Path -LiteralPath $download) {throw 'Download verification file already exists'}
    $downloadHeaders=$headers.Clone(); $downloadHeaders.Accept='application/octet-stream'
    Invoke-WebRequest -Uri ($api+'/releases/assets/'+$zipAsset.id) -Headers $downloadHeaders -OutFile $download
    if((Get-FileHash -LiteralPath $download).Hash -ne $manifest.ZipSHA256) {throw 'GitHub download hash mismatch'}
    $archive=[IO.Compression.ZipFile]::OpenRead($download)
    try {
        $entries=@($archive.Entries | Where-Object {$_.Name})
        if($entries.Count -ne $manifest.Files.Count) {throw 'Downloaded ZIP file count mismatch'}
        $seen=@{}
        foreach($entry in $entries) {
            if(-not $entry.FullName.StartsWith('loader/')) {throw 'Wrong ZIP root'}
            $relative=$entry.FullName.Substring('loader/'.Length)
            if($seen.ContainsKey($relative)) {throw 'Duplicate ZIP entry'}
            $seen[$relative]=$true
            $expected=@($manifest.Files | Where-Object Path -eq $relative)
            $stream=$entry.Open()
            try {$hash=[Convert]::ToHexString([Security.Cryptography.SHA256]::HashData($stream))} finally {$stream.Dispose()}
            if($expected.Count -ne 1 -or $hash -ne $expected[0].SHA256 -or $entry.Length -ne $expected[0].Bytes) {throw 'Downloaded file differs from the local package'}
        }
    } finally {$archive.Dispose()}
    Write-Output "GitHub ZIP download verified: $($manifest.Files.Count) actual package files match."
    Invoke-RestMethod -Uri ($api+'/releases/'+$release.id) -Method Patch -Headers $headers -ContentType 'application/json' -Body (@{draft=$false;make_latest='false'} | ConvertTo-Json) | Out-Null
    $verified=Invoke-RestMethod -Uri ($api+'/releases/tags/'+$tag) -Headers $headers
    $asset=@($verified.assets | Where-Object name -eq $zipName)
    if($verified.draft -or -not $verified.prerelease -or $asset.Count -ne 1 -or $asset[0].state -ne 'uploaded') {throw 'Published release verification failed'}
    $receipt=[pscustomobject]@{ReleaseUrl=$verified.html_url;DownloadUrl=$asset[0].browser_download_url;Commit=$commit;VerifiedFiles=$manifest.Files.Count;GitHubDownloadComparedToLocal=$true;SHA256=$manifest.ZipSHA256;Bytes=$asset[0].size;Prerelease=$true}
    $receipt | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $output 'published-receipt.json') -Encoding utf8
    $receipt | ConvertTo-Json
} finally {$env:GIT_TERMINAL_PROMPT=$savedPrompt;$raw=$null;$credential=$null;$headers=$null;$downloadHeaders=$null}
