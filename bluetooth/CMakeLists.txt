cmake_minimum_required(VERSION 3.15)

project(bluetooth)

# C++17 is required to build this library
set(CMAKE_CXX_STANDARD 17)

CPMAddPackage(
	GITHUB_REPOSITORY Kistler-Group/sdbus-cpp
	VERSION 1.1.0
	OPTIONS "BUILD_DOC OFF")
# Also add BUILD_LIBSYSTEMD ON if want to use on non systemd system

# Link against this `bluetooth` library, in cmake, it will also provide the application with the headers at bluetooth/*.h
add_library(bluetooth
	"src/file_transfer.cpp"
	"include/bluetooth/file_transfer.h"
	"include/bluetooth/device.h"
	"include/bluetooth/functions.h")
target_include_directories(bluetooth PUBLIC include/)
target_link_libraries(bluetooth PRIVATE sdbus-c++)

add_executable(test_bluetooth "main.cpp")
target_include_directories(test_bluetooth PRIVATE ../common)

# Note: Make a smaller test case, so sdbus-c++ can be removed from dependencies of dependents on `bluetooth`
target_link_libraries(test_bluetooth PRIVATE bluetooth sdbus-c++)
