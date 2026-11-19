// Philips Hue bridge polling: the HTTP response received from the bridge is parsed by onHueData()
// (status line skip, then the JSON body: errors, on/bri/xy/hue/sat/ct).
// onHueData() treats the buffer as a C string; AsyncTCP hands it lwIP pbuf payload, which carries
// no terminator, so the harness passes a NUL-terminated copy (the length still counts only the data).
#include "wled_fuzz_common.h"
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  static bool inited = (wled_fuzz_init(), true); (void)inited;
  wled_fuzz_reset();
  huePollingEnabled = true; hueApplyOnOff = true; hueApplyBri = true; hueApplyColor = true;
  char *buf = (char *)malloc(size + 1);
  memcpy(buf, data, size); buf[size] = 0;
  onHueData(nullptr, nullptr, buf, size);
  free(buf);
  wled_fuzz_tick(1, 20);
  return 0;
}
