#include "../wifi.h"
#include "mrvl.h"

//marvell 8777 start
#ifndef WIFI_DRIVER_MODULE_PATH
#define WIFI_DRIVER_MODULE_PATH         "/lib/modules/sd8xxx.ko"
#endif
#ifndef WIFI_MLAN_MODULE_PATH
#define WIFI_MLAN_MODULE_PATH           "/lib/modules/mlan.ko"
#endif
#ifndef WIFI_DRIVER_MODULE_NAME
#define WIFI_DRIVER_MODULE_NAME         "sd8xxx"
#endif
#ifndef WIFI_MLAN_MODULE_NAME
#define WIFI_MLAN_MODULE_NAME           "mlan"
#endif
#ifdef WIFI_DRIVER_MODULE_ARG
#undef WIFI_DRIVER_MODULE_ARG
#define WIFI_DRIVER_MODULE_ARG "drv_mode=5 cfg80211_wext=15 sta_name=wlan wfd_name=p2p max_vir_bss=1"

static const char* WIFI_DRIVER_MODULE_INIT_ARG = " init_cfg=";
static const char* WIFI_DRIVER_MODULE_INIT_CFG_PATH = "mrvl/wifi_init_cfg.conf";
static const char* WIFI_DRIVER_MODULE_INIT_CFG_STORE_PATH = "/data/misc/wireless/wifi_init_cfg.conf";
static const char* WIFI_DRIVER_MODULE_CAL_DATA_ARG = " cal_data_cfg=";
static const char* WIFI_DRIVER_MODULE_CAL_DATA_CFG_PATH = "mrvl/wifi_cal_data.conf";
static const char* WIFI_DRIVER_MODULE_CAL_DATA_CFG_STORE_PATH = "/system/etc/firmware/mrvl/wifi_cal_data.conf";

#endif
#ifndef WIFI_MLAN_MODULE_ARG
#define WIFI_MLAN_MODULE_ARG           ""
#endif
//marvell 8777 end

#ifndef WIFI_DRIVER_MODULE_ARG
#define WIFI_DRIVER_MODULE_ARG          ""
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

//#error ****** MRVL_WIFI_VENDOR defined ******
#undef WIFI_DRIVER_FW_PATH_STA
#define WIFI_DRIVER_FW_PATH_STA         "/etc/firmware/mrvl/sd8777_uapsta.bin"
#undef WIFI_DRIVER_FW_PATH_AP
#define WIFI_DRIVER_FW_PATH_AP          "AP"
#undef WIFI_DRIVER_FW_PATH_P2P
#define WIFI_DRIVER_FW_PATH_P2P         "P2P"

#ifndef WIFI_DRIVER_FW_PATH_PARAM
#define WIFI_DRIVER_FW_PATH_PARAM       "/sys/module/wlan/parameters/fwpath"
#endif

#define WIFI_DRIVER_LOADER_DELAY        1000000

static const char IFACE_DIR[]           = "/data/system/wpa_supplicant";
#ifdef WIFI_DRIVER_MODULE_PATH
static const char DRIVER_MODULE_NAME[]  = WIFI_DRIVER_MODULE_NAME;
static const char DRIVER_MODULE_TAG[]   = WIFI_DRIVER_MODULE_NAME " ";
static const char DRIVER_MODULE_PATH[]  = WIFI_DRIVER_MODULE_PATH;
static const char DRIVER_MODULE_ARG[]   = WIFI_DRIVER_MODULE_ARG;
static const char MLAN_MODULE_PATH[] =  WIFI_MLAN_MODULE_PATH;
static const char MLAN_MODULE_ARG[] =   WIFI_MLAN_MODULE_ARG;
static const char MLAN_MODULE_NAME[] =  WIFI_MLAN_MODULE_NAME;
#endif

static const char FIRMWARE_LOADER[]     = WIFI_FIRMWARE_LOADER;
static const char SUPPLICANT_NAME[]     = "wpa_supplicant";
static const char SUPP_PROP_NAME[]      = "init.svc.wpa_supplicant";
static const char P2P_SUPPLICANT_NAME[] = "p2p_supplicant";
static const char P2P_PROP_NAME[]       = "init.svc.p2p_supplicant";
static const char SUPP_CONFIG_TEMPLATE[]= "/system/etc/wifi/wpa_supplicant.conf";
static const char SUPP_CONFIG_FILE[]    = "/data/misc/wifi/wpa_supplicant.conf";
static const char P2P_CONFIG_FILE[]     = "/data/misc/wifi/p2p_supplicant.conf";
static const char CONTROL_IFACE_PATH[]  = "/data/misc/wifi/sockets";
static const char MODULE_FILE[]         = "/proc/modules";


static const char* WIFI_DRIVER_IFAC_NAME =         "/sys/class/net/wlan0";
int wait_interface_ready (const char* interface_path)
{
#define TIMEOUT_VALUE 2000    //at most 2 seconds
    int fd, count = TIMEOUT_VALUE;

    while(count--) {
        fd = open(interface_path, O_RDONLY);
        if (fd >= 0)
        {
            close(fd);
            return 0;
        }
        usleep(1000);
    }
    ALOGE("timeout(%dms) to wait %s", TIMEOUT_VALUE, interface_path);
    return -1;
}

int mrvl_load_driver(void)
{
	int  ret        = 0;
	char driver_status[PROPERTY_VALUE_MAX];
	char arg_buf[512] = {0};
	if (!property_get(DRIVER_PROP_NAME, driver_status, NULL))
	{
		ret = wait_interface_ready(WIFI_DRIVER_IFAC_NAME);
		if(ret < 0)
		{
			property_set(DRIVER_PROP_NAME, "timeout");
			return -1;
		}
		ALOGD("Property %s not set yet, init as ok!\n", DRIVER_PROP_NAME);
		property_set(DRIVER_PROP_NAME, "ok");
		return 0;
	}
	else	/* Already set the property, check the status */
	{
		if(strcmp(driver_status, "unloaded") == 0)	/* Already unloaded, need load the driver again */
		{
#if 1
			ret = wait_interface_ready(WIFI_DRIVER_IFAC_NAME);
			if(ret < 0)
			{
				property_set(DRIVER_PROP_NAME, "timeout");
				return -1;
			}
			property_set(DRIVER_PROP_NAME, "ok");
			return 0;
#else
			ALOGD("mrvl_load_driver, already unloaded, re-install the driver modules\n");
			ALOGD("Start to insmod %s.ko %s\n", MLAN_MODULE_PATH, MLAN_MODULE_ARG);
			insmod(MLAN_MODULE_PATH, MLAN_MODULE_ARG);
        
			strcpy(arg_buf, DRIVER_MODULE_ARG);
			if (access(WIFI_DRIVER_MODULE_INIT_CFG_STORE_PATH, F_OK) == 0)
			{
				strcat(arg_buf, WIFI_DRIVER_MODULE_INIT_ARG);
				strcat(arg_buf, WIFI_DRIVER_MODULE_INIT_CFG_PATH);
			}

			if (access(WIFI_DRIVER_MODULE_CAL_DATA_CFG_STORE_PATH, F_OK) == 0)
			{
				strcat(arg_buf, WIFI_DRIVER_MODULE_CAL_DATA_ARG);
				strcat(arg_buf, WIFI_DRIVER_MODULE_CAL_DATA_CFG_PATH);
			}

			ALOGD("Start to insmod %s.ko %s\n", WIFI_DRIVER_MODULE_NAME, arg_buf);
	    		insmod(DRIVER_MODULE_PATH, arg_buf);
#endif
		}
		else if(strcmp(driver_status, "ok") == 0)	/* Already loaded, do nothing */
		{
			ALOGD("mrvl_load_driver, already loaded, do nothing!\n");
			return 0;
		}
		else /* Got a wrong state! */
		{
			ALOGE("driver_status unknown: %s\n", driver_status);
			return -1;
		}
	}
	return ret;
}

int mrvl_unload_driver(void)
{
    char driver_status[PROPERTY_VALUE_MAX];
    if (!property_get(DRIVER_PROP_NAME, driver_status, NULL)
            || strcmp(driver_status, "ok") != 0) {
        ALOGE("Driver %s not loaded, do nothing!\n", DRIVER_PROP_NAME);
        return 0;  /* driver not loaded */
    }
#if 0
    if (rmmod(DRIVER_MODULE_NAME) == 0 && rmmod(MLAN_MODULE_NAME) == 0) {
	ALOGD("rmmod %s and %s done.\n", DRIVER_MODULE_NAME, MLAN_MODULE_NAME);
    }
    else
    {
        ALOGE("mrvl_unload_driver, rmmod fail!\n");
        return -1;
    }
#endif
    property_set(DRIVER_PROP_NAME, "unloaded");
    return 0;
}

#if 0

int mrvl_load_driver(void)
{
	char driver_status[PROPERTY_VALUE_MAX];
	int  count = 0;

	char arg_buf[512] = {0};
	char *p_strstr_wlan  = NULL;
	char *p_strstr_p2p	 = NULL;
	int  ret        = 0;
	FILE *fp        = NULL;

	ALOGD("Start to insmod %s.ko\n", WIFI_MLAN_MODULE_NAME);
	if (insmod(MLAN_MODULE_PATH, MLAN_MODULE_ARG) < 0) {
		rmmod(DRIVER_MODULE_NAME); 
		rmmod(MLAN_MODULE_NAME);
		if (insmod(MLAN_MODULE_PATH, MLAN_MODULE_ARG) < 0) {
			ALOGE("insmod %s ko failed!", WIFI_MLAN_MODULE_NAME);
			goto INSMOD_MLAN_FAILED;
		}
	}

	strcpy(arg_buf, DRIVER_MODULE_ARG);
#if 1
	
	if (access(WIFI_DRIVER_MODULE_INIT_CFG_STORE_PATH, F_OK) == 0)
	{
		strcat(arg_buf, WIFI_DRIVER_MODULE_INIT_ARG);
		strcat(arg_buf, WIFI_DRIVER_MODULE_INIT_CFG_PATH);
	}

	if (access(WIFI_DRIVER_MODULE_CAL_DATA_CFG_STORE_PATH, F_OK) == 0)
	{
		strcat(arg_buf, WIFI_DRIVER_MODULE_CAL_DATA_ARG);
		strcat(arg_buf, WIFI_DRIVER_MODULE_CAL_DATA_CFG_PATH);
	}
#endif

    ALOGD("Start to insmod %s.ko %s\n", WIFI_DRIVER_MODULE_NAME, arg_buf);
    if (insmod(DRIVER_MODULE_PATH, arg_buf) < 0) {
        ALOGE("insmod %s ko failed!", WIFI_DRIVER_MODULE_NAME);
        goto IMSMOD_DRIVER_FAILED;
    }

#if 0
    do{
       fp=fopen("/proc/net/wireless", "r");
       if (!fp) {
           ALOGE("failed to fopen file: /proc/net/wireless\n");
           property_set(DRIVER_PROP_NAME, "failed");
           goto IMSMOD_DRIVER_FAILED;
       }
       ret = fread(tmp_buf, sizeof(tmp_buf), 1, fp);
       if (ret==0){
           ALOGD("faied to read proc/net/wireless");
       }
       fclose(fp);

       ALOGD("loading wifi driver...");
       p_strstr_wlan = strstr(tmp_buf, "wlan0");
       //p_strstr_p2p  = strstr(tmp_buf, "p2p0");
       if (p_strstr_wlan != NULL /*&& p_strstr_p2p != NULL*/) {
           property_set(DRIVER_PROP_NAME, "ok");
           break;
       }
       usleep(200000);// 200ms

   } while (count++ <= TIME_COUNT);

   if(count > TIME_COUNT) {
       ALOGE("timeout, register netdevice wlan0 failed.");
       property_set(DRIVER_PROP_NAME, "timeout");
       goto IMSMOD_DRIVER_FAILED;
   }
#endif
	
ret = wait_interface_ready(WIFI_DRIVER_IFAC_NAME);
if(ret < 0)
{
   property_set(DRIVER_PROP_NAME, "timeout");
   goto IMSMOD_DRIVER_FAILED;
}
   return 0;

IMSMOD_DRIVER_FAILED:
	rmmod(DRIVER_MODULE_NAME); 
INSMOD_MLAN_FAILED:
	rmmod(DRIVER_MODULE_NAME); 
	rmmod(MLAN_MODULE_NAME);
	
	return -1;
}

int mrvl_unload_driver(void)
{
    ALOGD("Enter %s Function.\n", __FUNCTION__);
    usleep(200000); /* allow to finish interface down */
#ifdef WIFI_DRIVER_MODULE_PATH
    if (rmmod(DRIVER_MODULE_NAME) == 0 && rmmod(MLAN_MODULE_NAME) == 0) {
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

#endif
