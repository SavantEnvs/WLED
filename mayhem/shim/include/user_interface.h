#pragma once
#include <stdint.h>
struct ip_info { uint32_t ip, netmask, gw; };
static inline bool wifi_set_sleep_type(int) { return true; }
static inline uint32_t system_get_time() { return 0; }
static inline uint32_t system_get_chip_id() { return 0x123456; }
#define LIGHT_SLEEP_T 1
#define STATION_IF 0
#define SOFTAP_IF 1
static inline uint8_t wifi_get_channel() { return 1; }
#define NONE_SLEEP_T 0
#define MODEM_SLEEP_T 2
extern uint32_t shim_rtc_user_mem[128];
#define RTC_USER_MEM (shim_rtc_user_mem)
enum rst_reason { REASON_DEFAULT_RST = 0, REASON_WDT_RST, REASON_EXCEPTION_RST, REASON_SOFT_WDT_RST, REASON_SOFT_RESTART, REASON_DEEP_SLEEP_AWAKE, REASON_EXT_SYS_RST };
struct rst_info;
rst_info *system_get_rst_info();
static inline uint32_t system_get_rtc_time() { return 0; }
