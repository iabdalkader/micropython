CMSIS_MCU = RA6M5
MCU_SERIES = m33
LD_FILES = boards/ARDUINO_PORTENTA_C33/ra6m5.ld
CFLAGS += -DCFG_TUH_RHPORT=0 \
          -DCFG_TUD_RHPORT=1 \
          -DCFG_TUH_MAX_SPEED=OPT_MODE_FULL_SPEED \
          -DCFG_TUD_MAX_SPEED=OPT_MODE_HIGH_SPEED \
          -DCFG_TUSB_RHPORT0_MODE=0\
          -DCFG_TUSB_RHPORT1_MODE=OPT_MODE_DEVICE

# MicroPython settings
MICROPY_VFS_FAT = 1
MICROPY_HW_ENABLE_RNG = 1
MICROPY_PY_BLUETOOTH = 1
MICROPY_BLUETOOTH_NIMBLE = 1
MICROPY_BLUETOOTH_BTSTACK = 0
MICROPY_PY_LWIP = 1
MICROPY_PY_USSL = 1
MICROPY_SSL_MBEDTLS = 1
MICROPY_PY_NETWORK = 1
MICROPY_PY_NETWORK_ESP_HOSTED = 1

FROZEN_MANIFEST = $(BOARD_DIR)/manifest.py
MBEDTLS_CONFIG_FILE = '"$(BOARD_DIR)/mbedtls_config_board.h"'
