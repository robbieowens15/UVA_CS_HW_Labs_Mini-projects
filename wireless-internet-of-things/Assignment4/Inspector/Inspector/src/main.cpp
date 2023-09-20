#include <Arduino.h>
#include <TinyMqtt.h>
#include <string.h>
#include <WiFi.h> // Required for all wifi code
#include "heltec.h"
#include <deque>
#include <tuple>
/***
 * Code written by Robert Owens for wiot S23
 * 
 *  Requirements: 
 * 1. Connect to Wifi Network and MQTT Broker
 *      - subscribe to /temp-data topic
 * 2. Create a sliding window to average the last minute of temp readings
 * -> Data is received every 5, 15, 30 seconds (4)
 * 
 * 3. Display avg temp (2). UNLESS > 40 degrees C, then display ALERT!
 *    N.B. Display is not print statements but on the screen! 
 * 
 * 4. WHen the PRG button is pressed, cycle the sampling rate {5,15,30}
*/

// Sampling rate
unsigned int prg_pressed_counter = 0;
unsigned int SAMPLING_RATES[3] = {5,15,30}; //Measured in seconds!

unsigned int get_sampling_rate(){ //? Change this function to return in MS?
 return SAMPLING_RATES[prg_pressed_counter % 3];
}

// Data structures for recording sensor reading posted over MQTT
std::deque<float> data;

//_____________________________________________ //

//MQTT Setup
const char *ssid     = "Group 11";
const char *password = "Steve and Justin and Robbie";

const char* BROKER_IP = "";
const uint16_t BROKER_PORT = 1883;

std::string topic_temp="temp-data";
std::string topic_control="control-msgs";

MqttClient mqtt;

// Callback for how to handle temp-data
void onPublishTemp(const MqttClient* /* source */, const Topic& topic, const char* payload, size_t /* length */)
{ 
  Serial << "--> client A received " << topic.c_str() << ", " << payload << endl; //? debug

  // extract temp reading
  float temp_reading = std::stof(payload); 
  Serial << "Parsed Temp: " << temp_reading << endl;

  // Add data reading and time to data structures
  // 5s --> 1x
  // 15s --> 3x
  // 30s --> 6x
  if ((prg_pressed_counter % 3) == 0) {
    // 5s
    data.push_back(temp_reading);
  } else if ((prg_pressed_counter % 3) == 1) {
    // 15s
    data.push_back(temp_reading);
    data.push_back(temp_reading);
    data.push_back(temp_reading);
  } else {
    // 30s
    data.push_back(temp_reading);
    data.push_back(temp_reading);
    data.push_back(temp_reading);
    data.push_back(temp_reading);
    data.push_back(temp_reading);
    data.push_back(temp_reading);
  }
}

// Callback for how to read 
void onPublishControl(const MqttClient* /* source */, const Topic& topic, const char* payload, size_t /* length */)
{ Serial << "--> client B Received " << topic.c_str() << ", " << payload << endl; }

//_____________________________________________ //

bool btn_flag = false;

// When Button is pressed
void prg_handler() {
  btn_flag = true;
}

void setup() {
  // Set Serial 
  Serial.begin(115200);


  // Set PRG Pin Callback
  const byte pin = 0; //PRG Pin in Heltec
  pinMode(pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(pin), prg_handler, LOW /*Not Change. 2x invocation for 1 button press*/);

  // Initialize display and serial. 
  Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Disable*/, true /*Serial Enable*/);
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);

  Heltec.display->clear();
  Heltec.display->drawStringMaxWidth(0, 0, 128, "Hello World");
  Heltec.display->display();

  // Setup WIFI and MQTT
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {   Serial << '-'; delay(500); }
  Serial << "Connected to " << ssid << "IP address: " << WiFi.softAPIP() << " Local IP is " << WiFi.localIP() <<endl;

  mqtt.connect(WiFi.softAPIP().toString().c_str(), 1883, 16960);

  mqtt.setCallback(onPublishTemp);
  mqtt.subscribe(topic_temp);

  
  Serial << "Setup Complete" << endl;
}

float get_avg_temp() {
  if (data.size() == 0) return 0.0;
  // Average over a 1 minute, sliding interval period
  float counter = 0;
  for(float n : data)
        counter += n;
  return counter / data.size();
}

//! parse data to remove everything not a min old
// Loop through all data, remove data points that are older than a minute
void purge_old_temp_data() {
  while(data.size() > 12) {
    data.pop_front();
  }
}

void loop() {

  mqtt.loop();


  static int interval = 1000 * get_sampling_rate(); 
  static uint32_t timer = millis() + interval;

  if (btn_flag) {
    mqtt.publish(topic_control, std::to_string(get_sampling_rate())); // Send message over MQTT to DUT
    prg_pressed_counter++;
    Serial << "Button Pressed" << endl;
    btn_flag = false;
  }

  if (millis() > timer) {
    purge_old_temp_data();


    std::string msg = "no message";
    if (data.size() == 0) {
      msg = "No Data :(";
    } else {
      static float avg_temp = get_avg_temp();
      msg = (avg_temp >= 40) ? "ALERT !! Temp: "  + std::to_string(avg_temp) + "°C" : "Temp: " + std::to_string(avg_temp) + "°C";
    }

    Heltec.display->clear();
    Heltec.display->drawStringMaxWidth(0, 0, 128, msg.c_str());
    Heltec.display->display();


    timer += interval;
  }
}