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

#include <cstdint>
#include <exception>
#include <ios>
#include <iostream>
#include <map>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../common/adapter.h"
#include "peripheral.h"
#include "sdbus-c++/Message.h"
#include "sdbus-c++/Types.h"

void turnOnAdvertising() {
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

    auto bluez_root_obj = sdbus::createProxy(*conn, "org.bluez", "/");
    bluez_root_obj->uponSignal("InterfaceAdded")
        .onInterface("org.freedesktop.DBus.ObjectManager")
        .call([&conn](const sdbus::ObjectPath &object_path,
                      const std::map<std::string,
                                     std::map<std::string, sdbus::Variant>>
                          interfaces) {
            cout << "New object: " << object_path << endl;
            auto dev_interface = interfaces.find("org.bluez.Device1");
            if (dev_interface != interfaces.cend()) {
                auto iter_connected = dev_interface->second.find("Connected");

                if (iter_connected != dev_interface->second.cend()) {
                    cout << "Connected: " << std::boolalpha
                         << iter_connected->second.get<bool>() << endl;
                }

                // End advertisement, once this function ends, the scope of
                // `advertisement` object will end and will automatically be
                // unregistered
                conn->leaveEventLoop();
            }
        });
    bluez_root_obj->registerSignalHandler("org.freedesktop.DBus.Properties",
                                          "PropertiesChanged",
                                          [](sdbus::Signal& s) { cout << "called" << s.getPath() << endl; });
    // bluez_root_obj->uponSignal("PropertiesChanged")
    //     .onInterface("org.freedesktop.DBus.Properties")
    //     .call([&conn](const sdbus::Error *err, std::string interface,
    //                   std::map<std::string, sdbus::Variant> changes) {
    //         cout << "Called here" << err->what() << endl;
    //     });

    // Blocking wait
    conn->enterEventLoop();

    // Before leaving, resume the event loop in another thread to not miss
    // further signals,call
    conn->enterEventLoopAsync();
}

/**
 * @brief Unregisters advertisement
 *
 * @param advertisement_object_path Path of registered advertisement
 * @param adapter_object_path Path to adapter on which the advertisement is
 * registered with
 */
void turnOffAdvertising(std::string advertisement_object_path,
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

    auto is_adapter_powered_on = isAdapterPoweredOn(adapter_object_path);
    if (!is_adapter_powered_on) {
        // TODO
        try {
            tryPoweringOnAdapter(adapter_object_path);
        } catch (std::exception &e) {
            std::cerr << "ERR: Could not power on adapter, try manually: "
                      << e.what() << endl;
            std::cerr << "Failed registering advertisement" << endl;
            return;
        }
    }

    // ~/bluez-5.63/doc/advertising-api.txt
    const auto AD_INTERFACE_NAME = "org.bluez.LEAdvertisement1";
    auto ad = sdbus::createObject(connection, object_path);

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

    sdbus::createProxy(connection, "org.bluez", adapter_object_path)
        ->callMethod("RegisterAdvertisement")
        .onInterface("org.bluez.LEAdvertisingManager1")
        .withArguments(sdbus::ObjectPath(ad->getObjectPath()),
                       std::map<std::string, sdbus::Variant>());

    cout << "Successfully registered advertisement: " << ad->getObjectPath()
         << endl;
}

/**
 * @brief Destroy the Advertisement object, and unregister advertisement
 *
 */
Advertisement::~Advertisement() { turnOffAdvertising(object_path); }
