/**
 * @file central.h
 * @author Aditya Gupta (adityag.ug19.cs@nitp.ac.in)
 * @brief Interfaces for BLE central devices
 * @version 0.1
 * @date 2022-02-15
 *
 * @copyright Apache License (c) 2022
 *
 */

#include <iostream>
#include <ostream>
#include <string>
#include <vector>

#include "adapter.h"
#include "sdbus-c++/Error.h"
#include "sdbus-c++/IProxy.h"
#include "sdbus-c++/Types.h"

using std::vector, std::string;

/**
 * @brief Get the Available BLE Peripheral addresses
 *
 * @return vector<string> Array of bluetooth device addresses
 */
vector<string> getAvailableBLEPeripherals() { return {}; }

/**
 * @brief Start scanning for BLE devices
 *
 * @devref: bluez/doc/adapter-api.txt
 *
 * @note Caller should preferably wait some time (or register a signal handler
 * if needed) before calling getAvailableBLEPeripherals(), since new devices may
 * not be detected by bluez as soon as scanning started
 *
 * @return true if successful in turning scan on
 * @return false if could not turn on scanning
 */
bool startScanningForBLEDevices() {
    auto adapter_path = get_advertising_capable_adapter_path();

    auto adapter = sdbus::createProxy("org.bluez", adapter_path);

    const auto ADAPTER_INTERFACE = "org.bluez.Adapter1";
    try {
        /* ref: bluez/doc/adapter-api.txt, this method can be used to set filter
         * to discover only BLE (LE) devices */
        adapter->callMethod("SetDiscoveryFilter")
            .onInterface(ADAPTER_INTERFACE)
            .withArguments(
                std::map<std::string, sdbus::Variant>({{"Transport", "le"}}));
    } catch (sdbus::Error &e) {
        std::cerr << "ERROR [SetDiscoveryFilter]: " << e.what() << std::endl;

        return false;
    }

    try {
        /* Start scanning for new devices, this will automatically */
        adapter->callMethod("StartDiscovery").onInterface(ADAPTER_INTERFACE);
    } catch (sdbus::Error &e) {
        std::cerr << "Error Name: \"" << e.getName() << '"' << std::endl;
        // TODO: Return true if error is `org.bluez.Error.InProgress`
        std::cerr << "ERROR [StartDiscovery]: " << e.what() << std::endl;

        return false;
    }

    return true;
}

/**
 * @brief Get all Services object, for a specific device
 *
 * @param address Address of bluetooth device, eg. "XX:XX:XX:XX:XX:XX" (without
 * quotes)
 * @return vector<Service> Array of service objects
 */
// vector<Service> getAllServices(string address);

/**
 * @brief Get all Characteristics object, for a specific device
 *
 * @param address Address of bluetooth device, eg. "XX:XX:XX:XX:XX:XX" (without
 * quotes)
 * @return vector<Characteristic> Array of characteristic objects
 */
// vector<Characteristic> getAllCharacteristics(string address);
