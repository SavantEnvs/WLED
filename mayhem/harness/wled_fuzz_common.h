// Shared setup for the WLED host harnesses: boot the firmware once (WLED::setup() against the
// in-memory filesystem), then restore a known state before every input.
#pragma once
#include "wled.h"

void wled_fuzz_init();
void wled_fuzz_reset();
void wled_fuzz_tick(unsigned n, uint32_t ms);  // run the firmware's handlers n times, advancing the clock
