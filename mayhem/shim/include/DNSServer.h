#pragma once
#include <Arduino.h>
enum class DNSReplyCode { NoError = 0, FormError, ServerFailure, NonExistentDomain, NotImplemented, Refused };
class DNSServer {
 public:
  bool start(uint16_t, const String &, const IPAddress &) { return true; }
  void stop() {}
  void processNextRequest() {}
  void setErrorReplyCode(DNSReplyCode) {}
  void setTTL(uint32_t) {}
};
