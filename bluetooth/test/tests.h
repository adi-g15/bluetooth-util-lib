/**
 * @file tests.cpp
 * @brief Not automated tests, just manually checking if it 'not' fails
 *
 */

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <ios>
#include <iostream>
#include <map>
#include <ostream>
#include <regex>
#include <stdexcept>
#include <string>
#include <vector>

#include "common/declarations.h"
#include "common/adapter.h"

#include "bluetooth/functions.h"

#include "sdbus-c++/sdbus-c++.h"

using std::cout, std::cin, std::endl, std::string, std::map, std::vector;

const auto BLUEZ_DBUS_NAME = "org.bluez";
const auto DEFAULT_ADAPTER_PATH = "/org/bluez/hci0";

/**
 * @note PREREQUISIT: Device must be paired
 *
 * @param address
 */
void test_connect_to_device(string address,
                            string adapter_path = DEFAULT_ADAPTER_PATH) {
    connect_to_device_using_address(address, adapter_path);
}

/**
 * @note PREREQUISIT: Device must be connected
 *
 * @param address
 */
void test_disconnect_from_device(string address,
                                 string adapter_path = DEFAULT_ADAPTER_PATH) {
    disconnect_from_device_using_address(address, adapter_path);
}

void test_get_device_address(std::string device_name) {
    get_device_address_by_name(device_name);
}

void test_get_adapter_powered_status() {
    std::cout << "Is adapter powered: " << std::boolalpha
              << isAdapterPoweredOn() << std::endl;
}

void test_print_adapter_details() {
    auto adapter = sdbus::createProxy(BLUEZ_DBUS_NAME, DEFAULT_ADAPTER_PATH);

    // GetAll
    auto dict = std::map<std::string, sdbus::Variant>();
    adapter->callMethod("GetAll")
        .onInterface("org.freedesktop.DBus.Properties")
        .withArguments("org.bluez.Adapter1")
        .storeResultsTo(dict);

    for (const auto &p : dict) {
        cout << "* " << p.first << ": ";

        auto type = p.second.peekValueType();
        if (type == "s") {
            cout << p.second.get<std::string>();
        } else if (type == "u") {
            cout << p.second.get<u32>();
        } else if (type == "b") {
            cout << std::boolalpha << p.second.get<bool>();
        } else if (type == "as") {
            auto vec = p.second.get<vector<string>>();
            cout << "[ ";
            for (auto &s : vec) {
                cout << s << ", ";
            }
            cout << " ]";
        } else {
            cout << type;
        }

        cout << endl;
    }
}

void test_turn_on_adapter() {
    try {
        tryPoweringOnAdapter();
    } catch (std::exception &e) {
        std::cerr << "ERROR: Failed to power on adapter: " << e.what() << endl;
    }
}
void test_get_all_managed_objects() {
    auto result =
        std::map<sdbus::ObjectPath,
                 std::map<string, std::map<string, sdbus::Variant>>>();
    auto adapter = sdbus::createProxy("org.bluez", "/");
    adapter->callMethod("GetManagedObjects")
        .onInterface("org.freedesktop.DBus.ObjectManager")
        .storeResultsTo(result);

    cout << "Managed Objects:\n";
    for (auto &p : result) {
        cout << p.first << ": ";
        if (p.first == DEFAULT_ADAPTER_PATH) {
            cout << "Adapter";
        } else if (p.first.find("/org/bluez/hci0/dev_") == 0 &&
                   p.first.length() ==
                       sizeof("/org/bluez/hci0/dev_XX_XX_XX_XX_XX_XX") - 1) {
            cout << "Device\n";
            cout << "\tName: \""
                 << p.second["org.bluez.Device1"]["Alias"].get<string>()
                 << "\"\n";
            cout << "\tAddress: \""
                 << p.second["org.bluez.Device1"]["Address"].get<string>()
                 << "\"\n";
            cout << "\tInterfaces:\n";
            for (auto &dev_p : p.second) {
                cout << "\t\t" << dev_p.first << '\n';
            }
        } else {
            cout << "Ignore";
        }
        cout << endl;
    }
}

void test_look_for_new_devices() {
    // TODO
}

void test_send_file() { sendFile("30:4B:07:72:25:A4", "/etc/fstab"); }

void test_func() {
    test_get_adapter_powered_status();
    test_turn_on_adapter();
    test_print_adapter_details();
    test_get_all_managed_objects();

    test_get_device_address("Rockerz 450");
    test_connect_to_device("11:11:22:AF:5F:70");
    test_disconnect_from_device("11:11:22:AF:5F:70");

    // blocking
    test_look_for_new_devices();

    test_send_file();
}
