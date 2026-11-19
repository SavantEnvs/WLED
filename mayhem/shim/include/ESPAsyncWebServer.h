// Host stand-in for ESPAsyncWebServer: requests carry a parameter list the harness fills; responses
// are discarded. Only what WLED's wled00/ uses.
#pragma once
#include <Arduino.h>
#include <vector>
#include <list>
#include <functional>
#include "FS.h"
#include "ESPAsyncTCP.h"

static const char CONTENT_TYPE_JSON[] PROGMEM = "application/json";
static const char CONTENT_TYPE_PLAIN[] PROGMEM = "text/plain";
static const char CONTENT_TYPE_HTML[] PROGMEM = "text/html";
static const char CONTENT_TYPE_JAVASCRIPT[] PROGMEM = "application/javascript";
static const char CONTENT_TYPE_CSS[] PROGMEM = "text/css";
static const char CONTENT_TYPE_XML[] PROGMEM = "text/xml";
static const char CONTENT_TYPE_GZIP[] PROGMEM = "application/x-gzip";
static const char CONTENT_TYPE_OCTET_STREAM[] PROGMEM = "application/octet-stream";
static const char CONTENT_TYPE_EVENT_STREAM[] PROGMEM = "text/event-stream";
static const char CONTENT_TYPE_MSGPACK[] PROGMEM = "application/msgpack";
enum WebRequestMethod { HTTP_GET = 1, HTTP_POST = 2, HTTP_DELETE = 4, HTTP_PUT = 8, HTTP_PATCH = 16, HTTP_HEAD = 32, HTTP_OPTIONS = 64, HTTP_ANY = 127 };
typedef uint8_t WebRequestMethodComposite;

class AsyncWebParameter {
  String _name, _value; size_t _size; bool _isForm, _isFile;
 public:
  AsyncWebParameter(const String &n, const String &v, bool form = false, bool file = false, size_t size = 0)
      : _name(n), _value(v), _size(size), _isForm(form), _isFile(file) {}
  const String &name() const { return _name; }
  const String &value() const { return _value; }
  size_t size() const { return _size; }
  bool isPost() const { return _isForm; }
  bool isFile() const { return _isFile; }
};
class AsyncWebHeader {
  String _n, _v;
 public:
  AsyncWebHeader(const String &n, const String &v) : _n(n), _v(v) {}
  const String &name() const { return _n; }
  const String &value() const { return _v; }
  String toString() const { return _n + ": " + _v; }
};

class AsyncWebServerResponse {
 public:
  int _code = 200; String _contentType; size_t _contentLength = 0; size_t _sentLength = 0;
  virtual ~AsyncWebServerResponse() {}
  void addHeader(const String &, const String &, bool = true) {}
  void setCode(int c) { _code = c; }
  void setContentLength(size_t l) { _contentLength = l; }
  void setContentType(const String &t) { _contentType = t; }
};
class AsyncAbstractResponse : public AsyncWebServerResponse {
 public:
  virtual bool _sourceValid() const { return false; }
  virtual size_t _fillBuffer(uint8_t *, size_t) { return 0; }
};
class AsyncBasicResponse : public AsyncWebServerResponse {};
class AsyncResponseStream : public AsyncAbstractResponse, public Print {
 public:
  String body;
  size_t write(uint8_t c) override { body += (char)c; return 1; }
  size_t write(const uint8_t *b, size_t n) override { body.concat((const char *)b, n); return n; }
  using Print::write;
};

class AsyncWebServerRequest;
typedef std::function<void(AsyncWebServerRequest *)> ArRequestHandlerFunction;
typedef std::function<void(AsyncWebServerRequest *, const String &, size_t, uint8_t *, size_t, bool)> ArUploadHandlerFunction;
typedef std::function<void(AsyncWebServerRequest *, uint8_t *, size_t, size_t, size_t)> ArBodyHandlerFunction;
typedef std::function<void(void)> ArDisconnectHandler;
typedef std::function<size_t(uint8_t *, size_t, size_t)> AwsResponseFiller;
typedef std::function<String(const String &)> AwsTemplateProcessor;

class AsyncWebServerRequest {
 public:
  void *_tempObject = nullptr;
  File _tempFile;
  AsyncClient *_client = nullptr;
  std::vector<AsyncWebParameter> params;
  std::vector<AsyncWebHeader> headers;
  String _url = "/";
  WebRequestMethodComposite _method = HTTP_GET;
  int sentCode = 0;
  size_t _contentLength = 0;

  ~AsyncWebServerRequest() { if (_tempObject) free(_tempObject); }
  AsyncClient *client() { static AsyncClient c; return _client ? _client : &c; }
  WebRequestMethodComposite method() const { return _method; }
  const String &url() const { return _url; }
  String host() const { return String("wled.local"); }
  size_t contentLength() const { return _contentLength; }
  String contentType() const { return String("application/x-www-form-urlencoded"); }

  size_t args() const { return params.size(); }
  size_t params_count() const { return params.size(); }
  const String &arg(size_t i) const { static String e; return i < params.size() ? params[i].value() : e; }
  const String &argName(size_t i) const { static String e; return i < params.size() ? params[i].name() : e; }
  const String &arg(const String &n) const { static String e; for (auto &p : params) if (p.name() == n) return p.value(); return e; }
  const String &arg(const char *n) const { return arg(String(n)); }
  const String &arg(const __FlashStringHelper *n) const { return arg(String(n)); }
  bool hasArg(const String &n) const { for (auto &p : params) if (p.name() == n) return true; return false; }
  bool hasArg(const char *n) const { return hasArg(String(n)); }
  bool hasArg(const __FlashStringHelper *n) const { return hasArg(String(n)); }
  bool hasParam(const String &n, bool post = false, bool file = false) const { (void)post; (void)file; return hasArg(n); }
  bool hasParam(const __FlashStringHelper *n, bool post = false, bool file = false) const { return hasParam(String(n), post, file); }
  const AsyncWebParameter *getParam(const String &n, bool = false, bool = false) const { for (auto &p : params) if (p.name() == n) return &p; return nullptr; }
  const AsyncWebParameter *getParam(const __FlashStringHelper *n, bool a = false, bool b = false) const { return getParam(String(n), a, b); }
  const AsyncWebParameter *getParam(const char *n, bool a = false, bool b = false) const { return getParam(String(n), a, b); }
  const AsyncWebParameter *getParam(size_t i) const { return i < params.size() ? &params[i] : nullptr; }
  bool hasHeader(const String &n) const { for (auto &h : headers) if (h.name().equalsIgnoreCase(n)) return true; return false; }
  bool hasHeader(const __FlashStringHelper *n) const { return hasHeader(String(n)); }
  AsyncWebHeader *getHeader(const String &n) { for (auto &h : headers) if (h.name().equalsIgnoreCase(n)) return &h; return nullptr; }
  AsyncWebHeader *getHeader(const __FlashStringHelper *n) { return getHeader(String(n)); }
  void addInterestingHeader(const String &) {}
  void onDisconnect(ArDisconnectHandler) {}
  void redirect(const String &) { sentCode = 302; }
  bool authenticate(const char *, const char *, const char * = nullptr, bool = false) { return true; }
  void requestAuthentication(const char * = nullptr, bool = true) {}
  void deferResponse() {}

  void send(AsyncWebServerResponse *r) { if (r) { sentCode = r->_code; delete r; } }
  void send(int code) { sentCode = code; }
  template <typename A> void send(int code, const A &, const String & = String()) { sentCode = code; }
  template <typename A, typename B> void send(int code, const A &, const B &) { sentCode = code; }
  void send(FS &, const String &, const String & = String(), bool = false, AwsTemplateProcessor = nullptr) { sentCode = 200; }
  template <typename A> void send_P(int code, const A &, const char *, size_t = 0) { sentCode = code; }
  template <typename A> void send_P(int code, const A &, const uint8_t *, size_t) { sentCode = code; }
  template <typename A> void sendChunked(const A &, AwsResponseFiller) { sentCode = 200; }
  AsyncWebServerResponse *beginResponse(int code, const String & = String(), const String & = String()) { auto *r = new AsyncBasicResponse(); r->_code = code; return r; }
  AsyncWebServerResponse *beginResponse(int code, const __FlashStringHelper *, const String & = String()) { return beginResponse(code); }
  AsyncWebServerResponse *beginResponse(FS &, const String &, const String & = String(), bool = false, AwsTemplateProcessor = nullptr) { return beginResponse(200); }
  template <typename A> AsyncWebServerResponse *beginResponse_P(int code, const A &, const uint8_t *, size_t, AwsTemplateProcessor = nullptr) { return beginResponse(code); }
  template <typename A> AsyncWebServerResponse *beginResponse_P(int code, const A &, const char *, size_t = 0, AwsTemplateProcessor = nullptr) { return beginResponse(code); }
  template <typename A> AsyncResponseStream *beginResponseStream(const A &, size_t = 1460) { return new AsyncResponseStream(); }
};

class AsyncWebHandler {
 public:
  virtual ~AsyncWebHandler() {}
  virtual bool canHandle(AsyncWebServerRequest *) { return false; }
  virtual void handleRequest(AsyncWebServerRequest *) {}
  virtual void handleUpload(AsyncWebServerRequest *, const String &, size_t, uint8_t *, size_t, bool) {}
  virtual void handleBody(AsyncWebServerRequest *, uint8_t *, size_t, size_t, size_t) {}
  virtual bool isRequestHandlerTrivial() { return true; }
  AsyncWebHandler &setFilter(std::function<bool(AsyncWebServerRequest *)>) { return *this; }
};
class AsyncCallbackWebHandler : public AsyncWebHandler {};
class AsyncStaticWebHandler : public AsyncWebHandler {
 public:
  AsyncStaticWebHandler &setCacheControl(const char *) { return *this; }
  AsyncStaticWebHandler &setDefaultFile(const char *) { return *this; }
};

class AsyncWebServer {
 public:
  struct QueueConfig { int a, b, c, d; };
  AsyncWebServer(uint16_t, QueueConfig = {}) {}
  void begin() {}
  void end() {}
  void reset() {}
  AsyncWebHandler &addHandler(AsyncWebHandler *h) { return *h; }
  bool removeHandler(AsyncWebHandler *) { return true; }
  AsyncCallbackWebHandler &on(const char *, ...) { static AsyncCallbackWebHandler h; return h; }
  AsyncCallbackWebHandler &on(const String &, ...) { static AsyncCallbackWebHandler h; return h; }
  template <typename... T> AsyncCallbackWebHandler &on(const char *, T...) { static AsyncCallbackWebHandler h; return h; }
  template <typename... T> AsyncCallbackWebHandler &on(const __FlashStringHelper *, T...) { static AsyncCallbackWebHandler h; return h; }
  AsyncStaticWebHandler &serveStatic(const char *, FS &, const char *, const char * = nullptr) { static AsyncStaticWebHandler h; return h; }
  void onNotFound(ArRequestHandlerFunction) {}
  void onFileUpload(ArUploadHandlerFunction) {}
  void onRequestBody(ArBodyHandlerFunction) {}
};

// ---- WebSockets ----
typedef enum { WS_CONTINUATION, WS_TEXT, WS_BINARY, WS_DISCONNECT = 0x08, WS_PING, WS_PONG } AwsFrameType;
typedef enum { WS_MSG_SENDING, WS_MSG_SENT, WS_MSG_ERROR } AwsMessageStatus;
typedef enum { WS_EVT_CONNECT, WS_EVT_DISCONNECT, WS_EVT_PONG, WS_EVT_ERROR, WS_EVT_DATA } AwsEventType;
typedef enum { WS_DISCONNECTED, WS_CONNECTED, WS_DISCONNECTING } AwsClientStatus;
typedef struct {
  uint8_t message_opcode; uint32_t num; uint8_t final; uint8_t masked; uint8_t opcode;
  uint64_t len; uint8_t mask[4]; uint64_t index;
} AwsFrameInfo;

class AsyncWebSocketBuffer {
  std::vector<uint8_t> b;
 public:
  AsyncWebSocketBuffer() {}
  explicit AsyncWebSocketBuffer(size_t n) : b(n) {}
  AsyncWebSocketBuffer(const char *d, size_t n) : b((const uint8_t *)d, (const uint8_t *)d + n) {}
  uint8_t *data() { return b.data(); }
  size_t size() const { return b.size(); }
  operator bool() const { return !b.empty(); }
  bool operator!() const { return b.empty(); }
};
class AsyncWebSocketMessageBuffer {
 public:
  std::vector<uint8_t> b;
  explicit AsyncWebSocketMessageBuffer(size_t n) : b(n + 1) {}
  uint8_t *get() { return b.data(); }
  size_t length() const { return b.size() - 1; }
  void lock() {} void unlock() {}
};

class AsyncWebSocket;
class AsyncWebSocketClient {
 public:
  uint32_t _id = 1; IPAddress _ip{10, 0, 0, 2}; AsyncWebSocket *_server = nullptr;
  uint32_t id() const { return _id; }
  IPAddress remoteIP() const { return _ip; }
  uint16_t remotePort() const { return 80; }
  AwsClientStatus status() const { return WS_CONNECTED; }
  bool queueIsFull() const { return false; }
  bool canSend() const { return true; }
  size_t queueLen() const { return 0; }
  size_t queueLength() const { return 0; }
  template <typename... T> void text(T...) {}
  template <typename... T> void binary(T...) {}
  void close(uint16_t = 0, const char * = nullptr) {}
  void ping(const uint8_t * = nullptr, size_t = 0) {}
  void keepAlivePeriod(uint16_t) {}
};
typedef std::function<void(AsyncWebSocket *, AsyncWebSocketClient *, AwsEventType, void *, uint8_t *, size_t)> AwsEventHandler;
class AsyncWebSocket : public AsyncWebHandler {
 public:
  std::list<AsyncWebSocketClient> _clients;
  AsyncWebSocket(const String &) {}
  void onEvent(AwsEventHandler) {}
  size_t count() const { return _clients.size(); }
  AsyncWebSocketClient *client(uint32_t id) { for (auto &c : _clients) if (c.id() == id) return &c; return nullptr; }
  std::list<AsyncWebSocketClient> &getClients() { return _clients; }
  bool availableForWriteAll() { return true; }
  bool availableForWrite(uint32_t) { return true; }
  template <typename... T> void textAll(T...) {}
  template <typename... T> void binaryAll(T...) {}
  template <typename... T> void text(uint32_t, T...) {}
  template <typename... T> void binary(uint32_t, T...) {}
  void cleanupClients(uint16_t = 4) {}
  void closeAll(uint16_t = 0, const char * = nullptr) {}
  AsyncWebSocketMessageBuffer *makeBuffer(size_t n) { return new AsyncWebSocketMessageBuffer(n); }
};
