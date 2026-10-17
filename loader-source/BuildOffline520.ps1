param([Parameter(Mandatory)][string]$OutputDirectory)
$ErrorActionPreference='Stop'
$output=[IO.Path]::GetFullPath($OutputDirectory)
if(Test-Path -LiteralPath $output) {throw 'Use a new, empty build directory'}
New-Item -ItemType Directory -Path $output | Out-Null
$toolchain='C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207'
$sdk='C:\Program Files (x86)\Windows Kits\10'
$version='10.0.26100.0'
$savedPath=$env:PATH
try {
    $env:PATH="$toolchain\bin\Hostx64\x64;$sdk\bin\$version\x64;$savedPath"
    $systemDll=Join-Path ([Environment]::GetFolderPath('System')) 'wininet.dll'
    & python "$PSScriptRoot\GenerateWinInetExports.py" $systemDll $output
    if($LASTEXITCODE) {throw 'Export generation failed'}
    $common=@('/nologo','/O2','/MT','/W4','/utf-8','/EHsc','/std:c++17','/DUNICODE','/D_UNICODE','/DNOMINMAX','/DWIN32_LEAN_AND_MEAN',"/I$output","/I$toolchain\include","/I$sdk\Include\$version\ucrt","/I$sdk\Include\$version\shared","/I$sdk\Include\$version\um")
    $libs=@("/LIBPATH:$toolchain\lib\x64","/LIBPATH:$sdk\Lib\$version\ucrt\x64","/LIBPATH:$sdk\Lib\$version\um\x64",'kernel32.lib','user32.lib','shell32.lib','bcrypt.lib')
    & ml64 /nologo /c "/Fo$output\WinInetForwarders.obj" "$output\WinInetForwarders.asm"
    if($LASTEXITCODE) {throw 'Forwarder assembly failed'}
    foreach($test in @($false,$true)) {
        $folder=if($test) {Join-Path $output 'fixture'} else {Join-Path $output 'package'}
        New-Item -ItemType Directory -Path $folder | Out-Null
        $defines=@(if($test) {'/DFALLBACK_TESTING'})
        & cl @common @defines /LD "$PSScriptRoot\DriverDownloadFallback.cpp" "/Fo$folder\DriverDownloadFallback.obj" "$output\WinInetForwarders.obj" /link "/OUT:$folder\wininet.dll" "/IMPLIB:$folder\wininet.lib" "/DEF:$output\WinInetProxy.def" /DYNAMICBASE /NXCOMPAT @libs
        if($LASTEXITCODE) {throw 'Download extension build failed'}
    }
    & cl @common /DLOADER_BACKEND_520_ONLY "$PSScriptRoot\LoaderAmiBootstrapMain.cpp" "/Fo$output\Loader520.obj" /link "/OUT:$output\package\loader-AMIDEWIN520.exe" /SUBSYSTEM:WINDOWS /MANIFEST:EMBED "/MANIFESTUAC:level='requireAdministrator' uiAccess='false'" @libs
    if($LASTEXITCODE) {throw '5.20 launcher build failed'}
    & cl @common "$PSScriptRoot\TestDriverDownload.cpp" "/Fo$output\TestDriverDownload.obj" /link "/OUT:$output\fixture\TestDriverDownload.exe" /SUBSYSTEM:CONSOLE @libs
    if($LASTEXITCODE) {throw 'Fixture build failed'}
    Get-FileHash -LiteralPath "$output\package\loader-AMIDEWIN520.exe","$output\package\wininet.dll"
} finally {$env:PATH=$savedPath}
