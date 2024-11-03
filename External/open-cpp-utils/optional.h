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

#ifndef OPEN_CPP_UTILS_OPTIONAL_H
#define OPEN_CPP_UTILS_OPTIONAL_H

#include <assert.h>

namespace open_cpp_utils
{
    template<typename T>
    class optional
    {
    public:
        using value_type = T;

        optional() : valid_(false) { }
        optional(const value_type& data) : data_(data), valid_(true) { }
        optional(value_type&& data) : data_(data), valid_(true) { }
        optional(const optional& other) = default;
        optional(optional&& other) = default;

        optional& operator=(const optional& other) = default;
        optional& operator=(optional&& other) = default;

	    optional& operator=(const value_type& data) { data_ = data; valid_ = true; return *this; }
	    optional& operator=(value_type&& data) { data_ = data; valid_ = true; return *this; }

        bool operator==(const optional& b) const
	    {
	        if(not(valid_ || b.valid_)) return valid_ == b.valid_;
	        return data_ == b.data_;
	    }

        value_type& operator+=(const value_type& data) { assert(valid_); data_ += data; return data_; }
        value_type& operator-=(const value_type& data) { assert(valid_); data_ += data; return data_; }
        value_type& operator*=(const value_type& data) { assert(valid_); data_ += data; return data_; }
        value_type& operator/=(const value_type& data) { assert(valid_); data_ += data; return data_; }
        value_type& operator%=(const value_type& data) { assert(valid_); data_ += data; return data_; }

        value_type& operator<<=(const value_type& data) { assert(valid_); data_ <<= data; return data_; }
        value_type& operator>>=(const value_type& data) { assert(valid_); data_ >>= data; return data_; }
        value_type& operator|=(const value_type& data)  { assert(valid_); data_ |= data;  return data_; }
        value_type& operator&=(const value_type& data)  { assert(valid_); data_ &= data;  return data_; }
        value_type& operator^=(const value_type& data)  { assert(valid_); data_ ^= data;  return data_; }

        [[nodiscard]] bool operator()() const { return valid_; }

        operator value_type() const { assert(valid_); return data_; }

        operator       value_type&()       { assert(valid_); return data_; }
        operator const value_type&() const { assert(valid_); return data_; }

	          value_type* operator->()       { assert(valid_); return &data_; }
        const value_type* operator->() const { assert(valid_); return &data_; }

	          value_type& operator*()        { assert(valid_); return data_; }
        const value_type& operator*() const  { assert(valid_); return data_; }

        void reset() { valid_ = false; }

    private:
	    value_type data_;
        bool valid_;
    };
}

#endif // OPEN_CPP_UTILS_OPTIONAL_H
