// NeoPixel Setup
#include <neopixel.h>
#define PIN 2
#define PIXEL_COUNT 64
#define PIXEL_PIN D2
#define PIXEL_TYPE WS2812B
Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);



// Blynk Stuff
#define BLYNK_PRINT Serial  // Set serial output for debug prints
#define BLYNK_DEBUG       // Uncomment this to see detailed prints
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



void setup()
{
    Serial.begin(115200);
    delay(5000); // Allow board to settle
    strip.begin();
    strip.show();
    dht.begin();
    Blynk.begin(auth);
    timer.setInterval(1000L, increment);
}


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
  Blynk.virtualWrite(V5, temperature);
  Blynk.virtualWrite(V6, humidity);

  // Particle Cloud Calls
  //Particle.publish("temperature", String(temperature) + " °C");
  //Particle.publish("humidity", String(humidity) + " %");
  //Particle.publish("light", String(light) + "%");
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
