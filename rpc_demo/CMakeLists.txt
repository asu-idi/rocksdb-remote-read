add_executable(server server.cc)
target_link_libraries(server ${ROCKSDB_LIB} ${GFLAGS_LIB})

add_executable(client client.cc)
target_link_libraries(client ${ROCKSDB_LIB} ${GFLAGS_LIB})

add_subdirectory(gen-cpp)