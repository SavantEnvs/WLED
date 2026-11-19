// Host stand-in for the ESP8266 WiFi stack: a station that is "connected" with a fixed IP.
#pragma once
#include <Arduino.h>
#include <functional>
#include <memory>
#include "WiFiUdp.h"
#include "user_interface.h"
typedef enum { WL_NO_SHIELD = 255, WL_IDLE_STATUS = 0, WL_NO_SSID_AVAIL, WL_SCAN_COMPLETED, WL_CONNECTED, WL_CONNECT_FAILED, WL_CONNECTION_LOST, WL_WRONG_PASSWORD, WL_DISCONNECTED } wl_status_t;
typedef enum { WIFI_OFF = 0, WIFI_STA = 1, WIFI_AP = 2, WIFI_AP_STA = 3 } WiFiMode_t;
#define WIFI_MODE_NULL WIFI_OFF
typedef enum { WIFI_PHY_MODE_11B = 1, WIFI_PHY_MODE_11G = 2, WIFI_PHY_MODE_11N = 3 } WiFiPhyMode_t;
typedef enum { WIFI_NONE_SLEEP = 0, WIFI_LIGHT_SLEEP = 1, WIFI_MODEM_SLEEP = 2 } WiFiSleepType_t;
enum wl_enc_type { ENC_TYPE_WEP = 5, ENC_TYPE_TKIP = 2, ENC_TYPE_CCMP = 4, ENC_TYPE_NONE = 7, ENC_TYPE_AUTO = 8 };
#define WIFI_SCAN_RUNNING (-1)
#define WIFI_SCAN_FAILED (-2)
enum WiFiDisconnectReason { WIFI_DISCONNECT_REASON_UNSPECIFIED = 1 };
struct WiFiEventStationModeConnected {};
struct WiFiEventStationModeDisconnected { WiFiDisconnectReason reason; };
struct WiFiEventHandlerOpaque {};
typedef std::shared_ptr<WiFiEventHandlerOpaque> WiFiEventHandler;
typedef int WiFiEvent_t;
enum { WIFI_EVENT_STAMODE_CONNECTED = 0, WIFI_EVENT_STAMODE_DISCONNECTED, WIFI_EVENT_STAMODE_AUTHMODE_CHANGE, WIFI_EVENT_STAMODE_GOT_IP, WIFI_EVENT_STAMODE_DHCP_TIMEOUT, WIFI_EVENT_SOFTAPMODE_STACONNECTED, WIFI_EVENT_SOFTAPMODE_STADISCONNECTED, WIFI_EVENT_SOFTAPMODE_PROBEREQRECVED };
static inline uint8_t wifi_softap_get_station_num() { return 0; }

class WiFiClient : public Stream {
 public:
  int connect(IPAddress, uint16_t) { return 0; }
  int connect(const char *, uint16_t) { return 0; }
  size_t write(uint8_t) override { return 1; }
  size_t write(const uint8_t *, size_t n) override { return n; }
  using Print::write;
  int available() override { return 0; }
  int read() override { return -1; }
  int peek() override { return -1; }
  void stop() {}
  uint8_t connected() { return 0; }
  operator bool() { return false; }
  IPAddress remoteIP() { return IPAddress(); }
};
class WiFiServer {
 public:
  WiFiServer(uint16_t) {}
  void begin() {}
  WiFiClient available() { return WiFiClient(); }
};

class ESP8266WiFiClass {
 public:
  IPAddress ip{10, 0, 0, 1}, mask{255, 255, 255, 0}, gw{10, 0, 0, 254};
  wl_status_t status() { return WL_CONNECTED; }
  bool isConnected() { return true; }
  IPAddress localIP() { return ip; }
  IPAddress subnetMask() { return mask; }
  IPAddress gatewayIP() { return gw; }
  IPAddress dnsIP(uint8_t = 0) { return gw; }
  IPAddress softAPIP() { return IPAddress(4, 3, 2, 1); }
  IPAddress broadcastIP() { return IPAddress(10, 0, 0, 255); }
  String macAddress() { return String("02:00:00:00:00:01"); }
  uint8_t *macAddress(uint8_t *m) { static const uint8_t mm[6] = {2, 0, 0, 0, 0, 1}; memcpy(m, mm, 6); return m; }
  String softAPmacAddress() { return macAddress(); }
  uint8_t *softAPmacAddress(uint8_t *m) { return macAddress(m); }
  String SSID(uint8_t = 0) { return String("host"); }
  String BSSIDstr(uint8_t = 0) { return String("02:00:00:00:00:02"); }
  uint8_t *BSSID(uint8_t = 0) { static uint8_t b[6] = {2, 0, 0, 0, 0, 2}; return b; }
  int32_t RSSI(uint8_t = 0) { return -50; }
  int32_t channel(uint8_t = 0) { return 1; }
  uint8_t encryptionType(uint8_t = 0) { return ENC_TYPE_CCMP; }
  bool mode(WiFiMode_t) { return true; }
  WiFiMode_t getMode() { return WIFI_STA; }
  bool persistent(bool) { return true; }
  bool setAutoConnect(bool) { return true; }
  bool setAutoReconnect(bool) { return true; }
  bool setSleepMode(WiFiSleepType_t, uint8_t = 0) { return true; }
  bool setSleep(bool) { return true; }
  bool setPhyMode(WiFiPhyMode_t) { return true; }
  void setOutputPower(float) {}
  bool hostname(const char *) { return true; }
  bool hostname(const String &) { return true; }
  String hostname() { return String("wled"); }
  wl_status_t begin(const char * = nullptr, const char * = nullptr, int32_t = 0, const uint8_t * = nullptr, bool = true) { return WL_CONNECTED; }
  bool config(IPAddress, IPAddress, IPAddress, IPAddress = IPAddress(), IPAddress = IPAddress()) { return true; }
  bool disconnect(bool = false) { return true; }
  bool softAP(const char *, const char * = nullptr, int = 1, int = 0, int = 4) { return true; }
  bool softAPConfig(IPAddress, IPAddress, IPAddress) { return true; }
  bool softAPdisconnect(bool = false) { return true; }
  uint8_t softAPgetStationNum() { return 0; }
  int8_t scanNetworks(bool = false, bool = false, uint8_t = 0, uint8_t * = nullptr) { return 0; }
  int8_t scanComplete() { return 0; }
  void scanDelete() {}
  int hostByName(const char *, IPAddress &r) { r = IPAddress(); return 0; }
  void onEvent(void (*)(WiFiEvent_t)) {}
  WiFiEventHandler onStationModeConnected(std::function<void(const WiFiEventStationModeConnected &)>) { return nullptr; }
  WiFiEventHandler onStationModeDisconnected(std::function<void(const WiFiEventStationModeDisconnected &)>) { return nullptr; }
};
extern ESP8266WiFiClass WiFi;
