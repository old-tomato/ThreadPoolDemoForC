cmake_minimum_required(VERSION 3.8)
project(test_thread_pool)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp
        MyThreadPool.cpp
        MyThreadPool.h
        )
add_executable(ThreadPoolDemoForC ${SOURCE_FILES})

target_link_libraries(ThreadPoolDemoForC pthread)