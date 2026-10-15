# Loader

Bu klasör, 15.10.2026 tarihinde çalışma alanındaki `loader` dosyalarının
değiştirilmeden Git'e alınan kopyasıdır. EXE'ler, `libcurl-x64.dll`,
`libsodium.dll`, `AmiBackendProxy.exe` ve `AMITools527` dosyaları birlikte saklanır.
Kişisel profiller ve çalışma günlükleri pakete dahil değildir.

`loaderbypass.exe`, Git LFS ile saklanır. Depoyu Git ile indirdikten sonra:

```powershell
git lfs install
git lfs pull --include="loader/loaderbypass.exe"
```

`SHA256SUMS.txt`, paket içindeki EXE, DLL ve SYS dosyalarının SHA-256 değerlerini
içerir. `loaderbypass.exe` dosyasının gerçek boyutu 112921810 bayttır.

`AMIDEWIN527_README.txt` ve `IMPLEMENTATION_REPORT.txt`, önceki paketle gelen
teknik belgelerdir; içlerindeki eski dosya adları korunmuştur. Güncel klasörde
`4unityofflinetestsunucusu.exe` yerine `loader.exe`,
`4unityoffline-login-bypass.exe` yerine `loaderbypass.exe`,
`4unityoffline-login-bypass-AMIDEWIN527.exe` yerine
`loader-bypass-AMIDEWIN527.exe` bulunur. Bu kayıt sırasında dosyalar çalıştırılmadı;
yeniden adlandırılmış başlatıcıların birlikte çalışması doğrulanmadı.
