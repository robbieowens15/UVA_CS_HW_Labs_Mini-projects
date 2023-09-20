#include <Arduino.h>
#include <TinyMqtt.h>   // https://github.com/hsaturn/TinyMqtt
#include <esp_wifi.h>
#include <sstream>

const uint16_t PORT = 1883;
const uint8_t  RETAIN = 10;  // Max retained messages

MqttBroker broker(PORT);

void get_client_details() {
    wifi_sta_list_t clients;
    esp_wifi_ap_get_sta_list(&clients);
    Serial.printf("Number of connected clients: %d\n", clients.num);
    Serial.printf("Broker's IP Address: %d\n", WiFi.softAPIP());
    Serial.printf("Port: %d\n", PORT);
}

/** Basic Mqtt Broker
  *
  *  +-----------------------------+
  *  | ESP                         |
  *  |       +--------+            |
  *  |       | broker |            | 1883 <--- External client/s
  *  |       +--------+            |
  *  |                             |
  *  +-----------------------------+
  *
  *  Your ESP will become a MqttBroker.
	*  You can test it with any client such as mqtt-spy for example
	*
	* Messages are retained *only* if retain > 0
	*
  */

const char* ssid = "Group 11";
const char* password = "Steve and Justin and Robbie";

void setup()
{
  Serial.begin(115200);
  delay(10);

  /* setting up access point */ 

  Serial.println("Started!");
  Serial.print("MAC Address is: ");
  Serial.println(WiFi.macAddress());

  Serial.println("Setting up Access Point...");
  if (!WiFi.softAP(ssid, password)) {
    Serial.println("Soft AP creation failed.");
    while(1);
  }
   
  Serial.println("AP Started!");
  IPAddress myIP = WiFi.softAPIP();
  Serial.println(myIP);

  /* setting up the broker */

  broker.begin();
  Serial << "Broker ready : " << WiFi.localIP() << " on port " << PORT << endl;
}

void loop()
{
  broker.loop();
  static const int interval = 5000; // will send topic each 5s
  static uint32_t timer = millis() + interval;

  if (millis() > timer) {
    get_client_details();
    timer += interval;
  }

}