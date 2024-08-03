cmake_minimum_required( VERSION 3.4 )
project( rbtree )


set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(libyaml)

add_executable(rbtree main.c ${PROJECT_SOURCE_DIR}/timelib/timelib.c ${PROJECT_SOURCE_DIR}/container/list.c)

target_include_directories(rbtree PRIVATE 
    ${PROJECT_SOURCE_DIR}/
    ${PROJECT_SOURCE_DIR}/timelib
    ${PROJECT_SOURCE_DIR}/container
    ${PROJECT_SOURCE_DIR}/libs/libyaml/include
)

target_link_libraries(rbtree ${PROJECT_SOURCE_DIR}/libs/libyaml/lib/libyaml.a)
