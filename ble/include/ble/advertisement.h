#pragma once

#include <memory>
#include <string>

#include "sdbus-c++/sdbus-c++.h"

/**
 * @brief Advertisement class, this class can be used to easily create
 * advertisements with custom arguments and advertise
 *
 * @todo Allow user to define properties, such as name
 */
class Advertisement {
    std::string adapter_object_path;
    sdbus::IConnection &connection;
    std::unique_ptr<sdbus::IObject> ad;

  public:
    Advertisement(sdbus::IConnection &connection,
                  const std::string &object_path = "/ble/ad0");

    void turnOnAdvertising();
    void turnOffAdvertising();

    ~Advertisement();
};
