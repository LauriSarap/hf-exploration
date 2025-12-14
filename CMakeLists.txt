cmake_minimum_required(VERSION 3.15)
project(NanoTrader CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(fmt REQUIRED)

include_directories(include)

add_executable(producer src/producer.cpp)
target_link_libraries(producer PRIVATE fmt::fmt)
if(UNIX AND NOT APPLE)
    target_link_libraries(producer PRIVATE rt pthread)
endif()

add_executable(consumer src/consumer.cpp)
target_link_libraries(consumer PRIVATE fmt::fmt)
if(UNIX AND NOT APPLE)
    target_link_libraries(consumer PRIVATE rt pthread)
endif()