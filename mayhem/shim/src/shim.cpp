// Definitions behind the host Arduino/ESP8266 shim headers.
#include <Arduino.h>
#include <FS.h>
#include <LittleFS.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <Wire.h>
#include <SPI.h>
#include <Updater.h>
#include <Hash.h>
#include <user_interface.h>
#include <espnow.h>

uint32_t shim_millis = 0;
HardwareSerial Serial;
HardwareSerial Serial1;
ESP8266WiFiClass WiFi;
MDNSResponder MDNS;
TwoWire Wire;
SPIClass SPI;
fs::FS LittleFS;
UpdaterClass Update;
EspClass ESP;
uint32_t shim_rtc_user_mem[128];

rst_info *system_get_rst_info() { static rst_info r{}; return &r; }

static uint32_t shim_rng = 1;
void randomSeed(unsigned long seed) { shim_rng = (uint32_t)seed ? (uint32_t)seed : 1; }
uint32_t shim_rand32() {
  // xorshift32: deterministic so an input replays identically.
  uint32_t x = shim_rng;
  x ^= x << 13; x ^= x >> 17; x ^= x << 5;
  return shim_rng = x;
}
long random(long howbig) { return howbig <= 0 ? 0 : (long)(shim_rand32() % (uint32_t)howbig); }
long random(long howsmall, long howbig) { return howsmall >= howbig ? howsmall : howsmall + random(howbig - howsmall); }

static char *u2a(unsigned long v, char *str, int base) {
  char tmp[66]; int i = 0;
  if (base < 2 || base > 36) { str[0] = 0; return str; }
  do { int d = v % base; tmp[i++] = d < 10 ? '0' + d : 'a' + d - 10; v /= base; } while (v);
  int j = 0; while (i) str[j++] = tmp[--i]; str[j] = 0; return str;
}
char *ultoa(unsigned long v, char *s, int b) { return u2a(v, s, b); }
char *utoa(unsigned v, char *s, int b) { return u2a(v, s, b); }
char *ltoa(long v, char *s, int b) {
  if (v < 0 && b == 10) { s[0] = '-'; u2a((unsigned long)(-(v + 1)) + 1, s + 1, b); return s; }
  return u2a((unsigned long)v, s, b);
}
char *itoa(int v, char *s, int b) { return ltoa(v, s, b); }
char *dtostrf(double val, signed char width, unsigned char prec, char *s) { sprintf(s, "%*.*f", width, prec, val); return s; }

String sha1(const String &in) {
  // FNV-1a based 160-bit digest stand-in (the real core uses SHA1); only equality matters to WLED.
  uint64_t h = 1469598103934665603ULL; char out[41]; String r;
  for (int k = 0; k < 3; k++) {
    for (size_t i = 0; i < in.length(); i++) { h ^= (uint8_t)in[i]; h *= 1099511628211ULL; }
    h ^= k; h *= 1099511628211ULL;
    snprintf(out, sizeof(out), "%016llx", (unsigned long long)h); r += out;
  }
  return r.substring(0, 40);
}

size_t IPAddress::printTo(Print &p) const { return p.print(toString()); }
esp_now_recv_cb_t shim_espnow_recv_cb = nullptr;
