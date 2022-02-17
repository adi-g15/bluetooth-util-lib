cmake_minimum_required(VERSION 3.15)

project(bluetooth)

CPMAddPackage(
	GITHUB_REPOSITORY Kistler-Group/sdbus-cpp
	VERSION 1.1.0
	OPTIONS "BUILD_DOC OFF")
# Also add BUILD_LIBSYSTEMD ON if want to use on non systemd system

include_directories("../common")
add_library(bluetooth_functions bluetooth_functions.cpp)
add_executable(bluetooth "main.cpp")
target_link_libraries(bluetooth PRIVATE bluetooth_functions sdbus-c++)
