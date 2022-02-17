#include <iostream>
#include <map>

#include "../../common/adapter.h"
#include "advertisement.h"

using std::cout, std::cerr, std::endl;

void Advertisement::turnOnAdvertising() {
    /**
     * @references:
     * 1. adapter-api.txt -> Discoverable[=true]
     * 2. advertising-api.txt -> LEAdvertisement1, LEAdvertisementManager1
     */
    sdbus::createProxy(connection, "org.bluez", adapter_object_path)
        ->callMethod("RegisterAdvertisement")
        .onInterface("org.bluez.LEAdvertisingManager1")
        .withArguments(ad->getObjectPath(),
                       std::map<std::string, sdbus::Variant>());

    cout << "Successfully registered advertisement: " << ad->getObjectPath()
         << endl;

    auto bluez_root_obj = sdbus::createProxy(connection, "org.bluez", "/");
    bluez_root_obj->uponSignal("InterfaceAdded")
        .onInterface("org.freedesktop.DBus.ObjectManager")
        .call([this](const sdbus::ObjectPath &object_path,
                     const std::map<std::string,
                                    std::map<std::string, sdbus::Variant>>
                         interfaces) {
            cout << "New object: " << object_path << endl;
            auto dev_interface = interfaces.find("org.bluez.Device1");
            if (dev_interface != interfaces.cend()) {
                auto iter_connected = dev_interface->second.find("Connected");

                if (iter_connected != dev_interface->second.cend()) {
                    auto is_connected = iter_connected->second.get<bool>();
                    cout << "Connected: " << std::boolalpha << is_connected
                         << endl;

                    if (is_connected) {
                        // Once we are connected (only then eventLoop will be
                        // exited), unregister the advertisement
                        turnOffAdvertising();
                    }
                }

                /* This will end the blocking wait at end of the function */
                connection.leaveEventLoop();
            }
        });

    // TODO: Call turnOnAdvertising when a 'previously connected device
    // disconnects'
    bluez_root_obj->registerSignalHandler(
        "org.freedesktop.DBus.Properties", "PropertiesChanged",
        [](sdbus::Signal &s) { cout << "called" << s.getPath() << endl; });

    // Blocking wait
    connection.enterEventLoop();
}

/**
 * @brief Unregisters advertisement
 *
 * @param advertisement_object_path Path of registered advertisement
 * @param adapter_object_path Path to adapter on which the advertisement is
 * registered with
 */
void Advertisement::turnOffAdvertising() {
    sdbus::createProxy(connection, "org.bluez", adapter_object_path)
        ->callMethod("UnregisterAdvertisement")
        .onInterface("org.bluez.LEAdvertisingManager1")
        .withArguments(ad->getObjectPath());
}

/**
 * @brief Construct a new Advertisement object, and register it with bluez
 *
 * @param connection Connection to the system bus
 */
Advertisement::Advertisement(sdbus::IConnection &connection,
                             const std::string &object_path)
    : connection(connection) {
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

    // reference: bluez-5.63/doc/advertising-api.txt
    const auto AD_INTERFACE_NAME = "org.bluez.LEAdvertisement1";
    ad = sdbus::createObject(connection, object_path);

    ad->registerMethod("Release")
        .onInterface(AD_INTERFACE_NAME)
        .implementedAs([]() { /*No task*/ })
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
}

/**
 * @brief Destroy the Advertisement object, and unregister advertisement
 *
 */
Advertisement::~Advertisement() { turnOffAdvertising(); }
