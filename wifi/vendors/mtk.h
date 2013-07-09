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

#ifndef MTK_H_
# define MTK_H_

#include <linux/wireless.h>

#define MTK_PRIV_CMD_P2P_MODE   28
#define MTK_IOCTL_SET_INT       (SIOCIWFIRSTPRIV + 0)
#define MTK_IOCTL_GET_INT       (SIOCIWFIRSTPRIV + 1)
#define MTK_POWER_PATH          "/dev/wmtWifi"

int mtk_switch_driver_mode(int mode);
int mtk_load_driver(void);
int mtk_unload_driver(void);
int mtk_is_driver_loaded(void);

#endif /* !MTK_H_ */
