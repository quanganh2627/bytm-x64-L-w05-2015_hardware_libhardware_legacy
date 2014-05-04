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

#ifndef BCM_H_
# define BCM_H_

#define WIFI_MODULE_4430F_OPMODE        "/sys/module/bcm4430f/parameters/op_mode"
#define WIFI_MODULE_43362_OPMODE        "/sys/module/bcm43362/parameters/op_mode"
#define WIFI_MODULE_43241_OPMODE        "/sys/module/bcm43241/parameters/op_mode"
#define WIFI_MODULE_4334_OPMODE         "/sys/module/bcm4334/parameters/op_mode"
#define WIFI_MODULE_4334X_OPMODE        "/sys/module/bcm4334x/parameters/op_mode"
#define WIFI_MODULE_4335_OPMODE         "/sys/module/bcm4335/parameters/op_mode"

int bcm_load_driver(void);
int bcm_unload_driver(void);
int bcm_is_driver_loaded(void);
int bcm_switch_driver_mode(int mode);
int bcm_change_fw_path(const char *fwpath);

#endif /* !BCM_H_ */
