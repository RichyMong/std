#include <fstream>
#include <chrono>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

using namespace std;
using namespace std::chrono;

struct Item
{
    char buffer[1024];
};

static const char* FILE_NAME = "file_writer_test";

bool fetch_item(Item* item)
{
    item->buffer[0] = 'a';
    item->buffer[1] = '\0';
    return true;
}

int main(int argc, char** argv)
{
    const size_t count = 8192;
    const size_t size = count * sizeof(Item);

    auto start_tp = steady_clock::now();
    if (argc < 2) {
        vector<Item> items(8192);
        for (size_t i = 0; i < items.size(); i++) {
            fetch_item(&items[i]);
        }
        ofstream ofs(FILE_NAME, ofstream::out | ofstream::trunc);
        ofs.write(reinterpret_cast<char*>(&*items.begin()), size);
    } else {
        int fd = open(FILE_NAME, O_RDWR | O_CREAT, 0644);
        if (fd < 0) {
            cerr << "Failed to open file: " << errno << endl;
            exit(1);
        }
        ftruncate(fd, size);
        Item* items = reinterpret_cast<Item*>(mmap(NULL,
                                              size,
                                              PROT_WRITE,
                                              MAP_PRIVATE,
                                              fd,
                                              0));
        if (items == MAP_FAILED) {
            cerr << "mmap error: " << strerror(errno) << endl;
            exit(2);
        }
        for (size_t i = 0; i < count; i++) {
            fetch_item(items + i);
        }

        munmap(items, size);
        close(fd);
    }

    cout << "use " << duration_cast<microseconds>(steady_clock::now() - start_tp).count() << endl;
}
