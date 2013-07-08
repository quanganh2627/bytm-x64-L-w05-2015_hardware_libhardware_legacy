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

#ifndef WIFI_H_
# define WIFI_H_

#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include <pthread.h>
#include <assert.h>
#include <strings.h>

#include "hardware_legacy/wifi.h"
#include "libwpa_client/wpa_ctrl.h"

#define LOG_TAG "WifiHW"
#include "cutils/log.h"
#include "cutils/memory.h"
#include "cutils/misc.h"
#include "cutils/properties.h"
#include "private/android_filesystem_config.h"
#ifdef HAVE_LIBC_SYSTEM_PROPERTIES
#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>
#endif

/* PRIMARY refers to the connection on the primary interface
 * SECONDARY refers to an optional connection on a p2p interface
 *
 * For concurrency, we only support one active p2p connection and
 * one active STA connection at a time
 */
#define PRIMARY     0
#define SECONDARY   1
#define MAX_CONNS   2

#ifndef WIFI_DRIVER_MODULE_ARG
#define WIFI_DRIVER_MODULE_ARG          ""
#endif
#ifndef WIFI_FIRMWARE_LOADER
#define WIFI_FIRMWARE_LOADER            ""
#endif
#define WIFI_TEST_INTERFACE             "sta"

#ifndef WIFI_DRIVER_FW_PATH_STA
#define WIFI_DRIVER_FW_PATH_STA         NULL
#endif
#ifndef WIFI_DRIVER_FW_PATH_AP
#define WIFI_DRIVER_FW_PATH_AP          NULL
#endif
#ifndef WIFI_DRIVER_FW_PATH_P2P
#define WIFI_DRIVER_FW_PATH_P2P         NULL
#endif

#ifndef WIFI_DRIVER_FW_PATH_PARAM
#define WIFI_DRIVER_FW_PATH_PARAM       "/sys/module/wlan/parameters/fwpath"
#endif

#define DRIVER_PROP_NAME "wlan.driver.status"
#define VENDOR_PROP_NAME "wlan.driver.vendor"

/* libnetutils */
extern int do_dhcp();
extern int ifc_init();
extern void ifc_close();
extern char *dhcp_lasterror();
extern void get_dhcp_info();

/* bionic */
extern int init_module(void *, unsigned long, const char *);
extern int delete_module(const char *, unsigned int);
void wifi_close_sockets(int index);

/* utils.c */
int insmod(const char *filename, const char *args);
int rmmod(const char *modname);
int write_to_file(const char *path, const char *data, size_t len);
int file_exist(char *filename);
void log_cmd(const char *cmd);
void log_reply(char *reply, size_t *reply_len);


/* supplicant.c */
int ensure_entropy_file_exists();
int ensure_config_file_exists(const char *config_file);
int do_dhcp_request(int *ipaddr, int *gateway, int *mask,
                    int *dns1, int *dns2, int *server, int *lease);
const char *get_dhcp_error_string();
int wifi_start_supplicant(int p2p_supported);
int wifi_stop_supplicant(int p2p_supported);
int wifi_connect_on_socket_path(int index, const char *path);
int wifi_connect_to_supplicant(const char *ifname);
int wifi_connect_to_hostapd(void);
void wifi_close_hostapd_connection();
void wifi_close_supplicant_connection(const char *ifname);
int wifi_wait_for_event(const char *ifname, char *buf, size_t buflen);
void wifi_close_sockets(int index);
int wifi_command(const char *ifname, const char *command, char *reply, size_t *reply_len);
void wifi_wpa_ctrl_cleanup(void);
int wifi_send_command(int index, const char *cmd, char *reply, size_t *reply_len);
int wifi_ctrl_recv(int index, char *reply, size_t *reply_len);
int wifi_wait_on_socket(int index, char *buf, size_t buflen);
int update_ctrl_interface(const char *config_file);
int wifi_get_AP_station_list(char *reply, size_t *reply_len);
int wifi_get_AP_station(char *cmd, char *addr, size_t addr_len);

struct wifi_glue_ops {
        int             (*load_driver)(void);
        int             (*unload_driver)(void);
        int             (*switch_driver_mode)(int);
        int             (*change_fw_path)(const char*);
        const char *    (*get_fw_path)(int);
        int             (*is_driver_loaded)(void);
};

#endif /* !WIFI_H_ */
