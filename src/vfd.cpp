
#include "vfd.h"

DMAChannel *SPI_DMA_CHANNEL; // Declare the SPI DMA Channel

// Display Command Format
uint32_t vfd_cmd(uint8_t value, bool isCommand) {
  uint16_t _value = 0;
  uint16_t mask = 0x80;
  for (int i = 0; i < 8; i++) {
    _value <<= 1;
    _value |= value & mask;
    mask >>= 1;
  }
  _value |= 0xaaaa * isCommand;
  return _value;
}


// Send Command
void write_command(uint8_t value, bool isCommand) {
  LPSPI4_TDR = vfd_cmd(value, isCommand);
  while (LPSPI4_FSR)
    ;
  while (LPSPI4_SR & LPSPI_SR_MBF)
    ;
}

// Setup VFD
void VFD::begin() {
  // Setup the display control pin modes
  pinMode(csPin, OUTPUT);    // Slave Select
  pinMode(dcPin, OUTPUT);    // Data Control
  pinMode(resetPin, OUTPUT); // Display Reset

  // Reset the Display | Active LOW
  digitalWrite(resetPin, LOW);
  delay(1);
  digitalWrite(resetPin, HIGH);

  SPI.begin(); // Start the SPI Driver

  // Initialize the SPI bus parameters | 5MHZ | MSB | Mode 3
  SPI.beginTransaction(SPISettings(5000000, MSBFIRST, SPI_MODE3));
  SPI.endTransaction();
  SPI.setCS(csPin);

  int SPI_CLK_DIV = LPSPI_CCR_SCKDIV(LPSPI4_CCR); // Setup the SPI clock divider
  LPSPI4_CCR = LPSPI_CCR_SCKPCS(SPI_CLK_DIV * 3 / 4) |
               LPSPI_CCR_PCSSCK(SPI_CLK_DIV / 5) |
               LPSPI_CCR_DBT(SPI_CLK_DIV * 2 / 5) |
               LPSPI_CCR_SCKDIV(SPI_CLK_DIV);
  //Setup DMA to 16 bit mode - (8 bits data) (1 bit command/data type) - The dma engine handles the entire transaction
  LPSPI4_TCR = (LPSPI4_TCR & ~LPSPI_TCR_FRAMESZ(0xfff)) |
               LPSPI_TCR_FRAMESZ(15) | LPSPI_TCR_RXMSK | LPSPI_TCR_WIDTH(1);

  SPI_DMA_CHANNEL = new DMAChannel(); // Initialize the SPI DMA Channel

  write_command(CLEAR_DISPLAY, 1); // Clear the display
  delay(1);                  // Wait 1ms

  // Define the display area
  for (int i = 0; i < 8; i++) {
    write_command(SET_DISPLAY_AREA, 1);
    write_command(i, 1);
    write_command(0xff, 0);
  }

  write_command(INIT_DISPLAY | (4 << 0), 1);            // Initialize the display
  write_command(SET_GRAM_MODE, 1);                       // Set the GRAM Mode
  write_command(SET_ADDR_MODE | (1 << 2) | (0 << 1), 1); // Set address write mode

  setTextWrap(true);
  setSyncProvider(RTC.get);
}

void writeBuffer(uint8_t *fb) {
  int idx = 0;
  for (int i = 0; i < (DISPLAY_HEIGHT / 8); i++) {
    data_buffer[idx++] = vfd_cmd(SET_X, 1);
    data_buffer[idx++] = vfd_cmd(0, 1);
    data_buffer[idx++] = vfd_cmd(SET_Y, 1);
    data_buffer[idx++] = vfd_cmd(i, 1);
    for (int j = DISPLAY_WIDTH * i; j < DISPLAY_WIDTH * (i + 1); j++) {
      data_buffer[idx++] = vfd_cmd(fb[j], 0);
    }
  }

  arm_dcache_flush_delete(data_buffer, sizeof(data_buffer));
  LPSPI4_DER = LPSPI_DER_TDDE; // Request DMA transfer
  SPI_DMA_CHANNEL->disable();
  SPI_DMA_CHANNEL->destination((volatile uint16_t &)LPSPI4_TDR);
  SPI_DMA_CHANNEL->disableOnCompletion();
  SPI_DMA_CHANNEL->triggerAtHardwareEvent(DMAMUX_SOURCE_LPSPI4_TX);
  SPI_DMA_CHANNEL->sourceBuffer(data_buffer, sizeof(data_buffer));
  SPI_DMA_CHANNEL->enable();
  unsigned long startTime = millis();

  // Throw error if DMA transfer times out
  while (!SPI_DMA_CHANNEL->complete()) {
    if (millis() - startTime > 1000) {
      Serial.println("DMA Failed");
      startTime += 1000;
    }
  }
  SPI_DMA_CHANNEL->clearComplete();
  SPI_DMA_CHANNEL->disable();
}
//Transfer the internal buffer to the display
void VFD::swapBuffer() { writeBuffer(&buffer[0][0]); }

void VFD::drawPixel(int16_t x, int16_t y, uint16_t state) {
  // Exit function if the draw violates the displays bounds
  if (x >= DISPLAY_WIDTH || y >= DISPLAY_HEIGHT || x < 0 || y < 0) {
    return;
  }
  // Format pixel data
  uint8_t position = y / 8;
  uint8_t sub_pixel = y % 8;

  uint8_t *data = &buffer[position][x];

  switch (state) {
  case 0:
    *data &= ~(1 << sub_pixel); // Pixel Off
    break;
  case 1:
    *data ^= (1 << sub_pixel); // Pixel Invert
    break;
  case 2:
  default:
    *data |= (1 << sub_pixel); // Pixel On
    break;
  }
}

void VFD::clear() {
  memset(buffer, 0,
         (DISPLAY_WIDTH * (DISPLAY_HEIGHT / 8))); // Set entire internal buffer to 0
}

void VFD::setLevel(uint8_t level) {
  if (level > 16 || level < 0)
    return;
  write_command((0x40 | brightness_lut[level]), 1);
}

void VFD::playAnimation(uint16_t x, uint16_t y, const uint8_t animation_data[],
                        const uint16_t animation_header[]) {
  int16_t byteWidth = (animation_header[0] + 7) / 8; // Bitmap scanline pad = whole byte
  uint8_t byte = 0;

  startWrite();
  for (int16_t j = 0; j < animation_header[1]; j++, y++) {
    for (int16_t i = 0; i < animation_header[0]; i++) {
      if (i & 7)
        byte <<= 1;
      else
        byte = pgm_read_byte(&animation_data[(j * byteWidth + i / 8)+(currentFrame*((animation_header[0]*animation_header[1])/8))]);
      writePixel(x + i, y, (byte & 0x80) ? ON : OFF);
    }
  }
  endWrite();
  unsigned long current_time = millis();
  if(current_time - frame_delay >= (unsigned long)animation_header[currentFrame+3]){
    currentFrame++;
    if(currentFrame > animation_header[2]-1)
      currentFrame = 0;
    frame_delay = current_time;
  }
}
