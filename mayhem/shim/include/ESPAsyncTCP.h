// Host stand-in for ESPAsyncTCP's AsyncClient (no network; connect() always fails).
#pragma once
#include <Arduino.h>
#include <functional>
class AsyncClient;
typedef std::function<void(void *, AsyncClient *)> AcConnectHandler;
typedef std::function<void(void *, AsyncClient *, size_t, uint32_t)> AcAckHandler;
typedef std::function<void(void *, AsyncClient *, int8_t)> AcErrorHandler;
typedef std::function<void(void *, AsyncClient *, void *, size_t)> AcDataHandler;
typedef std::function<void(void *, AsyncClient *, uint32_t)> AcTimeoutHandler;
class AsyncClient {
 public:
  AsyncClient(void * = nullptr) {}
  virtual ~AsyncClient() {}
  bool connect(IPAddress, uint16_t) { return false; }
  bool connect(const char *, uint16_t) { return false; }
  void close(bool = false) {}
  void stop() {}
  bool connected() { return false; }
  bool connecting() { return false; }
  bool disconnected() { return true; }
  bool freeable() { return true; }
  size_t space() { return 1460; }
  size_t add(const char *, size_t n, uint8_t = 0) { return n; }
  bool send() { return true; }
  size_t write(const char *d) { return d ? strlen(d) : 0; }
  size_t write(const char *, size_t n, uint8_t = 0) { return n; }
  bool canSend() { return true; }
  void setRxTimeout(uint32_t) {}
  void setAckTimeout(uint32_t) {}
  void setNoDelay(bool) {}
  IPAddress remoteIP() { return IPAddress(10, 0, 0, 2); }
  uint16_t remotePort() { return 80; }
  IPAddress localIP() { return IPAddress(10, 0, 0, 1); }
  void onConnect(AcConnectHandler, void * = nullptr) {}
  void onDisconnect(AcConnectHandler, void * = nullptr) {}
  void onAck(AcAckHandler, void * = nullptr) {}
  void onError(AcErrorHandler, void * = nullptr) {}
  void onData(AcDataHandler, void * = nullptr) {}
  void onTimeout(AcTimeoutHandler, void * = nullptr) {}
  void onPoll(AcConnectHandler, void * = nullptr) {}
};
