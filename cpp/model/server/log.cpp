#include "log.h"
#include "time.h"
#include <sstream>
#include <stdexcept>
#include <cstdio>
#include <cstdarg>
#include <unistd.h>
#include <string.h>
#include <assert.h>

namespace util {

#define do_output(_level, _level_str, _fmt)          \
    do {                                             \
        if (level_ < (_level)) {                     \
            return;                                  \
        }                                            \
        va_list varg;                                \
        va_start(varg, _fmt);                        \
        output(_level_str, _fmt, varg);              \
        va_end(varg);                                \
    } while (0)                                      \


thread_local std::string Log::log_tag = "not set";

Log::Log(const std::string& path) {
    file_ = fopen(path.c_str(), "a");
    if (file_ == NULL) {
        throw std::runtime_error("cannot create log file");
    }
}

Log::Log(FILE* file)
    : file_ {file} {
}

Log::~Log() {
    fsync(fileno(file_));
    fclose(file_);
}

void Log::output(const char* level, const char* fmt, va_list varg) {
    assert(fmt && fmt[0]);

    char vargs[BUFSIZ];
    (void) vsnprintf(vargs, sizeof(vargs), fmt, varg);

    char buf[BUFSIZ];
    auto nwrite = snprintf(buf, sizeof(buf), "%s [%s] <%s> %s\n",
                           cached_log_time, Log::log_tag.c_str(),
                           level, vargs);

    if (nwrite >= (int) sizeof(buf)) {
        auto newline = buf + sizeof(buf) - 2;
        *newline = '\n';
        auto pos = strrchr(buf, ' '); // pos cannot be NULL
        while (pos != newline) {
            *pos++ = '.';
        }
        nwrite = sizeof(buf);
    }

    // fwrite is MT-Safe so we do not use any lock here.
    fwrite(buf, nwrite, 1, file_);
}

void Log::debug(const char* fmt, ...) {
    do_output(LogLevel::kDebug, "D", fmt);
}

void Log::warning(const char* fmt, ...) {
    do_output(LogLevel::kWarn, "W", fmt);
}

void Log::info(const char* fmt, ...) {
    do_output(LogLevel::kInfo, "I", fmt);
}

void Log::error(const char* fmt, ...) {
    do_output(LogLevel::kError, "E", fmt);
}

}
