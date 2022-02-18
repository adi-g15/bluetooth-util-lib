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

#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

#include "declarations.h"
#include "sdbus-c++/sdbus-c++.h"
#include "service.h"

using std::string;

Service::Service(sdbus::IConnection &connection, string application_path,
                 unsigned int index, std::string UUID)
    : connection(connection) {
    service = sdbus::createObject(connection, application_path + "/service" +
                                                  std::to_string(index));

    const auto GATT_SERVICE_IFACE = "org.bluez.GattService1";

    // First service registered will be considered the primary service
    static bool is_primary = true;

    service->registerProperty("UUID")
        .onInterface(GATT_SERVICE_IFACE)
        .withGetter([uuid = UUID]() { return uuid; });
    service->registerProperty("Primary")
        .onInterface(GATT_SERVICE_IFACE)
        .withGetter([is_primary = is_primary]() { return is_primary; });
    service->registerProperty("Characteristics")
        .onInterface(GATT_SERVICE_IFACE)
        .withGetter([]() {
            return std::vector<sdbus::ObjectPath>(
                {"/com/example/service0/char0", "/com/example/service0/char1"});
        });
    // service->registerProperty("Device")
    //     .onInterface(GATT_SERVICE_IFACE)
    //     .withGetter([application_path = std::move(application_path)]() {
    //         return application_path;
    //     });

    is_primary = false; // All next services will be non primary

    service->finishRegistration();

#ifdef VERBOSE_DEBUG
    std::cout << "Created service at path: " << service->getObjectPath()
              << std::endl;
#endif
}

Service::~Service() {
    for (auto &ptr : characteristics) {
        delete ptr;
    }
}
