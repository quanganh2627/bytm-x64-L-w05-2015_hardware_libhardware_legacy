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
#include "bc.h"

static int set_rfkill_soft_block(int val)
{
    struct rfkill_event ev;
    int fd = open("/dev/rfkill", O_WRONLY);
    if(fd < 0) {
        ALOGE("cannot open /dev/rfkill: %s", strerror(errno));
        return -1;
    }

    /* Use CHANGE_ALL to turn them all off.  Because rfkill devices
     * can be platform devices (e.g. things like hard "airplane mode"
     * switches), they aren't necessarily 1:1 with network devices.
     * Thankfully the Android HAL doesn't understand un/load_driver()
     * operating independently on different interfaces anyway. */
    ev.idx = 0;
    ev.type = RFKILL_TYPE_WLAN;
    ev.op = RFKILL_OP_CHANGE_ALL;
    ev.soft = val;
    ev.hard = 0;
    if(write(fd, (void*)&ev, sizeof(ev)) != sizeof(ev)) {
        ALOGE("error writing to /dev/rfkill: %s", strerror(errno));
        return -1;
    }

    close(fd);
    return 0;
}

static int is_iface_present()
{
    char ifc[PROPERTY_VALUE_MAX];
    char *sysfs_path;
    int ret = 0;

    property_get("wifi.interface", ifc, WIFI_TEST_INTERFACE);
    ret = asprintf(&sysfs_path, DRIVER_LOAD_CHECK, ifc);
    if (ret < 0) {
        ALOGE("Error allocating sysfs path");
        return 0;
    }

    ret = (!access(sysfs_path, F_OK));

    free(sysfs_path);

    return ret;
}

/*
 * Driver "loading" support: the wifi HAL framework uses this as its
 * metaphor for "turn the radio off", but the actual drivers
 * (i.e. mainline nl80211 drivers on platforms with integrated rfkill
 * support) don't require it.  Don't bother unloading kernel modules,
 * just use the rfkill framework to disable the radio state.
 */
int bc_is_wifi_driver_loaded()
{
    return is_iface_present();
}

int bc_load_driver()
{
    return set_rfkill_soft_block(0);
}

int bc_unload_driver()
{
    return set_rfkill_soft_block(1);
}
