#include "DHT.h"
#include "TFT_eSPI.h"

TFT_eSPI tft;

#define DHTPIN PIN_WIRE_SCL
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

#define BG_COLOR       0x0000
#define TEXT_COLOR     0xFFFF
#define SUBTEXT_COLOR  0xBDF7

#define TEMP_COLOR     0xFDD6
#define HUMIDITY_COLOR 0xBDF7
#define SOIL_COLOR     0xDFF2
#define LIGHT_COLOR    0xFFF0

void setup() {
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(BG_COLOR);
  Serial.begin(115200);
  dht.begin();

  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(TEXT_COLOR);
  tft.setTextSize(2);
  tft.drawString("plant monitor", tft.width() / 2, 20);

  tft.setTextColor(SUBTEXT_COLOR);
  tft.setTextSize(1);
  tft.drawString("environmental conditions", tft.width() / 2, 37);

  drawCard(10, 60, 145, 70, TEMP_COLOR);
  drawCard(165, 60, 145, 70, HUMIDITY_COLOR);
  drawCard(10, 140, 145, 70, SOIL_COLOR);
  drawCard(165, 140, 145, 70, LIGHT_COLOR);

  drawLabel("temp", 82, 75);
  drawLabel("humidity", 237, 75);
  drawLabel("soil moisture", 82, 155);
  drawLabel("light level", 237, 155);
}

void drawCard(int x, int y, int w, int h, uint16_t color) {
  tft.fillRoundRect(x, y, w, h, 10, BG_COLOR);
  tft.drawRoundRect(x, y, w, h, 10, color);
  tft.fillRoundRect(x + 4, y + 4, 4, h - 8, 2, color);
}

void drawLabel(const char* label, int x, int y) {
  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(SUBTEXT_COLOR);
  tft.setTextSize(1);
  tft.drawString(label, x, y);
}

void drawValue(int x, int y, int value, const char* unit, bool valid, uint16_t color) {
  tft.fillRect(x - 62, y - 19, 125, 35, BG_COLOR);
  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(color);
  tft.setTextSize(3);

  if (valid) {
    tft.drawNumber(value, x - 10, y);
    tft.setTextSize(1);
    tft.drawString(unit, x + 28, y);
  } else {
    tft.drawString("n/a", x, y);
  }
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  int soilRaw = analogRead(A0);
  int lightRaw = analogRead(WIO_LIGHT);

  int soilMoisture = -1;

  if (soilRaw >= 300) {
    soilMoisture = map(soilRaw, 1023, 500, 0, 100);
    soilMoisture = constrain(soilMoisture, 0, 100);
  }

  int lightLevel = map(lightRaw, 0, 1023, 0, 100);
  lightLevel = constrain(lightLevel, 0, 100);

  bool dhtValid = !isnan(temperature) && !isnan(humidity) && !(temperature == 0 && humidity == 0);

  drawValue(82, 104, (int)temperature, "C", dhtValid, TEMP_COLOR);
  drawValue(237, 104, (int)humidity, "%", dhtValid, HUMIDITY_COLOR);
  drawValue(82, 184, soilMoisture, "%", soilMoisture != -1, SOIL_COLOR);
  drawValue(237, 184, lightLevel, "%", true, LIGHT_COLOR);

  Serial.print("temp: ");
  if (dhtValid) Serial.print(temperature);
  else Serial.print("n/a");

  Serial.print(" C | humidity: ");
  if (dhtValid) Serial.print(humidity);
  else Serial.print("n/a");

  Serial.print(" % | soil: ");
  Serial.print(soilRaw);

  Serial.print(" | light: ");
  Serial.println(lightRaw);

  delay(2000);
}
