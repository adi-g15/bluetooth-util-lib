/**
 * @file service.h
 * @author Aditya Gupta (adityag.ug19.cs@nitp.ac.in)
 * @brief Service class representing BLE device services
 * @version 0.1
 * @date 2022-02-15
 *
 * @copyright Apache License (c) 2022
 *
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "characteristic.h"
#include "sdbus-c++/IObject.h"
#include "sdbus-c++/sdbus-c++.h"

class Service {
    /**
     * @references:
     * 1. gatt-api.txt -> GattService1, GattManager1 (requires application with
     * services heirarchy)
     *
     * @note:
     * 1. Must implement ObjectManager too
     * 2. For pins, agent-api.txt -> PinCodes
     */

    /* vector of unique_ptr won't work here */
    std::vector<Characteristic *> characteristics;
    sdbus::IConnection &connection;
    std::unique_ptr<sdbus::IObject> service;

  public:
    /**
     * @brief Construct a characteristic object from passed parameters, and add
     * it to this service
     *
     * @note DO NOT construct a Characteristic object yourself. You require to
     * do 2 things:
     * 1. Pass index, UUID and any other arguments 'your' characteristic class
     * needs
     * 2. Have a supported constructor, with signature as given in the
     * static_assert message below
     *
     * @tparam CharacteristicType
     * @param index Index of characteristic, starting from 0, you can pass any
     * other too, but prefer in sequencial order like 0, then next
     * characteristic pass 1, and so on
     * @param UUID UUID of the characteristic, as string
     * @param ...args Any others arguments needed for your class
     *
     * TODO: Incomplete doc
     */
    template <typename CharacteristicType, class... Args>
    CharacteristicType &addCharacteristic(unsigned int index, std::string UUID, Args... args) {
        static_assert(
            std::is_constructible_v<CharacteristicType, decltype(connection),
                                    std::string, unsigned int, std::string,
                                    Args...>,
            "\n======================================================\n"
            "Your Characteristic class must provide a constructor "
            "with this signature: \n"
            "YourCharacteristic::YourCharacteristic(sdbus::IConnection& "
            "connection, std::string service_path, unsigned int index, "
            "std::string UUID, ...otherargs)");

        /* Checks if ServiceType inherits from Service base class type */
        static_assert(std::is_base_of_v<Characteristic, CharacteristicType>,
                      "Invalid CharacteristicType: Characteristics should "
                      "inherit from the `Characteristic` base class");

        auto characteristic = new CharacteristicType(
            connection, service->getObjectPath(),
            index, UUID, args...);
        characteristics.push_back(characteristic);

        return *characteristic;
    }
    /**
     * @brief Construct a new Service object
     *
     * @param connection Connection object for System bus connection
     * @param application_path Parent path which is the owner of this service
     * @param index Index of this service among services implemented by the
     * device
     *
     * @note This must be instantiated BEFORE any characteristic object that
     * should be child of this service object
     */
    Service(sdbus::IConnection &connection, std::string application_path,
            unsigned int index);

    virtual ~Service();
};
