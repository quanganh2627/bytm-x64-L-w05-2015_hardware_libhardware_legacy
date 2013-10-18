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

#ifndef BC_H_
# define BC_H_

/* Sadly bionic doesn't seem to have imported this header, so
 * redeclare the bits need */
#ifdef HAVE_LINUX_RFKILL_H
#include <linux/rfkill.h>
#else
#define RFKILL_TYPE_WLAN 1
#define RFKILL_OP_CHANGE_ALL 3
struct rfkill_event {
        __u32 idx;
        __u8  type;
        __u8  op;
        __u8  soft, hard;
} __attribute__((packed));
#endif

static const char DRIVER_LOAD_CHECK[] = "/sys/class/net/%s/phy80211/name";

int bc_is_wifi_driver_loaded();
int bc_load_driver();
int bc_unload_driver();

#endif /* !BC_H_ */
