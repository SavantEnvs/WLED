#pragma once
#include <Arduino.h>
class TwoWire : public Stream {
 public:
  bool begin(int = -1, int = -1, uint32_t = 0) { return true; }
  void setClock(uint32_t) {}
  void beginTransmission(uint8_t) {}
  uint8_t endTransmission(bool = true) { return 2; }
  uint8_t requestFrom(uint8_t, uint8_t, bool = true) { return 0; }
  size_t write(uint8_t) override { return 1; }
  size_t write(const uint8_t *, size_t n) override { return n; }
  using Print::write;
  int available() override { return 0; }
  int read() override { return -1; }
  int peek() override { return -1; }
};
extern TwoWire Wire;
