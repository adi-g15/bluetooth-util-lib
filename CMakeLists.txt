cmake_minimum_required(VERSION 3.15)

project(bluetooth-util-lib)
set(CMAKE_CXX_STANDARD 17)

# Should be removed in release
set(CMAKE_BUILD_TYPE "Debug")

add_subdirectory(bluetooth)
add_subdirectory(ble)

# vim: shiftwidth=4
