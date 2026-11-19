// Shared setup for the WLED host harnesses.
//
// The firmware is booted once (WLED::setup() plus a few loop() passes, so WiFi counts as connected
// and the UDP/E1.31/DDP sockets are open), then its configuration (cfg.json, written to the
// in-memory LittleFS) and filesystem are snapshotted. Before every input the filesystem is put back,
// the configuration is re-read from that snapshot, playlists/realtime/segments/brightness are reset
// and the clock and PRNG are rewound, so an input's effect does not leak into the next one.
#include "wled_fuzz_common.h"
#include <LittleFS.h>

static fs::MemFiles baselineFiles;
static uint32_t baselineMillis;
static std::vector<WiFiConfig> baselineWiFi;  // cfg.json re-read only ever grows this list

static void restoreFiles() {
  LittleFS.files.clear();
  for (auto &kv : baselineFiles) LittleFS.files[kv.first] = std::make_shared<std::vector<uint8_t>>(*kv.second);
}

void wled_fuzz_init() {
  randomSeed(1);
  shim_millis = 1000;
  WLED::instance().setup();
  wled_fuzz_tick(40, 100);
  serializeConfigToFS();       // make the boot configuration an explicit /cfg.json
  baselineFiles = LittleFS.files;
  baselineMillis = shim_millis;
  baselineWiFi = multiWiFi;
}

void wled_fuzz_reset() {
  randomSeed(1);
  shim_millis = baselineMillis;
  restoreFiles();
  multiWiFi = baselineWiFi;
  selectedWiFi = 0;
  notifierUdp.reset(); notifier2Udp.reset(); rgbUdp.reset();
  Serial.setRx(nullptr, 0);
  unloadPlaylist();
  if (realtimeMode) exitRealtime();
  realtimeOverride = REALTIME_OVERRIDE_NONE;
  deserializeConfigFromFS();
  // rebuild the buses from the restored configuration (what loop() does when doInitBusses is set)
  // and recreate the default segments with their capabilities, as at boot
  strip.finalizeInit();
  strip.makeAutoSegments(true);
  doInitBusses = false;
  configNeedsWrite = false;
  loadLedmap = -1;
  strip.setMainSegmentId(0);
  bri = briS; briLast = briS;
  nightlightActive = false;
  doReboot = false;
  colorUpdated(CALL_MODE_INIT);
}

void wled_fuzz_tick(unsigned n, uint32_t ms) {
  for (unsigned i = 0; i < n; i++) { shim_millis += ms; WLED::instance().loop(); }
}
