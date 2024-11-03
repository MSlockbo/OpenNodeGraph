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

#ifndef OPEN_CPP_UTILS_OBJECT_POOL_H
#define OPEN_CPP_UTILS_OBJECT_POOL_H

#include "dynarray.h"
#include "optional.h"

namespace open_cpp_utils
{

/**
 * \brief
 * \tparam T
 */
template<typename T>
class object_list
{
// Typedefs ============================================================================================================

public:
    using value_type      = T;
    using pointer         = T*;
    using const_pointer   = const T*;
    using reference       = T&;
    using const_reference = const T&;

    using uuid_type  = uint64_t;

private:
    using node = optional<value_type>;


// Functions ===========================================================================================================

public:

// Constructors & Destructor -------------------------------------------------------------------------------------------

    object_list() = default;
    object_list(const object_list& other) = default;
    object_list(object_list&& other) = default;
    ~object_list() = default;


// Modifiers -----------------------------------------------------------------------------------------------------------

    size_t size() const { return data_.size(); }
	size_t capacity() const { return data_.capacity(); }


// Modifiers -----------------------------------------------------------------------------------------------------------

    void clear() { data_.clear(); freed_.clear(); }

    uuid_type insert(const_reference& value);
    void      erase(uuid_type id);

    object_list& operator=(const object_list&) = default;
    object_list& operator=(object_list&&) = default;


// Accessors -----------------------------------------------------------------------------------------------------------

    reference       operator[](uuid_type id)       { assert(data_[id]()); return data_[id]; }
    const_reference operator[](uuid_type id) const { assert(data_[id]()); return data_[id]; }
    bool            operator()(uuid_type id) const { return data_[id](); }

    typename dynarray<node>::iterator begin() { return data_.begin(); }
    typename dynarray<node>::iterator end()   { return data_.end(); }


// Variables ===========================================================================================================

private:
    dynarray<node>      data_;
    dynarray<uuid_type> freed_;
};

template<typename T>
typename object_list<T>::uuid_type object_list<T>::insert(const_reference value)
{
    if(freed_.empty()) { data_.push_back(value); return data_.size() - 1; }

    uuid_type id = freed_.back(); freed_.pop_back();
    data_[id] = value;
    return id;
}

template<typename T>
void object_list<T>::erase(uuid_type id)
{
    data_[id].reset();
    freed_.push_back(id);
}

} // open_cpp_utils

#endif // OPEN_CPP_UTILS_OBJECT_POOL_H
