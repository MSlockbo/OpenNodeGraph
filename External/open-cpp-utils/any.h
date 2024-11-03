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

#ifndef OPEN_CPP_UTILS_ANY_H
#define OPEN_CPP_UTILS_ANY_H

#include "template_utils.h"

namespace open_cpp_utils
{

/**
 * \brief Wrapper for a value with multiple types
 * \tparam Ts Types to include, must be unique
 */
template<typename...Ts> class any;

template<> class any<> { };

template<typename T, typename...Rest>
class any<T, Rest...> : public any<Rest...>
{
// Assertions ==========================================================================================================

    static_assert(is_unique<T, Rest...>);


// Typedefs ============================================================================================================

public:
    using base_type = any<Rest...>;

    using this_type       = T;
    using reference       = T&;
    using const_reference = const T&;
    using pointer         = T*;
    using const_pointer   = const T*;


// Constructors ========================================================================================================

public:
    any()                                            : base_type()        , Value() { }
    any(const this_type& value, const Rest&...other) : base_type(other...), Value(value) { }
    any(this_type&& value, Rest&&...other)           : base_type(other...), Value(value) { }
    any(const any& other)                            = default;
    any(any&& other)                                 = default;
    ~any()                                           = default;


// Operators ===========================================================================================================

public:

// Assignment operators ------------------------------------------------------------------------------------------------

    any& operator=(const any&) = default;
    any& operator=(any&&)      = default;

	template<typename V>
	any& operator=(const V& v)
	{
        static_assert(std::disjunction<std::is_same<V, T>, std::is_same<V, Rest>...>{});
		static_cast<V&>(*this) = v;
        return *this;
	}
	
	template<typename V>
	any& operator=(V&& v)
	{
		static_assert(std::disjunction<std::is_same<V, T>, std::is_same<V, Rest>...>{});
		static_cast<V&>(*this) = v;
		return *this;
	}


// Access --------------------------------------------------------------------------------------------------------------

    template<typename V>
    V& get()
    {
        static_assert(std::disjunction<std::is_same<V, T>, std::is_same<V, Rest>...>{});
        return static_cast<V&>(*this);
    }

    template<typename V>
    const V& get() const
    {
        static_assert(std::disjunction<std::is_same<V, T>, std::is_same<V, Rest>...>{});
        return static_cast<const V&>(*this);
    }

// Cast operators ------------------------------------------------------------------------------------------------------

    operator       reference()       { return  Value; }
    operator const_reference() const { return  Value; }
    operator         pointer()       { return &Value; }
    operator   const_pointer() const { return &Value; }


// Variables ===========================================================================================================

private:
    static constexpr size_t Size = sizeof...(Rest);
    this_type               Value;
};

}


#endif // OPEN_CPP_UTILS_ANY_H
