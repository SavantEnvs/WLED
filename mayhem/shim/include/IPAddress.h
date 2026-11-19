// Host Arduino IPAddress (IPv4).
#pragma once
#include <stdint.h>
#include "WString.h"
class IPAddress {
 public:
  union { uint8_t bytes[4]; uint32_t dword; } a;
  IPAddress() { a.dword = 0; }
  IPAddress(uint8_t o1, uint8_t o2, uint8_t o3, uint8_t o4) { a.bytes[0] = o1; a.bytes[1] = o2; a.bytes[2] = o3; a.bytes[3] = o4; }
  IPAddress(uint32_t v) { a.dword = v; }
  IPAddress(const uint8_t *p) { for (int i = 0; i < 4; i++) a.bytes[i] = p[i]; }
  operator uint32_t() const { return a.dword; }
  bool operator==(const IPAddress &o) const { return a.dword == o.a.dword; }
  bool operator!=(const IPAddress &o) const { return a.dword != o.a.dword; }
  bool operator==(uint32_t v) const { return a.dword == v; }
  bool operator==(const uint8_t *p) const { return memcmp(a.bytes, p, 4) == 0; }
  uint8_t operator[](int i) const { return a.bytes[i & 3]; }
  uint8_t &operator[](int i) { return a.bytes[i & 3]; }
  IPAddress &operator=(uint32_t v) { a.dword = v; return *this; }
  IPAddress &operator=(const uint8_t *p) { for (int i = 0; i < 4; i++) a.bytes[i] = p[i]; return *this; }
  bool isSet() const { return a.dword != 0; }
  bool isV4() const { return true; }
  bool fromString(const char *s) {
    unsigned v[4]; char tail;
    if (!s || sscanf(s, "%u.%u.%u.%u%c", &v[0], &v[1], &v[2], &v[3], &tail) != 4) return false;
    for (int i = 0; i < 4; i++) { if (v[i] > 255) return false; a.bytes[i] = v[i]; }
    return true;
  }
  bool fromString(const String &s) { return fromString(s.c_str()); }
  String toString() const { char b[16]; snprintf(b, sizeof(b), "%u.%u.%u.%u", a.bytes[0], a.bytes[1], a.bytes[2], a.bytes[3]); return String(b); }
  size_t printTo(class Print &p) const;
};
#define INADDR_NONE IPAddress(0, 0, 0, 0)
#define IPADDR_NONE ((uint32_t)0xffffffffUL)
#define IPADDR_ANY ((uint32_t)0)
#define IP_ADDR_ANY IPAddress(0, 0, 0, 0)
