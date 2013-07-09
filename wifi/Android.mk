# Copyright 2006 The Android Open Source Project

LOCAL_CFLAGS += -DCONFIG_CTRL_IFACE_CLIENT_DIR=\"/data/misc/wifi/sockets\"
LOCAL_CFLAGS += -DCONFIG_CTRL_IFACE_CLIENT_PREFIX=\"wpa_ctrl_\"

ifeq ($(TARGET_BOARD_PLATFORM),bigcore)
	ADDITIONAL_DEFAULT_PROPERTIES += wifi.interface=wlan0
	ADDITIONAL_DEFAULT_PROPERTIES += wlan.driver.vendor=bc
	LOCAL_CFLAGS += -DWIFI_GLUE_WITH_BC
	LOCAL_SRC_FILES += vendors/bc.c
endif

ifneq (,$(filter wifi_bcm%,$(PRODUCTS.$(INTERNAL_PRODUCT).PRODUCT_PACKAGES)))
	ifdef WIFI_DRIVER_MODULE_PATH
	LOCAL_CFLAGS += -DWIFI_DRIVER_MODULE_PATH=\"$(WIFI_DRIVER_MODULE_PATH)\"
	endif
	ifdef WIFI_DRIVER_MODULE_ARG
	LOCAL_CFLAGS += -DWIFI_DRIVER_MODULE_ARG=\"$(WIFI_DRIVER_MODULE_ARG)\"
	endif
	ifdef WIFI_DRIVER_MODULE_NAME
	LOCAL_CFLAGS += -DWIFI_DRIVER_MODULE_NAME=\"$(WIFI_DRIVER_MODULE_NAME)\"
	endif
	ifdef WIFI_FIRMWARE_LOADER
	LOCAL_CFLAGS += -DWIFI_FIRMWARE_LOADER=\"$(WIFI_FIRMWARE_LOADER)\"
	endif

	ifdef WIFI_DRIVER_FW_PATH_STA
	LOCAL_CFLAGS += -DWIFI_DRIVER_FW_PATH_STA=\"$(WIFI_DRIVER_FW_PATH_STA)\"
	endif
	LOCAL_CFLAGS += -DWIFI_DRIVER_43241_FW_PATH_STA=\"$(WIFI_DRIVER_43241_FW_PATH_STA)\"
	LOCAL_CFLAGS += -DWIFI_DRIVER_4334_FW_PATH_STA=\"$(WIFI_DRIVER_4334_FW_PATH_STA)\"
	LOCAL_CFLAGS += -DWIFI_DRIVER_4335_FW_PATH_STA=\"$(WIFI_DRIVER_4335_FW_PATH_STA)\"

	ifdef WIFI_DRIVER_FW_PATH_AP
	LOCAL_CFLAGS += -DWIFI_DRIVER_FW_PATH_AP=\"$(WIFI_DRIVER_FW_PATH_AP)\"
	endif
	LOCAL_CFLAGS += -DWIFI_DRIVER_43241_FW_PATH_AP=\"$(WIFI_DRIVER_43241_FW_PATH_AP)\"
	LOCAL_CFLAGS += -DWIFI_DRIVER_4334_FW_PATH_AP=\"$(WIFI_DRIVER_4334_FW_PATH_AP)\"
	LOCAL_CFLAGS += -DWIFI_DRIVER_4335_FW_PATH_AP=\"$(WIFI_DRIVER_4335_FW_PATH_AP)\"

	ifdef WIFI_DRIVER_FW_PATH_P2P
	LOCAL_CFLAGS += -DWIFI_DRIVER_FW_PATH_P2P=\"$(WIFI_DRIVER_FW_PATH_P2P)\"
	endif
	LOCAL_CFLAGS += -DWIFI_DRIVER_43241_FW_PATH_P2P=\"$(WIFI_DRIVER_43241_FW_PATH_P2P)\"
	LOCAL_CFLAGS += -DWIFI_DRIVER_4334_FW_PATH_P2P=\"$(WIFI_DRIVER_4334_FW_PATH_P2P)\"
	LOCAL_CFLAGS += -DWIFI_DRIVER_4335_FW_PATH_P2P=\"$(WIFI_DRIVER_4335_FW_PATH_P2P)\"

	ifdef WIFI_DRIVER_FW_PATH_PARAM
	LOCAL_CFLAGS += -DWIFI_DRIVER_FW_PATH_PARAM=\"$(WIFI_DRIVER_FW_PATH_PARAM)\"
	endif

	LOCAL_CFLAGS += -DWIFI_GLUE_WITH_BCM
	LOCAL_SRC_FILES += wifi/vendors/bcm.c
endif

ifneq (,$(filter wifi_ti%,$(PRODUCTS.$(INTERNAL_PRODUCT).PRODUCT_PACKAGES)))
	LOCAL_CFLAGS += -DWIFI_GLUE_WITH_TI
endif

LOCAL_SRC_FILES += wifi/wifi.c wifi/utils.c wifi/supplicant.c

LOCAL_SHARED_LIBRARIES += libnetutils
