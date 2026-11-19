// Settings form POSTs (/settings/wifi, /leds, /ui, /sync, /time, /sec, /dmx, /um, /2D, ...):
// byte 0 picks the settings page, the rest is an application/x-www-form-urlencoded body whose
// fields become the request arguments handleSettingsSet() reads.
//
// The WLED UI always submits the page's whole form, so the request also carries the page's
// identifying fields (kBaseForm) unless the body supplies them itself. A body field "!NAME" drops
// base field NAME, so a POST that lacks it (e.g. /settings/wifi without CS0) is still an input the
// fuzzer can send; it just is not what every one- or two-byte input turns into.
#include "wled_fuzz_common.h"

static int hexv(char c) { return c >= '0' && c <= '9' ? c - '0' : c >= 'a' && c <= 'f' ? c - 'a' + 10 : c >= 'A' && c <= 'F' ? c - 'A' + 10 : -1; }
static String urldecode(const char *s, size_t n) {
  String r;
  for (size_t i = 0; i < n; i++) {
    if (s[i] == '+') r += ' ';
    else if (s[i] == '%' && i + 2 < n && hexv(s[i + 1]) >= 0 && hexv(s[i + 2]) >= 0) { r += (char)(hexv(s[i + 1]) * 16 + hexv(s[i + 2])); i += 2; }
    else r += s[i];
  }
  return r;
}

struct BaseField { uint8_t page; const char *name, *value; };
static const BaseField kBaseForm[] = {
  {SUBPAGE_WIFI, "CS0", "WLED"},                                         // first WiFi network
  {SUBPAGE_LEDS, "L00", "2"}, {SUBPAGE_LEDS, "LC0", "30"}, {SUBPAGE_LEDS, "LT0", "22"},  // first bus
};

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  static bool inited = (wled_fuzz_init(), true); (void)inited;
  if (size < 1) return 0;
  wled_fuzz_reset();
  AsyncWebServerRequest request;
  request._method = HTTP_POST;
  const char *body = (const char *)data + 1;
  size_t n = size - 1, start = 0;
  uint8_t subPage = 1 + data[0] % SUBPAGE_LAST;  // SUBPAGE_WIFI .. SUBPAGE_PINS (const.h)
  std::vector<String> dropped;
  for (size_t i = 0; i <= n; i++) {
    if (i == n || body[i] == '&') {
      const char *kv = body + start; size_t len = i - start;
      const char *eq = (const char *)memchr(kv, '=', len);
      if (len && kv[0] == '!') dropped.push_back(urldecode(kv + 1, len - 1));
      else if (len) request.params.emplace_back(urldecode(kv, eq ? (size_t)(eq - kv) : len),
                                           eq ? urldecode(eq + 1, len - (eq - kv) - 1) : String(), true);
      start = i + 1;
    }
  }
  for (const BaseField &f : kBaseForm) {
    if (f.page != subPage || request.hasArg(f.name)) continue;
    bool drop = false;
    for (const String &d : dropped) drop |= d == f.name;
    if (!drop) request.params.emplace_back(String(f.name), String(f.value), true);
  }
  handleSettingsSet(&request, subPage);
  wled_fuzz_tick(3, 200);
  return 0;
}
