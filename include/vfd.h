

#ifndef VFD_H
#define VFD_H
// VFD Parameters, Objects, and Variable
#include <Adafruit_GFX.h>
#include <Arduino.h>
#include <DS1307RTC.h>
#include <SPI.h>
#include <TimeLib.h>
#include <Wire.h>

#define OFF 0
#define INVERSE 1
#define ON 2

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64

#define CLEAR_DISPLAY 0x5f
#define SET_DISPLAY_AREA 0x62
#define INIT_DISPLAY 0x20
#define SET_GRAM_MODE 0x40
#define SET_ADDR_MODE 0x80
#define SET_X 0x64
#define SET_Y 0x60

static DMAMEM uint16_t data_buffer[(8 * (DISPLAY_WIDTH + 4))];

// VFD Class

class VFD : public Adafruit_GFX {
  public:
  VFD(int csPin = 10, int dcPin = 12, int resetPin = 14)
      : Adafruit_GFX(DISPLAY_WIDTH, DISPLAY_HEIGHT), csPin(csPin), dcPin(dcPin),
        resetPin(resetPin){};

  virtual void
  drawPixel(int16_t x, int16_t y,
            uint16_t state) override; // Override adafruit draw pixel function

  void begin();      // Initialize display (with /RESET if pin defined)
  void swapBuffer(); // Flip buffer into the DMA Memory Sector
  void clear();      // Clear framebuffer;
  void setLevel(uint8_t level); // Set Display Brightness
  void setRotation();
  void playAnimation(uint16_t x, uint16_t y, const uint8_t animation_data[],
                     const uint16_t animation_header[]);

  uint8_t buffer[DISPLAY_HEIGHT / 8][DISPLAY_WIDTH]; // Main Buffer

  private:
  const int csPin, dcPin, resetPin; // Hardware Pins - Omitting Frame Ready, it
                                    // makes no speed difference
  int currentFrame;
  uint8_t frameWidth;
  uint8_t frameHeight;
  uint8_t animation_length;
  unsigned long frame_delay;
  unsigned long frame_timer = 0;

  char brightness_lut[16] = {0b1111, 0b1110, 0b1101, 0b1100, 0b1011,
                             0b1001, 0b1000, 0b0111, 0b0110, 0b0101,
                             0b0011, 0b0010, 0b0001, 0b0000

  };
};

#endif