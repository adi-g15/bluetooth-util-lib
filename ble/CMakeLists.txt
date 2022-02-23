cmake_minimum_required(VERSION 3.15)

project(ble)

# C++17 is required to build this library
set(CMAKE_CXX_STANDARD 17)

include(../cmake/CPM.cmake)
CPMAddPackage(
	GITHUB_REPOSITORY Kistler-Group/sdbus-cpp
	VERSION 1.1.0
	OPTIONS "BUILD_DOC OFF")
# Also add BUILD_LIBSYSTEMD ON if want to use on non systemd system

# For common/adapter.h
include_directories("../common")
add_library(peripheral
	"src/advertisement.cpp"
	"src/characteristic.cpp"
	"src/service.cpp"
	"src/application.cpp")
add_library(central
	"src/central.cpp")
target_include_directories(peripheral PRIVATE ..)
target_include_directories(peripheral PRIVATE include/ble/)
target_include_directories(central PRIVATE ..)
target_include_directories(central PRIVATE include/ble/)
target_link_libraries(peripheral PUBLIC sdbus-c++)
target_link_libraries(central PUBLIC sdbus-c++)

add_library(ble "include/ble/peripheral.h" "include/ble/central.h")
target_include_directories(ble PUBLIC include/)
target_link_libraries(ble PRIVATE peripheral central)
target_link_libraries(ble PUBLIC sdbus-c++)

add_executable(test_ble main.cpp test/tests.h)
target_include_directories(test_ble PUBLIC "../")
target_link_libraries(test_ble PRIVATE ble sdbus-c++)

# vim: shiftwidth=4
