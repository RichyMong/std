project (cm)

file( GLOB_RECURSE lib_src *.cpp )

add_library( ${PROJECT_NAME} ${lib_src} )

