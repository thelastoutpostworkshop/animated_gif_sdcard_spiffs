#include <SD.h>
#include <SPI.h>
#include <FS.h>
#include <SPIFFS.h>
#include <TFT_eSPI.h>
#include "AnimatedGIF.h"

#define SD_CS_PIN 12 // Change this to your SD card's CS pin

AnimatedGIF gif;
File gifFile;              // Global File object for the GIF file
TFT_eSPI tft = TFT_eSPI(); // TFT object

void setup()
{
  Serial.begin(115200);
  tft.begin();
  tft.setRotation(3); // Adjust the rotation as needed
  tft.fillScreen(TFT_BLACK);

  // Initialize SD card
  if (!SD.begin(SD_CS_PIN))
  {
    Serial.println("SD card initialization failed!");
    return;
  }

  Serial.println("Entering SPIFFS initialization...");

  // Initialize SPIFFS
  if (!SPIFFS.begin())
  {
    Serial.println("SPIFFS initialization failed! Formatting...");
    if (SPIFFS.format())
    {
      Serial.println("SPIFFS formatted successfully.");
    }
    else
    {
      Serial.println("SPIFFS formatting failed!");
      return;
    }
  }
  Serial.println("Exiting SPIFFS");

  // Open GIF file from SD card
  File sdFile = SD.open("/hud_1.gif");
  if (!sdFile)
  {
    Serial.println("Failed to open GIF file from SD card!");
    return;
  }
  Serial.println("Gif file opened from SD Card!");

  // Create a file in SPIFFS to store the GIF
  File spiffsFile = SPIFFS.open("/hud_1.gif", FILE_WRITE, true);
  if (!spiffsFile)
  {
    Serial.println("Failed to create file in SPIFFS!");
    return;
  }
  Serial.println("Created file in SPIFFS!");

  // Read the GIF from SD card and write to SPIFFS
  byte buffer[512];
  int totalBytesRead = 0;
  while (sdFile.available())
  {
    int bytesRead = sdFile.read(buffer, sizeof(buffer));
    spiffsFile.write(buffer, bytesRead);
    totalBytesRead += bytesRead;
  }
  Serial.println("Read the GIF from SD card and write to SPIFFS!");
  Serial.print("Total bytes read from SD card: ");
  Serial.println(totalBytesRead);

  spiffsFile.close();
  sdFile.close();

  // Open the GIF from SPIFFS
  // gifFile = SPIFFS.open("/darthvader.gif", FILE_READ);
  // if (gifFile)
  // {
  //   Serial.print("File size in SPIFFS: ");
  //   Serial.println(gifFile.size());
  //   gifFile.close();
  // }
  // else
  // {
  //   Serial.println("Failed to open GIF file from SPIFFS!");
  // }

  // Initialize the GIF
  gif.begin(BIG_ENDIAN_PIXELS);

  // // Play the GIF frame by frame
  // int delayMilliseconds;
  // while (gif.playFrame(true, &delayMilliseconds))
  // {
  //   delay(delayMilliseconds);
  // }

  // gifFile.close();
}

void loop()
{
  if (gif.open("/hud_1.gif", fileOpen, fileClose, fileRead, fileSeek, GIFDraw))
  {
    Serial.printf("Successfully opened GIF; Canvas size = %d x %d\n", gif.getCanvasWidth(), gif.getCanvasHeight());

    tft.startWrite(); // The TFT chip slect is locked low
    while (gif.playFrame(true, NULL))
    {
      // yield();
    }
    gif.close();
    tft.endWrite(); // Release TFT chip select for other SPI devices}
  }
  else
  {
    Serial.printf("Error gif.open!");
  }
}

// Callbacks for file operations
void *fileOpen(const char *filename, int32_t *pFileSize)
{
  gifFile = SPIFFS.open(filename, FILE_READ);
  *pFileSize = gifFile.size();
  if (gifFile)
  {
    Serial.print("File size in SPIFFS: ");
    Serial.println(*pFileSize);
  }
  else
  {
    Serial.println("Failed to open GIF file from SPIFFS!");
  }

  return &gifFile; // Return the address of the global File object
}

void fileClose(void *pHandle)
{
  gifFile.close();
}

int32_t fileRead(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen)
{
  int32_t bytesRead = gifFile.read(pBuf, iLen);
  Serial.print("Read ");
  Serial.print(bytesRead);
  Serial.println(" bytes");
  return bytesRead;
}

int32_t fileSeek(GIFFILE *pFile, int32_t iPosition)
{
  bool success = gifFile.seek(iPosition);
  Serial.print("Seek to ");
  Serial.print(iPosition);
  Serial.println(success ? " succeeded" : " failed");
  return success ? iPosition : -1;
}

// Callback to draw the GIF
// void gifDraw(GIFDRAW *pDraw) {
//   if (pDraw->iDir == 3) {
//     // Clear the background if it's the last frame
//     tft.fillRect(pDraw->iX, pDraw->iY, pDraw->iWidth, pDraw->iHeight, TFT_BLACK);
//   } else if (pDraw->pPixels) {
//     // Draw the pixels
//     uint16_t *pixels = (uint16_t *)pDraw->pPixels;
//     tft.pushImage(pDraw->iX, pDraw->iY, pDraw->iWidth, pDraw->iHeight, pixels);
//   }
// }
