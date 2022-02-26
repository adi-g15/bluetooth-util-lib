/**
 * @file central.h
 * @author Aditya Gupta (adityag.ug19.cs@nitp.ac.in)
 * @brief Interfaces for BLE central devices
 * @version 0.1
 * @date 2022-02-15
 *
 * @copyright Apache License (c) 2022
 *
 */

#include <iostream>
#include <ostream>
#include <string>
#include <vector>

#include "adapter.h"
#include "sdbus-c++/Error.h"
#include "sdbus-c++/IProxy.h"
#include "sdbus-c++/Types.h"

using std::vector, std::string;

/**
 * @brief Get the Available BLE Peripheral addresses
 *
 * @return vector<string> Array of bluetooth device addresses
 */
vector<string> getAvailableBLEPeripherals();

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
bool startScanningForBLEDevices();

/**
 * @brief Get all Services object, for a specific device
 *
 * @param address Address of bluetooth device, eg. "XX:XX:XX:XX:XX:XX"
 * (without quotes)
 * @return vector<Service> Array of service objects
 */
// vector<Service> getAllServices(string address);

/**
 * @brief Get all Characteristics object, for a specific device
 *
 * @param address Address of bluetooth device, eg. "XX:XX:XX:XX:XX:XX"
 * (without quotes)
 * @return vector<CharacteristicProxy> Array of characteristic objects
 */
// vector<CharacteristicProxy> getAllCharacteristics(string address);
