// Host stand-in for the ESP8266 NONOS ESP-NOW API (frames are delivered by harnesses).
#pragma once
#include <stdint.h>
enum esp_now_role { ESP_NOW_ROLE_IDLE = 0, ESP_NOW_ROLE_CONTROLLER, ESP_NOW_ROLE_SLAVE, ESP_NOW_ROLE_COMBO, ESP_NOW_ROLE_MAX };
typedef void (*esp_now_recv_cb_t)(uint8_t *mac_addr, uint8_t *data, uint8_t len);
typedef void (*esp_now_send_cb_t)(uint8_t *mac_addr, uint8_t status);
extern esp_now_recv_cb_t shim_espnow_recv_cb;
static inline int esp_now_init(void) { return 0; }
static inline int esp_now_deinit(void) { return 0; }
static inline int esp_now_register_send_cb(esp_now_send_cb_t) { return 0; }
static inline int esp_now_unregister_send_cb(void) { return 0; }
static inline int esp_now_register_recv_cb(esp_now_recv_cb_t cb) { shim_espnow_recv_cb = cb; return 0; }
static inline int esp_now_unregister_recv_cb(void) { shim_espnow_recv_cb = nullptr; return 0; }
static inline int esp_now_send(uint8_t *, uint8_t *, int) { return 0; }
static inline int esp_now_add_peer(uint8_t *, uint8_t, uint8_t, uint8_t *, uint8_t) { return 0; }
static inline int esp_now_del_peer(uint8_t *) { return 0; }
static inline int esp_now_set_self_role(uint8_t) { return 0; }
