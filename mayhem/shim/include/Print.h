// Host Arduino Print.
#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "WString.h"
class Printable;
class Print {
 public:
  virtual ~Print() {}
  virtual size_t write(uint8_t) = 0;
  virtual size_t write(const uint8_t *b, size_t n) { size_t k = 0; while (n--) k += write(*b++); return k; }
  size_t write(const char *s) { return s ? write((const uint8_t *)s, strlen(s)) : 0; }
  size_t write(const char *b, size_t n) { return write((const uint8_t *)b, n); }
  size_t write(int n) { return write((uint8_t)n); }
  size_t write(unsigned int n) { return write((uint8_t)n); }
  size_t write(long n) { return write((uint8_t)n); }
  size_t write(unsigned long n) { return write((uint8_t)n); }
  size_t write(char c) { return write((uint8_t)c); }
  virtual int availableForWrite() { return 0; }
  virtual void flush() {}
  size_t print(const __FlashStringHelper *f) { return write(reinterpret_cast<const char *>(f)); }
  size_t print(const String &s) { return write((const uint8_t *)s.c_str(), s.length()); }
  size_t print(const char *s) { return write(s); }
  size_t print(char c) { return write((uint8_t)c); }
  size_t print(unsigned char v, int b = 10) { return print(String(v, (unsigned char)b)); }
  size_t print(int v, int b = 10) { return print(String(v, (unsigned char)b)); }
  size_t print(unsigned v, int b = 10) { return print(String(v, (unsigned char)b)); }
  size_t print(long v, int b = 10) { return print(String(v, (unsigned char)b)); }
  size_t print(unsigned long v, int b = 10) { return print(String(v, (unsigned char)b)); }
  size_t print(long long v, int b = 10) { return print(String(v, (unsigned char)b)); }
  size_t print(unsigned long long v, int b = 10) { return print(String(v, (unsigned char)b)); }
  size_t print(double v, int d = 2) { return print(String(v, (unsigned char)d)); }
  size_t print(const Printable &);
  template <typename T> size_t println(const T &v) { return print(v) + println(); }
  template <typename T> size_t println(const T &v, int b) { return print(v, b) + println(); }
  size_t println() { return write("\r\n"); }
  size_t printf(const char *fmt, ...) __attribute__((format(printf, 2, 3))) {
    char buf[256]; va_list ap; va_start(ap, fmt); int n = vsnprintf(buf, sizeof(buf), fmt, ap); va_end(ap);
    if (n < 0) return 0; if ((size_t)n >= sizeof(buf)) n = sizeof(buf) - 1; return write((const uint8_t *)buf, n);
  }
};
class Printable { public: virtual ~Printable() {} virtual size_t printTo(Print &p) const = 0; };
inline size_t Print::print(const Printable &x) { return x.printTo(*this); }
