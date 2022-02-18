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
	"include/ble/peripheral.h"
	"include/ble/advertisement.h" "src/advertisement.cpp"
	"include/ble/characteristic.h" "src/characteristic.cpp"
	"include/ble/service.h" "src/service.cpp"
	"include/ble/application.h" "src/application.cpp"
	"include/ble/declarations.h")
add_library(central
	"include/ble/central.h")
target_include_directories(peripheral PRIVATE include/ble/)
target_include_directories(central PRIVATE include/ble/)
target_link_libraries(peripheral PRIVATE sdbus-c++)
target_link_libraries(central PRIVATE sdbus-c++)

add_library(ble "include/ble/peripheral.h" "include/ble/central.h")
target_include_directories(ble PUBLIC include/)
target_link_libraries(ble PRIVATE peripheral central sdbus-c++)

add_executable(test_ble main.cpp test/tests.h)
target_link_libraries(test_ble PRIVATE ble sdbus-c++)
