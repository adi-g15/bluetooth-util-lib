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
using i64 = int64_t;
using u64 = uint64_t;

namespace internal {
#if 0 /* NO LONGER IN USE, CANDIDATE FOR REMOVAL */
/* No longer used for automatic indices of services and chars BECAUSE, an
 * application may have services of different compile time types, this will
 * assign all of their first objects as index 0, similar for chars*/
/**
 * @brief Internal Counter, should not be used outside of addCharacteristic and
 * addService methods
 *
 * @tparam ServiceOrCharacteristicType
 */
template <typename ServiceOrCharacteristicType> struct Counter {
    static inline int curr_index;
};
#endif
} // namespace internal
