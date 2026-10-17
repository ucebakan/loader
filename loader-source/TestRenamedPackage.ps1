$ErrorActionPreference='Stop'
$project='C:\Users\ali\Documents\ChatGPT\4unityofflinetestsunucusu'
$package=Join-Path $project 'loader'
$build='C:\loader\ui-hook-project\build\filename-independent-20261017'
$fixture='C:\loader\ui-hook-project\runtime\filename-test-20261017'
if(Test-Path -LiteralPath $fixture) {throw 'Refusing to overwrite an existing filename test'}
New-Item -ItemType Directory -Path $fixture | Out-Null
Copy-Item -LiteralPath "$build\loaderbypass.exe" -Destination "$fixture\ara program.exe"
Copy-Item -LiteralPath "$package\loader.exe" -Destination "$fixture\ana program.EXE"
Copy-Item -LiteralPath "$build\loader-bypass-AMIDEWIN527.exe" -Destination "$fixture\baslat.exe"
foreach($asset in @('AmiBackendProxy.exe','libcurl-x64.dll','libsodium.dll','AMITools527')) {
    Copy-Item -LiteralPath (Join-Path $package $asset) -Destination $fixture -Recurse
}
$tool=Join-Path $PSScriptRoot 'NameResolutionTool\bin\Debug\net9.0\NameResolutionTool.dll'
& dotnet $tool --self-test
if($LASTEXITCODE -ne 0) {throw 'Managed resolver unit tests failed'}
& "$build\TestFileIdentity.exe" $fixture
if($LASTEXITCODE -ne 0) {throw 'Arbitrary-name native resolver failed'}
& dotnet $tool --resolve-ui $fixture
if($LASTEXITCODE -ne 0) {throw 'Arbitrary-name managed UI resolver failed'}
Move-Item -LiteralPath "$fixture\ana program.EXE" -Destination "$fixture\ikinci isim.exe"
Move-Item -LiteralPath "$fixture\ara program.exe" -Destination "$fixture\ikinci baslatici.EXE"
& "$build\TestFileIdentity.exe" $fixture
if($LASTEXITCODE -ne 0) {throw 'Second rename native resolver failed'}
& dotnet $tool --resolve-ui $fixture
if($LASTEXITCODE -ne 0) {throw 'Second rename managed resolver failed'}
# Keep every fixture; no test cleanup deletes user data.
[pscustomobject]@{TestedAt=(Get-Date).ToString('o');Fixture=$fixture;ArbitraryNamesPassed=$true;SecondRenamePassed=$true;ManagedUnitCases=5;NativeOrManagedTargetLaunched=$false;BackendOrHardwareCalled=$false} | ConvertTo-Json | Set-Content -LiteralPath "$build\rename-tests.json" -Encoding utf8
Write-Output 'Filename resolution passed with two unrelated name sets. No loader/backend process was started.'
