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

#include <string>
#include <vector>

#include "common/service.h"
#include "common/characteristic.h"

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
 * @return true if successful in turning scan on
 * @return false if could not turn on scanning
 */
bool startScanningForBLEDevices();

/**
 * @brief Get all Services object, for a specific device
 * 
 * @param address Address of bluetooth device, eg. "XX:XX:XX:XX:XX:XX" (without quotes)
 * @return vector<Service> Array of service objects
 */
vector<Service> getAllServices(string address);

/**
 * @brief Get all Characteristics object, for a specific device
 * 
 * @param address Address of bluetooth device, eg. "XX:XX:XX:XX:XX:XX" (without quotes)
 * @return vector<Characteristic> Array of characteristic objects
 */
vector<Characteristic> getAllCharacteristics(string address);
