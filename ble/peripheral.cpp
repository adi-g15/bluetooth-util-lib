/**
 * @file peripheral.cpp
 * @author Aditya Gupta (adityag.ug19.cs@nitp.ac.in)
 * @brief Implements functions/interfaces for BLE Peripheral devices
 * @version 0.1
 * @date 2022-02-15
 *
 * @copyright Apache License (c) 2022
 *
 */

#include "peripheral.h"
#include "sdbus-c++/IObject.h"
#include "sdbus-c++/IProxy.h"
#include "sdbus-c++/Types.h"
#include <cstdint>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

std::string get_advertising_capable_adapter_path() {
    auto results = std::map<
        sdbus::ObjectPath,
        std::map<std::string, std::map<std::string, sdbus::Variant>>>();
    sdbus::createProxy("org.bluez", "/")
        ->callMethod("GetManagedObjects")
        .onInterface("org.freedesktop.DBus.ObjectManager")
        .storeResultsTo(results);

    for (const auto &obj : results) {
        auto it = obj.second.find("org.bluez.LEAdvertisingManager1");
        if (it != obj.second.cend()) {
            return obj.first;
        }
    }

    throw std::logic_error(
        "No adapter implementing org.bluez.LEAdvertisingManager1 found !");
}

void turn_on_advertising() {
    auto conn = sdbus::createSystemBusConnection("me.adig.iotiot");
    conn->enterEventLoopAsync();

    auto root_obj = sdbus::createObject(*conn, "/");
    root_obj->registerProperty("name")
        .onInterface("me.adig.DBus.Properties")
        .withGetter([]() { return "gupta"; });
    root_obj->finishRegistration();

    // -EINVAL, man sd_bus_add_object_vtable, can't pass a reserved DBus
    // interface
    // ad->registerMethod("GetAll").onInterface("org.freedesktop.DBus.Properties").withInputParamNames("interface").withOutputParamNames("properties").implementedAs([](std::string&
    // s) { cout << "Called with: " << s << endl; return
    // std::map<std::string, sdbus::Variant>(); });

    // ~/bluez-5.63/doc/advertising-api.txt
    Advertisement advertisement(*conn);

    conn->enterEventLoop();
}

/**
 * @brief Unregisters advertisement
 *
 * @param advertisement_object_path Path of registered advertisement
 * @param adapter_object_path Path to adapter on which the advertisement is
 * registered with
 */
void turn_off_advertising(std::string advertisement_object_path,
                          std::string adapter_object_path = "/org/bluez/hci0") {
    // TODO: @adig make sure it doesn't require the same connection it was
    // created with
    sdbus::createProxy("org.bluez", adapter_object_path)
        ->callMethod("UnregisterAdvertisement")
        .onInterface("org.bluez.LEAdvertisingManager1")
        .withArguments(sdbus::ObjectPath(advertisement_object_path));
}

void toggleAdvertising() {
    /**
     * @references:
     * 1. adapter-api.txt -> Discoverable[=true]
     * 2. advertising-api.txt -> LEAdvertisement1, LEAdvertisementManager1
     */
}

/**
 * @brief Construct a new Advertisement object, and register it with bluez
 *
 * @param connection Connection to the system bus
 */
Advertisement::Advertisement(sdbus::IConnection &connection)
    : object_path("/ble/ad0"), conn(connection) {
    adapter_object_path = get_advertising_capable_adapter_path();

    auto ad = sdbus::createObject(connection, object_path);
    // -EINVAL, man sd_bus_add_object_vtable, can't pass a reserved DBus
    // interface
    // ad->registerMethod("GetAll").onInterface("org.freedesktop.DBus.Properties").withInputParamNames("interface").withOutputParamNames("properties").implementedAs([](std::string&
    // s) { cout << "Called with: " << s << endl; return
    // std::map<std::string, sdbus::Variant>(); });

    // ~/bluez-5.63/doc/advertising-api.txt
    const auto AD_INTERFACE_NAME = "org.bluez.LEAdvertisement1";

    /**
     * Failing parts, causes: Failed to parse advertisement.
     *
     */
    // ad->registerProperty("ServiceUUIDs")
    //     .onInterface(AD_INTERFACE_NAME)
    //     .withGetter([]() {
    //         return std::vector<std::string>({"180D", "180F"});
    //     });

    // ad->registerProperty("ManufacturerData")
    //     .onInterface(AD_INTERFACE_NAME)
    //     .withGetter([]() {
    //         return std::map<uint16_t, sdbus::Variant>(
    //             {{0xffff, sdbus::Variant(std::vector<uint16_t>(
    //                           {0x00, 0x01, 0x02, 0x03}))}});
    //     });

    // ad->registerProperty("SolicitUUIDs")
    //     .onInterface(AD_INTERFACE_NAME)
    //     .withGetter([]() { return std::vector<std::string>(); });

    // ad->registerProperty("ServiceData")
    //     .onInterface(AD_INTERFACE_NAME)
    //     .withGetter(
    //         []() { return std::map<std::string, sdbus::Variant>(); });

    // ad->registerProperty("Discoverable")
    //     .onInterface(AD_INTERFACE_NAME)
    //     .withGetter([]() { return true; });

    // ad->registerProperty("Data")
    //     .onInterface(AD_INTERFACE_NAME)
    //     .withGetter([]() { return std::map<uint8_t, sdbus::Variant>();
    //     });

    ad->registerMethod("Release")
        .onInterface(AD_INTERFACE_NAME)
        .implementedAs([]() { std::cout << "Releasing" << std::endl; })
        .withNoReply();

    ad->registerProperty("Type")
        .onInterface(AD_INTERFACE_NAME)
        .withGetter([]() { return "peripheral"; });

    ad->registerProperty("ServiceUUIDs")
        .onInterface(AD_INTERFACE_NAME)
        .withGetter([]() { return std::vector<std::string>(); });

    ad->registerProperty("ManufacturerData")
        .onInterface(AD_INTERFACE_NAME)
        .withGetter([]() { return std::map<std::string, sdbus::Variant>(); });

    ad->registerProperty("SolicitUUIDs")
        .onInterface(AD_INTERFACE_NAME)
        .withGetter([]() { return std::vector<std::string>(); });

    ad->registerProperty("ServiceData")
        .onInterface(AD_INTERFACE_NAME)
        .withGetter([]() { return std::map<std::string, sdbus::Variant>(); });

    ad->registerProperty("Includes")
        .onInterface(AD_INTERFACE_NAME)
        .withGetter([]() { return std::vector<std::string>({"tx-power"}); });

    ad->registerProperty("LocalName")
        .onInterface(AD_INTERFACE_NAME)
        .withGetter([]() { return "Pehchana ?"; });

    ad->finishRegistration();

    cout << "Advertisement inside constructor objectpath: "
         << ad->getObjectPath() << endl;

    sdbus::createProxy(connection, "org.bluez", adapter_object_path)
        ->callMethod("RegisterAdvertisement")
        .onInterface("org.bluez.LEAdvertisingManager1")
        .withArguments(sdbus::ObjectPath(ad->getObjectPath()),
                       std::map<std::string, sdbus::Variant>());
}

/**
 * @brief Destroy the Advertisement object, and unregister advertisement
 *
 */
Advertisement::~Advertisement() { turn_off_advertising(object_path); }
