cmake_minimum_required(VERSION 2.8)

project(hello)

option( USE_PYTHON2 "If on, link to Python 2 instead of 3" OFF  )

message("${PYTHON_INCLUDE_DIRS}")

if ( USE_PYTHON2  )
    find_package( PythonLibs 2.6 REQUIRED  ) 
else()
    find_package( PythonLibs 3.3 REQUIRED  ) 
endif()

include_directories(
    SYSTEM
    ${PYTHON_INCLUDE_DIRS}
)

file(GLOB PROJECT_SOURCE *.h *.cpp)

find_package( Boost REQUIRED COMPONENTS python )

add_library(${PROJECT_NAME}
            SHARED
            ${PROJECT_SOURCE})

set_target_properties( ${PROJECT_NAME} PROPERTIES PREFIX "" )

message("${PYTHON_LIBRARIES} ${Boost_LIBRARIES}")

target_link_libraries(${PROJECT_NAME}
                      ${Boost_LIBRARIES}
                      ${PYTHON_LIBRARIES})
