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

#ifdef WIFI_GLUE_WITH_BCM
#  include "vendors/bcm.h"
#endif

#ifdef WIFI_GLUE_WITH_BC
#  include "vendors/bc.h"
#endif

enum {
    /* See X Macro trick on Wikipedia */
#define WIFI_GLUE(Vendor, Ops...) VENDOR_ ## Vendor,
#include "vendors/vendors.def"
#undef WIFI_GLUE
    MAX_VENDORS
};

struct wifi_glue_ops wops[MAX_VENDORS] = {
    /* See X Macro trick on Wikipedia */
#define WIFI_GLUE(Vendor, Load_driver, Unload_driver, Switch_mode, Change_fw, Get_fw, Is_loaded) \
    [VENDOR_ ## Vendor] = {                                             \
        .load_driver =  Load_driver,                                    \
        .unload_driver = Unload_driver,                                 \
        .switch_driver_mode = Switch_mode,                              \
        .change_fw_path = Change_fw,                                    \
        .get_fw_path = Get_fw,                                          \
        .is_driver_loaded = Is_loaded                                   \
    },
#include "vendors/vendors.def"
#undef WIFI_GLUE
};

static int wifi_get_vendor(void)
{
    char wifi_vendor[PROPERTY_VALUE_MAX];

    if (!property_get(VENDOR_PROP_NAME, wifi_vendor, NULL)) {
        ALOGE("wifi_get_vendor: prop %s is not set!",
              VENDOR_PROP_NAME);
        return -ENODEV;
    }

    /* See X Macro trick on Wikipedia */
#define WIFI_GLUE(Vendor, Ops...)               \
    if (strcasecmp(wifi_vendor, # Vendor) == 0) \
        return VENDOR_ ## Vendor;
#include "vendors/vendors.def"
#undef WIFI_GLUE

    ALOGE("wifi_get_vendor: Unknown vendor %s!", wifi_vendor);
    return -ENODEV;
}

int wifi_switch_driver_mode(int mode)
{
    unsigned int vendor = 0;

    vendor = wifi_get_vendor();

    if (vendor < MAX_VENDORS && wops[vendor].switch_driver_mode)
        return wops[vendor].switch_driver_mode(mode);
    else if (vendor < MAX_VENDORS)
        return 0;

    return -1;
}

int wifi_change_fw_path(const char *fwpath)
{
    unsigned int vendor = 0;

    vendor = wifi_get_vendor();

    if (vendor < MAX_VENDORS && wops[vendor].change_fw_path)
        return wops[vendor].change_fw_path(fwpath);
    else if (vendor < MAX_VENDORS)
        return 0;

    return -1;
}

int wifi_load_driver(void)
{
    unsigned int vendor = 0;

    vendor = wifi_get_vendor();

    if (vendor < MAX_VENDORS && wops[vendor].load_driver)
        return wops[vendor].load_driver();
    else if (vendor < MAX_VENDORS)
        return 0;

    return -1;
}

int wifi_unload_driver(void)
{
    unsigned int vendor = 0;

    vendor = wifi_get_vendor();

    if (vendor < MAX_VENDORS && wops[vendor].unload_driver)
        return wops[vendor].unload_driver();
    else
        return 0;

    return -1;
}

int is_wifi_driver_loaded()
{
    unsigned int vendor = 0;

    vendor = wifi_get_vendor();

    if (vendor < MAX_VENDORS && wops[vendor].is_driver_loaded)
        return wops[vendor].is_driver_loaded();
    else if (vendor < MAX_VENDORS)
        return 0;

    return -1;
}

const char *wifi_get_fw_path(int fw_type)
{
    unsigned int vendor = 0;

    vendor = wifi_get_vendor();

    if (vendor < MAX_VENDORS && wops[vendor].get_fw_path)
        return wops[vendor].get_fw_path(fw_type);
    else if (vendor < MAX_VENDORS)
        return "NO_FW_PATH";

    return 0;
}
