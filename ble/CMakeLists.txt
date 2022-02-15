cmake_minimum_required(VERSION 3.15)

project(ble)

add_library(common common/service.cpp common/characteristic.cpp)
add_library(peripheral peripheral.cpp)
add_library(central central.cpp)

add_executable(ble main.cpp)
target_link_libraries(ble PRIVATE common peripheral central)
