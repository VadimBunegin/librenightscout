#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <AccelStepper.h>
#include <Ticker.h>
#include <LiquidCrystal_I2C.h>

const int tachPIN = D4;
const unsigned long sampleTime = 10;

LiquidCrystal_I2C lcd(0x27, 16, 2); 

int totalimps = 0;
int value = 0;
#define ON_Board_LED 2
const char* ssid = "Redmi Note 10v";
const char* password = "vadik1234";

const char* host = "script.google.com";
const int httpsPort = 443;

WiFiClientSecure client;

String GAS_ID = "AKfycbyT1XyQYQawxhznzRkS0ZtZka2jwsYS7GRKZjv_MxrQ_fUf1SuF"; 

#define IN1 0
#define IN2 2
#define IN3 14
#define IN4 12
const int buttonPin = D8; // Pin to which the button is connected

String val = "";

// AccelStepper setup
AccelStepper stepper(AccelStepper::FULL4WIRE, IN4, IN2, IN3, IN1);
bool shouldRotate = false;
bool tickerEnabled = false;

void IRAM_ATTR onTimerISR() {
  shouldRotate = true;
}

Ticker timerISR;

void setup() {
  pinMode(tachPIN, INPUT);
  digitalWrite(tachPIN, HIGH);
  Serial.begin(115200);

  // Wi-Fi setup
  WiFi.begin(ssid, password);
  Serial.println("");
  
  pinMode(ON_Board_LED, OUTPUT);
  digitalWrite(ON_Board_LED, HIGH);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    digitalWrite(ON_Board_LED, LOW);
    digitalWrite(ON_Board_LED, HIGH);
  }

  digitalWrite(ON_Board_LED, HIGH);
  Serial.println("");
  Serial.print("Successfully connected to: ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  client.setInsecure();

  // AccelStepper setup
  stepper.setMaxSpeed(500.0); // Set maximum speed in steps per second
  stepper.setAcceleration(500.0); // Set acceleration in steps per second squared

  // Timer ISR setup
  timerISR.attach(2.0, onTimerISR); // Trigger ISR every 2 seconds
  lcd.init();
  // turn on LCD backlight 
  lcd.backlight();
  lcd.print("");
}

void loop() {
  if ((WiFi.status() == WL_CONNECTED)) {
    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
    client->setInsecure();

    HTTPClient https;
    if (https.begin(*client, "https://demo0332388.mockable.io/api/v1/entries/sgv%3Fcount=1")) {
      int httpCode = https.GET();
      if (httpCode > 0) {
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = https.getString();
          val += String(payload[10]) + String(payload[11]) + String(payload[12]);
          Serial.print(val);
          value = val.toInt();
          val = "";

          // Check condition and set flag to rotate motor
          if (value > 110) {
            if (!tickerEnabled) {
              tickerEnabled = true;
              shouldRotate = true;
              Serial.println("Start");
            }
          } else {
            tickerEnabled = false;
            Serial.println("Stop");
          }
            lcd.setCursor(0, 0);

            lcd.print(value);


        }
      } else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }
      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  }

  // Control the stepper motor using the ticker and ISR mechanism
  if (shouldRotate && tickerEnabled) {
    shouldRotate = false;
    stepper.move(2000); // Rotate the motor by a certain number of steps (adjust as needed)
    stepper.runToPosition();
  }

  sendData(value);
  ESP.restart();
}

void sendData(int hum) {
  Serial.println("==========");
  Serial.print("connecting to ");
  Serial.println(host);

  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }
  



  String string_humidity = String(hum, DEC);
  String url = "https://script.google.com/macros/s/" + GAS_ID + "/exec?&humidity=" + string_humidity;
  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
         "Host: " + host + "\r\n" +
         "User-Agent: BuildFailureDetectorESP8266\r\n" +
         "Connection: close\r\n\r\n");

  Serial.println("request sent");

  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("esp8266/Arduino CI has failed");
  } else {
    Serial.println("esp8266/Arduino CI successful!");
  }
  Serial.print("reply was: ");
  Serial.println(line);
  Serial.println("closing connection");
  Serial.println("==========");
  Serial.println();
}
