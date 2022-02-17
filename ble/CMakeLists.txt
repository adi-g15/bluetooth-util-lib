cmake_minimum_required(VERSION 3.15)

project(ble)

CPMAddPackage(
	GITHUB_REPOSITORY Kistler-Group/sdbus-cpp
	VERSION 1.1.0
	OPTIONS "BUILD_DOC OFF")
# Also add BUILD_LIBSYSTEMD ON if want to use on non systemd system

include_directories("../common")
add_library(common common/service.cpp common/characteristic.cpp)
add_library(peripheral peripheral.cpp)
add_library(central central.cpp)

target_link_libraries(central PUBLIC sdbus-c++)
target_link_libraries(peripheral PUBLIC sdbus-c++)

add_executable(ble main.cpp)
target_link_libraries(ble PRIVATE common peripheral central)
