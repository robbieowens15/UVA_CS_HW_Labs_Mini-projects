#include <Arduino.h>
#include <TinyMqtt.h>   // https://github.com/hsaturn/TinyMqtt
#include "driver/temp_sensor.h"   




/**
 * Local broker that accept connections and two local clients
 *
 *
 *  +-----------------------------+
 *  | ESP                         |
 *  |                 +--------+  | 1883 <--- External client/s
 *  |       +-------->| broker |  | 1883 <--- External client/s
 *  |       |         +--------+  |
 *  |       |             ^       |
 *  |       |             |       |
 *  |       |             |       |     -----
 *  |       v             v       |      ---
 *  | +----------+  +----------+  |       -
 *  | | internal |  | internal |  +-------*  Wifi
 *  | | client   |  | client   |  |
 *  | +----------+  +----------+  |
 *  |                             |
 *  +-----------------------------+
 *
 * pros - Reduces internal latency (when publish is received by the same ESP)
 *      - Reduces wifi traffic
 *      - No need to have an external broker
 *      - can still report to a 'main' broker (TODO see documentation that have to be written)
 *      - accepts external clients
 *
 * cons - Takes more memory
 *      - a bit hard to understand
 *
 */


const char *ssid     = "Group 11";
const char *password = "Steve and Justin and Robbie";


std::string topic_temp="temp-data";
std::string topic_control="control-msgs";


MqttClient mqtt;


void initTempSensor(){
   temp_sensor_config_t temp_sensor = TSENS_CONFIG_DEFAULT();
   temp_sensor.dac_offset = TSENS_DAC_L2; //TSENS_DAC_L2 is default, range: -10 to 80 degree celcius
   temp_sensor_set_config(temp_sensor);
   temp_sensor_start();
}


static int interval = 5000; // will send topic each 5s


void onPublishControl(const MqttClient* /* source */, const Topic& topic, const char* payload, size_t /* length */)
{ Serial << "--> client received " << topic.c_str() << ", " << payload << endl; interval = std::stoi(payload) * 1000; }


void setup()
{
 Serial.begin(115200);
 delay(500);
 Serial << "Clients with wifi " << endl;
 initTempSensor();


 if (strlen(ssid)==0)
   Serial << "****** PLEASE EDIT THE EXAMPLE AND MODIFY ssid/password *************" << endl;


 WiFi.mode(WIFI_STA);
 WiFi.begin(ssid, password);


 while (WiFi.status() != WL_CONNECTED) {   Serial << '-'; delay(500); }


 Serial << "Connected to " << ssid << " IP address: " << WiFi.localIP() << endl;

 mqtt.connect(WiFi.softAPIP().toString().c_str(), 1883);

 mqtt.setCallback(onPublishControl);
 mqtt.subscribe(topic_control);


}


void loop()
{

 mqtt.loop();


 // ============= client B publish ================
 static uint32_t timer = millis() + interval;


 if (millis() > timer)
 {
   static float temperature = 0;
   temp_sensor_read_celsius(&temperature);
   Serial.println(temperature);
   Serial << "Steve is publishing " << topic_temp.c_str() << endl;
   timer += interval;
   mqtt.publish(topic_temp, std::to_string(temperature));
 }
}
