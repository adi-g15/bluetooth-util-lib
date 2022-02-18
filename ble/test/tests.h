/**
 * @file tests.h
 * @brief Not automated tests, just manually checking if it 'not' fails
 *
 */
#pragma once

#include <alloca.h>
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

#include "ble/advertisement.h"
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

        std::vector<uint8_t> ReadValue(
            std::map<std::string, sdbus::Variant> options) const override {
            return {};
        }

        void
        WriteValue(std::vector<uint8_t> value,
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

        std::vector<uint8_t> ReadValue(
            std::map<std::string, sdbus::Variant> options) const override {
            return {};
        }

        void
        WriteValue(std::vector<uint8_t> value,
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
        std::cout << "[Testcase] Will stop advertising after 1 seconds, for "
                     "next tests to run"
                  << endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "Times up!" << endl;
        conn.leaveEventLoop();
    }).detach();

    // ~/bluez-5.63/doc/advertising-api.txt
    auto advertisement = new Advertisement(conn);
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

void test_func() {
    auto conn = sdbus::createSystemBusConnection("me.adig.iotiot");

    test_create_root_object(*conn);
    test_start_advertising(*conn);
    test_register_application(*conn);

    conn->enterEventLoop();
}
