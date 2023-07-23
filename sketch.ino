#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <AccelStepper.h>
#include <Ticker.h>

const char* ssid = "Beeline_5E79";
const char* password = "92515952";
String part01;

#define IN1 5
#define IN2 4
#define IN3 14
#define IN4 12

// Настройки шагового двигателя
AccelStepper stepper(AccelStepper::FULL4WIRE, IN1, IN2, IN3, IN4);
bool shouldRotate = false;
bool tickerEnabled = false;

void IRAM_ATTR onTimerISR() {
  shouldRotate = true;
}

Ticker timerISR;

String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }

  // Настройка шагового двигателя
  stepper.setMaxSpeed(1000.0); // Установка максимальной скорости в шагах в секунду
  stepper.setAcceleration(1000.0); // Установка ускорения в шагах в секунду квадрат

  // Настройка аппаратного прерывания таймера
  timerISR.attach(2.0, onTimerISR); // Прерывание каждые 2 секунды
}

void loop() {
  if ((WiFi.status() == WL_CONNECTED)) {
    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
    client->setInsecure();

    HTTPClient https;
    if (https.begin(*client, "https://librenightscout.ru/api/v1/entries/sgv?count=1")) {
      int httpCode = https.GET();
      if (httpCode > 0) {
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = https.getString();
          part01 = getValue(payload, '\t', 2);
          Serial.println(part01);

          // Проверяем условие и управляем флагом для вращения двигателя
          int value = part01.toInt();
          if (value > 270) {
            tickerEnabled = true; // Включаем таймерное прерывание
          } else {
            tickerEnabled = false; // Выключаем таймерное прерывание
          }
        }
      } else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }
      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  }

  // Управление двигателем
  if (shouldRotate && tickerEnabled) {
    // Если должны вращать и флаг включен, то сделаем один шаг
    shouldRotate = false;
    stepper.move(1000); // Вращаем на 90 градусов (зависит от конкретного двигателя и механики)
    stepper.runToPosition();
  }

  // Возможно, вам понадобится небольшая задержка, чтобы дать время другим задачам обрабатываться
  delay(5000);
}
