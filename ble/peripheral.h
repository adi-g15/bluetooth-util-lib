/**
 * @file peripheral.h
 * @author Aditya Gupta (adityag.ug19.cs@nitp.ac.in)
 * @brief Declarations of functions related to BLE peripherals
 * @version 0.1
 * @date 2022-02-15
 *
 * @copyright Apache License (c) 2022
 *
 */
#pragma once

#include <cstdint>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <regex>
#include <string>
#include <vector>

#include "sdbus-c++/IConnection.h"
#include "sdbus-c++/IObject.h"
#include "sdbus-c++/IProxy.h"
#include "sdbus-c++/StandardInterfaces.h"
#include "sdbus-c++/Types.h"
#include "sdbus-c++/sdbus-c++.h"

using std::cout, std::cerr, std::endl;

class Advertisement {
    std::string object_path;
    std::string adapter_object_path;
    sdbus::IConnection &conn;

  public:
    Advertisement(sdbus::IConnection &connection);
    ~Advertisement();
};

void turnOnAdvertising();
void turnOffAdvertising();

void toggleAdvertising();

/**
 * @brief Characteristic interface
 *
 * Any characteristic must inherit from this, and it must provide a constructor
 * with the signature:
 *
 * MyCharacteristic::MyCharacteristic(sdbus::IConnection &connection,
 *                  std::string service_object_path, unsigned int index,
 *                  std::string UUID, std::vector<std::string> flags):
 * Characteristic(connection,service_object_path,index,UUID,flags) {
 *    ... yourlogic ...
 *    }
 *
 */
class Characteristic {
    std::string path;

  public:
    /**ReadValue and WriteValue functions must be implemented by the class that
     * implements this Characteristic interface*/
    virtual std::vector<uint8_t>
    ReadValue(std::map<std::string, sdbus::Variant> options) = 0;
    virtual void WriteValue(std::vector<uint8_t> value,
                            std::map<std::string, sdbus::Variant> options) = 0;

    /* Optional functions */
    virtual void StartNotify(){};
    virtual void StopNotify(){};

    Characteristic(sdbus::IConnection &connection,
                   std::string service_object_path, unsigned int index,
                   std::string UUID, std::vector<std::string> flags) {
        if (!std::regex_match(service_object_path,
                              std::regex(".*/service[0-9]+"))) {
            // https://github.com/bluez/bluez/blob/master/doc/gatt-api.txt
            cerr << "Device object path doesn't follow bluez suggested path. "
                    "Object path should be of the form: "
                 << "[variable prefix]/serviceXX"
                 << "\n";

            cerr << "Passed object path: " << service_object_path << endl;
            return;
        }

        path = service_object_path + "/char" + std::to_string(index);

        const auto CHARACTERISTIC_IFACE = "org.bluez.GattCharacteristic1";
        auto characteristic_obj = sdbus::createObject(connection, path);

        /*Methods according to bluez/docs/gatt-api.txt*/
        characteristic_obj->registerMethod("ReadValue")
            .onInterface(CHARACTERISTIC_IFACE)
            .withInputParamNames("options")
            .withOutputParamNames("value")
            .implementedAs(
                [this](std::map<std::string, sdbus::Variant> options) {
                    return this->ReadValue(options);
                });

        characteristic_obj->registerMethod("WriteValue")
            .onInterface(CHARACTERISTIC_IFACE)
            .withInputParamNames("value", "options")
            .implementedAs(
                [this](std::vector<uint8_t> value,
                       std::map<std::string, sdbus::Variant> options) {
                    return this->WriteValue(value, options);
                })
            .withNoReply();

        characteristic_obj->registerMethod("StartNotify")
            .onInterface(CHARACTERISTIC_IFACE)
            .implementedAs([this]() { return this->StartNotify(); })
            .withNoReply();

        characteristic_obj->registerMethod("StopNotify")
            .onInterface(CHARACTERISTIC_IFACE)
            .implementedAs([this]() { return this->StopNotify(); })
            .withNoReply();

        /*Properties according to bluez/docs/gatt-api.txt*/
        characteristic_obj->registerProperty("UUID")
            .onInterface(CHARACTERISTIC_IFACE)
            .withGetter([]() { return ""; });

        characteristic_obj->registerProperty("Service")
            .onInterface(CHARACTERISTIC_IFACE)
            .withGetter(
                [&service_object_path]() { return service_object_path; });

        /* Ignoring 'optional' properties such as `WriteAcquired` etc. */

        characteristic_obj->registerProperty("Flags")
            .onInterface(CHARACTERISTIC_IFACE)
            .withGetter([&flags]() { return flags; });

        characteristic_obj->registerProperty("MTU")
            .onInterface(CHARACTERISTIC_IFACE)
            .withGetter([]() {
                /*TODO*/
                return uint16_t(0);
            });

        characteristic_obj->finishRegistration();
    }

    ~Characteristic() {}
};

class Service {
    template <typename T> using Ptr = T *;
    // template <typename T> using Ptr = std::unique_ptr<T>;

    std::vector<Ptr<Characteristic>> characteristics;
    std::shared_ptr<sdbus::IConnection> connection;
    std::string path;

  public:
    /**
     * @brief Construct a characteristic object from passed parameters, and add
     * it to this service
     *
     * @note @adig The requirement on constructors of classes inheriting
     * Characteristic can be losened by using parameter pack here
     *
     * @tparam CharacteristicType
     * @param index Index of characteristic, starting from 0, you can pass any
     * other too, but prefer in sequencial order like 0, then next
     * characteristic pass 1, and so on
     * @param UUID UUID of the characteristic, as string
     * @param flags Default is {"read", "write"}
     */
    template <typename CharacteristicType>
    inline void addCharacteristic(unsigned int index, std::string UUID,
                                  std::vector<std::string> flags = {"read",
                                                                    "write"}) {
        characteristics.push_back(
            new CharacteristicType(connection, path, index, UUID, flags));
    }

    /**
     * @brief Construct a new Service object
     *
     * @param connection Connection object for System bus connection
     * @param device_object_path Parent path which is the owner of this service
     * @param index Index of this service among services implemented by the
     * device
     *
     * @note This must be instantiated BEFORE any characteristic object that
     * should be child of this service object
     */
    Service(sdbus::IConnection &connection, std::string device_object_path,
            unsigned int index = 0) {
        auto service =
            sdbus::createObject(connection, device_object_path + "/service" +
                                                std::to_string(index));

        const auto GATT_SERVICE_IFACE = "org.bluez.GattService1";

        service->registerProperty("UUID")
            .onInterface(GATT_SERVICE_IFACE)
            .withGetter([]() { return ""; });
        service->registerProperty("Primary")
            .onInterface(GATT_SERVICE_IFACE)
            .withGetter([]() { return false; });
        service->registerProperty("Device")
            .onInterface(GATT_SERVICE_IFACE)
            .withGetter([&device_object_path]() { return device_object_path; });

        service->finishRegistration();
    }
};

// createObject -> Application(ObjectManager)
class Application {
    template <typename T> using Ptr = std::unique_ptr<T>;

    /**
     * @brief Implementation of sdbus::ObjectManager_proxy for implementing
     * signal handlers for InterfaceAdded, and InterfaceRemoved
     *
     * @design_note: Why not inherit `Application` class itself from
     * `ObjectManager_proxy`?
     * This was because, ObjectManager_proxy requires a
     * proxy to our application object, So, if it was done that way, then the
     * user will have to create the object, pass the proxy to that object
     * Instead I want that work to be done in Application's contructor
     *
     * @note 1. Not to be used externally of this class
     * @note 2. Don't confuse this with DBus's Object Manager, this is like a
     * proxy to the ObjectManager that is needed for our Application
     */
    struct ObjectManager : public sdbus::ObjectManager_proxy {
      public:
        /**
         * @brief Construct a new Object Manager object
         *
         * @param proxy Proxy object to current application object (This should
         * be created inside the constructor of `Application` class, and then
         * passed on to this)
         */
        ObjectManager(sdbus::IProxy &proxy)
            : sdbus::ObjectManager_proxy(proxy) {}
    };

    ObjectManager *object_manager;

    std::string path;

  public:
    Application(sdbus::IConnection &connection, const std::string &bus_name,
                const std::string &application_object_path)
        : path(application_object_path) {
        auto proxy =
            sdbus::createProxy(connection, bus_name, application_object_path);
    }
    virtual void onInterfacesAdded(
        const sdbus::ObjectPath &object_path,
        const std::map<std::string, std::map<std::string, sdbus::Variant>>
            &interfaces_and_properties) {}
    virtual void
    onInterfacesRemoved(const sdbus::ObjectPath &object_path,
                        const std::vector<std::string> &interfaces) {}
};
