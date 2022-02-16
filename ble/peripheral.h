/**
 * @file peripheral.h
 * @author Aditya Gupta (adityag.ug19.cs@nitp.ac.in)
 * @brief Declarations of functions related to BLE peripherals
 * @version 0.1
 * @date 2022-02-15
 *
 * @copyright Apache License (c) 2022
 *
 */
#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "sdbus-c++/IConnection.h"
#include "sdbus-c++/IProxy.h"
#include "sdbus-c++/Types.h"
#include "sdbus-c++/sdbus-c++.h"

using std::cout, std::cerr, std::endl;

std::string get_advertising_capable_adapter_path();

class Advertisement {
    std::string object_path;
    std::string adapter_object_path;
    sdbus::IConnection &conn;

  public:
    Advertisement(sdbus::IConnection &connection);
    ~Advertisement();
};

void turn_on_advertising();
void turn_off_advertising();

void toggleAdvertising();

class Service;
class Characteristic;
