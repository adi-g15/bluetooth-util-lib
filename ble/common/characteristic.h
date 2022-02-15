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

#include <string>
#include <vector>

#include "types.h"

class Characteristic {
    /**
     * @references:
     * 1. gatt-api.txt -> GattCharacteristic1 <Confirm(), Flags>
     */

     public:
     std::vector<u8> readValue(std::string address) const;
     void writeValue(std::string address, std::vector<u8> new_value);
};
