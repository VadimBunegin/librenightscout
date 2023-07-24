#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <AccelStepper.h>
#include <Ticker.h>

// Настройки WiFi
const char* ssid = "Beeline_5E79";
const char* password = "92515952";
String part01;

// Пины для управления шаговым двигателем в GPIO
#define IN1 5
#define IN2 4
#define IN3 14
#define IN4 12

// Создание объекта для управления шаговым двигателем
AccelStepper stepper(AccelStepper::FULL4WIRE, IN4, IN2, IN3, IN1);

// Флаги и переменные для управления двигателем и таймером
bool shouldRotate = false;
bool tickerEnabled = false;
bool flag = false;
const int buttonPin = D8; // пин кнопки

// Прерывание таймера
void IRAM_ATTR onTimerISR() {
  shouldRotate = true;
}

Ticker timerISR;

// Функция для извлечения значения из строки данных
String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void setup() {
  Serial.begin(115200);

  // Настройка подключения к WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Подключение к WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }

  // Настройка параметров шагового двигателя
  stepper.setMaxSpeed(1000.0);
  stepper.setAcceleration(1000.0);

  // Настройка таймера для прерывания
  timerISR.attach(5.0, onTimerISR);
}

void loop() {
  if ((WiFi.status() == WL_CONNECTED)) {
    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
    client->setInsecure();

    HTTPClient https;
    if (https.begin(*client, "https://librenightscout.ru/api/v1/entries/sgv?count=1")) {
      int httpCode = https.GET();
      if (httpCode > 0) {
        Serial.printf("[HTTPS] GET... код: %d\n", httpCode);
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          // Получение данных из ответа сервера
          String payload = https.getString();
          part01 = getValue(payload, '\t', 2);
          Serial.println(part01);

          // Преобразование строки в число
          int value = part01.toInt();

          // Проверка значения и установка флагов в зависимости от результата
          if (value > 130) {
            tickerEnabled = true;
          } else {
            tickerEnabled = false;

            // Проверка состояния кнопки
            if (digitalRead(buttonPin) == HIGH) {
              flag = true;
              tickerEnabled = true;
            } else {
              flag = false;
              tickerEnabled = false;
            }
          }
        }
      } else {
        Serial.printf("[HTTPS] GET... не удался, ошибка: %s\n", https.errorToString(httpCode).c_str());
      }
      https.end();
    } else {
      Serial.printf("[HTTPS] Невозможно подключиться\n");
    }
  }

  // Поворот двигателя, если таймер активирован и флаг установлен
  if (shouldRotate && tickerEnabled) {
    if (flag) {
      shouldRotate = false;
      stepper.move(1000);
      stepper.runToPosition();
    } else {
      shouldRotate = false;
      stepper.move(-1000);
      stepper.runToPosition();
    }
  }
}
