#include <map>
#include <fstream>
#include <vector>
#include <string.h>
#include <algorithm>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <sstream>
#include <utility>
#include "types.h"
#include "stream.h"

constexpr size_t CODE_NAME_LENGTH = 32;
constexpr size_t DF_BLOCK_SIZE = 8192;
constexpr size_t DF_BLOCKS_GROWBY = 64;
constexpr size_t DF_MAX_GOODSNUM = 21840;

using namespace std;
using boost::property_tree::ptree;
using boost::property_tree::write_json;

# pragma pack(push, 1)

struct RtMin {
    char stock_code[32];
    uint wTime;
    uint dwOpen;
    uint dwHigh;
    uint dwLow;
    uint dwClose;
    uint dwAve;
    BigInt xvolume;
    BigInt xamount;
    BigInt xTradeNum;
    BigInt xWaiPan;
    BigInt xExt1;
    BigInt xExt2;
    uint dwExt1;
    uint dwExt2;
};

struct SummaryInfo {
    char header[32];
    uint version;
    uint nr_total_blocks;
    uint nr_used_blocks;
    uint nr_goods;
    byte reserved[208];
};

struct GoodsInfo {
    char name[CODE_NAME_LENGTH];
    uint nr_data;
    uint first_block_id;
    uint next_block_id;
    uint last_block_id;
    uint reserved[6];
};

std::ostream& operator<<(std::ostream& os, const GoodsInfo& gi)
{
    os << gi.name << " " << gi.nr_data << " " << gi.first_block_id << " "
       << gi.next_block_id << " " << gi.last_block_id;
    return os;
}

template <typename T>
void put_elem(ptree& pt, const T& v)
{
    ptree tmp;
    tmp.put("", to_string(v));
    pt.push_back(std::make_pair("", tmp));
}

void put_elem(ptree& pt, const RtMin& x)
{
    put_elem(pt, x.wTime);
    put_elem(pt, x.dwOpen);
    put_elem(pt, x.dwHigh);
    put_elem(pt, x.dwLow);
    put_elem(pt, x.dwClose);
    put_elem(pt, x.dwAve);
    put_elem(pt, x.xvolume);
    put_elem(pt, x.xamount);
    put_elem(pt, x.xTradeNum);
    put_elem(pt, x.xWaiPan);
    put_elem(pt, x.xExt1);
    put_elem(pt, x.xExt2);
    put_elem(pt, x.dwExt1);
    put_elem(pt, x.dwExt2);
}

# pragma pack(pop)

template <typename _Type>
struct File {
    SummaryInfo summary;
    GoodsInfo goods[DF_MAX_GOODSNUM];
    std::map<std::string, std::vector<_Type>> data;
};

template <typename _Type>
bool read(const std::string& filename, File<_Type>& file)
{
    fstream fs { filename, ios::binary | ios::in };

    fs.seekg(0, fs.end);
    auto length = fs.tellg();
    constexpr size_t header_size = sizeof(file.summary) + sizeof(file.goods);
    if (length < header_size) {
        return false;
    }

    byte buffer[header_size];
    fs.seekg(0, fs.beg);
    fs.read(reinterpret_cast<char*>(buffer), header_size);

    util::InputBufferStream ibs(buffer, header_size);
    ibs.read(file.summary);

    length -= header_size;
    uint nr_per_block = (DF_BLOCK_SIZE - 4) / sizeof(_Type);
    for (size_t i = 0; i < file.summary.nr_goods; i++) {
        ibs.read(file.goods[i]);

        const auto& x = file.goods[i];
        uint nr_data = x.nr_data;
        if (!nr_data) {
            continue;
        }

        uint block_id = x.first_block_id;
        while (nr_data) {
            char block[DF_BLOCK_SIZE];
            auto nr_read = min(nr_data, nr_per_block);
            fs.seekg(block_id * DF_BLOCK_SIZE);
            fs.read(block, sizeof(block));
            memcpy(&block_id, block, sizeof(block_id));
            auto start = (_Type*) (block + sizeof(block_id));
            copy(start, start + nr_read, back_inserter(file.data[x.name]));
            nr_data -= nr_read;
            if (block_id == 0 || block_id > x.last_block_id) {
                break;
            }
        }
    }

    // use boost json, the types of the nodes are lost. All of them will become
    // strings.
    ptree pt;
    for (auto it = file.data.begin(); it != file.data.end(); ++it) {
        ptree stock_array;
        for (const auto& x : it->second) {
            ptree tmp;
            put_elem(tmp, x);
            stock_array.push_back(std::make_pair("", tmp));
        }

        pt.push_back(std::make_pair(it->first, stock_array));
    }


    std::ostringstream json_file;
    auto slash = filename.rfind('/');
    auto index = filename.rfind('.');
    if (slash != std::string::npos && index != std::string::npos) {
        json_file << std::string(filename, slash + 1, index - slash - 1);
    } else if (slash != string::npos) {
        json_file << std::string(filename, slash + 1);
    } else {
        json_file << std::string(filename, 0, index);
    }

    json_file << ".json";

    fstream ofs(json_file.str(), ios::out);
    write_json(ofs, pt, true);

    return true;
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        cerr << "usage: " << argv[0] << " dat-file\n";
        return 1;
    }

    File<RtMin> dat;
    read(argv[1], dat);

    return 0;
}
