/**
 * @file tests.cpp
 * @brief Not automated tests, just manually checking if it 'not' fails
 *
 */

#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <vector>
#include <algorithm>

#include "sdbus-c++/sdbus-c++.h"

using std::cout, std::endl, std::vector, std::string, std::map;

const auto BLUEZ_DBUS_NAME = "org.bluez";
const auto ADAPTER_OBJECT_PATH = "/org/bluez/hci0";

/**
 * @note PRE-REQUISIT: Device must be paired
 *
 * @param address
 */
void test_connect_to_device(string address, string adapter_path = "/org/bluez/hci0" ) {
    if (std::regex_match(
            address,
            /*regex pattern to match a valid address, may require updation*/
            std::regex(
                "([\\[0-9\\]\\[A-F\\]]{2}:){5}[\\[0-9\\]\\[A-F\\]]{2}"))) {

        cout << "Matched: " << address << endl;
        std::replace(address.begin(), address.end(), ':', '_');

        auto device_path = adapter_path + "/dev_" + address;
        auto device = sdbus::createProxy("org.bluez", device_path);
        device->callMethod("Connect").onInterface("org.bluez.Device1");
    } else {
        cout << "Invalid address: " << address << ". Address should be of the form: XX:XX:XX:XX:XX:XX" << endl;
    }
}

void test_get_adapter_powered_status() {
    auto conn = sdbus::createSystemBusConnection();

    // conn->enterEventLoopAsync();

    auto adapter =
        sdbus::createProxy(*conn, BLUEZ_DBUS_NAME, ADAPTER_OBJECT_PATH);

    // Get if adapter powered on or not
    sdbus::Variant result;
    adapter->callMethod("Get")
        .onInterface("org.freedesktop.DBus.Properties")
        .withArguments("org.bluez.Adapter1", "Powered")
        .storeResultsTo(result);
    std::cout << "Is adapter powered: " << std::boolalpha << result.get<bool>()
              << std::endl;
}

void test_print_adapter_details() {
    auto adapter = sdbus::createProxy(BLUEZ_DBUS_NAME, ADAPTER_OBJECT_PATH);

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
            cout << p.second.get<uint32_t>();
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
    auto hci0_adapter = sdbus::createProxy("org.bluez", "/org/bluez/hci0");

    try {
        hci0_adapter->callMethod("Set")
            .onInterface("org.freedesktop.DBus.Properties")
            .withArguments("org.bluez.Adapter1", "Powered",
                           sdbus::Variant(true));

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
        if (p.first == "/org/bluez/hci0") {
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

void test_look_for_new_devices() {}

void test_func() {
    test_get_adapter_powered_status();
    test_turn_on_adapter();
    test_print_adapter_details();
    test_get_all_managed_objects();
    test_connect_to_device("11:11:22:AF:5F:70");

    // blocking
    test_look_for_new_devices();
}
