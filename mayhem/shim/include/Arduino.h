// Host (Linux) stand-in for the Arduino/ESP8266 core, just enough to compile and run WLED's
// wled00/ sources natively for fuzzing. Hardware calls are no-ops; time is a controllable clock.
#pragma once
#ifndef SHIM_ARDUINO_H
#define SHIM_ARDUINO_H

#include <stdint.h>
#include <stddef.h>
#include <type_traits>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <ctype.h>
#include <strings.h>
#include <string>
#include <algorithm>
#include <functional>
#include <cmath>
#include <type_traits>

typedef bool boolean;
typedef uint8_t byte;
typedef unsigned int word;
static inline uint16_t makeWord(uint16_t w) { return w; }
static inline uint16_t makeWord(uint8_t h, uint8_t l) { return (uint16_t)((h << 8) | l); }
#define word(...) makeWord(__VA_ARGS__)

#define PROGMEM
#define PGM_P const char *
#define PGM_VOID_P const void *
#define PSTR(s) (s)
#define FPSTR(p) ((const __FlashStringHelper *)(p))
#define IRAM_ATTR
#define ICACHE_RAM_ATTR
#define IRAM_ATTR_YN
#define ICACHE_FLASH_ATTR
#define RTC_DATA_ATTR
#define DRAM_ATTR
#define WORD_ALIGNED_ATTR __attribute__((aligned(4)))
#define __STRINGIFY(a) #a

class __FlashStringHelper;
#define F(s) (reinterpret_cast<const __FlashStringHelper *>(s))

#define pgm_read_byte(a) (*(const uint8_t *)(a))
#define pgm_read_byte_near(a) pgm_read_byte(a)
// On the 32-bit target pgm_read_dword() is also used to load pointers from PROGMEM pointer tables;
// on this LP64 host a pointer-typed element is read at full pointer width instead of truncated.
template <class T> static inline auto shim_pgm_read(const T *p, uint32_t (*)(void)) ->
    typename std::conditional<std::is_pointer<T>::value, uintptr_t, uint32_t>::type {
  if constexpr (std::is_pointer<T>::value) { uintptr_t v; memcpy(&v, (const void *)p, sizeof v); return v; }
  else { uint32_t v; memcpy(&v, (const void *)p, 4); return v; }
}
template <class T> static inline auto shim_pgm_read(const T *p, uint16_t (*)(void)) ->
    typename std::conditional<std::is_pointer<T>::value, uintptr_t, uint16_t>::type {
  if constexpr (std::is_pointer<T>::value) { uintptr_t v; memcpy(&v, (const void *)p, sizeof v); return v; }
  else { uint16_t v; memcpy(&v, (const void *)p, 2); return v; }
}
static inline uint32_t shim_pgm_read(const void *p, uint32_t (*)(void)) { uint32_t v; memcpy(&v, p, 4); return v; }
static inline uint16_t shim_pgm_read(const void *p, uint16_t (*)(void)) { uint16_t v; memcpy(&v, p, 2); return v; }
#define pgm_read_word(a) shim_pgm_read((a), (uint16_t (*)(void))nullptr)
#define pgm_read_word_near(a) pgm_read_word(a)
#define pgm_read_dword(a) shim_pgm_read((a), (uint32_t (*)(void))nullptr)
#define pgm_read_dword_near(a) pgm_read_dword(a)
#define pgm_read_float(a) (*(const float *)(a))
#define pgm_read_ptr(a) (*(void *const *)(a))
#define memcpy_P memcpy
#define memcmp_P memcmp
#define strcpy_P strcpy
#define strncpy_P strncpy
#define strcat_P strcat
#define strncat_P strncat
#define strcmp_P strcmp
#define strncmp_P strncmp
#define strcasecmp_P strcasecmp
#define strncasecmp_P strncasecmp
#define strlen_P strlen
#define strnlen_P strnlen
#define strstr_P strstr
#define strchr_P strchr
#define strrchr_P strrchr
#define sprintf_P sprintf
#define snprintf_P snprintf
#define vsnprintf_P vsnprintf
#define sscanf_P sscanf
#define printf_P printf

#define HIGH 1
#define LOW 0
#define INPUT 0x00
#define OUTPUT 0x01
#define INPUT_PULLUP 0x02
#define INPUT_PULLDOWN 0x04
#define INPUT_PULLDOWN_16 0x04
#define OUTPUT_OPEN_DRAIN 0x03
#define CHANGE 3
#define FALLING 2
#define RISING 1
#define LSBFIRST 0
#define MSBFIRST 1
#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2
#define A0 17
#define LED_BUILTIN 2
#define NOT_A_PIN -1

#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif
#define HALF_PI 1.5707963267948966192313216916398
#define TWO_PI 6.283185307179586476925286766559
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105

#define lowByte(w) ((uint8_t)((w) & 0xff))
#define highByte(w) ((uint8_t)((w) >> 8))
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) ((bitvalue) ? bitSet(value, bit) : bitClear(value, bit))
#define bit(b) (1UL << (b))
#define sq(x) ((x) * (x))
#define radians(deg) ((deg) * DEG_TO_RAD)
#define degrees(rad) ((rad) * RAD_TO_DEG)

// Arduino's min/max/constrain accept mixed argument types; results are values, never references.
template <class T, class U> static inline typename std::decay<decltype(true ? T() : U())>::type min(const T &a, const U &b) { return b < a ? b : a; }
template <class T, class U> static inline typename std::decay<decltype(true ? T() : U())>::type max(const T &a, const U &b) { return b > a ? b : a; }
#define _min(a, b) min(a, b)
#define _max(a, b) max(a, b)
#define F_CPU 80000000L
static inline bool can_yield() { return true; }
using std::isnan;
using std::isinf;
template <class T, class L, class H>
static inline typename std::decay<decltype(true ? T() : (true ? L() : H()))>::type constrain(T amt, L low, H high) {
  return amt < low ? low : (amt > high ? high : amt);
}
static inline long map(long x, long in_min, long in_max, long out_min, long out_max) {
  long d = in_max - in_min;
  if (d == 0) return out_min;
  return (x - in_min) * (out_max - out_min) / d + out_min;
}

// ---- time: a controllable clock (harnesses advance it) ----
extern uint32_t shim_millis;
static inline unsigned long millis() { return shim_millis; }
static inline unsigned long micros() { return shim_millis * 1000UL; }
static inline void delay(unsigned long ms) { shim_millis += ms; }
static inline void delayMicroseconds(unsigned int) {}
// Time passes while the firmware yields (it busy-waits on millis() around yield() in places).
static inline void yield() { shim_millis++; }
static inline void optimistic_yield(uint32_t) {}
static inline uint32_t ESP_getCycleCount() { return shim_millis * 80000u; }

// ---- GPIO (no-ops) ----
static inline void pinMode(uint8_t, uint8_t) {}
static inline void digitalWrite(uint8_t, uint8_t) {}
static inline int digitalRead(uint8_t) { return LOW; }
static inline int analogRead(uint8_t) { return 0; }
static inline void analogWrite(uint8_t, int) {}
static inline void analogWriteRange(uint32_t) {}
static inline void analogWriteFreq(uint32_t) {}
static inline void analogWriteResolution(int) {}
static inline void attachInterrupt(uint8_t, void (*)(void), int) {}
static inline void detachInterrupt(uint8_t) {}
static inline uint8_t digitalPinToInterrupt(uint8_t p) { return p; }
static inline void noInterrupts() {}
static inline void interrupts() {}
static inline void tone(uint8_t, unsigned int, unsigned long = 0) {}
static inline void noTone(uint8_t) {}
static inline unsigned long pulseIn(uint8_t, uint8_t, unsigned long = 1000000L) { return 0; }
#define digitalPinToPort(p) (0)
#define digitalPinToBitMask(p) (1)
#define isFlashInterfacePin(p) ((p) >= 6 && (p) <= 11)

// ---- random: deterministic per process (harnesses reseed per input) ----
void randomSeed(unsigned long seed);
long random(long max);
long random(long min, long max);
uint32_t shim_rand32();
static inline uint32_t esp_random() { return shim_rand32(); }
#define RANDOM_REG32 (shim_rand32())

char *itoa(int value, char *str, int base);
char *ltoa(long value, char *str, int base);
char *utoa(unsigned value, char *str, int base);
char *ultoa(unsigned long value, char *str, int base);
char *dtostrf(double val, signed char width, unsigned char prec, char *s);

#include "WString.h"
#include "Print.h"
#include "Stream.h"
#include "IPAddress.h"
#include "HardwareSerial.h"

// ---- ESP class ----
struct rst_info { uint32_t reason, exccause, epc1, epc2, epc3, excvaddr, depc; };
enum FlashMode_t { FM_QIO = 0, FM_QOUT, FM_DIO, FM_DOUT, FM_UNKNOWN = 0xff };
class EspClass {
 public:
  uint32_t getFreeHeap() { return 40000; }
  uint32_t getMaxFreeBlockSize() { return 30000; }
  uint8_t getHeapFragmentation() { return 10; }
  void getHeapStats(uint32_t *f = nullptr, uint32_t *m = nullptr, uint8_t *h = nullptr) {
    if (f) *f = 40000; if (m) *m = 30000; if (h) *h = 10;
  }
  uint32_t getFlashChipSize() { return 4 * 1024 * 1024; }
  uint32_t getFlashChipRealSize() { return 4 * 1024 * 1024; }
  uint32_t getFlashChipSpeed() { return 40000000; }
  FlashMode_t getFlashChipMode() { return FM_DIO; }
  uint32_t getFlashChipId() { return 0x1640ef; }
  uint8_t getFlashChipVendorId() { return 0xef; }
  uint32_t getChipId() { return 0x123456; }
  uint8_t getCpuFreqMHz() { return 80; }
  const char *getSdkVersion() { return "host"; }
  String getCoreVersion() { return String("host"); }
  String getResetReason() { return String("host"); }
  String getResetInfo() { return String("host"); }
  rst_info *getResetInfoPtr() { static rst_info r{}; return &r; }
  uint32_t getFreeSketchSpace() { return 1000000; }
  uint32_t getSketchSize() { return 500000; }
  uint32_t getCycleCount() { return ESP_getCycleCount(); }
  uint32_t random() { return shim_rand32(); }
  void restart() {}
  void reset() {}
  void wdtFeed() {}
  void wdtEnable(uint32_t = 0) {}
  void wdtDisable() {}
  void deepSleep(uint64_t, int = 0) {}
  bool flashEraseSector(uint32_t) { return false; }
  bool flashWrite(uint32_t, const uint32_t *, size_t) { return false; }
  bool flashRead(uint32_t, uint32_t *, size_t) { return false; }
  bool checkFlashCRC() { return true; }
  String getSketchMD5() { return String("0"); }
};
extern EspClass ESP;

static inline void *heap_caps_malloc(size_t s, uint32_t) { return malloc(s); }

void setup();
void loop();

#endif
