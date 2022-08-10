#include "Serv.h"
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <iostream>

#include "rocksdb/db.h"
#include "rocksdb/slice.h"
#include "rocksdb/options.h"

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

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

int main(int argc, char **argv) {
    std::shared_ptr<TSocket> socket(new TSocket("localhost", 9090)); //注意此处的ip和端口
    std::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    std::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    transport->open();

    ServClient client(protocol);
    
    options.create_if_missing = true;
    options.create_with_thrift_client = true;
    DB::Open(options, kDBPath, &db);// open DB

    rocksdb::Iterator* it = db->NewIterator(rocksdb::ReadOptions());
    for (it->SeekToFirst(); it->Valid(); it->Next()){
        std::cout<< it->key().ToString() <<" "<<it->value().ToString()<<std::endl;
    }
    delete it;

    db->Put(WriteOptions(), "zz", "70");// Put key-value
    std::string result;
    client.get(result, "zz");
    std::cout<<result<<" "<<std::endl;

    delete db;

    transport->close();
    return 0;
}
// g++ -std=c++11 Serv.cpp Serv_server.skeleton.cpp -o server -lthrift 

// g++ -std=c++11 Serv.cpp client.cpp -o client -lthrift
