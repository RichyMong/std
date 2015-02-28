#include <string>
#include <leveldb/db.h>

int main()
{
    leveldb::DB* db;
    leveldb::Options options;
    options.create_if_missing = true;
    auto status = leveldb::DB::Open(options, "test.db", &db);
    if (!status.ok()) {
        return 1;
    } else {
        db->Put(leveldb::WriteOptions(),"meng", "Richy Meng");
        delete db;
    }
}
