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

bool fetch_item(Item* item, int n)
{
    sprintf(item->buffer, "item%d", n);
    return true;
}

int main(int argc, char** argv)
{
    const size_t count = 8192;
    const size_t size = count * sizeof(Item);

    auto start_tp = steady_clock::now();
    FileHeader fh;
    sprintf(fh.type, "file-test");
    fh.item_size = count;
    if (argc < 2) {
        vector<Item> items(8192);
        for (size_t i = 0; i < items.size(); i++) {
            fetch_item(&items[i], i);
        }
        ofstream ofs(FILE_NAME, ofstream::out | ofstream::trunc);
        ofs.write(reinterpret_cast<char*>(&fh), sizeof(fh));
        ofs.write(reinterpret_cast<char*>(&*items.begin()), size);
    } else {
        int fd = open(FILE_NAME, O_RDWR | O_CREAT, 0644);
        if (fd < 0) {
            cerr << "Failed to open file: " << errno << endl;
            exit(1);
        }
        size_t mmap_size = size + sizeof(FileHeader);
        ftruncate(fd, mmap_size);
        void* addr = mmap(NULL, mmap_size, PROT_WRITE, MAP_SHARED, fd, 0);
        if (addr == MAP_FAILED) {
            cerr << "mmap error: " << strerror(errno) << endl;
            exit(2);
        }
        madvise(addr, mmap_size, MADV_SEQUENTIAL);
        *reinterpret_cast<FileHeader*>(addr) = fh;
        Item* items = reinterpret_cast<Item*>((char*) addr + sizeof(fh));
        for (size_t i = 0; i < count; i++) {
            fetch_item(items + i, i);
        }

        munmap(addr, mmap_size);
        close(fd);
    }

    cout << "use " << duration_cast<microseconds>(steady_clock::now() - start_tp).count() << endl;
}
