# pragma once

#include <string>
#include <deque>
#include <boost/thread/mutex.hpp>

namespace zkcfg
{

#include <zookeeper/zookeeper.h>

class ConfigManager
{
public:
    explicit ConfigManager(const char* xmlcfg);

    void start();

    void handle_zk_event(zhandle_t* zh, const char*path, int type, int state);
    void handle_data(int rc, const char *value, int value_len, const struct Stat *stat);
    void handle_stat(int rc, const struct Stat *stat);

private:
    void check_exists(const char* file);
    void check_path(const char* fmt, ...);

    static int DEFAULT_TIMEOUT;

    std::string xml_path_;
    zhandle_t* zhandle_;
    clientid_t zid_;
    std::string location_;
    std::string stype_;
    std::deque<std::string> pending_path_;
    boost::mutex lock_;
};

}
