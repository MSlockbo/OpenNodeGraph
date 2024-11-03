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

#ifndef OPEN_CPP_UTILS_MATH_H
#define OPEN_CPP_UTILS_MATH_H

#include <cmath>

namespace open_cpp_utils
{

template<typename T>
bool is_prime(T x)
{
    if(x <= 1)                   return false;
    if(x == 2     || x == 3)     return true;
    if(x % 2 == 0 || x % 3 == 0) return false;

    T limit = static_cast<T>(sqrt(x));
    for(T i = 5; i <= limit; i += 6)
    {
        if(x % i == 0 || x % (i + 2) == 0) return false;
    }

    return true;
}

}

#endif // OPEN_CPP_UTILS_MATH_H
