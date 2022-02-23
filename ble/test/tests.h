/**
 * @file tests.h
 * @brief Not automated tests, just manually checking if it 'not' fails
 *
 * @bug Sometime it will stuck at "Can verify that the root object has been
exported :) [Testcase] Will stop advertising after 1 seconds, for next tests to
run Times up!
"
 * This is probably due to a leaveEventLoop() signal getting un noticed in
multi threading
 */
#pragma once

#include <alloca.h>
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

#include "common/adapter.h"
#include "common/declarations.h"

#include "ble/advertisement.h"
#include "ble/central.h"
#include "ble/characteristic.h"
#include "ble/peripheral.h"
#include "ble/service.h"

#include "sdbus-c++/sdbus-c++.h"

using std::cout, std::endl, std::vector, std::string, std::map;

/* Intentionally, Almost all allocations in this file are heap allocated, maybe
 * not deleted either, if needed they are deleted at end of scope */

class MyApplication : public Application {
  public:
    MyApplication(sdbus::IConnection &connection,
                  const std::string &application_object_path)
        : Application(connection, application_object_path) {}

    void onInterfacesAdded(
        const sdbus::ObjectPath &object_path,
        const std::map<std::string, std::map<std::string, sdbus::Variant>>
            &interfaces_and_properties) override {
        cout << "Interface added" << endl;
    }
    void
    onInterfacesRemoved(const sdbus::ObjectPath &object_path,
                        const std::vector<std::string> &interfaces) override {
        cout << "Interface removed" << endl;
    }
};

class MyFailingService : public Service {
  public:
    class MyFailingCharacteristic : Characteristic {};
};

class MyCorrectService : public Service {
  public:
    MyCorrectService(sdbus::IConnection &connection,
                     std::string application_path, unsigned int index,
                     std::string UUID)
        : Service(connection, application_path, 0, UUID) {}

    struct MyCorrectCharacteristic1 : public Characteristic {
        MyCorrectCharacteristic1(sdbus::IConnection &connection,
                                 std::string service_path, unsigned int index,
                                 std::string UUID)
            : Characteristic(connection, service_path, index, UUID) {}

        std::vector<u8> ReadValue(
            std::map<std::string, sdbus::Variant> options) const override {
            return {};
        }

        void
        WriteValue(std::vector<u8> value,
                   std::map<std::string, sdbus::Variant> options) override {
            cout << "Kuchh write karo idhar\n";
        }
    };

    struct MyFailingCharacteristic2 : public Characteristic {};

    struct MyCorrectCharacteristic3 : public Characteristic {
        MyCorrectCharacteristic3(sdbus::IConnection &connection,
                                 std::string service_path, unsigned int index,
                                 std::string UUID)
            : Characteristic(connection, service_path, index, UUID) {}

        std::vector<u8> ReadValue(
            std::map<std::string, sdbus::Variant> options) const override {
            return {};
        }

        void
        WriteValue(std::vector<u8> value,
                   std::map<std::string, sdbus::Variant> options) override {
            cout << "Kuchh write karo idhar\n";
        }
    };
};

void test_register_application(sdbus::IConnection &conn) {
    auto myapp = new MyApplication(conn, "/com/example");

    /* This service will intentionally fail to compile */
    // auto &service1 = myapp.addService<MyFailingService>("9RFE87NNS");
    // service1.addCharacteristic<
    //    MyFailingService::MyFailingCharacteristic
    // >("CHAR11");

    auto &service2 = myapp->addService<MyCorrectService>(
        0, "0000180d-0000-1000-8000-00805f9b34fb");

    service2.addCharacteristic<MyCorrectService::MyCorrectCharacteristic1>(
        0, "00002a37-0000-1000-8000-00805f9b34fb");
    // Will intentionally fail
    // service2.addCharacteristic<MyCorrectService::MyFailingCharacteristic2>(
    //     2, "CHAR12");
    service2.addCharacteristic<MyCorrectService::MyCorrectCharacteristic3>(
        1, "00002a38-0000-1000-8000-00805f9b34fb");

    cout << "Created application at path: " << myapp->getObjectPath() << endl;

    // conn.enterEventLoop();
    myapp->registerWithGattManager();

    cout << "Registered application: " << myapp->getObjectPath()
         << " with GattManager" << endl;
}

void test_start_advertising(sdbus::IConnection &conn) {
    std::thread([&conn]() {
        std::cout << "[Testcase] Will stop advertising after 6 seconds, for "
                     "next tests to run"
                  << endl;
        std::this_thread::sleep_for(std::chrono::seconds(6));
        cout << "Changing name" << endl;
        conn.leaveEventLoop();
    }).detach();

    // ~/bluez-5.63/doc/advertising-api.txt
    auto advertisement = new Advertisement(conn);
    advertisement->setAdvertisedName("Naya naam");
    advertisement->turnOnAdvertising();
}

void test_create_root_object(sdbus::IConnection &conn) {
    auto root_obj = sdbus::createObject(conn, "/");
    root_obj->registerProperty("name")
        .onInterface("me.adig.DBus.Properties")
        .withGetter([]() { return "gupta"; });
    root_obj->finishRegistration();

    std::cout << "Can verify that the root object has been exported :)" << endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

void test_start_ble_scan(sdbus::IConnection &conn) {
    startScanningForBLEDevices();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    cout << "1\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));
    cout << "2\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));
    cout << "3\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));
    cout << "4\n";
    getAvailableBLEPeripherals();
}

void test_turn_on_adapter() {
    try {
        tryPoweringOnAdapter();
    } catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void test_func() {
    auto conn = sdbus::createSystemBusConnection("me.adig.iotiot");

    test_turn_on_adapter();
    test_create_root_object(*conn);

    // peripheral
    test_start_advertising(*conn);
    test_register_application(*conn);

    // central
    test_start_ble_scan(*conn);

    conn->enterEventLoop();
}
