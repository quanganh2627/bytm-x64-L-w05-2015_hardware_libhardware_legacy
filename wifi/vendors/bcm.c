/*
 * Copyright 2008, The Android Open Source Project
 * Copyright 2012-2013, Intel Corporation
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

#include "../wifi.h"
#include "bcm.h"

int bcm_load_driver(void)
{
    property_set(DRIVER_PROP_NAME, "ok");

    return 0;
}

int bcm_unload_driver(void)
{
    property_set(DRIVER_PROP_NAME, "unloaded");

    return 0;
}

int bcm_is_driver_loaded(void)
{
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

int bcm_switch_driver_mode(int mode)
{
    char mode_str[8];
    char bcm_prop_chip[PROPERTY_VALUE_MAX]="";
    char * wifi_module_opmode;
    struct dirent *dir;
    int ret = -1;

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

    ALOGE("wifi_switch_driver_mode: switching FW opmode");
    dir = dir_prefix_exist(SYS_MODULE, WIFI_PREFIX);
    if (dir) {
        if(asprintf(&wifi_module_opmode, "%s%s%s", SYS_MODULE, dir->d_name,
                PARAM_OPMODE) > 0) {
            ret = write_to_file(wifi_module_opmode, mode_str, strlen(mode_str));
            free(wifi_module_opmode);
        }
    }else
        ALOGE("wifi_switch_driver_mode: failed to switch opmode file not found");

    return ret;
}

int bcm_change_fw_path(const char *fwpath)
{
    return write_to_file(WIFI_DRIVER_FW_PATH_PARAM,
                         fwpath, strlen(fwpath) + 1);
}

