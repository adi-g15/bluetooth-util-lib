/**
 * @file tests.h
 * @brief Not automated tests, just manually checking if it 'not' fails
 *
 */
#pragma once

#include <algorithm>
#include <chrono>
#include <iostream>
#include <map>
#include <string>
#include <thread>
#include <vector>

#include "peripheral.h"
#include "sdbus-c++/sdbus-c++.h"

using std::cout, std::endl, std::vector, std::string, std::map;

void test_start_advertising() {
    turnOnAdvertising();
}

void test_func() { test_start_advertising(); }
