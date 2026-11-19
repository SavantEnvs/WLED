// Known-answer self-test of the host-built firmware: each check feeds a well-formed message through
// the same entry point a fuzz target uses and asserts the state the firmware ends up in.
// Prints one "PASS <name>" / "FAIL <name>" line per check; mayhem/test.sh counts them.
#include "wled_fuzz_common.h"
#include "wled_metadata.h"
#include <LittleFS.h>

static int failures = 0;
static void check(const char *name, bool ok) {
  printf("%s %s\n", ok ? "PASS" : "FAIL", name);
  if (!ok) failures++;
}

static void wsSend(AwsFrameType op, const uint8_t *d, size_t n) {
  AsyncWebSocketClient client;
  AwsFrameInfo info{};
  info.final = 1; info.len = n; info.opcode = op; info.message_opcode = op;
  std::vector<uint8_t> buf(d, d + n);
  wsEvent(&ws, &client, WS_EVT_DATA, &info, buf.data(), n);
}

int main() {
  wled_fuzz_init();
  check("boot_udp_sockets_open", udpConnected && udp2Connected && udpRgbConnected);
  check("boot_default_strip_length", strip.getLengthTotal() == DEFAULT_LED_COUNT);

  // JSON state API over a WebSocket text frame
  wled_fuzz_reset();
  const char *js = "{\"on\":true,\"bri\":77,\"transition\":0,\"seg\":[{\"id\":0,\"fx\":0,\"col\":[[255,0,0]]}]}";
  wsSend(WS_TEXT, (const uint8_t *)js, strlen(js));
  wled_fuzz_tick(2, 50);
  check("json_state_bri", bri == 77);
  check("json_state_seg_color", strip.getSegment(0).colors[0] == 0xFF0000);
  check("json_state_mode_solid", strip.getSegment(0).mode == 0);

  // HTTP win API
  wled_fuzz_reset();
  AsyncWebServerRequest req;
  handleSet(&req, String("win&A=33&R=0&G=255&B=16&FX=0&TT=0"), true);
  wled_fuzz_tick(2, 50);
  check("win_api_bri", bri == 33);
  check("win_api_color", colPri[0] == 0 && colPri[1] == 255 && colPri[2] == 16);

  // DDP over a WebSocket binary frame: 3 RGB pixels
  wled_fuzz_reset();
  const uint8_t ddp[] = {2 /*DDP*/, 0x41, 0x00, 0x01, 0x01, 0, 0, 0, 0, 0, 9,
                         10, 20, 30, 40, 50, 60, 70, 80, 90};
  wsSend(WS_BINARY, ddp, sizeof(ddp));
  check("ddp_realtime_mode", realtimeMode == REALTIME_MODE_DDP);
  check("ddp_pixel1", (strip.getPixelColor(1) & 0xFFFFFF) == 0x28323C);

  // Hyperion raw RGB on the realtime UDP port
  wled_fuzz_reset();
  const uint8_t hyp[] = {1, 2, 3, 4, 5, 6};
  rgbUdp.inject(hyp, sizeof(hyp));
  handleNotifications();
  check("hyperion_realtime_mode", realtimeMode == REALTIME_MODE_HYPERION);
  check("hyperion_pixel0", (strip.getPixelColor(0) & 0xFFFFFF) == 0x010203);

  // Configuration import
  wled_fuzz_reset();
  {
    DynamicJsonDocument d(1024);
    deserializeJson(d, "{\"id\":{\"name\":\"Fuzzy Lamp\"}}");
    deserializeConfig(d.as<JsonObject>(), true);
  }
  check("cfg_name", strcmp(serverDescription, "Fuzzy Lamp") == 0);
  wled_fuzz_reset();
  check("reset_restores_cfg", strcmp(serverDescription, "WLED") == 0 && strip.getLengthTotal() == DEFAULT_LED_COUNT);

  // Firmware metadata scan: our own descriptor embedded in an image is found and accepted
  {
    std::vector<uint8_t> img(4096, 0xAA);
    memcpy(img.data() + 1000, &WLED_BUILD_DESCRIPTION, sizeof(WLED_BUILD_DESCRIPTION));
    wled_metadata_t desc;
    bool found = findWledMetadata(img.data(), img.size(), &desc);
    check("metadata_found", found && strcmp(desc.release_name, releaseString) == 0);
    char err[128] = "";
    check("metadata_allow_ota", found && shouldAllowOTA(desc, err, sizeof(err)));
    std::vector<uint8_t> junk(4096, 0x55);
    check("metadata_absent", !findWledMetadata(junk.data(), junk.size(), &desc));
  }

  // LED map: reverse the 30-LED strip
  wled_fuzz_reset();
  {
    String m = "{\"map\":[";
    for (int i = DEFAULT_LED_COUNT - 1; i >= 0; i--) { m += String(i); if (i) m += ","; }
    m += "]}";
    LittleFS.put("/ledmap1.json", (const uint8_t *)m.c_str(), m.length());
    check("ledmap_loaded", strip.deserializeMap(1));
  }

  printf("%d failure(s)\n", failures);
  return failures ? 1 : 0;
}
