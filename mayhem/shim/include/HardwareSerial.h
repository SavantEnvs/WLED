// Host HardwareSerial: RX is fed from a buffer the harness provides; TX is discarded.
#pragma once
#include "Stream.h"
class HardwareSerial : public Stream {
 public:
  const uint8_t *rx = nullptr; size_t rxLen = 0, rxPos = 0;
  void setRx(const uint8_t *d, size_t n) { rx = d; rxLen = n; rxPos = 0; }
  void begin(unsigned long, int = 0, int = -1, int = -1, bool = false) {}
  void end() {}
  void updateBaudRate(unsigned long) {}
  unsigned long baudRate() { return 115200; }
  void setDebugOutput(bool) {}
  void setRxBufferSize(size_t) {}
  bool isTxEnabled() { return true; }
  bool isRxEnabled() { return true; }
  int available() override { return (int)(rxLen - rxPos); }
  int read() override { return rxPos < rxLen ? rx[rxPos++] : -1; }
  int peek() override { return rxPos < rxLen ? rx[rxPos] : -1; }
  size_t readBytes(char *b, size_t n) override { size_t k = std::min(n, rxLen - rxPos); memcpy(b, rx + rxPos, k); rxPos += k; return k; }
  using Stream::readBytes;
  size_t write(uint8_t) override { return 1; }
  size_t write(const uint8_t *, size_t n) override { return n; }
  int availableForWrite() override { return 128; }
  operator bool() const { return true; }
};
extern HardwareSerial Serial;
extern HardwareSerial Serial1;
