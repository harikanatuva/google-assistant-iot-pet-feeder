#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <Servo.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// WiFi and MQTT Configuration
#define WIFI_SSID "Galaxy-M20"
#define WIFI_PASS "ac312124"
#define MQTT_SERV "io.adafruit.com"
#define MQTT_PORT 1883
#define MQTT_NAME "aschoudhary"
#define MQTT_PASS "1ac95cb8580b4271bbb6d9f75d0668f1"

int SERVO_PIN = D3;
int CLOSE_ANGLE = 0;
int OPEN_ANGLE = 60;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800, 60000); // 19800 = IST offset
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERV, MQTT_PORT, MQTT_NAME, MQTT_PASS);
Adafruit_MQTT_Subscribe onoff = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/f/onoff");

Servo servo;
LiquidCrystal_I2C lcd(0x27, 16, 2);

int feed_hour = 0, feed_minute = 0;
bool feed = true;

void setup() {
  Serial.begin(9600);

  // Start LCD and Servo
  Wire.begin(D2, D1); // SDA, SCL
  lcd.begin();
  lcd.backlight();
  servo.attach(SERVO_PIN);
  servo.write(CLOSE_ANGLE);

  // Start WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected!");

  // Start MQTT and time client
  mqtt.subscribe(&onoff);
  timeClient.begin();
}

void loop() {
  MQTT_connect();
  timeClient.update();

  int hh = timeClient.getHours();
  int mm = timeClient.getMinutes();
  int ss = timeClient.getSeconds();

  // Display current time
  lcd.setCursor(0, 0);
  lcd.print("Time: ");
  lcd.print(hh > 12 ? hh - 12 : hh);
  lcd.print(":");
  if (mm < 10) lcd.print('0');
  lcd.print(mm);
  lcd.print(":");
  if (ss < 10) lcd.print('0');
  lcd.print(ss);
  lcd.print(hh > 12 ? " PM" : " AM");

  // Display feed time
  lcd.setCursor(0, 1);
  lcd.print("Feed Time: ");
  lcd.print(feed_hour);
  lcd.print(":");
  if (feed_minute < 10) lcd.print('0');
  lcd.print(feed_minute);

  // Read MQTT command
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &onoff) {
      String command = String((char*)onoff.lastread);
      Serial.println("Command: " + command);

      if (command == "ON") {
        open_door();
        delay(1000);
        close_door();
      } else if (command == "Morning") {
        feed_hour = 10;
        feed_minute = 30;
        feed = true;
      } else if (command == "Afternoon") {
        feed_hour = 13;
        feed_minute = 30;
        feed = true;
      } else if (command == "Evening") {
        feed_hour = 18;
        feed_minute = 30;
        feed = true;
      }
    }
  }

  // Auto Feed
  if (hh == feed_hour && mm == feed_minute && feed) {
    open_door();
    delay(1000);
    close_door();
    feed = false; // Avoid multiple feeding in the same minute
  }
}

void MQTT_connect() {
  int8_t ret;
  if (mqtt.connected()) return;

  Serial.print("Connecting to MQTT...");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) {
    Serial.println("Failed, retrying...");
    mqtt.disconnect();
    delay(5000);
    if (--retries == 0) {
      while (1); // Infinite loop on failure
    }
  }
  Serial.println("Connected to MQTT!");
}

void open_door() {
  servo.write(OPEN_ANGLE);
  Serial.println("Feeder Opened");
}

void close_door() {
  servo.write(CLOSE_ANGLE);
  Serial.println("Feeder Closed");
}
