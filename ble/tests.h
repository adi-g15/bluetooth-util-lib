/**
 * @file tests.h
 * @brief Not automated tests, just manually checking if it 'not' fails
 *
 */
#pragma once

#include <iostream>

#include "common/characteristic.h"
#include "common/service.h"
#include "peripheral.h"
#include "sdbus-c++/sdbus-c++.h"

using std::cout, std::endl, std::vector, std::string, std::map;

class MyApplication : public Application {
  public:
    MyApplication(sdbus::IConnection &connection,
                  const std::string &application_object_path)
        : Application(connection, application_object_path) {
        cout << "Created myapplication\n";
    }

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
        : Service(connection, application_path, 0) {}

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

void test_register_application() {
    auto conn = sdbus::createSystemBusConnection("me.adig.iotiot");
    auto myapp = MyApplication(*conn, "/app");

    /* This service will intentionally fail to compile */
    // auto &service1 = myapp.addService<MyFailingService>("9RFE87NNS");
    // service1.addCharacteristic<
    //    MyFailingService::MyFailingCharacteristic
    // >("CHAR11");

    auto &service2 = myapp.addService<MyCorrectService>("A8FNDF33FD");

    service2.addCharacteristic<MyCorrectService::MyCorrectCharacteristic1>(
        "CHAR11");
    // Will intentionally fail
    // service2.addCharacteristic<MyCorrectService::MyFailingCharacteristic2>(
    //     "CHAR12");
    service2.addCharacteristic<MyCorrectService::MyCorrectCharacteristic3>(
        "CHAR12");

    cout << "Created application at path: " << myapp.getObjectPath() << endl;
}

void test_start_advertising() {
    auto conn = sdbus::createSystemBusConnection("me.adig.iotiot");
    conn->enterEventLoopAsync();

    auto root_obj = sdbus::createObject(*conn, "/");
    root_obj->registerProperty("name")
        .onInterface("me.adig.DBus.Properties")
        .withGetter([]() { return "gupta"; });
    root_obj->finishRegistration();

    // ~/bluez-5.63/doc/advertising-api.txt
    Advertisement advertisement(*conn);
    advertisement.turnOnAdvertising();
}

void test_func() {
    test_start_advertising();

    // Before leaving, resume the event loop in another thread to not miss
    // further signals,call
    // conn->enterEventLoopAsync();

    test_register_application();
}
