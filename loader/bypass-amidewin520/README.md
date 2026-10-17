# AMIDEWIN 5.20 yerel paket yedeği

Bu çalışma mevcut `loader/` ve `bypass-amidewin527/` paketlerini değiştirmez.
Yeni test paketi depoda ayrı `loader/bypass-amidewin520/` klasöründedir.
Release ZIP'i ise yalnız bu paketi `loader/` kök klasörü altında içerir.
GitHub'ın otomatik "Source code" ZIP'i yerine release'e eklenen paket ZIP'ini indirin.
Başlangıç dosyası
`loader-AMIDEWIN520.exe`; klasördeki diğer dosyalarla birlikte kullanılmalıdır.

## Davranış

- Ana uygulama önce kendi mevcut paket adresini internetten ister.
- Başarılı ve yapısal olarak tam bir ZIP indirilirse o indirme kullanılır.
- Bağlantı/okuma hatası, HTTP hatası veya boş/eksik/geçersiz ZIP gövdesinde
  `Offline520/ks_driver.zip` yerel yedeği kullanılır.
- Yerel ZIP'in boyutu ve SHA-256 kimliği doğrulanır. Eksik/değişmiş yedek başarı
  gibi gösterilmez; mevcut indirme hatası akışı korunur.
- Yalnız bir sabit paket URL'si kapsamdadır. Diğer WinINet istekleri normal
  Windows işlevlerine aktarılır; TLS/sertifika kontrolü değiştirilmez.
- `driver-download.log`, çevrimiçi mi yerel mi seçildiğini kaydeder; URL,
  kimlik bilgisi veya indirme içeriği kaydetmez.

Gönderilen ZIP'teki AMIDEWIN başlığında `5.20` ve `0042` dizeleri doğrulandı;
dosya çalıştırılmadı. ZIP ayrıca `amifldrv64.sys` ve `sdeven.sys` içerir.
ZIP SHA-256: `24DD330F14BE62CDED3D1837124014FBFF2ABDF6D209BD4893C931064B50604D`.

Bu başlatıcı 5.27 backend seçicisini/proxy'sini kurmaz; indirilen veya yedekten
sunulan paketi mevcut ana uygulamanın kendi çıkarma/çalıştırma akışına bırakır.
Başarılı çevrimiçi paketin sürümü zorla değiştirilmez. Bu nedenle "her koşulda
5.20 zorlaması" değil, "indirme hatasında doğrulanmış yerel 5.20 yedeği"dir.
Kopyayı çalıştırmak, ana uygulamanın `C:\ks1` gibi ortak konumlara erişmesini
izole etmez. Sürücüler ve donanım işlemleri özgün uygulamadaki etkilerini korur.

## Sınır ve doğrulama

Ana EXE ve ara başlatıcı baytları değiştirilmedi; bellek enjeksiyonu, giriş/
koruma mantığı değişikliği, servis kurulumu veya sürücü çalıştırılması yapılmadı.
Uygulamaya özel `wininet.dll` yardımcı katmanı, gerçek sistem WinINet DLL'sini
tam System32 yoluyla yükler. Dört paket/akış işlevi dışında dışa aktarımlar
ABI-koruyan x64 aktarıcılarla gerçek sistem işlevlerine yönlendirilir.
Bu makinenin sistem DLL'sindeki 327 ad/ordinal eşleşmesi korunmuştur. Başka
Windows sürümleri için tam export yüzeyinin uyumluluğu ayrıca sınanmalıdır.

Bu yöntem Windows'un [DLL arama sırasına](https://learn.microsoft.com/en-us/windows/win32/dlls/dynamic-link-library-search-order)
ve [InternetReadFile akış sözleşmesine](https://learn.microsoft.com/en-us/windows/win32/api/wininet/nf-wininet-internetreadfile)
dayanır. Çalışan uygulama farklı bir DLL yükleme yolu uygularsa entegrasyon
ayrıca değerlendirilmelidir; bir DLL'nin mevcut olması tek başına yüklenme
kanıtı değildir.

Sürücü yüklemeyen fixture; A/W çağrılarını, çevrimiçi başarıyı, yerel yedek
olmadan çevrimiçi başarıyı, HTTP/bağlantı/eksik gövde hatalarını, yedek kimlik
kontrolünü ve ilgisiz URL'nin değişmeden aktarılmasını sınar. Yayın DLL'si
test URL ortam değişkenini kabul etmez; kendi gerçek paket URL'si için bağlantı
hatası da kapalı loopback proxy üzerinden sınanır. Fixture hiçbir arşivi
çıkartmaz, AMIDEWIN/loader çalıştırmaz ve donanım değeri değiştirmez.

Tam ana uygulamada indirme/çıkarma entegrasyonu henüz çalıştırılmadı.
Fixture başarısı, ana uygulamanın uçtan uca testi veya sürücü uyumluluğu değildir.
Bu nedenle GitHub yayını test/pre-release olarak işaretlenmiştir.

## Yeniden derleme ve test

```powershell
.\loader-source\BuildOffline520.ps1 -OutputDirectory C:\loader\offline520-build-new
.\loader-source\TestOffline520.ps1 -BuildDirectory C:\loader\offline520-build-new -LocalPackage 'C:\Users\ali\Downloads\ks_driver (1).zip'
```

Her derlemede yeni bir çıktı klasörü kullanılır. C++ MSVC/Windows SDK, Python
`pefile` ve mevcut yerel araç zinciri gerekir. Test çıktısı derleme klasöründeki
`test-results.jsonl` dosyasında saklanır. Kaynaklar `loader-source/` içindedir.
