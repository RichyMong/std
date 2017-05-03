#include <boost/property_tree/xml_parser.hpp>
#include <sstream>
#include <iostream>
#include <cstdio>
#include <cstdarg>
#include "config_manager.h"

namespace zkcfg
{

#define LOG(__fmt, ...) \
    fprintf(stderr, __FILE__ ":%d " __fmt "\n", __LINE__, ##__VA_ARGS__)

int ConfigManager::DEFAULT_TIMEOUT = 3000;

const char* state_to_string(int state)
{
    if (state == 0)
        return "CLOSED_STATE";
    if (state == ZOO_CONNECTING_STATE)
        return "CONNECTING_STATE";
    if (state == ZOO_ASSOCIATING_STATE)
        return "ASSOCIATING_STATE";
    if (state == ZOO_CONNECTED_STATE)
        return "CONNECTED_STATE";
    if (state == ZOO_EXPIRED_SESSION_STATE)
        return "EXPIRED_SESSION_STATE";
    if (state == ZOO_AUTH_FAILED_STATE)
        return "AUTH_FAILED_STATE";

    return "INVALID_STATE";
}

static const char* type_to_string(int state)
{
    if (state == ZOO_CREATED_EVENT)
        return "CREATED_EVENT";
    if (state == ZOO_DELETED_EVENT)
        return "DELETED_EVENT";
    if (state == ZOO_CHANGED_EVENT)
        return "CHANGED_EVENT";
    if (state == ZOO_CHILD_EVENT)
        return "CHILD_EVENT";
    if (state == ZOO_SESSION_EVENT)
        return "SESSION_EVENT";
    if (state == ZOO_NOTWATCHING_EVENT)
        return "NOTWATCHING_EVENT";

    return "UNKNOWN_EVENT_TYPE";
}

void global_watcher(zhandle_t *zzh, int type, int state,
                const char *path, void* context)
{
    /* Be careful using zh here rather than zzh - as this may be mt code
     * the client lib may call the global_watcher before zookeeper_init returns */

    if (path && strlen(path) > 0) {
        LOG("type=%s, state=%s, path=%s", type_to_string(type),
            state_to_string(state), path);
    } else {
        LOG("type=%s, state=%s", type_to_string(type), state_to_string(state));
    }

    ConfigManager* cm = reinterpret_cast<ConfigManager*>(context);
    cm->handle_zk_event(zzh, path, type, state);
}

void global_data_completion(int rc, const char *value, int value_len,
        const struct Stat *stat, const void *data)
{
    ConfigManager* cm = reinterpret_cast<ConfigManager*>(const_cast<void*>(data));
    cm->handle_data(rc, value, value_len, stat);
}

void global_stat_completion(int rc, const struct Stat *stat,
        const void *data)
{
    ConfigManager* cm = reinterpret_cast<ConfigManager*>(const_cast<void*>(data));
    cm->handle_stat(rc, stat);
}

ConfigManager::ConfigManager(const char* xmlpath)
    : xml_path_(xmlpath),
      zhandle_(NULL)
{
    zid_.client_id = 0;
}

void ConfigManager::start()
{
    using boost::property_tree::ptree;

    ptree pt;
    read_xml(xml_path_, pt);

    std::ostringstream oss;
    auto addresses = pt.get_child("app.config.server");
    for (auto it = addresses.begin(); it != addresses.end(); ++it) {
        oss << (it != addresses.begin() ? "," : "")
            << it->second.get_value<std::string>();
    }

    location_ = pt.get<std::string>("app.config.location");
    stype_ = pt.get<std::string>("app.config.type");
    auto timeout = pt.get<int>("app.config.timeout", DEFAULT_TIMEOUT);
    LOG("address=%s", oss.str().c_str());
    zhandle_ = zookeeper_init(oss.str().c_str(), global_watcher, timeout, &zid_, this, 0);
}

void ConfigManager::check_path(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    char path[256];
    vsprintf(path, fmt, args);
    va_end(args);
    if (zoo_aexists(zhandle_, path, 1, global_stat_completion, this)) {
        LOG("failed for %s", path);
    } else {
        pending_path_.push_back(path);
    }

}

void ConfigManager::check_exists(const char* file)
{
    char path[256];
    sprintf(path, "/%s", file);
    if (zoo_aexists(zhandle_, path, 1, global_stat_completion, this)) {
        LOG("failed for %s", path);
    } else {
        pending_path_.push_back(path);
    }

    check_path("/%s", file);
    check_path("/%s/%s", stype_.c_str(), file);
    check_path("/%s/%s/%s", location_.c_str(), stype_.c_str(), file);
}

void ConfigManager::handle_zk_event(zhandle_t* zh, const char*path, int type, int state)
{
    if (type == ZOO_SESSION_EVENT) {
        if (state == ZOO_CONNECTED_STATE) {
            const clientid_t *id = zoo_client_id(zh);
            zid_ = *id;
            check_exists("level.xml");
        } else if (state == ZOO_AUTH_FAILED_STATE || state == ZOO_EXPIRED_SESSION_STATE) {
            zookeeper_close(zh);
            zhandle_ = NULL;
        }
    } else if (type == ZOO_CREATED_EVENT || type == ZOO_CHANGED_EVENT) {
        pending_path_.push_back(path);
        zoo_aget(zh, path, 1, global_data_completion, this);
    }
}

void ConfigManager::handle_data(int rc, const char *value, int value_len,
        const struct Stat *stat)
{
    std::string path = pending_path_.front();
    pending_path_.pop_front();
    if (rc != ZOK) {
        LOG("failed to get data of %s, error=%d", path.c_str(), rc);
    } else {
        LOG("path=%s, data=%s", path.c_str(), value);
    }
}

void ConfigManager::handle_stat(int rc, const struct Stat *stat)
{
    std::string path = pending_path_.front();
    pending_path_.pop_front();
    if (rc == ZOK) {
        pending_path_.push_back(path);
        zoo_aget(zhandle_, path.c_str(), 1, global_data_completion, this);
    } else {
        LOG("path=%s, rc=%d", path.c_str(), rc);
    }
}

}

int main()
{
    zkcfg::ConfigManager cm("config.xml");
    cm.start();
    fgetc(stdin);
}

