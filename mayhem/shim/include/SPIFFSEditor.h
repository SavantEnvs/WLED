#pragma once
#define SPIFFS_EDITOR_AIRCOOOKIE
#include <ESPAsyncWebServer.h>
class SPIFFSEditor : public AsyncWebHandler {
 public:
  SPIFFSEditor(const fs::FS &, const String & = String(), const String & = String()) {}
};
