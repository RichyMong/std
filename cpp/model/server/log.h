#ifndef MODEL_SERVER_LOG_H
#define MODEL_SERVER_LOG_H

#include <memory>
#include <string>
#include <cstdarg>

namespace util {

enum class LogLevel : int {
    kEmerg = 0,
    kAlert,
    kError,
    kWarn,
    kInfo,
    kDebug
};

class Log {
public:
    explicit Log(const std::string& path);

    explicit Log(FILE* file);

    ~Log();

    void debug(const char* fmt, ...);

    void warning(const char* fmt, ...);

    void info(const char* fmt, ...);

    void error(const char* fmt, ...);

    static void set_log_tag(const std::string& tag) {
        Log::log_tag = tag;
    }

private:
    static thread_local std::string log_tag;

    void output(const char* level, const char* fmt, va_list varg);

    FILE*    file_;
    LogLevel level_ = LogLevel::kDebug;
};

typedef std::shared_ptr<util::Log> LogPtr;

}

#endif // MODEL_SERVER_LOG_H
