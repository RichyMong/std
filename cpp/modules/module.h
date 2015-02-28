# pragma once

#include "util.h"
#include <string>
#include <map>
#include <memory>

namespace meng {

class Module {
public:
    virtual ~Module() = default;
    virtual void work() = 0;
};

class ModuleManager : public util::SingleSton<ModuleManager> {
    using ModulesMap = std::map<std::string, std::shared_ptr<Module>>;
public:
    ModuleManager() = default;

    bool regist(const std::string &name, std::shared_ptr<Module> module);
    void unregist(const std::string &name);
    void work() const;

private:
    ModulesMap modules_;
};

}
