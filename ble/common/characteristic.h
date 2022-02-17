/**
 * @file characteristic.h
 * @author Aditya Gupta (adityag.ug19.cs@nitp.ac.in)
 * @brief Characteristic class representing BLE device Characteristic
 * @version 0.1
 * @date 2022-02-15
 *
 * @copyright Apache License (c) 2022
 *
 */
#pragma once

#include <map>
#include <memory>
#include <string>

#include "declarations.h"
#include "sdbus-c++/IObject.h"
#include "sdbus-c++/sdbus-c++.h"

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
    std::unique_ptr<sdbus::IObject> characteristic;

    /**
     * @references:
     * 1. gatt-api.txt -> GattCharacteristic1 <Confirm(), Flags>
     */

    public:
    /* ReadValue and WriteValue functions must be implemented by the class that
     * implements this Characteristic interface*/
    virtual std::vector<uint8_t>
    ReadValue(std::map<std::string, sdbus::Variant> options) const = 0;

    virtual void WriteValue(std::vector<uint8_t> value,
                            std::map<std::string, sdbus::Variant> options) = 0;

    /* Optional functions */
    virtual void StartNotify(){};
    virtual void StopNotify(){};

    Characteristic(sdbus::IConnection &connection,
                   std::string service_object_path, unsigned int index,
                   std::string UUID,
                   std::vector<std::string> flags = {"read", "write"});

    std::string getObjectPath() const;

    virtual ~Characteristic() { /*Nothing to do*/ }
};
