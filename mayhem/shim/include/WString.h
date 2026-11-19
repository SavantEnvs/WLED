// Host Arduino String (subset of the ESP8266 core API), backed by std::string.
#pragma once
#include <string>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <stdint.h>

class __FlashStringHelper;
class StringSumHelper;

class String {
 public:
  std::string s;
  String() {}
  String(const char *c) { if (c) s = c; }
  String(const char *c, size_t n) { if (c) s.assign(c, n); }
  String(const __FlashStringHelper *f) { if (f) s = reinterpret_cast<const char *>(f); }
  String(const std::string &o) : s(o) {}
  String(const String &o) = default;
  String(String &&o) = default;
  explicit String(char c) { s.assign(1, c); }
  explicit String(unsigned char v, unsigned char base = 10) { num((unsigned long)v, base); }
  explicit String(int v, unsigned char base = 10) { snum((long)v, base); }
  explicit String(unsigned int v, unsigned char base = 10) { num((unsigned long)v, base); }
  explicit String(long v, unsigned char base = 10) { snum(v, base); }
  explicit String(unsigned long v, unsigned char base = 10) { num(v, base); }
  explicit String(long long v, unsigned char base = 10) { snum((long)v, base); }
  explicit String(unsigned long long v, unsigned char base = 10) { num((unsigned long)v, base); }
  explicit String(float v, unsigned char dp = 2) { dbl(v, dp); }
  explicit String(double v, unsigned char dp = 2) { dbl(v, dp); }
  String &operator=(const String &o) = default;
  String &operator=(String &&o) = default;
  String &operator=(const char *c) { if (c) s = c; else s.clear(); return *this; }
  String &operator=(const __FlashStringHelper *f) { return *this = reinterpret_cast<const char *>(f); }

  unsigned int length() const { return s.size(); }
  bool isEmpty() const { return s.empty(); }
  const char *c_str() const { return s.c_str(); }
  char *begin() { return &s[0]; }
  char *end() { return &s[0] + s.size(); }
  const char *begin() const { return s.c_str(); }
  const char *end() const { return s.c_str() + s.size(); }
  bool reserve(unsigned int n) { s.reserve(n); return true; }
  void clear() { s.clear(); }
  explicit operator bool() const { return true; }

  bool concat(const String &o) { s += o.s; return true; }
  bool concat(const char *c) { if (c) s += c; return true; }
  bool concat(const char *c, unsigned int n) { if (c) s.append(c, n); return true; }
  bool concat(const __FlashStringHelper *f) { return concat(reinterpret_cast<const char *>(f)); }
  bool concat(char c) { s += c; return true; }
  bool concat(unsigned char v) { return concat(String(v)); }
  bool concat(int v) { return concat(String(v)); }
  bool concat(unsigned int v) { return concat(String(v)); }
  bool concat(long v) { return concat(String(v)); }
  bool concat(unsigned long v) { return concat(String(v)); }
  bool concat(long long v) { return concat(String(v)); }
  bool concat(unsigned long long v) { return concat(String(v)); }
  bool concat(float v) { return concat(String(v)); }
  bool concat(double v) { return concat(String(v)); }
  template <typename T> String &operator+=(const T &v) { concat(v); return *this; }

  friend StringSumHelper operator+(const StringSumHelper &l, const String &r);

  int compareTo(const String &o) const { return strcmp(c_str(), o.c_str()); }
  bool equals(const String &o) const { return s == o.s; }
  bool equals(const char *c) const { return c && s == c; }
  bool equalsIgnoreCase(const String &o) const { return strcasecmp(c_str(), o.c_str()) == 0; }
  bool operator==(const String &o) const { return s == o.s; }
  bool operator==(const char *c) const { return equals(c); }
  bool operator!=(const String &o) const { return s != o.s; }
  bool operator!=(const char *c) const { return !equals(c); }
  bool operator<(const String &o) const { return s < o.s; }
  bool operator>(const String &o) const { return s > o.s; }
  bool startsWith(const String &p) const { return s.compare(0, p.s.size(), p.s) == 0 && p.s.size() <= s.size(); }
  bool startsWith(const String &p, unsigned int off) const { return off <= s.size() && s.compare(off, p.s.size(), p.s) == 0 && off + p.s.size() <= s.size(); }
  bool endsWith(const String &p) const { return p.s.size() <= s.size() && s.compare(s.size() - p.s.size(), p.s.size(), p.s) == 0; }

  char charAt(unsigned int i) const { return i < s.size() ? s[i] : 0; }
  void setCharAt(unsigned int i, char c) { if (i < s.size()) s[i] = c; }
  char operator[](unsigned int i) const { return charAt(i); }
  char &operator[](unsigned int i) { static char dummy; if (i >= s.size()) { dummy = 0; return dummy; } return s[i]; }
  void getBytes(unsigned char *buf, unsigned int n, unsigned int idx = 0) const {
    if (!n || !buf) return;
    if (idx >= s.size()) { buf[0] = 0; return; }
    unsigned int k = std::min<size_t>(n - 1, s.size() - idx);
    memcpy(buf, s.c_str() + idx, k); buf[k] = 0;
  }
  void toCharArray(char *buf, unsigned int n, unsigned int idx = 0) const { getBytes((unsigned char *)buf, n, idx); }

  int indexOf(char c) const { return indexOf(c, 0); }
  int indexOf(char c, unsigned int from) const { if (from >= s.size()) return -1; auto p = s.find(c, from); return p == std::string::npos ? -1 : (int)p; }
  int indexOf(const String &o) const { return indexOf(o, 0); }
  int indexOf(const String &o, unsigned int from) const { if (from >= s.size()) return -1; auto p = s.find(o.s, from); return p == std::string::npos ? -1 : (int)p; }
  int indexOf(const char *c, unsigned int from = 0) const { return indexOf(String(c), from); }
  int indexOf(const __FlashStringHelper *c, unsigned int from = 0) const { return indexOf(String(c), from); }
  int lastIndexOf(char c) const { auto p = s.rfind(c); return p == std::string::npos ? -1 : (int)p; }
  int lastIndexOf(char c, unsigned int from) const { auto p = s.rfind(c, from); return p == std::string::npos ? -1 : (int)p; }
  int lastIndexOf(const String &o) const { auto p = s.rfind(o.s); return p == std::string::npos ? -1 : (int)p; }
  String substring(unsigned int from) const { return from >= s.size() ? String() : String(s.substr(from)); }
  String substring(unsigned int from, unsigned int to) const {
    if (from > to) std::swap(from, to);
    if (from >= s.size()) return String();
    if (to > s.size()) to = s.size();
    return String(s.substr(from, to - from));
  }
  void replace(char a, char b) { for (auto &c : s) if (c == a) c = b; }
  void replace(const String &a, const String &b) {
    if (a.s.empty()) return;
    size_t p = 0;
    while ((p = s.find(a.s, p)) != std::string::npos) { s.replace(p, a.s.size(), b.s); p += b.s.size(); }
  }
  void remove(unsigned int idx) { if (idx < s.size()) s.erase(idx); }
  void remove(unsigned int idx, unsigned int n) { if (idx < s.size()) s.erase(idx, n); }
  void toLowerCase() { for (auto &c : s) c = tolower((unsigned char)c); }
  void toUpperCase() { for (auto &c : s) c = toupper((unsigned char)c); }
  void trim() {
    size_t b = 0, e = s.size();
    while (b < e && isspace((unsigned char)s[b])) b++;
    while (e > b && isspace((unsigned char)s[e - 1])) e--;
    s = s.substr(b, e - b);
  }
  long toInt() const { return atol(c_str()); }
  float toFloat() const { return (float)atof(c_str()); }
  double toDouble() const { return atof(c_str()); }

 private:
  void num(unsigned long v, unsigned char base) {
    char buf[70]; char *p = buf + sizeof(buf) - 1; *p = 0;
    if (base < 2) base = 10;
    do { unsigned d = v % base; *--p = d < 10 ? '0' + d : 'a' + d - 10; v /= base; } while (v);
    s = p;
  }
  void snum(long v, unsigned char base) {
    if (base == 10 && v < 0) { num((unsigned long)(-(v + 1)) + 1, 10); s.insert(s.begin(), '-'); }
    else num((unsigned long)v, base);
  }
  void dbl(double v, unsigned char dp) { char buf[64]; snprintf(buf, sizeof(buf), "%.*f", dp, v); s = buf; }
};

class StringSumHelper : public String {
 public:
  StringSumHelper(const String &o) : String(o) {}
  StringSumHelper(const char *c) : String(c) {}
};
inline StringSumHelper operator+(const StringSumHelper &l, const String &r) { StringSumHelper t(l); t.s += r.s; return t; }
inline StringSumHelper operator+(const String &l, const String &r) { StringSumHelper t(l); t.s += r.s; return t; }
inline StringSumHelper operator+(const String &l, const char *r) { StringSumHelper t(l); if (r) t.s += r; return t; }
inline StringSumHelper operator+(const char *l, const String &r) { StringSumHelper t(l); t.s += r.s; return t; }
inline StringSumHelper operator+(const String &l, const __FlashStringHelper *r) { return l + reinterpret_cast<const char *>(r); }
inline StringSumHelper operator+(const String &l, char c) { StringSumHelper t(l); t.s += c; return t; }
inline StringSumHelper operator+(const String &l, int v) { return l + String(v); }
inline StringSumHelper operator+(const String &l, unsigned v) { return l + String(v); }
inline StringSumHelper operator+(const String &l, long v) { return l + String(v); }
inline StringSumHelper operator+(const String &l, unsigned long v) { return l + String(v); }
inline StringSumHelper operator+(const String &l, unsigned char v) { return l + String(v); }
inline StringSumHelper operator+(const String &l, float v) { return l + String(v); }
inline StringSumHelper operator+(const String &l, double v) { return l + String(v); }
inline bool operator==(const char *c, const String &s) { return s == c; }

#define emptyString String()
