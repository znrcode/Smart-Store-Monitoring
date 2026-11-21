# Smart Store Monitoring System

ESP32 tabanlı mağaza izleme prototipi. HC-SR04 sensörüyle geçişleri saymayı, OLED ekranda sayaçları göstermeyi ve gün sonunda temel bir kontrol/raporlama senaryosu çalıştırmayı hedefler.

## Özellikler

- HC-SR04 ile geçiş algılama
- OLED ekranda geçiş ve tahmini müşteri sayısını gösterme
- Buzzer ile geçiş geri bildirimi
- Gün sonu butonuyla kontrol senaryosunu başlatma
- Tek sayıda geçiş algılanırsa içeride müşteri kalmış olabileceğine dair LED/buzzer alarmı ve Telegram bildirimi
- Çift sayıda geçiş algılanırsa servo çıkışıyla kepenk kapatma komutu ve Telegram raporu

## Donanım bağlantıları

| Bileşen | ESP32 bağlantısı |
|---|---:|
| HC-SR04 TRIG | GPIO 12 |
| HC-SR04 ECHO | GPIO 13 |
| Gün sonu butonu | GPIO 14 (INPUT_PULLUP) |
| Buzzer | GPIO 25 |
| LED | GPIO 26 |
| Servo sinyal | GPIO 27 |
| SSD1306 OLED SDA/SCL | Kartın varsayılan I²C pinleri |
| SSD1306 OLED adresi | 0x3C |

**Elektriksel notlar:** HC-SR04 ECHO hattı 5 V olabilir; ESP32 girişini korumak için gerilim bölücü kullanın. Servo için yeterli akım sağlayan harici besleme kullanın ve ESP32 ile GND hattını ortaklayın. OLED ve diğer modüllerin besleme/lojik seviyelerini kullandığınız kartla doğrulayın.

## Gerekli kütüphaneler

- ESP32 kart paketi
- UniversalTelegramBot
- ArduinoJson
- Adafruit GFX Library
- Adafruit SSD1306
- ESP32Servo

Telegram HTTPS bağlantısı için kullandığınız UniversalTelegramBot örneklerinin gerektirdiği `TELEGRAM_CERTIFICATE_ROOT` sertifika tanımını projeye ekleyin.

## Kurulum

1. `Smart-Store-Monitoring.ino` dosyasını Arduino IDE ile açın.
2. Wi-Fi adı/şifresi ve Telegram bot token/chat ID alanlarını doldurun.
3. Pin bağlantılarını kontrol edin.
4. ESP32 kartını ve gerekli kütüphaneleri seçerek derleyip yükleyin.

Gerçek Wi-Fi şifresi, bot token'ı veya özel erişim bilgilerini herkese açık depoya koymayın. Kaynak dosyada yalnızca yer tutucular vardır.

## Çalışma mantığı

Sensörün algılama alanına giren nesne bir geçiş olarak sayılır. Prototip, her iki geçişi bir müşteri olarak kabul ederek `gecisSayisi / 2` hesabını kullanır. Gün sonu butonuna basıldığında geçiş sayısı tekse içeride biri kalmış olabileceği varsayımıyla alarm verir. Sayı çiftse servo motorla kepenk kapatma komutu verir, Telegram raporu gönderir ve sayaçları sıfırlar.

## Sınırlamalar ve test notları

- Bu yaklaşım tek sensörle gerçek kişi takibi yapmaz. İki geçişin bir müşteriye karşılık geldiği varsayımı; aynı anda birden fazla kişi, giriş/çıkış yönü, sensörün görüş alanı veya algılama kaçırmaları gibi durumlarda yanlış sonuç verebilir.
- Tek/çift geçiş kontrolü kesin bir güvenlik veya içeride kimse kalmadığı doğrulaması değildir.
- Servo açısı, mekanik kepenk düzeneği ve güç sistemi gerçek donanımda kalibre edilmelidir.
- Kod prototipin mevcut mantığını yansıtır; her donanım kombinasyonunda test edildiği iddia edilmez.

## Lisans

Henüz lisans belirtilmemiştir. Açık kaynak kullanım koşulları belirlenene kadar kodun yeniden dağıtım koşullarını varsaymayın.
