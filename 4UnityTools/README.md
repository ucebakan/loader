# 4UnityTools · Control Center 0.9.28

## Git dağıtımı

Bu klasördeki `4UnityTools.exe`, son teslim edilen 0.9.28 Windows x64 sürümüdür.
EXE, .NET çalışma zamanını, yerel DLL'leri ve yerleşik profilleri içerir;
çalıştırmak için ek DLL veya SDK kurulumu gerekmez. Klasörü indirip EXE'yi aç;
Windows yönetici izni ister. Mevcut kullanıcı ayarları `%LOCALAPPDATA%/4UnityTools`
altında kalır. Telegram token'ı Windows Credential Manager'dadır ve bu pakete dahil değildir.

EXE Git LFS ile saklanır. Git ile indirirken Git LFS kurulu olmalı:

```powershell
git lfs install
git lfs pull --include="4UnityTools/4UnityTools.exe"
Get-FileHash .\4UnityTools\4UnityTools.exe -Algorithm SHA256
```

Hash, bu klasördeki `SHA256SUMS.txt` ile eşleşmelidir. GitHub'ın otomatik kaynak ZIP'i
LFS ayarına göre yalnız işaretçi dosyasını içerebilir; EXE'nin gerçek boyutu
145392184 bayttır. `BOT.md` ve `NO-MOB-DAMAGE.md` ek kullanım belgeleridir;
güncel davranış için aşağıdaki sürüm notları esas alınır.

Aşağıdaki geçmiş sürüm notlarında anılan kaynak/kanıt yolları, ZIP'ler ve geri alma
betikleri geliştirme çalışma alanına aittir; bu Git dağıtımı bunları içermez.
Bu kayıt, mevcut EXE'yi değiştirmeden arşivler. Son teslimin çevrimdışı kontrolleri
geçmiştir; 0.9.28 için tam canlı oyun doğrulaması tamamlanmamıştır.

---

0.9.28 — Farm Alert Phase 2:

- Test Notification artık sayaç izlemesini durdurup yeniden başlatmaz.
  Sadece Telegram alanlarını gerektiğinde kaydeder; sayaç ayarları Kaydet ile uygulanır.
- Buton → hazırlık → kuyruk → HTTP başlangıcı → API yanıtı → tamamlanma
  aşamaları Stopwatch ile ölçülür. Son testin ayrıntıları ayar ekranında gösterilir.
  HTTP istemcisi yeniden kullanılır; günlük yazımı gönderim kuyruğundan ayrıdır.
  Hata tekrarları/rate limit beklemeleri korunur ve ayrıca ölçülür.
  Bu süreler Telegram API kabulünü ölçer; telefonun push gösterim zamanı değildir.
- Önceki yaklaşık 10 sn gözlemin kesin sebebi henüz ölçülmedi. Eski test akışındaki
  örnekleme kapanışı, HTTP zaman aşımı/tekrarları ve gönderim öncesi günlük yazımı
  olası bekleme noktalarıydı. Yeni gerçek testin ölçümleri hangi aşamanın geciktiğini ayırır.
- Katılım doğrulaması ayrı ayarlanır: varsayılan 2 aynı örnek / 200 ms;
  başlangıç ve boşalma 3 örnek / 600 ms kalır. Mevcut kayıtlı ayarlar korunur.
- Doğrulanmış seçili haritada 0→pozitif geçişi doluluk süresini başlatır.
  Sonraki katılımlar başlangıcı değiştirmez; AREA_EMPTY toplam süreyi taşır.
  Geçersiz okuma, oturum/harita değişimi ve Exit eski süreyi bildirimsiz kapatır.
  İlk örnek zaten pozitifse gerçek başlangıç bilinmediğinden süre uydurulmaz.
- Harita okuyucusu yalnız tam E9A33F7A… SHA için hazırlanmıştır; diğer build'lerde
  bilinmeyen döner. PID/oluşturulma zamanı/yol, yüklü PE, kod guard'ları, canlı
  oyun/karakter türleri ve sayaç okumasını çevreleyen aynı harita bağlamı aranır.
  Belleğe yazım, hook, patch veya oyun fonksiyonu çağrısı eklenmedi.
- Harita listesi sadece bu çalışmada canlı doğrulanan sayısal ID'lerle dolar.
  Ad tahmin edilmez. Listeden izinleri seçip otomatik bildirimi işaretleyerek
  Kaydet. İzinler game SHA'sına bağlanır; bilinmeyen/seçilmemiş haritada gönderilmez.

Yeni okuyucunun güncel açık oyunda canlı testi, bu geliştirme oturumunda
Win32 5 (erişim reddedildi) nedeniyle yapılamadı. Mevcut harita alanı disk
üzerinde yeniden eşleşti; önceki aynı-SHA çalışma kayıtları destekleyici kanıttır.
Sentetik testler gerçek oyun doğrulaması değildir. Yeni gerçek Telegram API
gecikmesi de ölçülmedi; 0.9.27 Telegram testi kullanıcı tarafından başarılı bildirildi.

157 izole kontrol, Debug/Release ve UAC denetimleri geçti. Kanıt:
AOE-source/Suite/evidence/farm-alert-phase2/. Tam oyun doğrulaması tamamlanmadı.
0.9.27 geri dönüşü: uygulamayı kapatıp Restore-Before-FarmAlertPhase2.ps1 çalıştır.
Yedek: 4UnityTools.exe.before-farm-alert-phase2. 0.9.26 yedeği de korunur.

---

0.9.27 — Farm Alert / Phase 1: Araçlar → Farm Alert… üzerinden Telegram Bot
Token, Chat ID, olay seçenekleri ve sayaç doğrulama ayarları yapılandırılır.
Test Notification görünür ayarları kaydedip ayrı bir bağlantı testi gönderir.
Token Windows Credential Manager'da tutulur; JSON ayarlarında veya olay
loglarında yer almaz. Boş token alanı mevcut kaydı korur; Token'ı sil açıkça siler.

Harita tespiti henüz doğrulanmadı. Harita seçimi ve otomatik gerçek bildirimler
kapalıdır. JSON'da otomatik gönderim açılsa bile üretim harita sağlayıcısı
bilinmeyen döndürdüğünden gerçek olaylar gönderilmez. Bu sürüm tam Farm Alert
sisteminin tamamlandığı anlamına gelmez.

Salt okunur sayaç izlemeyi seçip Kaydet ile son sayı görülebilir. SafeMode açık
olmak zorunda değildir. SafeMode, Farm Alert ve başlangıç kontrolü tek yönetilen
okuyucudan beslenir; çakışan okumalar aynı bekleyen sonucu paylaşır. Küçük sayaç
penceresinin native döngüsü, Exit ve oyun belleğine yazan mevcut yollar değişmedi.

Olay mantığı ilk okumayı başlangıç kabul eder; geçersiz okuma ve oturum değişimi
karşılaştırmayı sıfırlar. Varsayılan doğrulama 3 ardışık aynı okuma ve en az
600 ms kararlılıktır. İki koşul birlikte aranır; kısa ziyaretler bu nedenle
elenebilir. Zaman olay doğrulandığında alınır; Telegram gecikmesi zamanı
değiştirmez. Gösterim Europe/Istanbul, dd.MM.yyyy HH:mm:ss biçimindedir.

Gönderim sayacı bekletmeyen 32 öğelik ayrı kuyruktadır; doluluk hata olarak
gösterilir. İstek başına 10 sn sınır ve en fazla 3 deneme vardır. Ağ kesintileri,
5xx ve Telegram retry_after yönetilir. Kuyruk uygulama kapanınca iptal edilir;
yeniden açılışta eski mesajlar gönderilmez. Telegram yanıtı ağda kaybolursa
yeniden deneme çift teslim oluşturabilir; tam bir kez teslim garantisi yoktur.

Ayarlar: %LOCALAPPDATA%\4UnityTools\FarmAlert\settings.json
Olay/gönderim günlüğü: aynı klasörde events.jsonl (boyut sınırlı, token içermez).

Doğrulama: Debug ve Release derlemesi; sahte sayaç/HTTP testleri ve mevcut
SafeMode/kapanış/başlangıç regresyonları. Gerçek Windows Credential Manager,
benzersiz bir geçici hedefte sahte token ile yaz/oku/güncelle/sil turunda sınandı;
üretim token kaydına dokunulmadı. Gerçek Telegram gönderimi, canlı oyun sayacı
ve gerçek farm haritası denenmedi. Ayrıntılı kanıt:
AOE-source/Suite/evidence/farm-alert-phase1/farm-alert-tests.json.

Geri dönüş: uygulamayı kapatıp Restore-Before-FarmAlert.ps1 dosyasını çalıştır.
Yedek 4UnityTools.exe.before-farm-alert dosyasıdır. Kullanıcı ayarlarını silmez.

---

0.9.26: BOT'un SELL TRASH adımındaki eski Salesman açık/sahiplik şartı
kaldırıldı. Ana düğme ve BOT aynı doğrudan satış çağrısını kullanır.
Salesman aç/kapat adımlarını akışa eklemek gerekmez.
Canlı kapalı-pencere denemesinde envanterdeki eşya kaydı 54'ten 16'ya
düştü; Salesman kapalı kaldı. Gerçek BotBackend üzerinden tek SELL TRASH
adımı da aynı karakter ve kapalı Salesman ile tamamlandı.


0.9.25: Her map profili için **Nokta JSON → JSON seç…** ile ayrı Player XYZ
nokta dosyası seçilir. Dosya adı/nokta sayısı görünür; tam yol alanın ipucunda
yer alır. Işınlanma, nokta döngüsü ve seçili adım listeleri yalnız o profilin
seçtiği dosyayı kullanır. Ortak `last-points.json` artık BOT'a otomatik eklenmez.

Dosya yolu ve doğrulanmış noktaların kopyası profil içinde saklanır. **Noktaları
yenile** aynı dosyayı tekrar okur. Eksik/bozuk dosya eski listeyi değiştirmez;
dosya taşınsa da kaydedilmiş noktalar kaybolmaz. Başka bir JSON seçmek mevcut
adım koordinatlarını değiştirmez; seçili listede bulunmayan bir ışınlanma adımı
varsa Başlat, o adımın Nokta alanından yeniden seçilmesini ister.

Yeni map başka mapin dosyasını devralmaz; profil kopyası dosya bağını ve nokta
listesini de kopyalar. Eski profiller JSON seçilene kadar yalnız kendi akışındaki
noktaları gösterir. Yedi ilgili test grubu geçti; iki ayrı JSON, liste ayrımı,
geçersiz dosya, eksik dosya, yeniden açılış ve yanlış koordinat kontrolü doğrulandı.


0.9.24: **Map / profil** listesinden ayrı BOT akışları seçilir. **Yeni map**
boş akış oluşturur; adı yazılıp Kaydet ile saklanır. **Profili kopyala** mevcut
akıştan bağımsız bir kopya oluşturur. Her profil kendi skill tuşunu, tur sayısını,
adımlarını ve nokta koordinatlarını saklar. Map değiştirirken geçerli düzenleme
önce kaydedilir; seçilen map bir sonraki açılışta hatırlanır. Seçim BOT'u başlatmaz.

Sabit 1. nokta / 2. nokta / Home alanları kaldırıldı. **+ Işınlanma** ile her
adım için herhangi bir kayıtlı XYZ noktası eklenir; seçili adımın Nokta alanından
değiştirilir. Yedi veya daha fazla nokta eklenebilir (akış başına 200 adım).
**+ Nokta döngüsü**, seçilen nokta için ışınlanma → MobTP → imleç → skill →
3 sn bekleme → SELL TRASH adımlarını birlikte ekler. Her adım sonra değiştirilebilir.
Home noktaları ışınlanma menüsünden veya seçili adımdaki Home işaretiyle atanır;
Home üzerinde MobTP kuralı korunur.

Eski akış, mevcut koordinatları ve süreleri korunarak **Mevcut map** profiline
aktarılır. Önce `settings.json.before-map-profiles.json` yedeği alınır.
Profiller `%LOCALAPPDATA%/4UnityTools/Bot/profiles.json` dosyasındadır.
Altı ilgili arayüz/kalıcılık test grubu geçti; iki map ve yedi noktalı, 42 adımlı
akış özel backend ile doğrulandı. Gerçek oyunda yeni rota çalıştırılmadı.


0.9.23: BOT ayarları geniş çalışma alanında açılır; Butonlar'a dönülünce önceki
kompakt boyut geri gelir. Son BOT pencere boyutu oturum içinde hatırlanır.
Tek tıkla **+ Bekleme** / **+ Skill**, kayıtlı noktalar için **+ Işınlanma**,
diğer işlemler için **+ Diğer adım** kullanılır. Ekleme yeri üst/alt/son olarak
seçilir. Satırda Sil; Yukarı/Aşağı, Kopyala ve Geri al kontrolleri vardır.

Bekleme süresi listede veya seçili adımın sayı alanında düzenlenir. Virgüllü
süre kabul edilir; 0–600 dışındaki giriş eski değeri korur. Kaydet, düzenlenmiş
değeri hemen alır. Skill tuşu üstten seçilir ve skill satırlarında görünür.
DataGridView sıra numarası metin olarak biçimlenir; dönüşüm hataları Windows'un
varsayılan hata penceresi yerine satır/uygulama mesajıyla gösterilir.

Beş ilgili arayüz testi grubu geçti; normal/minimum pencere ve büyük yazı
önizlemeleri incelendi. Kanıt: `AOE-source/Suite/evidence/0.9.23/editor/`.


0.9.22: Jump açılış/kapanışındaki kod yazımı, sahiplik kaydı ve ortak hareket
kodu doğrulaması aynı kilitle korunur. Collection, Jump kapanırken geçici bir
kod değişikliği görüp durmaz; bilinmeyen yamalar hâlâ reddedilir.

noMOBDMG, oyun ana penceresi yok edilirken kendi thread'inde hücreyi geri alır.
Pencere kapanmış ama süreç henüz bitmemişse özgün hücre okunarak doğrulanır.
Oyun sürecinin sonlandığı saklanan süreç handle'ıyla kesinleşince artık geçersiz
hook için sonsuz temizleme döngüsüne girilmez. Canlı sürecin belirsiz geri alma
sonucu başarılı sayılmaz. Ayrıntılar: `AOE-source/Suite/evidence/0.9.22/`.

0.9.21: **SELL TRASH artık Salesman açmaz.** Ana düğme ve BOT, oyunun çöp
filtreleme/seri satış rutinini oyun pencere thread'inde doğrudan çalıştırır.
Shop açma isteği veya görünürlük yazımı yoktur. Satış rutini süresince NPC
bağlamı geçici atanır ve her çıkışta eski değerine döner; sunucu kabulü ayrı konudur.

BOT girişinde OCR/fare tıklaması kaldırıldı. Aktif oyun/karakter-seçim ekranı
ayrılır; başlangıç karakteri roster'da kimliğiyle bulunur, hazır Start game
rutini bir kez çağrılır ve aynı actor'ın dünyaya dönüşü beklenir. Yükleme/
hazır olmayan seçim ekranında giriş çağrısı yapılmaz.

Varsayılan BOT akışı 17 adımdır; Salesman aç/kapat yoktur. Eski v1 akışından bu
iki eylem çıkarılır, eklenmiş süreler/noktalar/skill korunur ve eski dosya
`settings.json.before-direct-sale.json` olarak yedeklenir. v2'de kullanıcı
Salesman aç/kapat adımlarını açıkça eklerse bunlar normal işlevini korur.

19 ilgili yerel kontrol geçti: üretim native yolunda kapalı-shop satışı,
görünürlüğün değişmemesi/NPC bağlamının geri dönmesi, eski actor seçim ekranında
yaşarken World=false, aynı karakter seçimi, dispatch/kimlik/iptal ve ayar geçişi.
Canlı test için Windows UAC başlatması iptal sonucu döndü; bu sürümün gerçek
envanter satışı ve karaktere giriş sonucu henüz canlı doğrulanmadı.
Kullanıcı AOE'nin 0.9.20'de sorunsuz çalıştığını bildirdi; AOE native payload'ı değiştirilmedi.

0.9.20: BOT'ta seçili satırın önüne/arkasına adım eklenir. Bekleme süreleri
doğrudan listede düzenlenir; virgüllü değerler kabul edilir, 0–600 saniye
dışındaki girişlerde önceki süre korunur. Adım ayarları, sıralama ve kayıt sürer.

SELL TRASH eski seçili-eşya satış düğmesi yerine gerçek **Sell Trash** kontrolüne
bağlandı. Aynı doğrulanmış Salesman penceresi oyun içinden açılmışsa da kabul
edilir; ana düğme ve BOT aynı yolu kullanır. Oyun arayüz dosyası değişirse eski
kontrol kimliği kullanılmaz. noMOBDMG'de oyun kapandıktan sonra Windows'un
zaten kaldırdığı hook'u tekrar kaldırmayı bekleyen kapanış döngüsü düzeltildi.

AOE 500 çökme dump'larında geçersiz karakter sınıfı üzerinden sanal çağrı
bulundu. Her ek tekrar öncesi karakter/owner sınıfı, kimliği, dünya ve skill
denetimi eklendi. Sınıfın neden değiştiği ve 500'ün canlı kararlılığı henüz
doğrulanmadı. Rapor: `AOE-source/Suite/evidence/0.9.20/diagnosis.md`.

Görsel değişiklik grafit–mavi renklerle sınırlıdır; mevcut işlev düğmelerinin
dikdörtgen şekli ve yerleşimi korunur. 17 BOT/satış, 15 noMOBDMG ve 12 AOE
worker kontrolü geçti. Kullanıcının isteğiyle oyun kapalı bırakıldı; bu sürümde
canlı satış sonucu veya 500 tekrar denemesi yapılmadı. UAC yönetici manifesti korunur.

0.9.19: Butonlar listesine ve Salesman sayfasına **SELL TRASH** eklendi.
BOT satış adımı aynı doğrudan komutu kullanır; ekran/OCR araması kaldırıldı.
Güncel SHA'da CTShopDlg → TButton → Notify bağlantısı, kontrol kimliği,
komut, etkinlik, dünya/oturum ve canlı kod doğrulanır. Çağrı oyun pencere
thread'inde bir kez yapılır. Sunucu satış ACK'ı ayrıca doğrulanmadı.

BOT artık odağı değişen, başka pencere altında kalan oyunla çalışır.
İmleç/tuş/tıklama seçili oyun HWND'sine gönderilir; giriş görüntüsü Windows
Graphics Capture ile alınır. Oyunu küçültmeden açık tut. 16 ilgili kontrol
geçti; canlı karaktere arka plan giriş, Salesman açma/doğrudan satış komutu
ve geri alma denendi. F2 dahil tüm ana döngü henüz uçtan uca doğrulanmadı.

0.9.18: BOT'un SELL TRASH tıklamasında araç penceresini oyun düğmesiyle
karıştırması düzeltildi. İmleç ve OCR/tıklama sırasında oyunu kapatan kendi
araç pencereleri kısa süreli gizlenir; hata/iptalde de odak alınmadan geri
gösterilir. Oyun köküne bağlı, aynı oyun sürecine ait satış pencereleri kabul
edilir. OCR yalnız görünür oyun alanındaki yazıyı aday sayar.
İlgili yerel pencere testi ve BOT kontrolleri geçti; canlı satış denenmedi.

0.9.17: 12 Ekim gece patchindeki `3D9D9930...` EXE incelendi. Ortak oyuncu
yolu artık R12/SIB kodlamasındaki değişken talimat uzunluğunu da çözer;
iki pointer yüklemesinin yedişer bayt olduğu varsayımı kaldırıldı. Oyuncu
alanı, iki çağrı, owner ilişkisi ve CTClientChar metot bağlantısı yeniden
doğrulanır. Bu build'deki oyuncu alanı koddan `0x4BC0` olarak türetildi;
eski build'in `0x4B68` alanı kullanılmaz. Yeni SHA otomatik yeni profil üretir.

noMOBDMG paket dispatcher'ında eklenen giriş/çıkış sayacı da doğrulanır.
Çıkış, eşleşen artırma/azaltma ve flag temizliğini korur; opcode okuyucusu,
case argümanları, handler/gönderici ve default sonuç veri akışı kontrol edilir.
Giriş/çıkış kodunun tamamı canlı byte guard kapsamındadır.

Yeni build'de yalnız etkilenen çözümleyiciler ve ilişkili ret kontrolleri
çalıştırıldı: 13 kontrol PASS; 17 Jump sahiplik senaryosu, dört noMOBDMG
bozma/ret senaryosu ve SHA-cache/native ABI kontrolleri dahil. Önceki
31813B61 build'inde SIB kullanmayan oyuncu yolu ve ilgili on kontrol PASS.
Canlı oyunda işlev açma/patch uygulama yapılmadı; oyun içi etki doğrulanmadı.
BOT ve noMOBDMG dahil 0.9.16 özellikleri aynı dağıtım EXE'sinde korunur.

İlgili disk kontrolü: `dotnet <Suite.dll> --owner-recovery-audit <TClient.exe> <rapor.json>`.

0.9.16: **BOT** ayrı üst sekmedir; ayarlanabilir adım motoru `src/Bot`
altındadır. XYZ/MobTP/Salesman/Counter aynı uygulamanın mevcut doğrulanmış
yöntemlerini kullanır. Varsayılan 21 adım, 3/10/3/2 saniye beklemeler ve home
hariç iki MobTP çağrısı içerir. Kullanıcının 2026-10-12 açık entegrasyon isteği
uygulanmıştır; canlı uçtan uca sonuç henüz doğrulanmadı.
Yalnız ilgili kontroller: `dotnet <Suite.dll> --bot-test <rapor-dizini>`.
Kullanım: [BOT](../../4UnityTools/BOT.md).

0.9.15: Butonlar'a **noMOBDMG** Aç/Kapat eklendi. Kullanım süresi sınırı
yoktur; buton açık kaldığı sürece mob saldırısıyla ilişkili olayın istemci
işlemi atlanır. Kapat, SafeMode, karakter/oyun oturumu değişimi ve uygulama
kapanışı özgün yönlendirmeyi geri alır. SHA profili, tekil imza/metot bağlantısı
recovery, canlı PE/kod/karakter kontrolü, atomik hücre değişimi, sayfa koruması
ve geri alma kaydı korunur. Tarama yalnız okur; işlevi açmaz.

Güncel disk build'inde ve izole fixture belleğinde 15 ilgili kontrol geçti;
21 saniye boyunca açık kalma ve manuel kapatma doğrulandı. Birleşik sürümün
canlı oyun etkisi bu entegrasyonda yeniden denenmedi. Önceki bağımsız aracın
canlı kanıtı ve kapsamı [noMOBDMG ayrıntılarında](NO-MOB-DAMAGE.md).

0.9.14: Jump açıkken XYZ, MobTP ve Collection'ın kendi 8 baytlık Jump
patchini yabancı kod değişikliği olarak reddetmesi düzeltildi. Kabul yalnız
aynı uygulamada aktif sahibi, PID/oluşturulma zamanı/modül tabanı/SHA,
recovery journal ve canlı patch baytları yeniden doğrulanırsa mümkündür.
Patch dışındaki tüm kod baytları birebir doğrulanır. Kapanış/geri almada bu
uyumluluk kaydı silinir; eski journal tek başına yeterli değildir. Salt okuma
başlangıç taraması aktif, sahipli Jump patchini geri almadan doğrulayabilir.

Auctioneer savaş takımı alanını ülke olarak seçmez. Karakter ülke alanı,
Auction NPC işleyicisinin oyuncu yüklemesi, 2/3 ülke kontrolleri ve
CTClientChar'a bağlı karşılaştırma yöntemiyle yeniden türetilir. Eski
Auctioneer profil şeması otomatik geçersiz sayılıp yeniden taranır. Native
çağrı öncesi ülke alanı ile oyuncu yüklemesinin ilişkisi de doğrulanır.

Bildirilen alanların güncel ve önceki disk build'lerinde sekizer kontrolü
geçti. Gerçek XYZ/MobTP/Collection doğrulayıcılarını da çalıştıran 17 Jump
uyumluluk/ret senaryosu geçti. Oyun belleği yazılmadı veya işlev açılmadı.
Canlı salt okuma kontrolünün Windows yönetici onayı iptal edildi;
bu sürümün canlı işlev sonuçları henüz doğrulanmadı.

Yalnız bildirilen alanların yerel kontrolü:
`dotnet 4UnityTools.dll --reported-errors-audit <TClient.exe> <rapor.json>`.
Yalnız açık istemcide okuma: `4UnityTools.exe --reported-errors-probe <rapor.json>`.
İkinci komut oyuna yazmaz, çağrı yapmaz, işlev açmaz ve patch geri almaz;
normal EXE gibi Windows yönetici onayı gerektirir.

0.9.13: Oyun güncellemesi sonrası ortak owner/koordinat yolları talimat ve
çağrı ilişkilerinden yeniden çözülür; disp8/disp32, stack yerleşimi ve değişen
alanlar koddan türetilir. Invisible/Aggro adayları bağlı yazıcı fonksiyonuyla,
MobTP/Collection fabrikası registry/constructor çağrılarıyla seçilir. AOE
worker ve kayıt ekleme yolu yapısal ilişkilerden çözülür; sahiplik slotu,
alternatif cast alanları ve konum alanı da yeniden türetilir. Auctioneer eski
SHA'ya sabitlenmez; her SHA için kod, RTTI, ülke karşılaştırması ve pencere
çağrı zinciri doğrulanır. Resource/NPC kimliği kapısı korunur.
Kaynak paketi değiştiğinde yedi gerekli NPC dosyasının içerik imzası
doğrulanır; ilgisiz paket değişiklikleri eski bütün-paket SHA'sına takılmaz.
Bu dosyalardan biri değişirse NPC kimliği yeniden analiz edilmeden açılmaz.

SHA profilleri ve her yeni oturumdaki canlı kod/kimlik kontrolleri korunur.
Tarama işlev açmaz veya patch uygulamaz; sıfır/çoklu semantik aday reddedilir.
65DB42AC... ve önceki 31813B61... disk build'lerinde 15'er recovery/ret
kontrolü; yerel native ABI,
bozuk kod/alan ve çoklu aday kontrolleri çalıştırıldı. Bu sürümün canlı oyun
etkileri henüz denenmedi. Kodun anlamını/yapısını değiştiren her gelecekteki
patchin otomatik çözüleceği garantisi verilmez; desteklenen yapılar yeniden
bulunur, doğrulanamayan değişiklikler adres tahminiyle devam etmez.

Yalnız disk recovery kontrolü:
`dotnet 4UnityTools.dll --recovery-audit <TClient.exe> <rapor.json>`.
Test profilleri geçici dizine yazılır; oyun belleği veya işlev çağrısı kullanılmaz.

0.9.12: Counter Exit, CreateRemoteThread yerine WH_GETMESSAGE/PM_REMOVE ile
oyunun pencere iş parçacığında çalışır. Exit hiçbir aracı durdurmaz veya
kapatmaz, AOE detach başlatmaz. Bekleyen isteğin kodu, oturumu ve root'u çağrı
anında yeniden doğrulanır; otomatik retry/fallback yoktur. Henüz canlı oyun
Exit deneyi yapılmadı. SafeMode Counter overlay'ini de açık bırakır; Collection
istisnası korunur. Salesman artık koruması aktifken Kapat gösterir ve bu tık
geçici korumayı geri alır (oyun satış penceresine zorla kapatma göndermez).
Tüm uygulama testleri yeniden çalıştırılmaz; ilgili derleme/UAC ve izole mesaj
köprüsü kontrolü yapılır. Güncelleme aynı dağıtım EXE'sine uygulanır.

0.9.9: Canlıda doğrulanan Auctioneer yolu ayrı bir tek-tık buton olarak eklendi.
Karakter ülkesine göre Markut/Keter/Tebekut Auctioneer kimliği seçilir; NPC yanında
olmadan Auction penceresi açılır. TClient ve FileMerger SHA, canlı kod, PE, oturum
pointer'ları ve ülke her çağrıda doğrulanır; satın alma/teklif otomatikleştirilmez.

0.9.8: Butonlar listesindeki kaydırma çubuğu görseli gizlendi; tekerlek ve
sürüklerken kenarda otomatik kaydırma korunur. Kullanıcının son tercihine göre
bu güncellemede tüm uygulama test/taraması çalıştırılmadı; derleme kontrolü yapıldı.

0.9.7: Butonlar satırları tutamaç, isim veya komut düğmesinden sürüklenerek
sıralanabilir. Bırakma konumu çizgiyle gösterilir; listede kenara yaklaşınca
otomatik kaydırma yapılır. Sürükleme Aç/Kapat komutunu tetiklemez. Sıra
`%LOCALAPPDATA%\4UnityTools\button-order.json` dosyasında kalıcıdır; yeni araçlar
sona eklenir. Tutamaçta sağ tıkla taşıma/varsayılan sıra veya Alt+Yukarı/Aşağı
ile klavyeden sıralama yapılabilir.

0.9.6: AOE, Salesman, Collection ve Player XYZ SafeMode kapatma/durdurma
listesinden çıkarıldı. Player Count ≥ 1 veya eksik sayaç okumasında yalnız
diğer araçlar sıralı olarak geri alınır. Bu dört aracın mevcut penceresi,
bağlantısı ve çalışma döngüsü korunur; SafeMode işlem kapıları da aynı istisnaları
uygular. Ana uygulama kapanışında tüm araçların normal geri alma ve temizliği
korunur.

0.9.5: AOE Manager bağlantı/durum alanı pencere genişliği ve DPI'ye göre önceden
ölçülür; ATTACH NOW sonrasında girişler ve düğmeler yer değiştirmez. Kaydırma
çubuğu görünürken panel genişliğinin ikinci kez daralması önlenir.

0.9.4: Sabit `C:\Games\4Unity\TClient.exe` bağımlılığı kaldırıldı. Araç çalışan
`TClient.exe` sürecinin tam yolunu doğrular, son doğrulanmış yolu saklar ve oyun
kapalıyken standart kurulum konumlarını denetler. Birden fazla farklı istemci
çalışıyorsa güvenli biçimde durur. C# araçları, AOE ve PlayerCounter aynı seçilen
yolu kullanır. [0.9.4 ayrıntıları](PATH-DISCOVERY-0.9.4.md).

0.9.3: Rain of Arrows başlangıcı farklı çağrı yolundan takip edilir; periyodik nesnenin local caster sahipliği doğrulanır. Dört takip noktası Hide/Nx boyunca sabittir. Uzun beyaz tanı yazıları kaldırıldı. [0.9.3 ayrıntıları](AOE-0.9.3.md).

0.9.2: AOE Manager Priest, Mage ve Archer yeteneklerini normal castten tanır; aileler arasında canlı kanıt taşınmaz. [AOE 0.9.2 ayrıntıları](AOE-0.9.2.md). Archer/Mage canlı oyun doğrulaması henüz bekler.

0.9.1: Player XYZ'de **Kendi konumum · canlı** kutuları salt okunurdur.
**Hedef konum** için ayrı, başlangıçta boş X/Y/Z kutuları bulunur; **Işınlan**
ve **Listeye ekle** bu hedefi kullanır. **Konumumu hedefe kopyala** isteğe bağlıdır.
Genel bakış kaldırıldı; ana sayfa Butonlar'dır. XYZ'yi kapatınca pencere gizlenir,
yeniden Aç aynı hazır pencereyi gösterir. Gizliyken koordinat sorguları durur;
ana uygulama kapanınca XYZ de kapanır. [0.9.1 ayrıntıları](XYZ-0.9.1.md).

0.9.0: **Butonlar → Player XYZ → Aç**, ana EXE içinden ayrı PlayerXYZ EXE
penceresini açar. Pencere dar, dikey ve topmost olarak kullanıcı kapatana kadar
açık kalır. Yalnız üç X/Y/Z kutusu vardır: canlı konum izlenir; kullanıcı
düzenlediğinde hedef olur ve okuma yazılan hedefi ezmez. **Konumu getir** canlı
izlemeye geri döner, **Işınlan** tek seferlik doğrulanmış yazım yapar.

Noktaya ad verip **Listeye ekle** ile sıralı liste oluşturabilirsin. **Liste
kaydet** JSON dosyası oluşturur; **Liste yükle** oyunu yeniden açtıktan sonra bu
dosyayı geri getirir. Son liste ayrıca otomatik saklanır ve pencere açılınca
geri gelir. Nokta yanındaki **↗** yalnız o noktaya bir kez ışınlanır; listeyi
açmak, kaydetmek veya pencereyi açmak ışınlanma yapmaz. Oyun kapalıyken de
listeler yönetilebilir. Genel bakış kartındaki mevcut Işınlan işlemi korunur.

Çocuk pencere oyun belleğine erişmez. İstekleri ana uygulamanın mevcut SHA /
profil / canlı oturum doğrulaması ve SafeMode kapısı üzerinden uygulanır.
Ana uygulama kapanırsa çocuk pencere açık kalabilir; listeler görülebilir,
ışınlanma devre dışı kalır. Her iki EXE aynı requireAdministrator / uiAccess=false
manifestini taşır; zaten yönetici olan ana uygulamadan açılan çocuk için Windows
yeniden UAC istemeyebilir. Ayrı çalışma zamanı kurulumu gerekmez.

0.8.1, PlayerCounter **Exit** tıklamasının oyun patchinden sonra eski profili
kullanıp reddedilmesini düzeltir. Hazır olma ve tıklama aynı güncel profil ile
doğrulanır. [Exit düzeltmesi](COUNTER-EXIT-0.8.1.md).

0.8.0, genel bakışa **Collection** aç/kapat düğmesini ekler. Yeni ölen mobların
loot'unu hedef seçmeden, araç mesafe sınırı olmadan hızlı gruplar halinde
toplar. SHA/profil, AOB ve semantik patch recovery, açılış taraması, SafeMode
ve sıralı kapanış ile bütünleşiktir. [Collection ayrıntıları](COLLECTION-0.8.0.md).

0.7.4, Salesman'da değişmeyen kaynak dosyasının tekrar tekrar hash edilmesini
kaldırır ve doğrulama sürelerini kaydeder. [Hızlandırma ayrıntıları](SALESMAN-SPEED-0.7.4.md).

0.7.3, MobTP'de bir mobun konum geri okuması farklı olduğunda kalan mobların hiç
işlenmemesini düzeltir. Y farkı ve XZ sonucu ayrı raporlanır; kimlik ve kısmi yazım
hataları işlemi durdurur. [Düzeltme ayrıntıları](MOBTP-BATCH-0.7.3.md).

0.7.1, MobTP'nin sabit 50 Home sınırını kaldırır. MobTP ayarlarında **Home range (XZ)**
alanına kendi mesafeni girebilirsin. Değer kaydedilir; liste, teleport ve kısayol
aynı mesafeyi kullanır. [MobTP ayrıntıları](MOBTP-RANGE-0.7.1.md).

0.7.0, genel bakışa **Salesman** butonunu ekler. Tuş ataması kullanmadan satış
penceresini doğrudan açar; SELL TRASH'a oyun içinde sen basarsın. SHA/profil,
tekil AOB + metot bağlantısı recovery, salt okunur açılış taraması ve SafeMode /
kapanışta geri alma ile bütünleşiktir. [Salesman ayrıntıları](SALESMAN-0.7.0.md).

Yeni uygulama: [4UnityTools 0.9.4 ZIP](releases/0.9.4/4UnityTools-0.9.4-win-x64.zip).
Önce açık eski 4UnityTools'u kapat, ardından yeni EXE'yi aç.

Butonlar sayfasındaki düğmeler gerçek işlevleri çalıştırır. Player XYZ, Speed / Jump,
Invisible / Aggro, MobTP ve AOE aynı uygulamada yönetilir. PlayerCounter eski
görünümüyle ayrı, küçük, taşınabilir ve üstte kalan pencere olarak açılır.

ZIP içindeki `4UnityTools.exe` doğrudan çalıştırılır. SHA-256 değerleri aynı klasördeki `SHA256SUMS.txt` dosyasındadır.

0.6.3, Hide + Initial birlikteyken görsel reader geçişinin bekleyen DR6
bitlerini silmesini düzeltir. Core breakpoint'ler yerinde korunur; reader
slotu oturum boyunca tutulur. Ayrıntılar: [Detach düzeltmesi](AOE-DETACH-0.6.3.md).

0.6.2, Hide AOE Visual'in Initial Nx tur geçişlerinde bekleyen görsel izleme noktasını
ve sonrasında gerekli prep/call/return kapsamını korur. Gizleme geçişi debugger
worker'da uygulanır; aktif çağrı tamamlanmadan hasar girdisi sıfırlanmaz.
Ayrıntılar: [Hide AOE Visual düzeltmesi](AOE-VISUAL-0.6.2.md).

0.6.1, aynı uygulamanın Multikill'i açıkken AOE attach'in reddedilmesini düzeltir.
Yalnız güncel SHA, PID, oluşturulma zamanı, modül tabanı, kayıt ve canlı kodla
doğrulanan Multikill sahibinin tek opcode değişikliği kabul edilir. Başka
değişiklikler reddedilir; AOE canlı cast doğrulaması ve geri alma yolu korunur.

0.6.0 oyun patchinden sonra yolları yeni SHA için otomatik tarayıp profil üretir.
Alan offsetleri ve metot bağlantıları koddan türetilir. Ayrıntılar ve test
kapsamı: [Patch Recovery](PATCH-RECOVERY.md).
Dosya kendi .NET çalışma zamanını içerir; kurulum gerekmez.
İlk açılışta genel bakış ve kullanım bilgisi görünür. Üst araç barından bir araç
seçildiğinde mevcut kontrolleri içerik alanına yerleşir. Sekme değişimi araçları
kapatmaz; girilen değerler, açık özellikler ve atanmış kısayollar korunur.
Genel bakışta Speed, Jump, Invisible, Aggro, AOE, PlayerCounter ve Collection açılıp kapatılır.
XYZ ve MobTP düğmeleri tek seferlik işlem yapar. Ayarlar ayrı bağlantıdan açılır.
Doğrulama eksikse kısa bilgi penceresi ilgili sayfaya yönlendirir; işlem kuyruğa
alınmaz ve doğrulamadan sonra düğmeye yeniden basılması gerekir.

Kartlar pencere genişliğine göre 1–3 sütuna geçer; metinler ve kontrol satırları
içeriğe göre ölçülür. Dar/yüksek DPI ekranlarında kaydırma ile bütün içerik erişilebilir.
AOE'nin Research/Recovery ve Profiles sayfaları bu build'de kaldırılmıştır;
çalışma profili doğrulaması içeride korunur.

XYZ alanları ASCII rakam, başta eksi ve tek ondalık ayırıcı (`.` veya `,`) kabul
eder. Geçersiz yapıştırma eski değeri korur. Boş/geçici girişler düzenlenebilir;
yazma işleminden önce üç alanın da sonlu float32 sayı olması gerekir.

Dağıtılan EXE her sürümde `requireAdministrator` manifestiyle yönetici yetkisi
ister. Normal masaüstünden açıldığında Windows'un UAC ayarlarına göre onay veya
yönetici kimlik bilgisi ekranı gösterilir. Yönetici yetkisi zaten bulunan bir
işlemden başlatıldığında Windows tekrar onay göstermeyebilir.
PlayerCounter içindeki **Exit** oyuna yöneliktir; ana pencerenin kapatma düğmesi
4UnityTools'u ve küçük sayacı kapatır. Sayaç genel bakıştan veya kendi sağ tık
menüsünden kapatılabilir; ana pencere küçültülünce görünür kalır.
AOE sekmesinde bağlantı ve arm otomatik başlatılmaz.
İlk kez açılan diğer paneller mevcut araçların bağlantı/okuma davranışını kullanır.

## Derleme ve doğrulama

Windows x64, .NET 9 SDK, Visual Studio C++ Build Tools ve CMake gerekir:

```powershell
.\Suite\build.ps1
```

Betik iki yerel DLL modülünü derler, .NET uygulamasını tek dosya olarak yayımlar,
oyuna bağlanmadan yönetilen DLL üzerinden arayüz/doğrulama testlerini çalıştırır ve başarılı sonucu
projedeki sürüme ait `releases` klasörüne kopyalar. `-SkipTests` UI testlerini atlar.
Test raporu `evidence/suite-tests.json`; ekran görüntüleri aynı klasördedir.
Her derlemede kaynak manifestin yönetici yetkisi şartı denetlenir. Yayımlama ve
teslim kopyasının gömülü EXE manifesti `verify-uac.ps1` ile ayrıca doğrulanır;
şart kaldırılmışsa derleme/teslim betiği hata verir. `-SkipTests` bu denetimi atlamaz.

## Yapı

- `src/SuiteForm.cs`: ortak pencere, navigasyon ve kontrollü kapanış.
- `src/OverviewPanel.cs`: dinamik kartlar, bağımsız işlev düğmeleri ve durumları.
- `src/FeatureActions.cs`: doğrulama, eylem yönlendirme ve araç adaptörleri.
- `src/ToolWorkspace.cs`: araç sayfalarının yaşam döngüsü.
- `src/CounterOverlay.cs`: küçük sayaç penceresinin yaşam döngüsü.
- `Controls`: paylaşılan ölçüme dayalı WinForms yerleşim bileşenleri.
- Her C# aracının `SuiteIntegration.cs` dosyası: mevcut işlevleri ortak arayüze bağlayan API ve yerleşim.
- `src/Palette.cs`: antrasit/platin, lavanta ve mavi tasarım.
- `src/NativeTool.cs`: C++ panellerinin aynı işlem içinde barındırılması.
- `native/CMakeLists.txt`: AOE ve PlayerCounter kaynaklarından DLL üretimi.
- `native/aoe_layout.inc`: birleşik build için ölçüme dayalı AOE yerleşimi.
- C# araçları proje referansları ile kullanılır; ayrı EXE başlatılmaz.
- Yerel DLL'ler ve AOE profilleri tek EXE'ye gömülüdür. DLL'ler Windows tarafından
  yüklenebilmek için `%LOCALAPPDATA%/4UnityTools/native` altına içerik özetiyle çıkarılır.
- AOE profil/belgeleri ve Suite'e ait ayarları `%LOCALAPPDATA%/4UnityTools/AOE`
  altında bulunur. XYZ ve Speed/Jump önbellekleri kendi alt klasörlerindedir.
  Invisible/Aggro ve MobTP mevcut kullanıcı ayar/geri-alma konumlarını kullanır.

Ortak pencere ve çevrimdışı testler, canlı oyunda bütün özelliklerin birlikte
çalıştığının doğrulanması anlamına gelmez. Canlı oyun testi ayrıca yapılmalıdır.

## SafeMode ve Multikill

SafeMode genel bakıştan açılır. PlayerCounter penceresinden bağımsız salt okunur
sayaç yaklaşık 400 ms aralıkla örneklenir. Count 0 ise müdahale etmez; >=1 veya
okunamayan sayaç etkin araçları kapanıştaki geri alma yollarıyla durdurur. Yeni
UI/kısayol işlemleri engellenir. Bir geri alma başarısızsa diğer araçlar yine
kapatılır, başarısız işlem yeniden denenir ve engel korunur. Count tekrar 0
olduğunda araçları kullanıcı yeniden açar; otomatik etkinleştirme yapılmaz.
Tamamlanmış XYZ/MobTP taşıma geçmişi geri alınmaz. SafeMode bu uygulamanın
araçlarını yönetir; başka bağımsız EXE'lere kumanda etmez.

Multikill, genel bakış düğmesi ve ayrı fonksiyon sayfasıyla bütünleşiktir.
**Profili doğrula**, **AÇ/KAPAT** ve **Patch Recovery** kontrolleri bulunur.
Aynı kaynak ayrıca [Multikill 1.2.0 ZIP](../Multikill/releases/1.2.0/4UnityMultikill-1.2.0-win-x64.zip) olarak yayımlanır.
Profil/kurtarma ayrıntıları `../Multikill/README.md` içindedir.

## 0.4: Ana ekran koordinatları ve düzen

Player XYZ kartındaki X/Y/Z alanlarını doldurup **Işınlan** düğmesine bas.
Sayı filtresi ve mevcut canlı profil/oyuncu doğrulaması korunur. Detay sayfası
ile ana kart aynı hedef değerlerini kullanır; boş/geçersiz sayılarda düğme pasiftir.

Kartın sağ üstündeki **↕** tutamacını başka bir kartın üzerine sürükle.
Sağ tık menüsüyle öne/arkaya veya en başa/sona taşıma da yapılabilir.
Düzen `%LOCALAPPDATA%/4UnityTools/card-order.json` içinde saklanır.
Pencere genişliği değiştiğinde sütun sayısı değişir, kart sırası korunur.

Multikill **Patch Recovery**, artık oyun güncellemesinden sonra AOB/semantik
fingerprint ile adresi yeniden bulmayı ifade eder. SHA değişirse profil
kontrolü sırasında otomatik taranır; **Yeniden tara** düğmesi de aynı yolu çalıştırır.
Tarama oyuna yazmaz ve işlevi kendiliğinden açmaz. Yeni profil canlı olarak
onaylanınca kullanıcı AÇ düğmesine basar. Çoklu/eksik aday yeni analiz gerektirir.

## Açılış taraması ve kalıcı profiller (0.5)

Açılışta **Tarama başlasın mı?** penceresi gösterilir. **Şimdi değil** hiçbir kontrol başlatmaz. Kabul edilirse XYZ, Speed, Jump, Invisible, Aggro, MobTP, AOE, PlayerCounter ve Multikill sırasıyla kontrol edilir. **Tarama / durum** düğmesi sonuçları tekrar açar. İptal, mevcut salt okunur kontrolün tamamlanmasından sonra kalan sırayı durdurur.

Tarama yalnız okur: arm/toggle, teleport, overlay, hotkey veya crash journal geri alması çalıştırmaz. AOE için SHA eşleşen profil dosyası aranır; ATTACH NOW ve gerekiyorsa Live Validation işlev sayfasında tamamlanır. Bu satırın manuel beklemesi diğer kontrolleri durdurmaz. Açık modüller kendi oturumlarının mevcut durumunu bildirir.

Aynı SHA için kayıtlı AOB/offset profilleri yerel kod kontrolleriyle yüklenir. PID, module base ve heap pointer saklanmaz. Mevcut bilinen SHA'da XYZ ve Speed/Jump doğrulanmış global root yolunu kullanır; eski heap taraması tekrarlanmaz. Bilinmeyen build'de sabit yol kanıtlanamadığında heap çözümlemesi gerekebilir. Multikill yeni SHA'da tekil semantik tarama yapar, sonraki bağlantılarda kayıtlı konumu sınırlı semantik kontrolle doğrular.

Tarama kabul edilmişse ve araç panelleri henüz yüklenmemişse yeni oyun oturumu 10 saniyelik aralıklarla fark edilir. Oyun yüklenirken bekleyen canlı kontroller en fazla iki dakika yeniden denenir; sonra Tarama / durum ile yeniden denenebilir. Yüklenmiş araçlar kendi bağlantı kontrollerini kullanır. Fonksiyonlar bu akışta kendiliğinden açılmaz. AOE'nin oturuma özel manuel doğrulaması SHA aynı olsa da atlanmaz.

## 0.5.1: Sıralı SafeMode kapanışı

Geri almalar tek kuyrukta yürür: Multikill → XYZ → PlayerCounter → MobTP → Speed/Jump → Invisible/Aggro → AOE. Araç kontrolleri ve periyodik sorguları önce duraklatılır; alt durum alanı o an kapanan aracı gösterir. Tamamlanan bir geri alma hata döndürürse diğer araçlara sırayla devam edilir. Hâlâ çalışan bir geri alma 15 saniyede bitmezse sonraki araç başlatılmaz; SafeMode aynı tamamlanmamış işlemi bekleyip tekrar kontrol eder. Sayı 0 olsa da çözülmemiş geri alma varken engel kalkmaz.

AOE kapanışı debugger temizliğini ister ve asenkron tamamlanmasını izler. Pencere ancak iş parçacığının tüm kaynakları bırakması sonrasında yok edilir; arayüz üzerinde bloklayan join beklenmez. Üç kaldırılan kontrol Auto Attach to TClient.exe, Auto Arm after attach, Advanced / Diagnostics'tir. Birleşik build bu kontrolleri oluşturmaz ve kayıtlı otomatik attach/arm seçeneklerini kullanmaz.
