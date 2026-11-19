// Host Arduino Stream.
#pragma once
#include "Print.h"
class Stream : public Print {
 public:
  virtual int available() = 0;
  virtual int read() = 0;
  virtual int peek() = 0;
  void setTimeout(unsigned long) {}
  unsigned long getTimeout() { return 0; }
  virtual size_t readBytes(char *b, size_t n) { size_t k = 0; while (k < n) { int c = read(); if (c < 0) break; b[k++] = (char)c; } return k; }
  size_t readBytes(uint8_t *b, size_t n) { return readBytes((char *)b, n); }
  size_t readBytesUntil(char t, char *b, size_t n) { size_t k = 0; while (k < n) { int c = read(); if (c < 0 || c == t) break; b[k++] = (char)c; } return k; }
  String readStringUntil(char t) { String r; for (;;) { int c = read(); if (c < 0 || c == t) break; r += (char)c; } return r; }
  String readString() { String r; for (;;) { int c = read(); if (c < 0) break; r += (char)c; } return r; }
  bool find(const char *) { return false; }
  long parseInt() { return 0; }
};
