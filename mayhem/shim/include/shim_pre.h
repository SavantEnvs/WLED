// Force-included before every wled00 translation unit in the host build: libc headers the
// Arduino core pulls in implicitly, and ESP8266 SDK constants.
#pragma once
#include <limits.h>
#include <assert.h>
#include <arpa/inet.h>
#include <time.h>
#include <ctype.h>
#ifndef M_TWOPI
#define M_TWOPI 6.28318530717958647692
#endif
#define GPIO_PIN_COUNT 17
#define SDA 4
#define SCL 5
#define MOSI 13
#define MISO 12
#define SCK 14
#define SS 15
#define RX 3
#define TX 1
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
static inline char *strlwr(char *s) { for (char *p = s; p && *p; p++) *p = tolower((unsigned char)*p); return s; }
static inline char *strupr(char *s) { for (char *p = s; p && *p; p++) *p = toupper((unsigned char)*p); return s; }
