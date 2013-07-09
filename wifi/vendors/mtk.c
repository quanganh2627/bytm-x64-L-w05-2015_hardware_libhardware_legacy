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
#include "mtk.h"

static int mtk_set_power(int on)
{
    ALOGE("set_power %d\n", on);
    return write_to_file(MTK_POWER_PATH, on ? "1" : "0", 1);
}

static int mtk_set_p2p_mode(int enable, int mode)
{
    struct iwreq req = {0};
    int param[2];
    int sk = -1;

    ALOGD("mtk_set_p2p_mode: enable==%d mode==%d", enable, mode);

    if ((sk = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        ALOGE("switch_driver_mode: Cannot open socket!");
        return -errno;
    }

    param[0] = enable;
    param[1] = mode;

    req.u.data.pointer = &(param[0]);
    req.u.data.length = 2;
    req.u.mode =  MTK_PRIV_CMD_P2P_MODE;
    memcpy(req.u.name + 4, param, sizeof(param));
    strncpy(req.ifr_name, "wlan0", IFNAMSIZ);

    if (ioctl(sk, MTK_IOCTL_SET_INT, &req) < 0) {
        ALOGE("switch_driver_mode: Error while sending ioctl %s", strerror(errno));
        close(sk);
        return -errno;
    }

    close(sk);

    return 0;
}

int mtk_switch_driver_mode(int mode)
{
    ALOGE("wifi_switch_driver_mode: %d", mode);

    switch (mode) {
    case WIFI_STA_MODE:
    case WIFI_P2P_MODE:
        return mtk_set_p2p_mode(1, 0);
        break;
    case WIFI_AP_MODE:
        if (!mtk_is_driver_loaded())
                mtk_set_power(1);
        mtk_set_p2p_mode(1, 1);
        return 0;
        break;
    default:
        ALOGE("switch_driver_mode: Unknown mode!");
        return -EINVAL;
    }

    return 0;
}

int mtk_load_driver(void)
{
    ALOGE("load_driver called");

    if (mtk_set_power(1) > 0)
        return -1;

    property_set(DRIVER_PROP_NAME, "loaded");

    return 0;
}

int mtk_unload_driver(void)
{
    ALOGE("unload_driver called");

    /* if (mtk_set_power(0) > 0) */
    /*  return -1; */

    property_set(DRIVER_PROP_NAME, "unloaded");

    return 0;
}

int mtk_is_driver_loaded(void)
{
        char driver_loaded[PROPERTY_VALUE_MAX];

        if (!property_get(DRIVER_PROP_NAME, driver_loaded, NULL) ||
            strcmp(driver_loaded, "loaded") != 0)
                return 0;

        return 1;
}
