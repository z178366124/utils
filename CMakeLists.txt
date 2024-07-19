cmake_minimum_required( VERSION 3.4 )
project( rbtree )


set(CMAKE_BUILD_TYPE Debug)

add_executable(rbtree main.c ${PROJECT_SOURCE_DIR}/timelib/timelib.c)

target_include_directories(rbtree PRIVATE 
    ${PROJECT_SOURCE_DIR}/
    ${PROJECT_SOURCE_DIR}/timelib
)