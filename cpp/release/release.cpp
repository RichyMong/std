#include "util.h"
#include "epoll.h"
#include "login.h"
#include <thread>
#include <chrono>

void worker_thread(release::Login* lclient) {
    while (true) {
        lclient->random_action();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main(int argc, char* argv[]) {
    release::Login lclient("arcrobot_0001", "123456", "en", "arcversion:V99.99.99.9999 OS:Centos");
    util::Epoll epoll;
    lclient.connect("127.0.0.1:9881");
    std::thread tid (&worker_thread, &lclient);
    epoll.add(lclient.sockfd(), EPOLLIN | EPOLLOUT | EPOLLONESHOT, &lclient);
    epoll.mainloop(-1);
    return 0;
}
