// Host stand-in for ESPAsyncUDP (packets are delivered by harnesses calling the handler directly).
#pragma once
#include <Arduino.h>
#include <functional>
class AsyncUDPPacket : public Print {
 public:
  uint8_t *_data; size_t _len; IPAddress _rip{10, 0, 0, 2};
  AsyncUDPPacket(uint8_t *d, size_t n) : _data(d), _len(n) {}
  uint8_t *data() { return _data; }
  size_t length() { return _len; }
  bool isBroadcast() { return false; }
  bool isMulticast() { return false; }
  IPAddress remoteIP() { return _rip; }
  uint16_t remotePort() { return 5568; }
  uint16_t _lport = 5568;
  uint16_t localPort() { return _lport; }
  IPAddress localIP() { return IPAddress(10, 0, 0, 1); }
  size_t write(uint8_t) override { return 1; }
  size_t write(const uint8_t *, size_t n) override { return n; }
  using Print::write;
};
typedef std::function<void(AsyncUDPPacket &packet)> AuPacketHandlerFunction;
class AsyncUDP : public Print {
 public:
  void onPacket(AuPacketHandlerFunction) {}
  bool listen(uint16_t) { return true; }
  bool listen(const IPAddress &, uint16_t) { return true; }
  bool listenMulticast(const IPAddress &, uint16_t, uint8_t = 1) { return true; }
  size_t writeTo(const uint8_t *, size_t n, const IPAddress &, uint16_t) { return n; }
  size_t broadcastTo(uint8_t *, size_t n, uint16_t) { return n; }
  void close() {}
  size_t write(uint8_t) override { return 1; }
  size_t write(const uint8_t *, size_t n) override { return n; }
  using Print::write;
};
