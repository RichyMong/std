#include <iostream>
#include <etcd/Client.hpp>

int main()
{
    etcd::Client etcd("http://127.0.0.1:2379");
    etcd::Response response = etcd.get("/mine/front/sh/info.xml").get();
    std::cout << response.value().as_string();

    return 0;
}
