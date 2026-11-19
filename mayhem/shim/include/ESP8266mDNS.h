#pragma once
#include <Arduino.h>
class MDNSResponder {
 public:
  bool begin(const char *) { return true; }
  bool begin(const String &) { return true; }
  void close() {}
  void end() {}
  void update() {}
  void addService(const char *, const char *, uint16_t) {}
  void addServiceTxt(const char *, const char *, const char *, const char *) {}
  int queryService(const char *, const char *) { return 0; }
  IPAddress IP(int) { return IPAddress(); }
  String hostname(int) { return String(); }
};
extern MDNSResponder MDNS;
