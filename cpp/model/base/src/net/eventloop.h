# pragma once

#include "fileobj.h"

namespace net {

class BaseEventLoop {
public:
    virtual void add_reader(FileObj* obj) = 0;
    virtual void add_writer(FileObj* obj) = 0;
};

}
