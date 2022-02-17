/**
 * @file types.h
 * @author Aditya Gupta (adityag.ug19.cs@nitp.ac.in)
 * @brief Integral type declarations
 * @version 0.1
 * @date 2022-02-15
 *
 * @copyright Apache License (c) 2022
 *
 */

#pragma once

#include <cstdint>

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

namespace internal {
/**
 * @brief Internal Counter, should not be used outside of addCharacteristic and
 * addService methods
 *
 * @tparam ServiceOrCharacteristicType
 */
template <typename ServiceOrCharacteristicType> struct Counter {
    static inline int curr_index;
};
} // namespace internal
