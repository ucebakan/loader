$ErrorActionPreference='Stop'
$package='C:\Users\ali\Documents\ChatGPT\4unityofflinetestsunucusu\loader'
$build='C:\loader\ui-hook-project\build\filename-independent-20261017'
$backup='C:\loader\ui-hook-project\backups\filename-fix-20261017'
$replacements=@(
    [pscustomobject]@{Name='loader-bypass-AMIDEWIN527.exe';Old='B2A24B7427DDA7ABD112DB5B9DE928B503D00B7C601E93A401CFC94DD2D55F99';New='7CE31BA8BB97A6979D8DBCE5340D71A6C2AAB423E632D0715071BD78D924686C';Staged="$build\native-manifest-final\loader-bypass-AMIDEWIN527.exe"},
    [pscustomobject]@{Name='loaderbypass.exe';Old='1790F51D79CFC55C488C95FA4F5F6BBC1F56E2CAB1368172793CD60D7537AD73';New='B39D96667DFECCE5D11E33D9DAB6744DCDE09F93C097047E676417307ECC3FBB';Staged="$build\loaderbypass.exe"}
)
if(Test-Path -LiteralPath $backup) {throw 'Refusing to overwrite an existing rollback backup'}
if(@(Get-Process | Where-Object {$_.ProcessName -in @('loader','loaderbypass','loader-bypass-AMIDEWIN527')}).Count) {throw 'Close the loader package before deploying this fix'}
$manifest=@(foreach($line in Get-Content -LiteralPath "$package\SHA256SUMS.txt") {
    if($line -notmatch '^([A-F0-9]{64})\s+(.+)$') {throw 'Unexpected checksum line'}
    $path=Join-Path $package $Matches[2]
    $actual=(Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash
    if($actual -ne $Matches[1]) {throw "Existing package differs from its recorded checksum: $path"}
    [pscustomobject]@{Name=$Matches[2];SHA256=$actual}
})
foreach($entry in $replacements) {
    if((Get-FileHash -LiteralPath (Join-Path $package $entry.Name) -Algorithm SHA256).Hash -ne $entry.Old) {throw 'Unexpected original launcher'}
    if((Get-FileHash -LiteralPath $entry.Staged -Algorithm SHA256).Hash -ne $entry.New) {throw 'Unexpected staged launcher'}
}
New-Item -ItemType Directory -Path $backup | Out-Null
foreach($entry in $replacements) {
    Copy-Item -LiteralPath (Join-Path $package $entry.Name) -Destination (Join-Path $backup $entry.Name)
    if((Get-FileHash -LiteralPath (Join-Path $backup $entry.Name) -Algorithm SHA256).Hash -ne $entry.Old) {throw 'Backup verification failed'}
}
Copy-Item -LiteralPath "$package\SHA256SUMS.txt" -Destination "$backup\SHA256SUMS.txt"
try {
    foreach($entry in $replacements) {
        Copy-Item -LiteralPath $entry.Staged -Destination (Join-Path $package $entry.Name) -Force
        if((Get-FileHash -LiteralPath (Join-Path $package $entry.Name) -Algorithm SHA256).Hash -ne $entry.New) {throw 'Deployed launcher verification failed'}
    }
    foreach($entry in $manifest | Where-Object {$_.Name -notin $replacements.Name}) {
        if((Get-FileHash -LiteralPath (Join-Path $package $entry.Name) -Algorithm SHA256).Hash -ne $entry.SHA256) {throw 'Unrelated package file changed'}
    }
} catch {
    foreach($entry in $replacements) {Copy-Item -LiteralPath (Join-Path $backup $entry.Name) -Destination (Join-Path $package $entry.Name) -Force}
    throw
}
[pscustomobject]@{DeployedAt=(Get-Date).ToString('o');Package=$package;Backup=$backup;Replacements=$replacements;UnchangedOtherBinaryFiles=@($manifest | Where-Object {$_.Name -notin $replacements.Name});Scope='Filename discovery only. Native administration manifest embedded; original backend deployment/arguments and UI executable unchanged. No login or hardware operation tested.'} | ConvertTo-Json -Depth 6 | Set-Content -LiteralPath "$build\deployment.json" -Encoding utf8
Write-Output "Updated two launchers; other $($manifest.Count-2) recorded binaries unchanged. Rollback: $backup"
