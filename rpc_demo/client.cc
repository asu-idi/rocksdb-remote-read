#include <chrono>
#include <fstream>
#include <iostream>
#include <rest_rpc.hpp>
#include <cstdio>
#include <string>

#include "rocksdb/db.h"
#include "rocksdb/slice.h"
#include "rocksdb/options.h"

using namespace rest_rpc;
using namespace rest_rpc::rpc_service;
using namespace std;
using ROCKSDB_NAMESPACE::DB;
using ROCKSDB_NAMESPACE::Options;
using ROCKSDB_NAMESPACE::PinnableSlice;
using ROCKSDB_NAMESPACE::ReadOptions;
using ROCKSDB_NAMESPACE::FlushOptions;
using ROCKSDB_NAMESPACE::Status;
using ROCKSDB_NAMESPACE::WriteBatch;
using ROCKSDB_NAMESPACE::WriteOptions;

std::string kDBPath = "/Users/zhenghanghu/Desktop/rest_rpc_demo";
DB* db;
Options options;

int main() {//primary instance

  options.create_if_missing = true;
  DB::Open(options, kDBPath, &db);// open DB

  rocksdb::Iterator* it = db->NewIterator(rocksdb::ReadOptions());
  for (it->SeekToFirst(); it->Valid(); it->Next()){
    cout<< it->key().ToString() <<" "<<it->value().ToString()<<endl;
  }
  delete it;

  db->Put(WriteOptions(), "zz", "70");// Put key-value
  //db->Put(WriteOptions(), "xx", "2");
  //db->Put(WriteOptions(), "yy", "3");
  //db->Flush(FlushOptions());

  try {
    rpc_client client("127.0.0.1", 9000);
    bool r = client.connect();
    if (!r) {
      std::cout << "connect timeout" << std::endl;
      return 0;
    }

    string result = client.call<std::string>("get_value", "zz");
    cout<<result<<endl;
  } catch (const std::exception &e) {
    std::cout << e.what() << std::endl;
  }

  delete db;
  return 0;
}

