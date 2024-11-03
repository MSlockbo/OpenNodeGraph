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

#ifndef OPEN_CPP_UTILS_TEMPLATE_UTILS_H
#define OPEN_CPP_UTILS_TEMPLATE_UTILS_H

namespace open_cpp_utils
{

/**
 * \file template_utils.h
 * \brief Provides compile time evaluation utilities for templates and template packs
 */

/**
 * \brief Check if a sequence of types is unique
 * \tparam Ts
 */
template<typename...Ts>
inline static constexpr bool is_unique = std::true_type{};

template<typename T, typename...Ts>
inline constexpr bool is_unique<T, Ts...> = std::bool_constant<(!std::is_same_v<T, Ts> && ...) && is_unique<Ts...>>{};

}

#endif // OPEN_CPP_UTILS_TEMPLATE_UTILS_H
