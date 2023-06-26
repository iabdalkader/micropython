/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Arduino SA
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * ESP-Hosted WiFi driver.
 */

#ifndef MICROPY_INCLUDED_DRIVERS_ESP_HOSTED_WIFI_H
#define MICROPY_INCLUDED_DRIVERS_ESP_HOSTED_WIFI_H

#include "esp_hosted_proto.h"

#define ESP_HOSTED_IPV4_ADDR_LEN    (4)
#define ESP_HOSTED_MAC_ADDR_LEN     (6)
#define ESP_HOSTED_MAC_STR_LEN      (18)
#define ESP_HOSTED_MAX_SSID_LEN     (32)
#define ESP_HOSTED_MAX_WEP_LEN      (13)
#define ESP_HOSTED_MAX_WPA_LEN      (63)
#define ESP_HOSTED_MAX_AP_CLIENTS   (3)

#define ESP_HOSTED_AP_GATEWAY       LWIP_MAKEU32(192, 168, 1, 1)
#define ESP_HOSTED_AP_ADDRESS       LWIP_MAKEU32(192, 168, 1, 1)
#define ESP_HOSTED_AP_NETMASK       LWIP_MAKEU32(255, 255, 255, 0)
#define ESP_HOSTED_HOSTNAME         "esphosted"

typedef enum {
    ESP_HOSTED_STA_IF = 0,
    ESP_HOSTED_AP_IF,
    ESP_HOSTED_SERIAL_IF,
    ESP_HOSTED_HCI_IF,
    ESP_HOSTED_PRIV_IF,
    ESP_HOSTED_TEST_IF,
    ESP_HOSTED_MAX_IF,
} esp_hosted_interface_t;

typedef enum {
    ESP_HOSTED_SEC_INVALID = -1,
    ESP_HOSTED_SEC_OPEN = CTRL__WIFI_SEC_PROT__Open,
    ESP_HOSTED_SEC_WEP = CTRL__WIFI_SEC_PROT__WEP,
    ESP_HOSTED_SEC_WPA_PSK = CTRL__WIFI_SEC_PROT__WPA_PSK,
    ESP_HOSTED_SEC_WPA2_PSK = CTRL__WIFI_SEC_PROT__WPA2_PSK,
    ESP_HOSTED_SEC_WPA_WPA2_PSK = CTRL__WIFI_SEC_PROT__WPA_WPA2_PSK,
    ESP_HOSTED_SEC_WPA2_ENTERPRISE = CTRL__WIFI_SEC_PROT__WPA2_ENTERPRISE,
    ESP_HOSTED_SEC_WPA3_PSK = CTRL__WIFI_SEC_PROT__WPA3_PSK,
    ESP_HOSTED_SEC_WPA2_WPA3_PSK= CTRL__WIFI_SEC_PROT__WPA2_WPA3_PSK,
} esp_hosted_security_t;

typedef struct {
    uint8_t ip_addr[ESP_HOSTED_IPV4_ADDR_LEN];
    uint8_t subnet_addr[ESP_HOSTED_IPV4_ADDR_LEN];
    uint8_t gateway_addr[ESP_HOSTED_IPV4_ADDR_LEN];
    uint8_t dns_addr[ESP_HOSTED_IPV4_ADDR_LEN];
} esp_hosted_ifconfig_t;

typedef struct {
    int32_t rssi;
    uint8_t security;
    uint8_t channel;
    char ssid[ESP_HOSTED_MAX_SSID_LEN];
    uint8_t bssid[ESP_HOSTED_MAC_ADDR_LEN];
} esp_hosted_scan_result_t;

typedef struct {
    int32_t rssi;
    uint8_t security;
    uint8_t channel;
    char ssid[ESP_HOSTED_MAX_SSID_LEN];
    uint8_t bssid[ESP_HOSTED_MAC_ADDR_LEN];
} esp_hosted_netinfo_t;

typedef int (*esp_hosted_scan_callback_t)(esp_hosted_scan_result_t *, void *);

int esp_hosted_init(uint32_t itf);
int esp_hosted_deinit(uint32_t itf);
int esp_hosted_poll(void);
void *esp_hosted_get_netif(uint32_t itf);
int esp_hosted_get_mac(int itf, uint8_t *mac);
int esp_hosted_connect(const char *ssid, const char *bssid, uint8_t security, const char *key, uint16_t channel);
int esp_hosted_start_ap(const char *ssid, uint8_t security, const char *key, uint16_t channel);
int esp_hosted_disconnect(uint32_t itf);
int esp_hosted_status(uint32_t itf);
int esp_hosted_get_stations(uint8_t *sta_list, size_t *sta_count);
int esp_hosted_netinfo(esp_hosted_netinfo_t *netinfo);
int esp_hosted_scan(esp_hosted_scan_callback_t scan_callback, void *arg, uint32_t timeout);
#endif // MICROPY_INCLUDED_DRIVERS_ESPHOST_WIFI_H
