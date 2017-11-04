// This #include statement was automatically added by the Particle IDE.
#include <Adafruit_MQTT.h>
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "rickysalmon"
#define AIO_KEY         "aa3704dd4abd884c158c2b0e716ca9179beaf110"

#include "Adafruit_MQTT/Adafruit_MQTT.h"
#include "Adafruit_MQTT/Adafruit_MQTT_SPARK.h"
#include "Adafruit_MQTT/Adafruit_MQTT.h"


TCPClient TheClient;
Adafruit_MQTT_SPARK mqtt(&TheClient,AIO_SERVER,AIO_SERVERPORT,AIO_USERNAME,AIO_KEY);

Adafruit_MQTT_Publish temp = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temperature");
Adafruit_MQTT_Publish humid = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/humidity");



// NeoPixel Setup
#include <neopixel.h>
#define PIN 2
#define PIXEL_COUNT 64
#define PIXEL_PIN D2
#define PIXEL_TYPE WS2812B
Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);



// Blynk Stuff
#define BLYNK_PRINT Serial  // Set serial output for debug prints
//#define BLYNK_DEBUG       // Uncomment this to see detailed prints
#include <blynk.h>
char auth[] = "9ed76d258e464555ba1c0537cbbb8f61";
BlynkTimer timer;
int uptimeCounter;

// DHT Stuff
#define DHTPIN 5
#define DHTTYPE DHT11
#include <Adafruit_DHT.h>
// Variables
int temperature;
int humidity;
int light;
int light_sensor_pin = A0;
DHT dht(DHTPIN, DHTTYPE);



void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
  }
  Serial.println("MQTT Connected!");
}


void setup()
{
    Serial.begin(115200);
    delay(5000); // Allow board to settle
    strip.begin();
    strip.show();
    dht.begin();
    Blynk.begin(auth);
    timer.setInterval(10000L, increment);
    MQTT_connect();
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.


void publishUptime() {
     unsigned long now = millis();
     unsigned long lastTime;
    //Every 30 seconds publish uptime
    if (now-lastTime>30000UL) {
        lastTime = now;
        // now is in milliseconds
        unsigned nowSec = now/1000UL;
        unsigned sec = nowSec%60;
        unsigned min = (nowSec%3600)/60;
        unsigned hours = (nowSec%86400)/3600;
        char* publishString;
        sprintf(publishString,"%u:%u:%u",hours,min,sec);
        Particle.publish("Uptime",publishString);
    }
}
void updateValues() {
  temperature = dht.getTempCelcius();
  humidity = dht.getHumidity();

  float light_measurement = analogRead(light_sensor_pin);
  light = (int)(light_measurement/4096*100);

  // Blynk Value Updates
  temperature = (temperature * 1.8) + 32;
  Blynk.virtualWrite(V5, temperature);
  Blynk.virtualWrite(V6, humidity);

  // Particle Cloud Calls
  //Particle.publish("temperature", String(temperature) + " °C");
  //Particle.publish("humidity", String(humidity) + " %");
  //Particle.publish("light", String(light) + "%");



   Serial.print(F("\nSending MQTT Data "));
   Serial.print("...");
   if (! temp.publish(temperature)) {
     Serial.println(F("Failed sending temperature"));
   } else {
     Serial.println(F("OK!"));
   }
   Serial.print(F("\nSending MQTT Data "));
   Serial.print("...");
   if (! humid.publish(humidity)) {
     Serial.println(F("Failed sending humidity"));
   } else {
     Serial.println(F("OK!"));
   }


}


// LED Control
BLYNK_WRITE(V1) {
    int bright = param[0].asInt();
    strip.setBrightness(bright);
    strip.show();
}
BLYNK_WRITE(V2) {
    int r = param[0].asInt();
    int g = param[1].asInt();
    int b = param[2].asInt();
    if (r > 0 || g > 0 || b > 0) {
        RGB.control(true);
        RGB.color(r, g, b);
    } else {
        RGB.control(false);
    }
}
BLYNK_WRITE(V3)
    {
      int r = param[0].asInt(); // get a RED channel value
      int g = param[1].asInt(); // get a GREEN channel value
      int b = param[2].asInt(); // get a BLUE channel value
      for (int i = 0; i < strip.numPixels(); i++)
      {
        strip.setPixelColor(i, r, g, b);
      }
      strip.show();
    }


// Timer Callback.  Put updates here.
void increment() {
      publishUptime();
      updateValues();
      uptimeCounter++;

}

void loop()
{
    Blynk.run();
    timer.run();
}
