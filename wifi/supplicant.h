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

#ifndef SUPPLICANT_H_
# define SUPPLICANT_H_

# include <sys/types.h>
# include <sys/wait.h>

#define IFACE_DIR                       "/data/system/wpa_supplicant"
#define SUPPLICANT_NAME                 "wpa_supplicant"
#define SUPP_PROP_NAME                  "init.svc.wpa_supplicant"
#define P2P_SUPPLICANT_NAME             "p2p_supplicant"
#define P2P_PROP_NAME                   "init.svc.p2p_supplicant"
#define SUPP_CONFIG_TEMPLATE            "/system/etc/wifi/wpa_supplicant.conf"
#define SUPP_CONFIG_FILE                "/data/misc/wifi/wpa_supplicant.conf"
#define P2P_CONFIG_FILE                 "/data/misc/wifi/p2p_supplicant.conf"
#define CONTROL_IFACE_PATH              "/data/misc/wifi/sockets"
#define SUPP_ENTROPY_FILE               WIFI_ENTROPY_FILE

#endif /* !SUPPLICANT_H_ */
