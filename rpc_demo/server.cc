// Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).

#include <cstdio>
#include <string>
#include<iostream>
#include <rest_rpc.hpp>
#include <fstream>
#include "util/gflags_compat.h"

#include "rocksdb/db.h"
#include "rocksdb/slice.h"
#include "rocksdb/options.h"

using GFLAGS_NAMESPACE::ParseCommandLineFlags;
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

DEFINE_int32(port, 9000, "port to deploy the service");

const std::string kDBPath = "/tmp/rocksdbtest-501/dbbench"; // "/Users/zhenghanghu/Desktop/rest_rpc_demo";

Options options;
DB* db_secondary = nullptr;

std::string get_value(rpc_conn conn, string key) {

  db_secondary->TryCatchUpWithPrimary();
  std::string value;
  db_secondary->Get(ReadOptions(), key, &value); 

  return value; 
}

void async_get_value(rpc_conn conn, string key) {

  auto req_id = conn.lock()->request_id(); // note: you need keep the request id at that
                                 // time, and pass it into the async thread
  std::thread thd([conn, req_id, key] {
    //std::this_thread::sleep_for(std::chrono::seconds(1));

    db_secondary->TryCatchUpWithPrimary();
    std::string value;
    db_secondary->Get(ReadOptions(), key, &value); 

    auto conn_sp = conn.lock();
    if (conn_sp) {
      conn_sp->pack_and_response(req_id, std::move(value));
    }
  });
  thd.detach();
}

int main(int argc, char** argv) {

  ParseCommandLineFlags(&argc, &argv, true);

  options.create_if_missing = true;
  const std::string kSecondaryPath = "/tmp/" + to_string(FLAGS_port);// Secondary instance needs its own directory to store info logs (LOG)
  cout<<kSecondaryPath<<endl;

  DB::OpenAsSecondary(options, kDBPath, kSecondaryPath, &db_secondary);
  
  rpc_server server(9000, std::thread::hardware_concurrency());
  cout<<std::thread::hardware_concurrency()<<endl;//8
  server.register_handler("get_value", get_value);
  server.register_handler<Async>("async_get_value", async_get_value);

  server.run();
  delete db_secondary;

  return 0;
}
