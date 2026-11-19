#pragma once
#include <stdint.h>
static inline int startWaveform(uint8_t, uint32_t, uint32_t, uint32_t, uint32_t = 0) { return 0; }
static inline int stopWaveform(uint8_t) { return 0; }
static inline void setTimer1Callback(uint32_t (*)()) {}
static inline int startWaveformClockCycles(uint8_t, uint32_t, uint32_t, uint32_t, int8_t = -1, int32_t = 0, bool = false) { return 0; }
