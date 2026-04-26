//---- information about the code ----
// project: Teknologi IoT Pekan 5
// created by: Muhammad Ibrahim Yusuf (10824006)(Tx)
// project partner: Refan Rustoni Putra (10824005)(Rx)
// deskripsi: LoRa TX Heltec V3 + pembacaan sensor DHT(dummy)

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <RadioLib.h>
#include "DHT.h"

// =========================
// DHT CONFIG 
// =========================
#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

// =========================
// Heltec WiFi LoRa 32 V3
// =========================
#define LORA_NSS   8
#define LORA_SCK   9
#define LORA_MOSI 10
#define LORA_MISO 11
#define LORA_RST  12
#define LORA_BUSY 13
#define LORA_DIO1 14

#define OLED_SDA  17
#define OLED_SCL  18
#define OLED_RST  21

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(
  U8G2_R0,
  OLED_SCL,
  OLED_SDA,
  OLED_RST
);

SX1262 radio = new Module(LORA_NSS, LORA_DIO1, LORA_RST, LORA_BUSY);

int counter = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  dht.begin(); // tetap ada walaupun dummy

  // seed random
  randomSeed(analogRead(0));

  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_NSS);

  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x12_tf);
  u8g2.drawStr(0, 12, "Heltec TX Dummy DHT");
  u8g2.sendBuffer();

  int state = radio.begin(923.0);
  if (state != RADIOLIB_ERR_NONE) {
    Serial.print("Radio init failed, code: ");
    Serial.println(state);
    while (true) delay(1000);
  }

  radio.setOutputPower(14);
  radio.setSpreadingFactor(7);
  radio.setBandwidth(125.0);
  radio.setCodingRate(5);
  radio.setSyncWord(0x12);

  Serial.println("TX ready");
}

void loop() {

  // =========================
  // BACA DHT
  // =========================
  float suhu = dht.readTemperature();
  float hum  = dht.readHumidity();

  suhu = random(200, 350) / 10.0;   // 20.0 - 35.0 °C (data dummy)
  hum  = random(400, 900) / 10.0;   // 40.0 - 90.0 % (data dummy

  // =========================
  // FORMAT DATA
  // =========================
  String payload = "Suhu:" + String(suhu) + ",Hum:" + String(hum);

  Serial.print("Sending: ");
  Serial.println(payload);

  int state = radio.transmit(payload);

  // =========================
  // OLED DISPLAY
  // =========================
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x12_tf);

  u8g2.drawStr(0, 12, "TX Dummy DHT");

  u8g2.setCursor(0, 28);
  u8g2.print("Suhu: ");
  u8g2.print(suhu);

  u8g2.setCursor(0, 44);
  u8g2.print("Hum : ");
  u8g2.print(hum);

  if (state == RADIOLIB_ERR_NONE) {
    Serial.println("Transmit OK");
    u8g2.drawStr(0, 60, "Status: OK");
  } else {
    Serial.println("Transmit Failed");
    u8g2.drawStr(0, 60, "Status: FAIL");
  }

  u8g2.sendBuffer();

  counter++;
  delay(3000);
}
