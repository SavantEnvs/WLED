#pragma once
#include <Arduino.h>
#define SPI_MODE0 0
#define SPI_MODE1 1
#define SPI_MODE2 2
#define SPI_MODE3 3
struct SPISettings { SPISettings(uint32_t = 0, uint8_t = 0, uint8_t = 0) {} };
class SPIClass {
 public:
  void begin(int8_t = -1, int8_t = -1, int8_t = -1, int8_t = -1) {}
  bool pins(int8_t, int8_t, int8_t, int8_t) { return true; }
  void end() {}
  void beginTransaction(SPISettings) {}
  void endTransaction() {}
  uint8_t transfer(uint8_t) { return 0; }
  void transfer(void *, uint32_t) {}
  void writeBytes(const uint8_t *, uint32_t) {}
  void setFrequency(uint32_t) {}
  void setDataMode(uint8_t) {}
  void setBitOrder(uint8_t) {}
  void setHwCs(bool) {}
};
extern SPIClass SPI;
