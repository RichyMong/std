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

struct FileHeader
{
    char type[16];
    int item_size;
};

struct Item
{
    char buffer[1024];
};

static const char* FILE_NAME = "file_writer_test";

int main(int argc, char** argv)
{
    auto start_tp = steady_clock::now();
    struct stat fst;
    if (stat(FILE_NAME, &fst) < 0) {
        cerr << "failed to stat " << FILE_NAME << ": " << strerror(errno) << endl;
        exit(1);
    }

    vector<Item> items;
    if (argc < 2) {
        ifstream ifs(FILE_NAME, ofstream::in);
        FileHeader fh;
        ifs.read(reinterpret_cast<char*>(&fh), sizeof(fh));
        cout << "FileType: " << fh.type << ", item size: " << fh.item_size << endl;
        assert(fst.st_size = (sizeof(fh) + fh.item_size * sizeof(Item)));
        items.resize(fh.item_size);
        ifs.read(reinterpret_cast<char*>(&*items.begin()),
                 fh.item_size * sizeof(Item));
    } else {
        int fd = open(FILE_NAME, O_RDONLY);
        if (fd < 0) {
            cerr << "Failed to open file: " << errno << endl;
            exit(1);
        }
        void* addr = mmap(NULL, fst.st_size, PROT_READ, MAP_PRIVATE | MAP_POPULATE, fd, 0);
        if (addr == MAP_FAILED) {
            cerr << "mmap error: " << strerror(errno) << endl;
            exit(2);
        }

        madvise(addr, fst.st_size, MADV_SEQUENTIAL);
        FileHeader* fh = reinterpret_cast<FileHeader*>(addr);
        cout << "FileType: " << fh->type << ", item size: " << fh->item_size << endl;
        assert(fst.st_size = (sizeof(*fh) + fh->item_size * sizeof(Item)));
        Item* src = reinterpret_cast<Item*>(fh + 1);
        items.resize(fh->item_size);
        mempcpy(&*items.begin(), src, fst.st_size - sizeof(fh));
        munmap(addr, fst.st_size);

        close(fd);
    }

    cout << "use " << duration_cast<microseconds>(steady_clock::now() - start_tp).count() << endl;
}
