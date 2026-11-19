// HTTP "win" API (/win&A=128&R=255..., also used by MQTT /api, IR and button macros): the input is
// the query string appended to "win&" and parsed by handleSet().
#include "wled_fuzz_common.h"
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  static bool inited = (wled_fuzz_init(), true); (void)inited;
  // handleSet() takes an Arduino String, which (like the HTTP layer's URL decoding) ends at NUL.
  String req("win&");
  req.concat((const char *)data, strnlen((const char *)data, size));
  wled_fuzz_reset();
  AsyncWebServerRequest request;
  handleSet(&request, req, true);
  wled_fuzz_tick(3, 200);
  return 0;
}
