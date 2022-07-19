// Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).

#include <cstdio>
#include <string>
#include<iostream>
#include <rest_rpc.hpp>
#include <fstream>

#include "rocksdb/db.h"
#include "rocksdb/slice.h"
#include "rocksdb/options.h"

using namespace std;
using ROCKSDB_NAMESPACE::DB;
using ROCKSDB_NAMESPACE::Options;
using ROCKSDB_NAMESPACE::PinnableSlice;
using ROCKSDB_NAMESPACE::ReadOptions;
using ROCKSDB_NAMESPACE::Status;
using ROCKSDB_NAMESPACE::WriteBatch;
using ROCKSDB_NAMESPACE::WriteOptions;
using namespace rest_rpc;
using namespace rpc_service;

std::string kDBPath = "/Users/zhenghanghu/Desktop/rest_rpc_demo";
DB* db_naive;
DB* db;
Options options;

std::string get_value_naive(rpc_conn conn, string key) {
  string value;
  Status s = db_naive->Get(ReadOptions(), key, &value);// get value
  return value; 
}

std::string get_value_catch_up(rpc_conn conn, string key) {

  DB::OpenForReadOnly(options, kDBPath, &db);// open DB
  string value;
  db->Get(ReadOptions(), key, &value);// get value
  delete db;

  return value; 

}

int main() {//read-only instance


  options.create_if_missing = true;
  DB::OpenForReadOnly(options, kDBPath, &db_naive);// open DB

  rpc_server server(9000, std::thread::hardware_concurrency());
  server.register_handler("get_value_naive", get_value_naive);
  server.register_handler("get_value_catch_up", get_value_catch_up);

  server.run();
  delete db_naive;

  return 0;
}
