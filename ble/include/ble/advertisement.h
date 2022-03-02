#pragma once

#include <memory>
#include <string>

#include "sdbus-c++/sdbus-c++.h"

/**
 * @brief Advertisement class, this class can be used to easily create
 * advertisements with custom arguments and advertise
 */
class Advertisement {
    std::string adapter_object_path;
    std::string advertised_name = "A BLE G";
    sdbus::IConnection &connection;
    std::unique_ptr<sdbus::IObject> ad;

  public:
    Advertisement(sdbus::IConnection &connection,
                  const std::string &object_path = "/ble/ad0");

    void turnOnAdvertising();
    void turnOffAdvertising();

    void setAdvertisedName(const std::string& new_name);

    ~Advertisement();
};
