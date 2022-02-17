/**
 * @file service.cpp
 * @author Aditya Gupta (adityag.ug19.cs@nitp.ac.in)
 * @brief Implementation of Service class representing BLE device services
 * @version 0.1
 * @date 2022-02-15
 *
 * @copyright Apache License (c) 2022
 *
 */

#include <string>
#include <type_traits>

#include "declarations.h"
#include "sdbus-c++/sdbus-c++.h"
#include "service.h"

using std::string;

Service::Service(sdbus::IConnection &connection, string application_path,
                 unsigned int index)
    : connection(connection) {
    auto service = sdbus::createObject(
        connection, application_path + "/service" + std::to_string(index));

    const auto GATT_SERVICE_IFACE = "org.bluez.GattService1";

    service->registerProperty("UUID")
        .onInterface(GATT_SERVICE_IFACE)
        .withGetter([]() { return ""; });
    service->registerProperty("Primary")
        .onInterface(GATT_SERVICE_IFACE)
        .withGetter([]() { return false; });
    service->registerProperty("Device")
        .onInterface(GATT_SERVICE_IFACE)
        .withGetter([&application_path]() { return application_path; });

    service->finishRegistration();
}

Service::~Service() {
    for (auto &ptr : characteristics) {
        delete ptr;
    }
}
