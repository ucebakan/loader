# BOT · 4UnityTools 0.9.21

**Butonlar → Araçlar → BOT** üst gezinmesinde BOT ayrı sekmedir. Ayrı bir makro
EXE'si açılmaz. Player XYZ, MobTP, Salesman ve Counter Exit aynı uygulamanın
mevcut doğrulama ve işlem yöntemleri üzerinden çalışır.

Varsayılan 17 adım:

1. Birinci kayıtlı noktaya ışınlan; MobTP'yi bir kez çalıştır; imleci oyun
   istemci alanının tam ortasına taşı; seçili skill tuşuna bas; 3 saniye bekle.
2. SELL TRASH doğrudan satış rutinini bir kez çalıştırır; Salesman açılmaz.
3. Seçilen home noktasına ışınlan ve 10 saniye bekle. Home seçilmemişse
   home ışınlanma adımı atlanır, bekleme mevcut noktada yapılır.
4. İkinci noktaya ışınlan; MobTP, imleç, skill, 3 saniye bekleme ve satış
   adımlarını tekrarla. İkinci satıştan sonra home/10 saniye bekleme yoktur.
5. Mevcut Counter Exit yolunu bir kez çalıştır; 2 saniye bekle; mevcut
   başlangıç karakterini kimliğiyle bul ve hazır Start game rutinini bir kez çalıştır.
   Oyun durumu ve başlangıçtaki actor kimliği eşleşince ana akış başa döner.

Normal kamera düzeninde karakterin gövdesinin ekranın tam ortasında olduğu
kullanıcı tarafından belirtildi; imleç hedefi bu düzene göre hesaplanır.
İmleç adımı tıklama göndermez. Skill tuşu karaktere göre seçilebilir;
cooldown / skill başarı ekranı tanıtma yoktur.

## Düzenleme

- İlk iki nokta ve home, Player XYZ'nin son JSON listesinden seçilir.
  XYZ'de yeni home noktası kaydet, BOT'ta **XYZ listesini yenile** ve seç.
- **İşlem** seç; **Ekle** ile adım oluştur. Seçili satırı **Güncelle**, **Sil**
  veya **↑ / ↓** ile değiştir. **Bekle** adımının süresi saniye cinsindendir;
  3, 10 ve 2 saniyeyi değiştirebilir, yeni beklemeler ekleyebilirsin.
- **Seçili noktaya ışınlan** herhangi bir kayıtlı XYZ noktasını kullanır.
  Home bölümüne MobTP eklenirse akış başlamadan reddedilir.
- **Varsayılan akış** ilk 17 adımı geri getirir. **Kaydet** planı
  `%LOCALAPPDATA%/4UnityTools/Bot/settings.json` dosyasına yazar.
- Tur sayısı **0 sürekli**, **1 bir ana tur**dur. **F8 / Başlat**, **F9 / Durdur**.
  Akışın başında 3 saniyelik hazırlık süresi vardır; oyunu öne getirmek gerekmez.
  Plan başlatılırken kopyalanır; çalışırken dosya/draft değişiklikleri aktif
  planı değiştirmez. Açılışta veya sekme değişiminde BOT kendiliğinden başlamaz.

BOT arka plandaki oyunla çalışır: Alt+Tab yapabilir, oyunun üzerine başka
pencere açabilirsin. Oyunu **küçültmeden açık tut**. İmleç hareketi ve
tuş/tıklama mesajları yalnız seçilen oyun penceresine gönderilir. Giriş
düğmesi ve aktif ekran oyun içindeki doğrulanmış UI yapısından okunur;
girişte OCR veya ekran koordinatı kullanılmaz. SELL TRASH ekran araması kullanmaz;
görünür shop olmadan çöp filtreleme ve satış rutini çalıştırılır.
Butonlar'daki **SELL TRASH** ve Salesman sayfasındaki düğme aynı işlemdir;
elle kullanımda Salesman'ı açmak gerekmez. Oyun/karakter kimliği değişirse durur.
Bulunamayan / çoklu giriş düğmesine tıklamaz. Başarısız MobTP batch'i skill
adımına geçmez; iptal token'ı mevcut batch'e iletilir. Süre aşımında işlemler
otomatik yeniden denenmez.

0.9.21 v1 ayar geçişi eski Salesman aç/kapat adımlarını yedekleyerek kaldırır;
özel delay/sıra/XYZ/skill değerleri korunur. v2 planına bu eylemler açıkça eklenebilir.

SafeMode BOT'u durdurur; eski AOE, Salesman, XYZ, Collection ve Counter
istisnaları korunur. Ana uygulama kapanırken BOT önce durdurulur, ardından
mevcut sıralı geri alma akışı yürür. Exit tek başına diğer araçları durdurmaz.
BOT'un normal durdurulmasında yalnız kendi açtığı Salesman koruması geri alınır;
tamamlanmış ışınlanma, satış veya Exit geri alınmaz.

## Doğrulama

16 ilgili kontrol geçti: arka plan tuş/imleç/tıklama sırası, odak/masaüstü imleci,
üstü kapalı pencere yakalama, iptal/kimlik; güncel build doğrudan komut bağlantısı,
native ABI/window-thread dispatch ve ret kontrolleri; SELL TRASH düğmeleri;
satış popup/araç örtüşmesi, OCR aday ayırımı ve
iptalde pencere geri gösterme; varsayılan sıra / süreler, MobTP sayısı, home kuralı,
plan kopyalama, tekrar, iptal, MobTP hatası, kimlik değişimi, ayar kaydı ve
normal/dar/büyük yazılı arayüz ve UI üzerinden bekleme ekle/güncelle/sırala/sil.
Counter'ın önizlemede Exit'i reddetmesi de
kontrol edildi. Rapor: `AOE-source/Suite/evidence/bot-background/bot-tests.json`.
Yönetici UAC manifesti yayımlanan ve dağıtılan EXE'de korunur.

Canlı istemcide arka plan giriş tıklaması karakteri dünyaya aldı; odak ve
masaüstü imleci bu giriş örneğinde korunmuştu. Salesman açıldı, doğrudan
SELL TRASH bildirimi bir kez kuyruğa alındı ve koruma geri alındı; oyun shop'u
arka plan Esc ile kapandı. Satış örneğinde kullanıcı odağı/imleci değiştirdiği
için o kayıtta sabit arka plan odağı ayrıca kanıtlanmadı. Sunucu satış ACK'ı,
envanter satış sonucu ve F2 dahil tüm ana tur henüz doğrulanmadı. Kullanıcının
12 Ekim BOT ve 13 Ekim doğrudan satış düğmesi entegrasyon isteği uygulandı.

## Son kullanıcı notları · 13 Ekim 2026

Önceki görüşmede kullanıcı oyunu kapattı. O görüşme yalnız kayıt incelemesi ve gereksinim notuydu;
kod değişikliği, build veya yeni canlı oyun denemesi yapılmayacak. Sonraki
build 14 Ekim için planlanıyor. Yeni eklemeler bu listeye eklenecek.

İstenen BOT düzenleyicisi:

- BOT akışı ve adım ayarları kullanıcı tarafından tamamen düzenlenebilir olacak.
- Delay/bekleme adımı listenin istenen yerine eklenebilecek: seçili satırın
  önüne veya arkasına; yalnız listenin sonuna ekleme yeterli değil.
- Önce delay adımı eklenebilecek, ardından süresi belirlenebilecek.
- Her delay'in süresi doğrudan kendi liste satırında değiştirilebilecek.
  Üstteki süre kutusunu değiştirip ayrıca Güncelle'ye basma zorunluluğu olmayacak.
- Adım ekleme, silme, taşıma/sıralama ve her adıma ait ayarları değiştirme
  korunacak. Varsayılan süreler başlangıç değeri olacak, sabit akış şartı değil.
- Düzenlenen sıra ve süreler kaydedilip yeniden açılışta geri yüklenecek.
- Önceki gereksinimler korunacak: saldırı noktalarında bir MobTP, home'da
  MobTP yok; skill tuşu seçilebilir; merkez imleci; satış; Exit/giriş;
  arka planda çalışma. Kullanıcıya yeni ekran tanıtma yükü getirilmeyecek.

SELL TRASH açık hata kaydı / araştırma:

- Kullanıcı 0.9.19'da satışın gerçekleşmediğini bildirdi. Sorun açık kalıyor;
  kullanıcı hatası olduğuna dair kanıt yok.
- Son BOT denemesi 13 Ekim 01:16 civarında iki SELL TRASH adımını geçti.
  Salesman logunda `SELL_TRASH_NOTIFY_QUEUED command=58 ... server_ack_verified=0`
  iki kez var; bu yalnız istemci komutunun kuyruğa alındığını gösteriyor.
- Açık bir SELL TRASH istisnası/durdurma hatası kaydedilmemiş. BOT komuttan
  sonra yaklaşık 800 ms bekleyip Salesman'ı kapatıyor; mevcut kod gerçek
  envanter/satış sonucunu beklemiyor veya doğrulamıyor.
- Sonraki inceleme: oyun düğmesinin gerçek satış davranışıyla doğrudan bildirim
  yolunu karşılaştır; kuyruğun tüketimi, satış isteği/yanıtı, varsa onay penceresi
  ve envanter sonucunu ayır. 800 ms beklemenin yeterliliğini kanıt olmadan varsayma.
  Sorun yalnız gecikmeye bağlanmayacak ve komut gönderilmesi satış başarısı sayılmayacak.
- Kaynaklar: `%LOCALAPPDATA%/4UnityTools/Bot/events.jsonl`,
  `%LOCALAPPDATA%/4UnityTools/Salesman/logs/salesman.log`;
  `AOE-source/Suite/src/Bot/BotBackend.cs`,
  `AOE-source/Suite/src/Bot/BotPanel.cs` (0.9.19'da liste salt okunurdu),
  `AOE-source/Suite/src/Salesman/SellTrash.cs`.

## Uygulandı · 0.9.20 · 13 Ekim 2026

Yeni açık build talebiyle yukarıdaki düzenleyici gereksinimleri uygulandı.
Ekle seçili satırın arkasına, Önüne ekle önüne ekler. Bekleme adımı önce
eklenip Süre · sn hücresinde çift tıklama/F2 ile değiştirilebilir. 0–600 saniye,
virgül veya noktalı ondalık kabul edilir; geçersiz giriş önceki süreyi değiştirmez.
Güncelle, Sil, ↑/↓ ve Kaydet korunur; akış çalışırken ayarlar kilitlenir.

Satış hatasının kod tarafındaki nedeni bulundu: eski 0x67CA/58 bağlantısı
tek seçili eşyayı satma yoluydu. Güncel tclientcmd.tif tam frame ağacında
Salesman/shop 1152 altında metni Sell Trash olan düğme 95713, click komutu
2632'dir. Gerçek düğme FindKid ile alınır ve oyun pencere thread'inde bildirimi
bir kez çalıştırılır; fare konumu ve OCR gerekmez. Arşiv SHA kontrolü ile
eşleşmeyen UI'de çağrı yapılmaz. Salesman'ı uygulamanın açmış olması şartı
hem ana düğmeden hem controller'dan kaldırıldı; aynı shop/karakter/oturum
doğrulamaları korunur.

17 ilgili kontrol geçti. Oyun kullanıcının tercihiyle kapalı; gerçek envanter
satışı ve sunucu ACK'ı bu sürümde henüz sınanmadı. Komut kuyruğa alınması
satış sonucu olarak raporlanmaz. İlgili raporlar `evidence/0.9.20` altındadır.
