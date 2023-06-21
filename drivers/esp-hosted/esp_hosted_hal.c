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
 * ESP-Hosted WiFi HAL.
 */

#include "py/mphal.h"

#if MICROPY_PY_NETWORK_ESP_HOSTED

#include <stdint.h>
#include <string.h>

#include "py/runtime.h"
#include "modmachine.h"
#include "extmod/machine_spi.h"
#include "mpconfigboard.h"

#include "esp_hosted_hal.h"

MP_WEAK int esp_hosted_hal_init() {
    mp_hal_pin_input(MICROPY_HW_WIFI_HANDSHAKE);
    mp_hal_pin_input(MICROPY_HW_WIFI_DATAREADY);

    mp_hal_pin_output(MICROPY_HW_ESP_HOSTED_GPIO0);
    mp_hal_pin_output(MICROPY_HW_ESP_HOSTED_RESET);

    mp_hal_pin_write(MICROPY_HW_ESP_HOSTED_GPIO0, 1);
    mp_hal_pin_write(MICROPY_HW_ESP_HOSTED_RESET, 1);

    // Reset module
    mp_hal_pin_write(MICROPY_HW_ESP_HOSTED_RESET, 0);
    mp_hal_delay_ms(100);
    mp_hal_pin_write(MICROPY_HW_ESP_HOSTED_RESET, 1);
    mp_hal_delay_ms(500);

    // Initialize SPI.
    mp_obj_t args[] = {
        MP_OBJ_NEW_SMALL_INT(MICROPY_HW_WIFI_SPI_ID),
        MP_OBJ_NEW_SMALL_INT(MICROPY_HW_WIFI_SPI_BAUDRATE),
        MP_OBJ_NEW_QSTR(MP_QSTR_polarity), MP_OBJ_NEW_SMALL_INT(1),
    };

    MP_STATE_PORT(mp_wifi_spi) = MP_OBJ_TYPE_GET_SLOT(&machine_spi_type, make_new)((mp_obj_t)&machine_spi_type, 2, 1, args);

    mp_hal_pin_output(MICROPY_HW_WIFI_SPI_CS);
    mp_hal_pin_write(MICROPY_HW_WIFI_SPI_CS, 1);
    return 0;
}

MP_WEAK int esp_hosted_hal_deinit(void) {
    mp_hal_pin_output(MICROPY_HW_ESP_HOSTED_GPIO0);
    mp_hal_pin_output(MICROPY_HW_ESP_HOSTED_RESET);

    mp_hal_pin_write(MICROPY_HW_ESP_HOSTED_GPIO0, 1);
    mp_hal_pin_write(MICROPY_HW_ESP_HOSTED_RESET, 1);

    // Reset module
    mp_hal_pin_write(MICROPY_HW_ESP_HOSTED_RESET, 0);
    mp_hal_delay_ms(100);
    mp_hal_pin_write(MICROPY_HW_ESP_HOSTED_RESET, 1);
    mp_hal_delay_ms(250);

    mp_hal_pin_output(MICROPY_HW_WIFI_SPI_CS);
    mp_hal_pin_write(MICROPY_HW_WIFI_SPI_CS, 1);
    return 0;
}

MP_WEAK int esp_hosted_hal_atomic_enter(void) {
    #if MICROPY_ENABLE_SCHEDULER
    mp_sched_lock();
    #endif
    return 0;
}

MP_WEAK int esp_hosted_hal_atomic_exit(void) {
    #if MICROPY_ENABLE_SCHEDULER
    mp_sched_unlock();
    #endif
    return 0;
}

MP_WEAK bool esp_hosted_hal_data_ready(void) {
    return mp_hal_pin_read(MICROPY_HW_WIFI_DATAREADY) && mp_hal_pin_read(MICROPY_HW_WIFI_HANDSHAKE);
}

MP_WEAK int esp_hosted_hal_spi_transfer(const uint8_t *tx_buf, uint8_t *rx_buf, uint32_t size) {
    mp_obj_t mp_wifi_spi = MP_STATE_PORT(mp_wifi_spi);

    // Wait for handshake pin to go high.
    for (mp_uint_t start = mp_hal_ticks_ms(); ; mp_hal_delay_ms(1)) {
        if (mp_hal_pin_read(MICROPY_HW_WIFI_HANDSHAKE)) {
            break;
        }
        if ((mp_hal_ticks_ms() - start) >= 1000) {
            error_printf("timeout waiting for handshake\n");
            return -1;
        }
    }

    mp_hal_pin_write(MICROPY_HW_WIFI_SPI_CS, 0);
    mp_hal_delay_ms(1);
    ((mp_machine_spi_p_t *)MP_OBJ_TYPE_GET_SLOT(&machine_spi_type, protocol))->transfer(mp_wifi_spi, size, tx_buf, rx_buf);
    mp_hal_pin_write(MICROPY_HW_WIFI_SPI_CS, 1);
    return 0;
}

MP_WEAK void *esp_hosted_hal_alloc(void *user, size_t size) {
    (void)user;
    void *mem = m_malloc0(size);
    return mem;
}

MP_WEAK void esp_hosted_hal_free(void *user, void *ptr) {
    (void)user;
    m_free(ptr);
}

MP_WEAK void *esp_hosted_hal_calloc(size_t nmemb, size_t size) {
    return NULL;
}

MP_WEAK void *esp_hosted_hal_realloc(void *ptr, size_t size) {
    return NULL;
}

// Those are provided for protobuf-c's internally
// defined allocator, and are not actually used.
MP_WEAK void *malloc(size_t size) {
    (void)size;
    debug_printf("system malloc called\n");
    return NULL;
}

MP_WEAK void free(void *ptr) {
    (void)ptr;
    debug_printf("system free called\n");
}

#endif // MICROPY_PY_NETWORK_NINAW10
