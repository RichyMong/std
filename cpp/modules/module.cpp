#include "module.h"
#include <iostream>
#include <algorithm>

namespace meng {

bool ModuleManager::regist(const std::string &name, std::shared_ptr<Module> module)
{
    auto it = modules_.find(name);
    if (it != modules_.cend()) {
        std::cerr << name << " has already registered" << std::endl;
        return false;
    }
    modules_.insert(std::make_pair(name, module));

    return true;
}

void ModuleManager::unregist(const std::string &name)
{
    if (modules_.erase(name)) {
        std::cout << "unregistered " << name << std::endl;
    }
}

void ModuleManager::work() const
{
    for_each(modules_.cbegin(), modules_.cend(), [] (const auto &it) { it.second->work(); });
}

}
