#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <vector>
#include <algorithm>

using namespace std;

class FileCopy {
public:
    FileCopy(const std::string& src_path, const std::string& dst_path)
        : in_stream { src_path, ifstream::binary },
          out_stream { dst_path, ofstream::binary }
    {
    }

    void do_copy(off_t pos, size_t size)
    {
        std::lock_guard<mutex> guard { file_lock };
        cout << pos << ", " << size << endl;
        in_stream.seekg(pos);
        char* buffer = new char[size];
        in_stream.read(buffer, size);
        out_stream.write(buffer, size);
        delete[] buffer;
    }

    void start()
    {
        auto cpu_count = std::thread::hardware_concurrency();
        vector<thread> threads;
        in_stream.seekg(0, in_stream.end);
        size_t remain_size = in_stream.tellg();
        auto chunk_size = (remain_size + cpu_count - 1) / cpu_count;
        for (unsigned int i = 0; i != cpu_count; ++i) {
            auto thread_size = (remain_size > chunk_size ? chunk_size : remain_size);
            threads.push_back(thread { &FileCopy::do_copy, this, i * chunk_size,
                    thread_size });
            remain_size -= thread_size;
        }

        for_each(threads.begin(), threads.end(), mem_fn(&thread::join));
    }

private:
    ifstream in_stream;
    ofstream out_stream;
    mutex file_lock;
};

int main(int argc, char** argv)
{
    if (argc < 2) {
        cerr << "usage: " << argv[0] << " <source file> [target file]" << endl;
        return 1;
    }

    auto src_file = string(argv[1]);
    FileCopy file_copy { src_file, argc > 2 ? argv[2] : src_file + ".copy" };
    file_copy.start();
    return 0;
}
