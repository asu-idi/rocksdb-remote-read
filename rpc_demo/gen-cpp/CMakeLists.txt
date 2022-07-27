add_executable(thrift_server Serv.cpp server.cpp)
target_link_libraries(thrift_server ${ROCKSDB_LIB} ${GFLAGS_LIB})

add_executable(thrift_client Serv.cpp client.cpp)
target_link_libraries(thrift_client ${ROCKSDB_LIB} ${GFLAGS_LIB})

# g++ -std=c++11 Serv.cpp Serv_server.skeleton.cpp -o server -lthrift 

# g++ -std=c++11 Serv.cpp client.cpp -o client -lthrift