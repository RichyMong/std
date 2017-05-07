#include <fstream>
#include <chrono>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <unistd.h>
#include <string.h>
#include <assert.h>
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

int main(int argc, char** argv)
{
    const size_t count = 8192;
    const size_t size = count * sizeof(Item);

    auto start_tp = steady_clock::now();
    struct stat fst;
    if (stat(FILE_NAME, &fst) < 0) {
        cerr << "failed to stat " << FILE_NAME << ": " << strerror(errno) << endl;
        exit(1);
    }
    vector<Item> items;
    items.reserve(fst.st_size / sizeof(Item));
    assert(fst.st_size % sizeof(Item) == 0);
    if (argc < 2) {
        items.resize(fst.st_size / sizeof(Item));
        ifstream ifs(FILE_NAME, ofstream::in);
        ifs.read(reinterpret_cast<char*>(&*items.begin()), size);
    } else {
        int fd = open(FILE_NAME, O_RDONLY);
        if (fd < 0) {
            cerr << "Failed to open file: " << errno << endl;
            exit(1);
        }
        Item* addr = reinterpret_cast<Item*>(mmap(NULL,
                                              fst.st_size,
                                              PROT_READ,
                                              MAP_SHARED,
                                              fd,
                                              0));
        if (addr == MAP_FAILED) {
            cerr << "mmap error: " << strerror(errno) << endl;
            exit(2);
        }

        std::copy(addr, addr + items.capacity(), back_inserter(items));
        munmap(addr, size);

        close(fd);
    }

    cout << "use " << duration_cast<microseconds>(steady_clock::now() - start_tp).count() << endl;
}
