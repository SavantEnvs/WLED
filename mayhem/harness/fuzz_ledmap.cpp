// LED map upload: /ledmap1.json (written through the file editor) is parsed by
// WS2812FX::deserializeMap(); the strip is then rendered through the resulting mapping table.
#include "wled_fuzz_common.h"
#include <LittleFS.h>
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  static bool inited = (wled_fuzz_init(), true); (void)inited;
  wled_fuzz_reset();
  LittleFS.put("/ledmap1.json", data, size);
  strip.deserializeMap(1);
  for (unsigned i = 0; i < strip.getLengthTotal(); i++) strip.setPixelColor(i, 0x102030 * (i + 1));
  strip.show();
  wled_fuzz_tick(1, 20);
  return 0;
}
