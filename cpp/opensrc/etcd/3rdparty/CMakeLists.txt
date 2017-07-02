cmake_minimum_required (VERSION 3.1.3 FATAL_ERROR)

project (3rdparty)

find_library(CPPREST_LIB NAMES cpprest)
find_library(ETCD_LIB NAMES etcd-cpp-api)

find_package(Boost REQUIRED COMPONENTS system)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -Werror -std=c++11")

FILE(GLOB SOURCE_FILES *.cpp)

ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} etcd-cpp-api boost_system ssl crypto grpc++ cpprest)
