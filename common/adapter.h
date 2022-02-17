/**
 * @file adapter.h
 * @author Aditya Gupta (adityag.ug.in@nitp.ac.in)
 * @brief Common functions/utilities for both 'usual' bluetooth and ble
 * connections
 * @version 0.1
 * @date 2022-02-16
 *
 * @copyright Apache License (c) 2022
 *
 */
#pragma once

#include <exception>
#include <iostream>

#include "sdbus-c++/sdbus-c++.h"

static bool
isAdapterPoweredOn(const std::string &adapter_object_path = "/org/bluez/hci0") {
    try {
        return sdbus::createProxy("org.bluez", adapter_object_path)
            ->getProperty("Powered")
            .onInterface("org.bluez.Adapter1")
            .get<bool>();
    } catch (std::exception &e) {
        std::cerr << "WARN: " << e.what() << std::endl;
        return false;
    }
}

static void tryPoweringOnAdapter(
    const std::string &adapter_object_path = "/org/bluez/hci0") {
    auto adapter = sdbus::createProxy("org.bluez", adapter_object_path);
    adapter->callMethod("Set")
        .onInterface("org.freedesktop.DBus.Properties")
        .withArguments("org.bluez.Adapter1", "Powered", sdbus::Variant(true));
}

static std::string get_advertising_capable_adapter_path() {
    auto results = std::map<
        sdbus::ObjectPath,
        std::map<std::string, std::map<std::string, sdbus::Variant>>>();
    // Note: This creates a temporary connection, and a new thread for a event loop
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
