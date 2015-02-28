#include <string>
#include <iostream>
#include <leveldb/db.h>

int main()
{
    leveldb::DB* db;
    leveldb::Options options;
    auto status = leveldb::DB::Open(options, "test.db", &db);
    if (status.ok()) {
        leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
        for (it->SeekToFirst(); it->Valid(); it->Next()) {
            std::cout << it->key().ToString() << " - "
                      << it->value().ToString() << std::endl;
        }
        delete it;
        delete db;
    }
}
