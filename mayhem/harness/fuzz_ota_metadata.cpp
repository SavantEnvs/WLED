// Firmware upload validation: the uploaded image is scanned for WLED's build-metadata record
// (findWledMetadata) and, when one is found, checked for compatibility (shouldAllowOTA).
#include "wled_fuzz_common.h"
#include "wled_metadata.h"
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  wled_metadata_t desc;
  if (findWledMetadata(data, size, &desc)) {
    char err[128];
    shouldAllowOTA(desc, err, sizeof(err));
  }
  return 0;
}
