cmake_minimum_required (VERSION 2.8.5 FATAL_ERROR)

PROJECT(etcd_client)

ADD_SUBDIRECTORY(protos)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

add_executable(${PROJECT_NAME} etcd_client.cc)
target_link_libraries(${PROJECT_NAME} protos ${GRPC_CPP_LIB})
