#include <map>
#include <fstream>
#include <vector>
#include <string.h>
#include <algorithm>
#include <sstream>
#include <utility>
#include "types.h"
#include "stream.h"

using namespace std;

# pragma pack(push, 1)
struct FileHeader
{
    string data_type;
    int date; // YYYYMMDD

    template <typename Stream>
    static FileHeader from_stream(Stream& is)
    {
        FileHeader fh;
        is.read(fh.data_type);
        is.read(fh.date);
        return fh;
    }
};

struct RtMinData
{
    struct Item
    {
        int date_time; // yyMMddHHmm
        int open_price;
        int highest_price;
        int lowest_price;
        int close_price;
        int avg_price;
        LargeInt volume;
        LargeInt amount;
        uint tick_count;

        template <typename Stream>
        static Item from_stream(Stream& is)
        {
            Item item;
            is.read(item.date_time);
            is.read(item.open_price);
            is.read(item.highest_price);
            is.read(item.lowest_price);
            is.read(item.close_price);
            is.read(item.avg_price);
            item.volume = LargeInt::from_stream(is);
            item.amount = LargeInt::from_stream(is);
            is.read(item.tick_count);
        }
    };

    string stock_id; // no market
    ushort total_num; // 数据总数
    ushort call_auction_num; // 集合竞价数据总数
    vector<Item> data; // 分时成交数据

    template <typename Stream>
    static RtMinData from_stream(Stream& is)
    {
        RtMinData rtmin;
        is.read(rtmin.stock_id);
        is.read(rtmin.total_num);
        is.read(rtmin.call_auction_num);
        ushort size = is.template read<ushort>();
        for (int i = 0; i < size; i++) {
            rtmin.data.push_back(Item::from_stream(is));
        }
        return rtmin;
    }
};

struct QuoteData
{
    using Price = Digit<3>;
    using AvgPrice = Digit<4>;
    string stock_id;
    string stock_name;
    Price current_price;
    AvgPrice avg_price;
    Price open_price;
    Price last_close;
    Price highest_price;
    Price lowest_price;
    Digit<4> volume_ratio;
    int turn_over_ratio; // 5 digits, display 2
    LargeInt volume;
    LargeInt amount;
    Price zhangting;
    Price dieting;
    int wai_pan;
    int nei_pan;
    Digit<3> dynamic_pe;
    LargeInt tradable_value; // 流通市值(万元)
    Digit<4> income;
    uchar quarter;
    int total_value; // 总市值(万元)
    int tradable_share; // 流通股本(万股)
    int total_share; // 总股本(万股)
    Digit<3> pe;
    Digit<3> eps;
    Digit<2> pb;
    string block_code; // 所属板块代码
    string block_name; // 所属板块名称
    string block_rise; // 板块涨幅, 4 digits, display 2

    template <typename Stream>
    static QuoteData from_stream(Stream& is)
    {
        QuoteData quote;
        is.read(quote.stock_id);
        is.read(quote.stock_name);
        is.read(quote.current_price);
        is.read(quote.avg_price);
        is.read(quote.open_price);
        is.read(quote.last_close);
        is.read(quote.highest_price);
        is.read(quote.lowest_price);
        is.read(quote.volume_ratio);
        is.read(quote.turn_over_ratio);
        is.read(quote.volume);
        is.read(quote.amount);
        is.read(quote.zhangting);
        is.read(quote.dieting);
        is.read(quote.wai_pan);
        is.read(quote.nei_pan);
        is.read(quote.dynamic_pe);
        is.read(quote.tradable_value);
        is.read(quote.income);
        is.read(quote.quarter);
        is.read(quote.total_value);
        is.read(quote.tradable_share);
        is.read(quote.total_share);
        is.read(quote.pe);
        is.read(quote.eps);
        is.read(quote.pe);
        is.read(quote.block_code);
        is.read(quote.block_name);
        is.read(quote.block_rise);

        return quote;
    }
};

# pragma pack(pop)

template <typename DataType>
struct File {
    FileHeader header_;
    std::map<std::string, DataType> data_;

    template <typename Stream>
    static File<DataType> from_stream(Stream& is)
    {
        File<DataType> file_data;
        file_data.header_ = FileHeader::from_stream(is);
        while (!is.eof()) {
            DataType elem = DataType::from_stream(is);
            file_data.data_[elem.stock_id] = elem;
        }
        return file_data;
    }
};

template <typename DataType>
File<DataType> read(const std::string& filename)
{
    util::InputFileStream ifs { filename };

    return File<DataType>::from_stream(ifs);
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        cerr << "usage: " << argv[0] << " dat-file\n";
        return 1;
    }

    auto rtmin = read<RtMinData>(argv[1]);
    auto quote = read<QuoteData>(argv[2]);

    return 0;
}
