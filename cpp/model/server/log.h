#ifndef MODEL_SERVER_LOG_H
#define MODEL_SERVER_LOG_H

#include <string>

namespace util {

class Log {
public:
    explicit Log(const std::string& path);

    explicit Log(FILE* file);

    ~Log();

    void debug(const char* fmt, ...);

    void warning(const char* fmt, ...);

private:
    FILE* file_;
};

}

#endif // MODEL_SERVER_LOG_H
