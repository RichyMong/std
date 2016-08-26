#include <map>
#include <fstream>
#include <vector>
#include <string.h>
#include <algorithm>
#include <sstream>
#include <utility>
#include "types.h"
#include "stream.h"

constexpr size_t CODE_NAME_LENGTH = 32;
constexpr size_t DF_BLOCK_SIZE = 8192;
constexpr size_t DF_BLOCKS_GROWBY = 64;
constexpr size_t DF_MAX_GOODSNUM = 21840;

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

# pragma pack(pop)

template <typename _Type>
struct DatFile {
    SummaryInfo summary;
    GoodsInfo goods[DF_MAX_GOODSNUM];
    std::vector<_Type> data;
};

template <typename T>
void put_elem(ptree& pt, const T& v)
{
    ptree tmp;
    tmp.put("", to_string(v));
    pt.push_back(std::make_pair("", tmp));
}

template <typename _Type>
bool read(const std::string& filename, DatFile<_Type>& file)
{
    std::fstream fs { filename, std::ios::binary | std::ios::in };

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
            auto nr_read = std::min(nr_data, nr_per_block);
            fs.seekg(block_id * DF_BLOCK_SIZE);
            fs.read(block, sizeof(block));
            memcpy(&block_id, block, sizeof(block_id));
            auto start = (_Type*) (block + sizeof(block_id));
            std::copy(start, start + nr_read, back_inserter(file.data));
            nr_data -= nr_read;
            if (block_id == 0 || block_id > x.last_block_id) {
                break;
            }
        }
    }

    std::ostringstream json_file;
    auto slash = filename.rfind('/');
    auto index = filename.rfind('.');
    if (slash != std::string::npos && index != std::string::npos) {
        json_file << std::string(filename, slash + 1, index - slash - 1);
    } else if (slash != std::string::npos) {
        json_file << std::string(filename, slash + 1);
    } else {
        json_file << std::string(filename, 0, index);
    }

    json_file << ".json";

    std::fstream ofs(json_file.str(), std::ios::out);

    return true;
}
