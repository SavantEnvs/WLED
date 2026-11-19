// Configuration import: a cfg.json uploaded through the file editor / backup restore or POSTed to
// /json/cfg is parsed by deserializeConfig() (buses, pins, LED maps, network, sync, timers, ...).
// The resulting configuration is then serialized back and the strip is (re)initialised from it.
#include "wled_fuzz_common.h"
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  static bool inited = (wled_fuzz_init(), true); (void)inited;
  wled_fuzz_reset();
  if (!requestJSONBufferLock(JSON_LOCK_CFG_DES)) return 0;
  DeserializationError err = deserializeJson(*pDoc, data, size);
  if (!err) {
    JsonObject root = pDoc->as<JsonObject>();
    if (!root.isNull()) deserializeConfig(root, true);
  }
  releaseJSONBufferLock();
  if (!err) {
    serializeConfigToFS();
    wled_fuzz_tick(3, 100);
  }
  return 0;
}
