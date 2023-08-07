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

const char *filename = "/hud_2.gif";
void setup()
{
  Serial.begin(115200);
  tft.begin();
  tft.setRotation(2); // Adjust the rotation as needed
  tft.fillScreen(TFT_BLACK);

  // Initialize SD card
  Serial.println("SD card initialization...");
  if (!SD.begin(SD_CS_PIN))
  {
    Serial.println("SD card initialization failed!");
    return;
  }

  // Initialize SPIFFS
  Serial.println("Initialize SPIFFS...");
  if (!SPIFFS.begin(true))
  {
    Serial.println("SPIFFS initialization failed!");
  }

  // Reformmating the SPIFFS to have space if a large GIF is loaded
  Serial.println("Formatting SPIFFS...");
  SPIFFS.format(); // This will erase all the files
  Serial.println("SPIFFS formatted successfully.");

  // Open GIF file from SD card
  Serial.println("Openning GIF file from SD card...");
  File sdFile = SD.open(filename);
  if (!sdFile)
  {
    Serial.println("Failed to open GIF file from SD card!");
    return;
  }

  // Create a file in SPIFFS to store the GIF
  Serial.println("Copy GIF in SPIFFS...");
  File spiffsFile = SPIFFS.open(filename, FILE_WRITE, true);
  if (!spiffsFile)
  {
    Serial.println("Failed to copy GIF in SPIFFS!");
    return;
  }

  // Read the GIF from SD card and write to SPIFFS
  byte buffer[512];
  while (sdFile.available())
  {
    int bytesRead = sdFile.read(buffer, sizeof(buffer));
    spiffsFile.write(buffer, bytesRead);
  }

  spiffsFile.close();
  sdFile.close();

  // Initialize the GIF
  gif.begin(BIG_ENDIAN_PIXELS);
}

void loop()
{
  if (gif.open(filename, fileOpen, fileClose, fileRead, fileSeek, GIFDraw))
  {
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
  if (!gifFile)
  {
    Serial.println("Failed to open GIF file from SPIFFS!");
  }
  return &gifFile;
}

void fileClose(void *pHandle)
{
  gifFile.close();
}

int32_t fileRead(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen)
{
  int32_t iBytesRead;
  iBytesRead = iLen;
  if ((pFile->iSize - pFile->iPos) < iLen)
    iBytesRead = pFile->iSize - pFile->iPos;
  if (iBytesRead <= 0)
    return 0;

  gifFile.seek(pFile->iPos);
  int32_t bytesRead = gifFile.read(pBuf, iLen);
  pFile->iPos += iBytesRead;

  return bytesRead;
}

int32_t fileSeek(GIFFILE *pFile, int32_t iPosition)
{
  if (iPosition < 0)
    iPosition = 0;
  else if (iPosition >= pFile->iSize)
    iPosition = pFile->iSize - 1;
  pFile->iPos = iPosition;
  gifFile.seek(pFile->iPos);
  return iPosition;
}
