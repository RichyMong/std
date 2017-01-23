#include <map>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <utility>
#include <cstdio>
#include <string.h>
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
            return item;
        }
    };

    string stock_id; // no market
    ushort total_num; // 数据总数
    ushort call_auction_num; // 集合竞价数据总数
    vector<Item> data; // 分时成交数据

    const string& get_stock_key() const { return stock_id; }

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
    int turn_over_ratio; // 0.01 is 1000
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
    int block_rise; // 板块涨幅, 4 digits, display 2

    string get_stock_key() const { return stock_id.substr(2); }

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
            file_data.data_[elem.get_stock_key()] = elem;
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

void select_from_rtmin_and_quote(const RtMinData& rm, const QuoteData& qt)
{
    auto last_close = qt.last_close;
    if (!last_close) {
        return;
    }

    auto total = rm.data.size();
    if (qt.turn_over_ratio < 200) { // turn over ratio < 1%
        printf("turn_over_ratio too small: %d\n", qt.turn_over_ratio);
        return;
    }

    auto up = 0;
    auto down = 0;
    auto small_tr = 0;
    auto huge_tr = 0;
    long net_buy = 0;
    for (size_t i = 15; i < total; i++) {
        auto& data = rm.data[i];
        long int volume = rm.data[i].volume;
        auto turn_over = volume / (qt.tradable_share * 100.0);
        if (turn_over < 0.00003) {
            ++small_tr;
        } else if (turn_over > 0.01) {
            ++huge_tr;
        }
        if (rm.data[i].close_price >= rm.data[i - 1].close_price) {
            net_buy += data.volume;
            ++up;
        } else {
            net_buy -= data.volume;
            ++down;
        }
    }

    printf("small hr=%d, huge_tr=%d\n", small_tr, huge_tr);
    auto ten_percent = 0.1 * total;
    if (small_tr >= ten_percent || huge_tr >= ten_percent) {
        return;
    }

    if (up > (down * 2.5 + 1) || net_buy > 0) {
        printf("select stock %s\n", qt.stock_id.c_str());
    }
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        cerr << "usage: " << argv[0] << " dat-file\n";
        return 1;
    }

    auto rtmin = read<RtMinData>(argv[1]);
    auto quote = read<QuoteData>(argv[2]);
    for (auto it = rtmin.data_.begin(); it != rtmin.data_.end(); ++it) {
        // auto qit = quote.data_.find(it->first);
        auto qit = quote.data_.find("600119");
        if (qit != quote.data_.end()) {
            select_from_rtmin_and_quote(rtmin.data_["600119"], qit->second);
        }
        return 0;
    }

    return 0;
}
