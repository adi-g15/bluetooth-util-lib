/**
 * @file device.h
 * @author Aditya Gupta (adityag.ug19.cs@nitp.ac.in)
 * @brief Implementation of bluetooth device related functions
 * @version 0.1
 * @date 2022-02-15
 *
 * @copyright Apache License (c) 2022
 *
 */
#pragma once

#include <iostream>
#include <map>
#include <regex>
#include <string>

#include "sdbus-c++/sdbus-c++.h"

using std::map, std::string, std::cout, std::endl;

/**
 * @brief Get the device address using the device's name
 *
 * @pre Device should already be discovered
 *
 * @param device_name Device name/alias (Case-Insensitive), can also be
 * substring of the name
 *
 * @note Can be modified to take adapter path also
 *
 * @return std::string Address of first matching device found is returned
 */
inline std::string get_device_address_by_name(std::string device_name) {
    // Ignore case
    for (auto &c : device_name) {
        c = tolower(c);
    }

    auto actual_name = string();
    auto address = string();
    auto result =
        std::map<sdbus::ObjectPath,
                 std::map<string, std::map<string, sdbus::Variant>>>();

    sdbus::createProxy("org.bluez", "/")
        ->callMethod("GetManagedObjects")
        .onInterface("org.freedesktop.DBus.ObjectManager")
        .storeResultsTo(result);

    for (auto &p : result) {
        if (p.first.find("/org/bluez/hci0/dev_") == 0 &&
            p.first.length() ==
                sizeof("/org/bluez/hci0/dev_XX_XX_XX_XX_XX_XX") - 1) {
            // It is a device, can also use regex instead of those
            auto name = p.second["org.bluez.Device1"]["Alias"].get<string>();
            const auto &addr =
                p.second["org.bluez.Device1"]["Address"].get<string>();

            // Ignore case
            for (auto &c : name) {
                c = tolower(c);
            }

            if (name.find(device_name) != string::npos) {
                // `device_name` matched a substring in name
                cout << "Actual Name: " << name << endl;
                cout << "Address: " << addr << endl;

                return addr;
            }
        }
    }

    cout << "ERROR: Couldn't find a device with matching name: " << device_name
         << endl;

    return "";
}

/**
 * @brief Connect to device
 *
 * @pre Device may need to be already paired
 *
 * @param address Address in form of eg. XX:XX:XX:XX:XX:XX
 * @param adapter_path Path to adapter, with which the device is registered, for
 * most the default is a safe option, which is "/org/bluez/hci0"
 */
inline void
connect_to_device_using_address(std::string address,
                                string adapter_path = "/org/bluez/hci0") {
    if (std::regex_match(
            address,
            /*regex pattern to match a valid address, may require updation*/
            std::regex(
                "([\\[0-9\\]\\[A-F\\]]{2}:){5}[\\[0-9\\]\\[A-F\\]]{2}"))) {

        cout << "Matched: " << address << endl;
        std::replace(address.begin(), address.end(), ':', '_');

        auto device_path = adapter_path + "/dev_" + address;
        auto device = sdbus::createProxy("org.bluez", device_path);
        try {
            device->callMethod("Connect")
                .onInterface("org.bluez.Device1")
                .withTimeout(std::chrono::seconds(1));
        } catch (sdbus::Error &e) {
            std::cerr << "ERROR: " << e.what() << endl;
            return;
        }
    } else {
        cout << "Invalid address: " << address
             << ". Address should be of the form: XX:XX:XX:XX:XX:XX" << endl;
    }
}

/**
 * @brief Disconnect
 *
 * @param address Address in form of eg. XX:XX:XX:XX:XX:XX
 * @param adapter_path Path to adapter, with which the device is registered, for
 * most the default is a safe option, which is "/org/bluez/hci0"
 */
inline void disconnect_from_device_using_address(
    std::string address, std::string adapter_path = "/org/bluez/hci0") {
    if (std::regex_match(
            address,
            /*regex pattern to match a valid address, may require updation*/
            std::regex(
                "([\\[0-9\\]\\[A-F\\]]{2}:){5}[\\[0-9\\]\\[A-F\\]]{2}"))) {

        cout << "[Disconnect] Matched: " << address << endl;
        std::replace(address.begin(), address.end(), ':', '_');

        auto device_path = adapter_path + "/dev_" + address;
        auto device = sdbus::createProxy("org.bluez", device_path);
        device->callMethod("Disconnect").onInterface("org.bluez.Device1");
        cout << "Disconnected: " << address << endl;
    } else {
        cout << "Invalid address: " << address
             << ". Address should be of the form: XX:XX:XX:XX:XX:XX" << endl;
    }
}

inline void
connect_to_device_using_name(std::string name,
                             string _adapter_path = "/org/bluez/hci0") {
    connect_to_device_using_address(get_device_address_by_name(name));
}
