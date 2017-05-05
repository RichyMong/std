project (app)

file( GLOB_RECURSE app_src *.cpp )

include_directories( ${cm_SOURCE_DIR} )

find_package( Boost REQUIRED COMPONENTS system )

add_executable( ${PROJECT_NAME} ${app_src} )

target_link_libraries( ${PROJECT_NAME} cm zookeeper_mt ${Boost_LIBRARIES} )

