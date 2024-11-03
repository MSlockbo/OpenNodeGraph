// =====================================================================================================================
//  open-cpp-utils, an open-source cpp library with data structures that extend the STL.
//  Copyright (C) 2024  Medusa Slockbower
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.
// =====================================================================================================================

#ifndef OPEN_CPP_UTILS_UNIQUE_ID_H
#define OPEN_CPP_UTILS_UNIQUE_ID_H

#include <cstdint>

namespace open_cpp_utils
{

// Internal function for incrementing an id associated with a type
template<typename Base> uint64_t _increment_id()
{
    static uint64_t current = 0;
    return current++;
}

/**
 * \brief Generate a unique id for a type given a base type
 * \tparam Base Base type for id categorization
 * \tparam Type Type for id generation
 * \return Generated ID for Type and Base combination
 *
 * Example:
 *
 * unique_id<void, int>() = 0
 * unique_id<void, unsigned int>() = 1
 *
 * unique_id<int, int>() = 0
 */
template<typename Base, typename Type> uint64_t unique_id()
{
    static bool initialized = false;
    static uint64_t id = 0;

    if(initialized) return id;
    initialized = true;
    return id = _increment_id<Base>();
}

}

#endif // OPEN_CPP_UTILS_UNIQUE_ID_H
