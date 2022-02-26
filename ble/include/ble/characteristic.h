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
#include <vector>

#include "declarations.h"
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
    virtual std::vector<u8>
    ReadValue(std::map<std::string, sdbus::Variant> options = {}) const = 0;

    virtual void
    WriteValue(std::vector<u8> value,
               std::map<std::string, sdbus::Variant> options = {}) = 0;

    /* Optional functions */
    virtual void StartNotify(){};
    virtual void StopNotify(){};

    Characteristic(sdbus::IConnection &connection,
                   std::string service_object_path, unsigned int index,
                   std::string UUID,
                   std::vector<std::string> flags = {"read", "write"});

    std::string getObjectPath() const;

    virtual ~Characteristic() { /*Nothing to do*/
    }
};

/**
 * @brief Proxy class to call methods on a Characteristic such as ReadValue,
 * WriteValue etc
 *
 * @note It is different than the above `Characteristic` class as that class
 * 'implements' a characteristic and so it is implemented on this system, and is
 * should be used by only the peripheral.
 * While this is a proxy to an already
 * existing characteristic provided by some other remote device
 */
class CharacteristicProxy {
    /*No support for descripters for now*/
  private:
    std::unique_ptr<sdbus::IProxy> _proxy;

  public:
    CharacteristicProxy(sdbus::IConnection &connection, std::string path);

    /* ReadValue and WriteValue functions provided by the characteristic */
    std::vector<u8>
    ReadValue(std::map<std::string, sdbus::Variant> options = {}) const;

    void WriteValue(std::vector<u8> value,
                    std::map<std::string, sdbus::Variant> options = {});

    std::string getPath() const;
};
