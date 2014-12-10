# pragma once

struct module_t {
    const char *m_name;
    int         m_priority;
};

extern void register_module(const char *name);
extern void unregister_module(const char *name);
