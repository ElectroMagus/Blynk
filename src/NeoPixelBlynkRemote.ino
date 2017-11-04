// This #include statement was automatically added by the Particle IDE.
#include <neopixel.h>
#define PIN 2

/*************************************************************
  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************

  No coding required for direct digital/analog pin operations!

 *************************************************************/

#define BLYNK_PRINT Serial  // Set serial output for debug prints
#define BLYNK_DEBUG       // Uncomment this to see detailed prints

#include <blynk.h>

// IMPORTANT: Set pixel COUNT, PIN and TYPE
#define PIXEL_COUNT 64
#define PIXEL_PIN D2
#define PIXEL_TYPE WS2812B

Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);


// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "9ed76d258e464555ba1c0537cbbb8f61";


BlynkTimer timer;
int uptimeCounter;


// Attach a Button widget (mode: Switch) to the Digital pin 7 - and control the built-in blue led.
// Attach a Graph widget to Analog pin 1
// Attach a Gauge widget to Analog pin 2

void setup()
{
    Serial.begin(115200);
    delay(5000); // Allow board to settle
    strip.begin();
    strip.show();
    Blynk.begin(auth);
    timer.setInterval(1000L, increment);
}

void increment() {
  uptimeCounter++;

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
/* Attach a Button widget (mode: Push) to the Virtual pin 1 - and send sweet tweets!
BLYNK_WRITE(V1) {
    if (param.asInt() == 1) { // On button down...
        // Tweeting!
        // Note:
        //   We allow 1 tweet per minute for now.
        //   Twitter doesn't allow identical subsequent messages.
        Blynk.tweet("My Particle project is tweeting using @blynk_app and it’s awesome!\n @Particle #IoT #blynk");

        // Pushing notification to the app!
        // Note:
        //   We allow 1 notification per minute for now.
        Blynk.notify("You pressed the button and I know it ;)");
    }
}

// Attach a ZeRGBa widget (mode: Merge) to the Virtual pin 2 - and control the built-in RGB led!
*/

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

void loop()
{
    Blynk.run();
    timer.run();
}
