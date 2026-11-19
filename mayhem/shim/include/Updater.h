// Host stand-in for the ESP8266 Updater (OTA is not exercised on host).
#pragma once
#include <Arduino.h>
#define U_FLASH 0
#define U_FS 100
class UpdaterClass {
 public:
  bool begin(size_t, int = U_FLASH, int = -1, uint8_t = 0) { return false; }
  size_t write(uint8_t *, size_t) { return 0; }
  bool end(bool = false) { return false; }
  bool isRunning() { return false; }
  bool hasError() { return false; }
  bool isFinished() { return false; }
  uint8_t getError() { return 0; }
  void printError(Print &) {}
  String getErrorString() { return String(); }
  void runAsync(bool) {}
  bool canRollBack() { return false; }
  bool rollBack() { return false; }
  void abort() {}
};
extern UpdaterClass Update;
