#include "central.h"

/**
 * @brief Get the Available BLE Peripheral addresses
 *
 * @return vector<string> Array of bluetooth device addresses
 */
vector<string> getAvailableBLEPeripherals() {
    /* TODO- This may return non-ble devices with ObjectManager, despite
     * starting the scan for LE transport only, handle it
     */
    auto result =
        std::map<sdbus::ObjectPath,
                 std::map<string, std::map<string, sdbus::Variant>>>();
    auto adapter = sdbus::createProxy("org.bluez", "/");
    adapter->callMethod("GetManagedObjects")
        .onInterface("org.freedesktop.DBus.ObjectManager")
        .storeResultsTo(result);

    vector<string> addresses;
    for (auto &p : result) {
        if (p.first.find("/org/bluez/hci0/dev_") == 0 &&
            p.first.length() ==
                sizeof("/org/bluez/hci0/dev_XX_XX_XX_XX_XX_XX") - 1) {
            addresses.push_back(
                p.second["org.bluez.Device1"]["Address"].get<string>());
        }
    }

    return addresses;
}

/**
 * @brief Start scanning for BLE devices
 *
 * @devref: bluez/doc/adapter-api.txt
 *
 * @note Caller should preferably wait some time (or register a signal
 * handler if needed) before calling getAvailableBLEPeripherals(), since new
 * devices may not be detected by bluez as soon as scanning started
 *
 * @return true if successful in turning scan on
 * @return false if could not turn on scanning
 */
bool startScanningForBLEDevices() {
    auto adapter_path = get_advertising_capable_adapter_path();

    auto adapter = sdbus::createProxy("org.bluez", adapter_path);

    const auto ADAPTER_INTERFACE = "org.bluez.Adapter1";
    try {
        /* ref: bluez/doc/adapter-api.txt, this method can be used to set
         * filter to discover only BLE (LE) devices */
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
