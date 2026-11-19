// UDP input of the running firmware: WLED sync notifications (udpPort / udpPort2), realtime
// WARLS/DRGB/DRGBW/DNRGB/DNRGBW/TPM2.NET and node-discovery packets, and Hyperion raw RGB
// (udpRgbPort). Byte 0 selects the socket the rest arrives on; then the firmware's own
// handleNotifications() reads it and one loop() pass renders the result.
#include "wled_fuzz_common.h"
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  static bool inited = (wled_fuzz_init(), true); (void)inited;
  if (size < 1) return 0;
  wled_fuzz_reset();
  switch (data[0] % 3) {
    case 0: notifierUdp.inject(data + 1, size - 1); break;
    case 1: notifier2Udp.inject(data + 1, size - 1); break;
    default: rgbUdp.inject(data + 1, size - 1); break;
  }
  handleNotifications();
  wled_fuzz_tick(1, 20);
  return 0;
}
