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
// #define image starwars
// #define image trench
// #define image hyperspace
// #define image nostromo
// #define image darthvader
// #define image hud_1
// #define image hud_2
// #define image hud_5
// #define image hud_6
// #define image hud_7
// #define image x_wing

uint8_t *gif_image;
int image_size;

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
  gif_image = readImageFile("darthvader.gif");
}

void loop()
{
  // Put your main code here, to run repeatedly:
  if (gif_image && gif.open((uint8_t *)gif_image, image_size, GIFDraw))
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

uint8_t *readImageFile(String filename)
{
  uint8_t *image;

  gif_file = SD.open("/" + filename);

  if (!gif_file)
  {
    Serial.println("Failed to open file for reading");
    return NULL;
  }
  // Find out the size of the file to create a suitable buffer
  image_size = gif_file.size();
  image = (uint8_t *)malloc(image_size);

  if (image == NULL)
  {
    Serial.print("Failed to allocate memory:");
    Serial.println(image_size);
    gif_file.close();
    return NULL;
  }

  // Read the entire file into the buffer
  size_t bytesRead = gif_file.read(image, image_size);

  if (bytesRead != image_size)
  {
    Serial.println("Failed to read complete file");
    return NULL;
  }
  else
  {
    Serial.println("Success to read complete file");
  }

  // free(image); // Free the allocated memory
  gif_file.close(); // Close the file
  return image;
}
