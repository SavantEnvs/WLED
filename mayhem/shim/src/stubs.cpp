// Link-time stand-ins for code the host build leaves out:
//  - the ESP8266 LED output drivers (UART/I2S-DMA/bit-bang/SPI register access) become RAM-only
//    buses: pixels are encoded into the PixelBus buffer exactly as on the device, show() sends nothing;
//  - wled_server.cpp (needs the npm-generated html_*.h web UI) and ota_update.cpp (OTA disabled).
#include "wled.h"
#include "src/WLEDpixelBus/WLEDpixelBus.h"
#include "src/WLEDpixelBus/WLEDpixelBus_SPI.h"
#include "src/WLEDpixelBus/WLEDpixelBus_ESP8266.h"
#include "src/WLEDpixelBus/WLEDpixelBus_BitBang.h"

namespace WLEDpixelBus {
Esp8266UartBus::Esp8266UartBus(int8_t pin, const LedTiming &timing, uint8_t ledType)
    : _pin(pin), _timing(timing), _initialized(false) { _ledType = ledType; }
Esp8266UartBus::~Esp8266UartBus() {}
bool Esp8266UartBus::begin() { _initialized = true; return allocateEncodeBuffer(_numPixels, _encoder.getBytesPerPixel()); }
void Esp8266UartBus::end() { _initialized = false; }
bool Esp8266UartBus::show() { return true; }
bool Esp8266UartBus::canShow() const { return true; }

Esp8266DmaBus *Esp8266DmaBus::_dmaInstance = nullptr;
Esp8266DmaBus::Esp8266DmaBus(int8_t pin, const LedTiming &timing, uint8_t ledType)
    : _pin(pin), _timing(timing), _initialized(false), _sending(false), _txStartMillis(0) { _ledType = ledType; }
Esp8266DmaBus::~Esp8266DmaBus() {}
bool Esp8266DmaBus::begin() { _initialized = true; return allocateEncodeBuffer(_numPixels, _encoder.getBytesPerPixel()); }
void Esp8266DmaBus::end() { _initialized = false; }
bool Esp8266DmaBus::show() { return true; }
bool Esp8266DmaBus::canShow() const { return true; }
bool Esp8266DmaBus::setPixelColor(uint16_t pos, uint32_t c, uint16_t wwcw) { return PixelBus::setPixelColor(pos, c, wwcw); }
uint32_t Esp8266DmaBus::getPixelColor(uint16_t pix) const { return PixelBus::getPixelColor(pix); }
bool Esp8266DmaBus::allocateEncodeBuffer(uint16_t n, uint8_t bpp) { return PixelBus::allocateEncodeBuffer(n, bpp); }
void Esp8266DmaBus::clearEncodeBuffer() { PixelBus::clearEncodeBuffer(); }
void Esp8266DmaBus::updatePrefix(const uint8_t *d, uint8_t l) { PixelBus::updatePrefix(d, l); }
void Esp8266DmaBus::updateSuffix(const uint8_t *d, uint8_t l) { PixelBus::updateSuffix(d, l); }
void Esp8266DmaBus::scaleEncodeBuffer(uint8_t s) { PixelBus::scaleEncodeBuffer(s); }

BitBangBus::BitBangBus(int8_t pin, const LedTiming &timing, uint8_t ledType)
    : _pin(pin), _timing(timing), _initialized(false) { _ledType = ledType; }
BitBangBus::~BitBangBus() {}
bool BitBangBus::begin() { _initialized = true; return allocateEncodeBuffer(_numPixels, _encoder.getBytesPerPixel()); }
void BitBangBus::end() { _initialized = false; }
bool BitBangBus::show() { return true; }
void BitBangBus::resetChannelTracking() {}

SpiBus::SpiBus(int8_t dataPin, int8_t clockPin, uint16_t, bool useHardware, uint8_t ledType)
    : _dataPin(dataPin), _clockPin(clockPin), _useHardware(useHardware), _initialized(false), _clockHz(0) { _ledType = ledType; }
SpiBus::~SpiBus() {}
bool SpiBus::begin() {
  _initialized = true;
  uint8_t allocBytes = _encoder.getBytesPerPixel();
  if (_ledType == TYPE_APA102 || _ledType == TYPE_P9813) allocBytes++;  // as the real driver
  return allocateEncodeBuffer(_numPixels, allocBytes);
}
void SpiBus::end() { _initialized = false; }
bool SpiBus::show() { return true; }
bool SpiBus::canShow() const { return true; }
}  // namespace WLEDpixelBus

extern "C" void usePWMFixedNMI() {}
void markOTAvalid() {}
void initServer() {}
void serveMessage(AsyncWebServerRequest *request, uint16_t code, const String &, const String &, byte) { if (request) request->send(code); }
void serveJsonError(AsyncWebServerRequest *request, uint16_t code, uint16_t) { if (request) request->send(code); }
