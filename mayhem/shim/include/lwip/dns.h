#pragma once
#include "ip_addr.h"
#include "err.h"

#define ERR_INPROGRESS -5
typedef void (*dns_found_callback)(const char *name, const ip_addr_t *ipaddr, void *arg);
static inline err_t dns_gethostbyname(const char *, ip_addr_t *, dns_found_callback, void *) { return -6; }
