// JSON state API (/json/state over HTTP, WebSocket text frames, serial, MQTT all end in
// deserializeState()): the input is one WebSocket text frame handed to wsEvent(). The firmware
// loop then runs for a few hundred simulated milliseconds so presets and playlists it queued are applied.
#include "wled_fuzz_common.h"
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  static bool inited = (wled_fuzz_init(), true); (void)inited;
  wled_fuzz_reset();
  AsyncWebSocketClient client;
  AwsFrameInfo info{};
  info.final = 1; info.index = 0; info.len = size; info.opcode = WS_TEXT; info.message_opcode = WS_TEXT;
  uint8_t *buf = (uint8_t *)malloc(size ? size : 1);
  memcpy(buf, data, size);
  wsEvent(&ws, &client, WS_EVT_DATA, &info, buf, size);
  free(buf);
  wled_fuzz_tick(3, 200);
  return 0;
}
