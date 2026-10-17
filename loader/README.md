# Loader

Bu klasör, 15.10.2026 tarihinde çalışma alanındaki `loader` dosyalarından
oluşturuldu. 17.10.2026 tarihinde iki başlatıcıya dosya adından bağımsız bulma
düzeltmesi uygulandı. EXE'ler, `libcurl-x64.dll`,
`libsodium.dll`, `AmiBackendProxy.exe` ve `AMITools527` dosyaları birlikte saklanır.
Kişisel profiller ve çalışma günlükleri pakete dahil değildir.

`loaderbypass.exe`, Git LFS ile saklanır. Depoyu Git ile indirdikten sonra:

```powershell
git lfs install
git lfs pull --include="loader/loaderbypass.exe"
```

`SHA256SUMS.txt`, paket içindeki EXE, DLL ve SYS dosyalarının SHA-256 değerlerini
içerir. Güncel `loaderbypass.exe` dosyasının gerçek boyutu 112938496 bayttır.

`AMIDEWIN527_README.txt` ve `IMPLEMENTATION_REPORT.txt`, önceki paketle gelen
teknik belgelerdir; içlerindeki eski dosya adları korunmuştur. Güncel klasörde
`4unityofflinetestsunucusu.exe` yerine `loader.exe`,
`4unityoffline-login-bypass.exe` yerine `loaderbypass.exe`,
`4unityoffline-login-bypass-AMIDEWIN527.exe` yerine
`loader-bypass-AMIDEWIN527.exe` bulunur. İlk kayıt sırasında dosyalar çalıştırılmadı.

## Dosya adından bağımsız başlatma — 17.10.2026

Normal başlangıç dosyası `loader-bypass-AMIDEWIN527.exe` dosyasıdır. Windows'un
yönetici izni gereksinimi korunur. Başlatıcı, ara başlatıcıyı ve ana uygulamayı
dosya adına göre değil, bu sürümün boyut ve SHA-256 kimliğine göre bulur. Ara
başlatıcının ana uygulama yolunu ve süreç adını belirleyen iki sabit ad kullanımı
da aynı içerik doğrulamalı keşifle değiştirilmiştir.

Üç uygulama EXE'si yeniden adlandırılabilir; `.exe` uzantısı, aynı klasörde
bulunmaları ve bu sürümün dosya içerikleri korunmalıdır. Aynı uygulamanın birden
fazla kopyası varsa rastgele seçim yapılmaz. Farklı sürüm dosyası, ad değiştirmekle
aynı şey değildir; bu durumda kimlikler yeniden doğrulanmalıdır. DLL'lerin,
`AmiBackendProxy.exe` ve `AMITools527` içindeki yardımcıların adları değiştirilmedi.

`loader.exe`, DLL/SYS dosyaları ve AMIDEWIN backend'i değiştirilmedi. Ara başlatıcı
bundle'ında yalnız ana uygulama dosyası/süreç adı çözümlemesi değiştirildi; diğer
metotların talimat/branch/exception-handler yapıları ve diğer 250 bundle girdisi
doğrulandı. Eski iki başlatıcı ve checksum dosyası yerel olarak
`C:\loader\ui-hook-project\backups\filename-fix-20261017` altında korunur.

Kaynaklar ve kontrol araçları `../loader-source/` içindedir. Beş yönetilen resolver
kontrolü ve iki farklı ad setiyle native/yönetilen dosya bulma testleri geçti.
İlk canlı test derlemesi yönetici bildirimi eksikliği nedeniyle Windows 740 verdi;
son derlemede `requireAdministrator` manifesti EXE'ye gömüldü. Son sürümün canlı
başlatmasında bu klasördeki `loader.exe` süreci, `Serial Changer` penceresi ve ana
ekran gözlemlendi; eski dosya adı hatası oluşmadı. Giriş veya donanım işlemi
otomasyonu yapılmadı; ekrandaki işlem sonuçları backend doğrulaması sayılmadı.

Önceki kontrolde Windows, güncel `loader-bypass-AMIDEWIN527.exe` dosyasını okumayı
"Dosya virüslü olduğundan veya istenmeyebilecek yazılım içerdiğinden" uyarısıyla
engelledi. Bu tespitin nedeni doğrulanmadı; yanlış pozitif olduğu iddia edilmez.
Kullanıcı daha sonra uygulamanın çalıştığını bildirdi. GitHub güncellemesi öncesinde
paketteki dokuz dosyanın güncel SHA-256 değerleri yeniden doğrulandı. Bu, güvenlik
tespitinin nedenini veya farklı bilgisayarlardaki davranışı doğrulamaz. Koruma
kapatılmadı, istisna eklenmedi ve engeli aşmak için yeniden paketleme yapılmadı.
Önceki başlatıcıların doğrulanmış yedekleri yukarıdaki dizindedir.
