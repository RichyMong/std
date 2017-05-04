#include <cstdio>
#include "config_manager.h"

int main()
{
    zkcfg::ConfigManager cm("config.xml");
    cm.start();
    fgetc(stdin);
}

