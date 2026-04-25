//---- information about the code ----
// project: Teknologi IoT Pekan 5
// created by: Muhammad Ibrahim Yusuf (IoT Engineer)
// created at: 20 April 2026
// deskripsi: LoRa TX Heltec V3

//============================================================
// Heltec WiFi LoRa 32 V3 - LoRa TX 
//============================================================

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <RadioLib.h>

// =========================
// Heltec WiFi LoRa 32 V3
// SX1262 pin mapping
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

// OLED software I2C
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(
  U8G2_R0,
  OLED_SCL,
  OLED_SDA,
  OLED_RST
);

// SX1262 module(cs, irq, rst, busy)
SX1262 radio = new Module(LORA_NSS, LORA_DIO1, LORA_RST, LORA_BUSY);

int counter = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // SPI for SX1262
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_NSS);

  // OLED
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x12_tf);
  u8g2.drawStr(0, 12, "Heltec V3 SX1262 TX");
  u8g2.sendBuffer();

  // Radio init
  int state = radio.begin(923.0);   // bisa ganti 920.0 / 923.0 sesuai region/lab
  if (state != RADIOLIB_ERR_NONE) {
    Serial.print("Radio init failed, code: ");
    Serial.println(state);

    u8g2.clearBuffer();
    u8g2.drawStr(0, 12, "Radio init FAILED");
    u8g2.sendBuffer();

    while (true) {
      delay(1000);
    }
  }

  // optional tuning
  radio.setOutputPower(14);
  radio.setSpreadingFactor(7);
  radio.setBandwidth(125.0);
  radio.setCodingRate(5);
  radio.setSyncWord(0x12);

  Serial.println("TX ready");
}

void loop() {
  String payload = "Pesan dari Ibra ke refan, pesan ke-" + String(counter);

  Serial.print("Sending: ");
  Serial.println(payload);

  int state = radio.transmit(payload);

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x12_tf);
  u8g2.drawStr(0, 12, "Heltec V3 SX1262 TX");
  u8g2.drawStr(0, 28, "Payload:");
  u8g2.drawStr(0, 44, payload.c_str());

  if (state == RADIOLIB_ERR_NONE) {
    Serial.println("Transmit OK");
    u8g2.drawStr(0, 60, "Status: OK");
  } else {
    Serial.print("Transmit failed, code: ");
    Serial.println(state);
    u8g2.drawStr(0, 60, "Status: FAILED");
  }

  u8g2.sendBuffer();

  counter++;
  delay(3000);
}