// Host WiFiUDP: each socket returns the packet the harness queued (one per parsePacket()); sends are discarded.
#pragma once
#include <Arduino.h>
#include <vector>
#include <deque>
class UDP : public Stream {};
class WiFiUDP : public UDP {
 public:
  std::deque<std::vector<uint8_t>> queue;
  std::vector<uint8_t> cur; size_t pos = 0;
  IPAddress rIP{10, 0, 0, 2}; uint16_t rPort = 21324;
  void inject(const uint8_t *d, size_t n) { queue.emplace_back(d, d + n); }
  void reset() { queue.clear(); cur.clear(); pos = 0; }
  uint8_t begin(uint16_t) { return 1; }
  uint8_t beginMulticast(IPAddress, IPAddress, uint16_t) { return 1; }
  uint8_t beginMulticast(IPAddress, uint16_t) { return 1; }
  void stop() {}
  int beginPacket(IPAddress, uint16_t) { return 1; }
  int beginPacket(const char *, uint16_t) { return 1; }
  int beginPacketMulticast(IPAddress, uint16_t, IPAddress, int = 1) { return 1; }
  int endPacket() { return 1; }
  size_t write(uint8_t) override { return 1; }
  size_t write(const uint8_t *, size_t n) override { return n; }
  using Print::write;
  int parsePacket() {
    if (queue.empty()) { cur.clear(); pos = 0; return 0; }
    cur = std::move(queue.front()); queue.pop_front(); pos = 0; return (int)cur.size();
  }
  int available() override { return (int)(cur.size() - pos); }
  int read() override { return pos < cur.size() ? cur[pos++] : -1; }
  int read(unsigned char *b, size_t n) { size_t k = std::min(n, cur.size() - pos); memcpy(b, cur.data() + pos, k); pos += k; return (int)k; }
  int read(char *b, size_t n) { return read((unsigned char *)b, n); }
  int peek() override { return pos < cur.size() ? cur[pos] : -1; }
  void flush() override { cur.clear(); pos = 0; }
  IPAddress remoteIP() { return rIP; }
  uint16_t remotePort() { return rPort; }
  IPAddress destinationIP() { return IPAddress(10, 0, 0, 1); }
  uint16_t localPort() { return 21324; }
};
