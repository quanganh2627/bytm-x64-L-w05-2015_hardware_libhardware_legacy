/*
 * Copyright 2008, The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "wifi.h"

#define WIFI_DRIVER_LOADER_DELAY	1000000

/*
 * This gets defined by the script load_bcmdriver in
 * vendor/intel/common/wifi/bcm_specific/
 */
static const char BCM_PROP_CHIP[]	= "wlan.bcm.chip";

int is_wifi_driver_loaded() {
    char driver_status[PROPERTY_VALUE_MAX];
#ifdef WIFI_DRIVER_MODULE_PATH
    FILE *proc;
    char line[sizeof(DRIVER_MODULE_TAG)+10];
#endif

    if (!property_get(DRIVER_PROP_NAME, driver_status, NULL)
            || strcmp(driver_status, "ok") != 0) {
        return 0;  /* driver not loaded */
    }
#ifdef WIFI_DRIVER_MODULE_PATH
    /*
     * If the property says the driver is loaded, check to
     * make sure that the property setting isn't just left
     * over from a previous manual shutdown or a runtime
     * crash.
     */
    if ((proc = fopen(MODULE_FILE, "r")) == NULL) {
        ALOGW("Could not open %s: %s", MODULE_FILE, strerror(errno));
        property_set(DRIVER_PROP_NAME, "unloaded");
        return 0;
    }
    while ((fgets(line, sizeof(line), proc)) != NULL) {
        if (strncmp(line, DRIVER_MODULE_TAG, strlen(DRIVER_MODULE_TAG)) == 0) {
            fclose(proc);
            return 1;
        }
    }
    fclose(proc);
    property_set(DRIVER_PROP_NAME, "unloaded");
    return 0;
#else
    return 1;
#endif
}

int wifi_load_driver()
{
#ifdef WIFI_DRIVER_MODULE_PATH
    char driver_status[PROPERTY_VALUE_MAX];
    int count = 100; /* wait at most 20 seconds for completion */

    if (is_wifi_driver_loaded()) {
        return 0;
    }

    if (insmod(DRIVER_MODULE_PATH, DRIVER_MODULE_ARG) < 0)
        return -1;

    if (strcmp(FIRMWARE_LOADER,"") == 0) {
        /* usleep(WIFI_DRIVER_LOADER_DELAY); */
        property_set(DRIVER_PROP_NAME, "ok");
    }
    else {
        property_set("ctl.start", FIRMWARE_LOADER);
    }
    sched_yield();
    while (count-- > 0) {
        if (property_get(DRIVER_PROP_NAME, driver_status, NULL)) {
            if (strcmp(driver_status, "ok") == 0)
                return 0;
            else if (strcmp(DRIVER_PROP_NAME, "failed") == 0) {
                wifi_unload_driver();
                return -1;
            }
        }
        usleep(200000);
    }
    property_set(DRIVER_PROP_NAME, "timeout");
    wifi_unload_driver();
    return -1;
#else
    property_set(DRIVER_PROP_NAME, "ok");
    return 0;
#endif
}

int wifi_unload_driver()
{
    usleep(200000); /* allow to finish interface down */
#ifdef WIFI_DRIVER_MODULE_PATH
    if (rmmod(DRIVER_MODULE_NAME) == 0) {
        int count = 20; /* wait at most 10 seconds for completion */
        while (count-- > 0) {
            if (!is_wifi_driver_loaded())
                break;
            usleep(500000);
        }
        usleep(500000); /* allow card removal */
        if (count) {
            return 0;
        }
        return -1;
    } else
        return -1;
#else
    property_set(DRIVER_PROP_NAME, "unloaded");
    return 0;
#endif
}

const char *wifi_get_fw_path(int fw_type)
{
    char bcm_prop_chip[PROPERTY_VALUE_MAX];

    switch (fw_type) {
    case WIFI_GET_FW_PATH_STA:
	if (property_get(BCM_PROP_CHIP, bcm_prop_chip, NULL)) {
	    if (strstr(bcm_prop_chip, "43241"))
		return WIFI_DRIVER_43241_FW_PATH_STA;
	    else if (strstr(bcm_prop_chip, "4334"))
		return WIFI_DRIVER_4334_FW_PATH_STA;
	    else if (strstr(bcm_prop_chip, "4335"))
		return WIFI_DRIVER_4335_FW_PATH_STA;
	}
	else
	    return WIFI_DRIVER_FW_PATH_STA;
    case WIFI_GET_FW_PATH_AP:
	if (property_get(BCM_PROP_CHIP, bcm_prop_chip, NULL)) {
	    if (strstr(bcm_prop_chip, "43241"))
		return WIFI_DRIVER_43241_FW_PATH_AP;
	    else if (strstr(bcm_prop_chip, "4334"))
		return WIFI_DRIVER_4334_FW_PATH_AP;
	    else if (strstr(bcm_prop_chip, "4335"))
		return WIFI_DRIVER_4335_FW_PATH_AP;
	}
	else
	    return WIFI_DRIVER_FW_PATH_AP;
    case WIFI_GET_FW_PATH_P2P:
	if (property_get(BCM_PROP_CHIP, bcm_prop_chip, NULL)) {
	    if (strstr(bcm_prop_chip, "43241"))
		return WIFI_DRIVER_43241_FW_PATH_P2P;
	    else if (strstr(bcm_prop_chip, "4334"))
		return WIFI_DRIVER_4334_FW_PATH_P2P;
	    else if (strstr(bcm_prop_chip, "4335"))
		return WIFI_DRIVER_4335_FW_PATH_P2P;
	}
	else
	    return WIFI_DRIVER_FW_PATH_P2P;
    default:
	    ALOGE("Unknown firmware type (%d)", fw_type);
    }

    return NULL;
}

int wifi_change_fw_path(const char *fwpath)
{
    return write_to_file(WIFI_DRIVER_FW_PATH_PARAM,
			 fwpath, strlen(fwpath) + 1);
}

int wifi_switch_driver_mode(int mode)
{
    char mode_str[8];
    char bcm_prop_chip[PROPERTY_VALUE_MAX]="";

    /**
     * BIT(0), BIT(1),.. come from dhd.h in the driver code, and we need to
     * stay aligned with their definition.
     *
     * TODO:
     *   - Find a way to include dhd.h and use the values from there directly to
     *     prevent any problems in future modifications of the ABI.
     */
    switch (mode) {
    case WIFI_STA_MODE:
	snprintf(mode_str, sizeof(mode_str), "%u\n", BIT(0) | BIT(2) | BIT(4));
	break;
    case WIFI_AP_MODE:
	snprintf(mode_str, sizeof(mode_str), "%u\n", BIT(1));
	break;
    case WIFI_P2P_MODE:
	snprintf(mode_str, sizeof(mode_str), "%u\n", BIT(2));
	break;
    default:
	ALOGE("wifi_switch_driver_mode: invalid mode %ud", mode);
	return -EINVAL;
    }

    ALOGE("wifi_switch_driver_mode:  %s switching FW opmode", BCM_PROP_CHIP);
    if (file_exist(WIFI_MODULE_43241_OPMODE))
        return write_to_file(WIFI_MODULE_43241_OPMODE, mode_str, strlen(mode_str));
    else if (file_exist(WIFI_MODULE_4334_OPMODE))
        return write_to_file(WIFI_MODULE_4334_OPMODE, mode_str, strlen(mode_str));
    else if (file_exist(WIFI_MODULE_4335_OPMODE))
        return write_to_file(WIFI_MODULE_4335_OPMODE, mode_str, strlen(mode_str));
    else {
        ALOGE("wifi_switch_driver_mode: failed to switch opmode file not found");
        return -1;
    }
}
