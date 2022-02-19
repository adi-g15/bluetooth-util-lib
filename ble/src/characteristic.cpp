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
#include "sdbus-c++/Types.h"

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
        .implementedAs(
            [this](vector<u8> value, std::map<string, sdbus::Variant> options) {
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
        .withGetter([uuid = UUID]() { return uuid; });

    characteristic->registerProperty("Service")
        .onInterface(CHARACTERISTIC_IFACE)
        .withGetter([service_object = std::move(service_object_path)]() {
            return sdbus::ObjectPath(service_object);
        });

    characteristic->registerProperty("Descriptors")
        .onInterface(CHARACTERISTIC_IFACE)
        .withGetter([]() {
            // TODO: Handle case when descriptors supported later
            return std::vector<sdbus::ObjectPath>();
        });

    /* Ignoring 'optional' properties such as `WriteAcquired` etc. */

    characteristic->registerProperty("Flags")
        .onInterface(CHARACTERISTIC_IFACE)
        .withGetter([flags = std::move(flags)]() { return flags; });

    characteristic->finishRegistration();

#ifdef VERBOSE_DEBUG
    std::cout << "Created characteristic at path: "
              << characteristic->getObjectPath() << std::endl;
#endif
}

std::string Characteristic::getObjectPath() const {
    return characteristic->getObjectPath();
}

CharacteristicProxy::CharacteristicProxy(sdbus::IConnection &connection,
                                         std::string path)
    : _proxy(sdbus::createProxy(connection, "org.bluez", path)) {}

/* ReadValue and WriteValue functions provided by the characteristic */
std::vector<u8> CharacteristicProxy::ReadValue(
    std::map<std::string, sdbus::Variant> options) const {
/*TODO*/
}

void CharacteristicProxy::WriteValue(
    std::vector<u8> value, std::map<std::string, sdbus::Variant> options) {
/*TODO*/
}

std::string CharacteristicProxy::getPath() const {
    return _proxy->getObjectPath();
}
