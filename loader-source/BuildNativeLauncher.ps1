param([string]$OutputDirectory='C:\loader\ui-hook-project\build\filename-independent-20261017')
$ErrorActionPreference='Stop'
$toolchain='C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207'
$sdk='C:\Program Files (x86)\Windows Kits\10'
$version='10.0.26100.0'
$output=[IO.Path]::GetFullPath($OutputDirectory)
if(Test-Path -LiteralPath (Join-Path $output 'loader-bypass-AMIDEWIN527.exe')) {throw 'Refusing to overwrite a staged launcher'}
if(-not (Test-Path -LiteralPath $output)) {New-Item -ItemType Directory -Path $output | Out-Null}
$savedPath=$env:PATH
try {
    $env:PATH="$toolchain\bin\Hostx64\x64;$sdk\bin\$version\x64;$savedPath"
    $common=@('/nologo','/O2','/MT','/W4','/utf-8','/EHsc','/std:c++17','/DUNICODE','/D_UNICODE','/DNOMINMAX','/DWIN32_LEAN_AND_MEAN',"/I$toolchain\include","/I$sdk\Include\$version\ucrt","/I$sdk\Include\$version\shared","/I$sdk\Include\$version\um")
    $libs=@("/LIBPATH:$toolchain\lib\x64","/LIBPATH:$sdk\Lib\$version\ucrt\x64","/LIBPATH:$sdk\Lib\$version\um\x64",'kernel32.lib','user32.lib','shell32.lib','bcrypt.lib')
    $compiler="$toolchain\bin\Hostx64\x64\cl.exe"
    & $compiler @common "$PSScriptRoot\LoaderAmiBootstrapMain.cpp" "/Fo$output\LoaderAmiBootstrapMain.obj" /link "/OUT:$output\loader-bypass-AMIDEWIN527.exe" /SUBSYSTEM:WINDOWS /MANIFEST:EMBED "/MANIFESTUAC:level='requireAdministrator' uiAccess='false'" @libs
    if($LASTEXITCODE -ne 0) {throw 'Native launcher compilation failed'}
    & $compiler @common "$PSScriptRoot\TestFileIdentity.cpp" "/Fo$output\TestFileIdentity.obj" /link "/OUT:$output\TestFileIdentity.exe" /SUBSYSTEM:CONSOLE @libs
    if($LASTEXITCODE -ne 0) {throw 'Native identity checker compilation failed'}
    Get-FileHash -LiteralPath "$output\loader-bypass-AMIDEWIN527.exe" -Algorithm SHA256 | Format-List
} finally {$env:PATH=$savedPath}
