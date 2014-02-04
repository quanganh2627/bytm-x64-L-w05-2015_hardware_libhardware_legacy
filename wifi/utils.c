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

#include "wifi.h"

int insmod(const char *filename, const char *args)
{
    void *module;
    unsigned int size;
    int ret;

    module = load_file(filename, &size);
    if (!module)
        return -1;

    ret = init_module(module, size, args);

    free(module);

    return ret;
}

int rmmod(const char *modname)
{
    int ret = -1;
    int maxtry = 10;

    while (maxtry-- > 0) {
        ret = delete_module(modname, O_NONBLOCK | O_EXCL);
        if (ret < 0 && errno == EAGAIN)
            usleep(500000);
        else
            break;
    }

    if (ret != 0)
        ALOGD("Unable to unload driver module \"%s\": %s\n",
             modname, strerror(errno));
    return ret;
}

int file_exist(char *filename)
{
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

int write_to_file(const char *path, const char *data, size_t len)
{
    int ret = 0;
    int fd = -1;

    assert(path);
    assert(data);

    fd = TEMP_FAILURE_RETRY(open(path, O_WRONLY));
    if (fd < 0) {
	ALOGE("Failed to open %s (%s)",
	      path, strerror(errno));
	return -errno;
    }

    if (TEMP_FAILURE_RETRY(write(fd, data, len)) != (int) len) {
	ALOGE("Failed to write %s in %s (%s)",
	      data, path, strerror(errno));
	ret = -errno;
    }

    close(fd);
    return ret;
}

void log_cmd(const char *cmd)
{
    if (strstr (cmd, "SET_NETWORK") && (strstr(cmd, "password") || strstr(cmd, "psk"))) {
        char *pbuf = malloc(strlen(cmd) + 1);
        if (pbuf) {
            strncpy(pbuf, cmd, strlen(cmd) + 1);
            pbuf[strlen(cmd)]='\0';
            char *p = strchr(pbuf, '\"');
            if (p)
                *p = '\0';
            LOGI("CMD: %s\n", pbuf);
        }
        free(pbuf);
    }
    else
        LOGI("CMD: %s\n", cmd);
}

void log_reply(char *reply, size_t *reply_len)
{
    char replyLocal[*reply_len];
    char delims[] = "\n";
    char *result = NULL;

    strncpy(replyLocal, reply, *reply_len);

    if (*reply_len > 0 && replyLocal[*reply_len-1] == '\n')
        replyLocal[*reply_len-1] = '\0';
    else
        replyLocal[*reply_len] = '\0';

    result = strtok(replyLocal , delims );
    while( result != NULL ) {
        LOGI("REPLY: %s\n", result);
        result = strtok( NULL, delims );
    }
}
