#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Servo.h>

// Wi-Fi ve Telegram bilgilerinizi girin.
const char* ssid = "WIFI_ADI";
const char* password = "WIFI_SIFRE";
#define BOTtoken "TELEGRAM_BOT_TOKEN"
#define CHAT_ID "TELEGRAM_CHAT_ID"

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Donanım pinleri
#define TRIG_PIN 12
#define ECHO_PIN 13
#define BUTTON_PIN 14
#define BUZZER_PIN 25
#define LED_PIN 26
#define SERVO_PIN 27

// OLED ekran
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

Servo kepenkServo;

// Sayaç ve algılama değişkenleri
int gecisSayisi = 0;
int musteriSayisi = 0;
bool engelVar = false;
unsigned long sonGecisSuresi = 0;
const int esikMesafe = 80; // cm

void ekranGuncelle(bool alarmDurumu);
void gunSonuIslemi();

void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  kepenkServo.attach(SERVO_PIN);
  kepenkServo.write(0);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 başlatılamadı!"));
    for (;;) {
      delay(100);
    }
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println("Sistem Baslatiliyor...");
  display.display();

  WiFi.begin(ssid, password);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWi-Fi bağlantısı kuruldu.");

  display.clearDisplay();
  display.setCursor(0, 10);
  display.println("WiFi Baglandi!");
  display.display();
  delay(1500);
}

void loop() {
  // 1. Mesafe ölçümü
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  unsigned long sure = pulseIn(ECHO_PIN, HIGH, 30000UL);
  long mesafe = sure > 0 ? (long)((sure / 2) / 29.1) : 0;

  // 2. Geçiş algılama
  if (mesafe > 0 && mesafe < esikMesafe) {
    if (!engelVar && (millis() - sonGecisSuresi > 1500)) {
      gecisSayisi++;
      musteriSayisi = gecisSayisi / 2;
      engelVar = true;
      sonGecisSuresi = millis();

      digitalWrite(BUZZER_PIN, HIGH);
      delay(100);
      digitalWrite(BUZZER_PIN, LOW);

      ekranGuncelle(false);
    }
  } else {
    engelVar = false;
  }

  // 3. Gün sonu butonu
  if (digitalRead(BUTTON_PIN) == LOW) {
    delay(50);
    if (digitalRead(BUTTON_PIN) == LOW) {
      gunSonuIslemi();

      // Buton bırakılana kadar tekrar işlem yapılmasını önle.
      while (digitalRead(BUTTON_PIN) == LOW) {
        delay(10);
      }
    }
  }

  ekranGuncelle(false);
  delay(100);
}

// OLED ekranı güncelle
void ekranGuncelle(bool alarmDurumu) {
  display.clearDisplay();

  if (alarmDurumu) {
    display.setTextSize(2);
    display.setCursor(10, 10);
    display.println("ALARM!");
    display.setTextSize(1);
    display.setCursor(10, 40);
    display.println("Iceride musteri var!");
  } else {
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("Gecis Sayisi: ");
    display.println(gecisSayisi);

    display.setTextSize(2);
    display.setCursor(0, 25);
    display.print("Musteri:");
    display.println(musteriSayisi);
  }

  display.display();
}

// Gün sonu ve alarm senaryosu
void gunSonuIslemi() {
  // Tek sayıda geçiş: içeride biri kalmış olabilir.
  if (gecisSayisi % 2 != 0) {
    String acilMesaj = "DIKKAT! Gun sonu kontrolunde iceride musteri kalmis olabilir.\n";
    acilMesaj += "Toplam gecis sayisi: " + String(gecisSayisi) + "\n";
    acilMesaj += "Hesaplanan musteri: " + String(musteriSayisi);
    bot.sendMessage(CHAT_ID, acilMesaj, "");

    for (int i = 0; i < 15; i++) {
      ekranGuncelle(true);
      digitalWrite(LED_PIN, HIGH);
      digitalWrite(BUZZER_PIN, HIGH);
      delay(500);

      digitalWrite(LED_PIN, LOW);
      digitalWrite(BUZZER_PIN, LOW);
      delay(500);
    }
  } else {
    // Çift sayıda geçiş: mevcut prototip mantığına göre mağazayı kapat.
    kepenkServo.write(180);

    String raporMesaj = "Gun Sonu Raporu\n";
    raporMesaj += "Kepenk kapatma komutu verildi.\n";
    raporMesaj += "Tahmini musteri sayisi: " + String(musteriSayisi);
    bot.sendMessage(CHAT_ID, raporMesaj, "");

    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 20);
    display.println("Kepenk Kapatildi.");
    display.println("Rapor gonderildi!");
    display.display();

    delay(5000);
    gecisSayisi = 0;
    musteriSayisi = 0;
    kepenkServo.write(0);
  }
}
