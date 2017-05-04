# pragma once

#include <stdio.h>
#include <string.h>

#define ROOT_PATH "/foreign"

#define SERVICE_PATH ROOT_PATH "/service"
#define SERVICE_DS SERVICE_PATH "/downstream"
#define SERVICE_US SERVICE_PATH "/upstream"

#define CONFIG_PATH ROOT_PATH "/config"
#define UNIVERSAL_PATH CONFIG_PATH "/universal"
#define ID_CONFIG "id.xml"
#define UNIVERSAL_ID_PATH UNIVERSAL_PATH "/" ID_CONFIG

static inline
char* get_config_path(const char* location, const char* cfgfile)
{
    char path[256];
    if (location && location[0]) {
        sprintf(path, CONFIG_PATH "/%s/%s", location, cfgfile);
    } else {
        sprintf(path, UNIVERSAL_PATH "/%s", cfgfile);
    }

    return strdup(path);
}

