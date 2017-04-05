cmake_minimum_required(VERSION 3.5)

project( appmaster )

file( GLOB MCM_SOURCES src/*.h src/*.cpp ) 

SET(CMAKE_CXX_FLAGS_DEBUG  "-O0 -g")

#############################################################################
include_directories(${Boost_INCLUDE_DIR})

find_package( Boost REQUIRED COMPONENTS thread system )

add_executable( ${PROJECT_NAME} ${MCM_SOURCES})

message ( STATUS "this is ${Boost_LIBRARIES}" )

target_link_libraries( ${PROJECT_NAME} ${Boost_LIBRARIES} )
