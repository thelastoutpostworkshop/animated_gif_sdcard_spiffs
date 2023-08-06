// Animated GIF with Round Display
//

#include <SPI.h>
#include <TFT_eSPI.h>
#include <AnimatedGIF.h>
#include <SD.h>

// Examples images
#include "images/hyperspace.h"
#include "images/nostromo.h"
#include "images/hud_1.h"
#include "images/hud_2.h"
#include "images/hud_5.h"
#include "images/hud_6.h"
#include "images/hud_7.h"
#include "images/darthvader.h"
#include "images/x_wing.h"

// Uncomment the image to display
//
// #define GIF_IMAGE starwars
// #define GIF_IMAGE trench
// #define GIF_IMAGE hyperspace
// #define GIF_IMAGE nostromo
// #define GIF_IMAGE darthvader
// #define GIF_IMAGE hud_1
#define GIF_IMAGE hud_2
// #define GIF_IMAGE hud_5
// #define GIF_IMAGE hud_6
// #define GIF_IMAGE hud_7
// #define GIF_IMAGE x_wing

#define chipSelectSDCard 12
File gif_file;

AnimatedGIF gif;
TFT_eSPI tft = TFT_eSPI();

void setup()
{
  Serial.begin(115200);

  initSDCard();
  tft.begin();
  tft.setRotation(2);
  tft.fillScreen(TFT_BLACK);

  gif.begin(BIG_ENDIAN_PIXELS);
}

void loop()
{
  // Put your main code here, to run repeatedly:
  if (gif.open((uint8_t *)GIF_IMAGE, sizeof(GIF_IMAGE), GIFDraw))
  {
    Serial.printf("Successfully opened GIF; Canvas size = %d x %d\n", gif.getCanvasWidth(), gif.getCanvasHeight());
    tft.startWrite(); // The TFT chip slect is locked low
    while (gif.playFrame(true, NULL))
    {
      yield();
    }
    gif.close();
    tft.endWrite(); // Release TFT chip select for other SPI devices
  }
}

void initSDCard(void)
{
  if (!SD.begin(chipSelectSDCard))
  {
    Serial.println("SD Card initialization failed!");
    while (1)
      ;
  }
  Serial.println("SD initialization done.");
}
