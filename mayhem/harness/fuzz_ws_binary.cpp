// Binary WebSocket frames (/ws): byte 0 picks the protocol (E1.31, Art-Net, DDP) and the rest is
// the packet, handed by wsEvent() to handleE131Packet() exactly as for a browser client. The first
// input byte selects the DMX mode/start settings so every handleDMXData() mapping is reachable.
#include "wled_fuzz_common.h"
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  static bool inited = (wled_fuzz_init(), true); (void)inited;
  if (size < 2) return 0;
  wled_fuzz_reset();
  DMXMode = data[0] % 11;           // DMX_MODE_DISABLED .. DMX_MODE_PRESET (const.h)
  DMXAddress = 1 + (data[0] >> 4);
  e131SkipOutOfSequence = data[0] & 1;
  data++; size--;
  AsyncWebSocketClient client;
  AwsFrameInfo info{};
  info.final = 1; info.index = 0; info.len = size; info.opcode = WS_BINARY; info.message_opcode = WS_BINARY;
  uint8_t *buf = (uint8_t *)malloc(size);
  memcpy(buf, data, size);
  wsEvent(&ws, &client, WS_EVT_DATA, &info, buf, size);
  free(buf);
  wled_fuzz_tick(1, 20);
  return 0;
}
