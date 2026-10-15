# noMOBDMG — 0.9.15

Butonlar'daki noMOBDMG → Aç gerçek işlevi açar; Kapat özgün yönlendirmeyi geri
yükler. Kullanıcı isteğiyle 20 saniyelik otomatik kapanma kaldırıldı. Başlangıç
kapalıdır; kendiliğinden açılmaz. Araçlar → noMOBDMG sayfasında salt okunur
profil kontrolü, yeniden tarama ve geri yükleme bulunur. Satırın sırası mevcut
button-order.json mekanizmasıyla saklanır.

## Kanıt ve kapsam

Araştırma bağlamı: ctx_8be98f50633048c5b486526996f1ad4f.
Kaynak: kullanıcının 12 Ekim 2026 mob saldırı yolu raporu ve
C:/Users/ali/Documents/DomainCodex/tools/MobHpExperiment/bridge.cpp,
experiment.h, dispatch_rollback_evidence.log ile MobHpObserver/profile.h.
Bağımsız v2 aracındaki iki canlı açılış ve geri yükleme raporda kaydedilmiştir;
ikinci açılışın yaklaşık ilk 220 ms'sinde 11 HP kaybı gözlenmiştir.
Yeni birleşik/süresiz sürümün canlı hasar sonucu ayrıca denenmemiştir.

65DB42ACB60DCB2AAF1F6C50364A059EA343242A32ABB2B9DDAD1D40533F558C SHA'lı
TClient.exe için disk recovery şu yolu bağımsız yeniden buldu:

- 0x502E hücresi: RVA 0x1929BC; özgün case 0x190645; ortak çıkış 0x1928F9.
- DWORD tablo: mesaj kimliğinden 0x5002 çıkarılır; indeks ölçeği 4'tür.
- EDX tabloya girmeden sıfırlanır; ortak çıkış stack/register geri yüklemesini
  yapar. Mob case, handler ve 0x5031 oluşturma yoluna çağrı ilişkisi doğrulanır.

HP alanlarına yazılmaz; iyileşme/HPMP işleyicisi engellenmez. Yalnız belirli
mob saldırı olayının istemci işlemi atlanır. Tüm hasar türleri, PvP ve DoT için
bağışıklık iddiası yoktur.

## Uygulama

İş mantığı src/NoMobDamage, UI NoMobDamageForm ve araç adaptörleri ortak
FeatureActions/ToolWorkspace katmanındadır. Native köprü EXE'ye gömülüdür;
diğer modüller gibi içerik hash'ine göre yerel native klasörüne çıkarılır.
WH_CALLWNDPROC ile oyun pencere thread'inde, hizalı DWORD üzerinde
InterlockedCompareExchange kullanılır. Sayfa koruması işlem sonunda eski
değerine döner. Yabancı hücre değerleri ve başka kontrolcünün aktif sahipliği
ezilmez. Başarısız/belirsiz açılışta da geri alma sorumluluğu korunur.

Her SHA için yalnız build operandları/imzaları profiles altında kaydedilir;
PID, modül tabanı ve heap adresleri profile yazılmaz. Aynı SHA profili disk
semantiğiyle yeniden doğrulanır; SHA değişince tekil AOB ve çağrı/veri akışı
yeniden çözülür. Desteklenen talimat yapısı değişmişse eski adres denenmez.
Oyuncu yolu mevcut ortak PlayerXYZ/registry recovery'den türetilir. Aynı
uygulamanın aktif Jump patchi ancak mevcut sahiplik kanıtıyla kabul edilir.

Komutların ayrıca 2.5 saniyelik geçerlilik süresi vardır; bu açık kalma süresi
değildir. Hedefteki timer yalnız kontrolcünün kapanmasını izler. Normal kapatma
ve SafeMode worker'ı durdurur ve özgün hücreyi geri okur. Karakter/oturum/kod
değişimi de worker'ı durdurur. Geri yükleme doğrulanmadan hook/oturum bırakılmaz;
bekleyen geri alma ana kapanış sırasını engeller. noMOBDMG kapanışta ilk sıradadır.
Hedef UI thread'i donarsa timer ve geri yükleme gecikebilir.

%LOCALAPPDATA%/4UnityTools/noMOBDMG altında profiles, recovery ve logs bulunur.
Recovery journal oturum/before-after kanıtıdır; tek başına yabancı bir patchi
sahiplenme yetkisi vermez. RESTORATION=VERIFIED ancak özgün hücre geri okununca
yazılır. UAC requireAdministrator/uiAccess=false korunmuştur.

## İlgili doğrulama

--nomob-test <TClient.exe> <rapor.json> yalnız disk ve bu aracın özel fixture
belleğinde çalışır. Oyun belleği yazımı veya oyun çağrısı yapmaz. Kanıt:
evidence/nomob-tests.json. 15 kontrol, sıfır hata: native ABI, CAS/koruma/geri
alma/kontrolcü kapanması/yabancı hücre, exact-build disk recovery, SHA cache ve
bozuk profil, değiştirilmiş dispatcher/exit/case/cell/outgoing ID, çoklu AOB,
buton/tarama/SafeMode/kapanış bağlantıları, 21 saniye açık kalma ve manuel kapatma,
oturum hatası ve belirsiz aktivasyonda temizlik. Tüm uygulama UI testleri yeniden
çalıştırılmadı.
