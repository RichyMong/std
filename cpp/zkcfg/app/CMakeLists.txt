project (app)

file( GLOB_RECURSE app_src *.cpp )

include_directories( ${cm_SOURCE_DIR} )

MESSAGE ( STATUS ${cm_SOURCE_DIR} )

add_executable( ${PROJECT_NAME} ${app_src} )

target_link_libraries( ${PROJECT_NAME} cm zookeeper_mt )

