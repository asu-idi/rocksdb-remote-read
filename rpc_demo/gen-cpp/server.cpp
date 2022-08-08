// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

#include "Serv.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <iostream>

#include "rocksdb/db.h"
#include "rocksdb/slice.h"
#include "rocksdb/options.h"

using ROCKSDB_NAMESPACE::DB;
using ROCKSDB_NAMESPACE::Options;
using ROCKSDB_NAMESPACE::PinnableSlice;
using ROCKSDB_NAMESPACE::ReadOptions;
using ROCKSDB_NAMESPACE::FlushOptions;
using ROCKSDB_NAMESPACE::Status;
using ROCKSDB_NAMESPACE::WriteBatch;
using ROCKSDB_NAMESPACE::WriteOptions;

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

const std::string kDBPath = "/tmp/rocksdbtest-501/dbbench"; //"/Users/zhenghanghu/Desktop/rest_rpc_demo";

Options options;
DB* db_secondary = nullptr;

class ServHandler : virtual public ServIf {
 public:
  ServHandler() {
    // Your initialization goes here
  }

  void get(std::string& _return, const std::string& key) {
    //test if the overhead is caused by TryCatchUpWithPrimary()
    db_secondary->TryCatchUpWithPrimary();
    db_secondary->Get(ReadOptions(), key, &_return);
    //std::cout<<_return<<std::endl;
  }

};

int main(int argc, char **argv) {

  options.create_if_missing = true;
  const std::string kSecondaryPath = "/tmp/9090";// Secondary instance needs its own directory to store info logs (LOG)
  std::cout<<kSecondaryPath<<std::endl;

  DB::OpenAsSecondary(options, kDBPath, kSecondaryPath, &db_secondary);

  int port = 9090;//doesn't work because the db instance created need to connect to server
  ::std::shared_ptr<ServHandler> handler(new ServHandler());
  ::std::shared_ptr<TProcessor> processor(new ServProcessor(handler));
  ::std::shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  ::std::shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  ::std::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
  server.serve();

  delete db_secondary;
  return 0;
}

