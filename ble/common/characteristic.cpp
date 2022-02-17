/**
 * @file characteristic.cpp
 * @author Aditya Gupta (adityag.ug19.cs@nitp.ac.in)
 * @brief Implementation of Characteristic class representing BLE device
 * Characteristic
 * @version 0.1
 * @date 2022-02-15
 *
 * @copyright Apache License (c) 2022
 *
 */

#include <iostream>
#include <regex>
#include <string>
#include <vector>

#include "characteristic.h"

using std::vector, std::string, std::cerr, std::endl;

Characteristic::Characteristic(sdbus::IConnection &connection,
                               string service_object_path, unsigned int index,
                               string UUID, vector<string> flags) {
    if (!std::regex_match(service_object_path,
                          std::regex(".*/service[0-9]+"))) {
        // https://github.com/bluez/bluez/blob/master/doc/gatt-api.txt
        cerr << "Device object path doesn't follow bluez suggested path. "
                "Object path should be of the form: "
             << "[variable prefix]/serviceXX"
             << "\n";

        cerr << "Passed object path: " << service_object_path << endl;
        return;
    }

    auto path = service_object_path + "/char" + std::to_string(index);

    const auto CHARACTERISTIC_IFACE = "org.bluez.GattCharacteristic1";
    characteristic = sdbus::createObject(connection, path);

    /*Methods according to bluez/docs/gatt-api.txt*/
    characteristic->registerMethod("ReadValue")
        .onInterface(CHARACTERISTIC_IFACE)
        .withInputParamNames("options")
        .withOutputParamNames("value")
        .implementedAs([this](std::map<string, sdbus::Variant> options) {
            return this->ReadValue(options);
        });

    characteristic->registerMethod("WriteValue")
        .onInterface(CHARACTERISTIC_IFACE)
        .withInputParamNames("value", "options")
        .implementedAs([this](vector<uint8_t> value,
                              std::map<string, sdbus::Variant> options) {
            return this->WriteValue(value, options);
        })
        .withNoReply();

    characteristic->registerMethod("StartNotify")
        .onInterface(CHARACTERISTIC_IFACE)
        .implementedAs([this]() { return this->StartNotify(); })
        .withNoReply();

    characteristic->registerMethod("StopNotify")
        .onInterface(CHARACTERISTIC_IFACE)
        .implementedAs([this]() { return this->StopNotify(); })
        .withNoReply();

    /*Properties according to bluez/docs/gatt-api.txt*/
    characteristic->registerProperty("UUID")
        .onInterface(CHARACTERISTIC_IFACE)
        .withGetter([]() { return ""; });

    characteristic->registerProperty("Service")
        .onInterface(CHARACTERISTIC_IFACE)
        .withGetter([&service_object_path]() { return service_object_path; });

    /* Ignoring 'optional' properties such as `WriteAcquired` etc. */

    characteristic->registerProperty("Flags")
        .onInterface(CHARACTERISTIC_IFACE)
        .withGetter([&flags]() { return flags; });

    characteristic->registerProperty("MTU")
        .onInterface(CHARACTERISTIC_IFACE)
        .withGetter([]() {
            /*TODO*/
            return uint16_t(0);
        });

    characteristic->finishRegistration();
}

std::string Characteristic::getObjectPath() const {
    return characteristic->getObjectPath();
}
