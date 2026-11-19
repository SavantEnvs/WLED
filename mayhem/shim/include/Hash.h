// Host stand-in for the ESP8266 core's sha1() (only used for the settings PIN hash). Not a real SHA1.
#pragma once
#include <Arduino.h>
String sha1(const String &in);
