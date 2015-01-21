#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <json/json.h>

namespace {

struct Status {
    Status();
    Status(const Status &) = delete;
    std::string get();
    const time_t uptime;
    std::ifstream is;
    Json::FastWriter writer;
};

Status::Status() : uptime (time(NULL))
{
    is.open("/proc/self/status");
    if (is.fail())
        std::cout << "failed to open file" << std::endl;
    std::cout << is.is_open() << std::endl;
    writer.omitEndingLineFeed();
}

std::string Status::get()
{
    is.seekg(0, is.beg);

    Json::Value memory;

    std::string line;
    while (std::getline(is, line)) {
        auto pos = line.find(':');
        if (pos != std::string::npos) {
            memory[line.substr(0, pos)] = line.substr(line.find_first_not_of(" \t", pos + 1));
        }
    }

    if (is.eof())
       is.clear();

    Json::Value info;
    info["uptime"] = (unsigned int) (time(NULL) - uptime);
    info["memory"] = memory;

    return writer.write(info);
}

}

int main()
{
    Status s;
    for (int i = 0; i != 2; ++i) {
        std::cout << "--------------------" << std::endl;
        std::cout << s.get() << std::endl;
        sleep(2);
    }
}
