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

#ifndef OPEN_CPP_UTILS_MAP_H
#define OPEN_CPP_UTILS_MAP_H

#include <utility>

#include "set.h"

namespace open_cpp_utils
{

template<typename Key, typename Value, class Alloc = std::allocator<struct map_pair>>
class map
{
// Typedefs ============================================================================================================

public:
    using key_type   = Key;
    using value_type = Value;
    using pair_type  = struct map_pair;

    using key_pointer         = key_type*;
    using const_key_pointer   = const key_type*;
    using key_reference       = key_type&;
    using const_key_reference = const key_type&;

    using value_pointer         = value_type*;
    using const_value_pointer   = const value_type*;
    using value_reference       = value_type&;
    using const_value_reference = const value_type&;
    
    
    struct hash { size_t operator()(const pair_type& pair) const { return std::hash<key_type>{}(pair.key); } };
    
    using table_type = set<pair_type, hash, Alloc>;
    using iterator = typename table_type::iterator;
    using const_iterator = typename table_type::const_iterator;

    struct map_pair
    {
        key_type   key;
        value_type value;

        bool operator==(const map_pair& other) const { return key == other.key; }

        map_pair() : key(), value() { }
        map_pair(const_key_reference k) : key(k), value() { }
        map_pair(const_key_reference k, const_value_reference v) : key(k), value(v) { }
        map_pair(const map_pair& v) : key(v.key), value(v.value) { }
        map_pair(map_pair&& v) noexcept : key(std::move(v.key)), value(std::move(v.value)) { }
        ~map_pair() = default;

        map_pair& operator=(const map_pair& v)
        {
            key = v.key;
            value = v.value;
            return *this;
        }
        
        map_pair& operator=(map_pair&& v) noexcept
        {
            if(&v == this) return *this;
            
            key = std::move(v.key);
            value = std::move(v.value);
            return *this;
        }
    };


// Functions ===========================================================================================================

// Constructors & Destructor -------------------------------------------------------------------------------------------

public:
    map()           = default;
    map(std::initializer_list<pair_type> data) : table_(data) { }
    map(const map&) = default;
    map(map&&)      = default;
    ~map()          = default;

    void reserve(size_t size) { table_.reserve(size); }

    void insert(const_key_reference key, const_value_reference value);
    void erase(const_key_reference key);
    
    value_reference& operator[](const_key_reference key);
    
    const_value_reference& get(const_key_reference key, const_value_reference& def = value_type()) const;
    
    iterator find(const_key_reference key) { return table_.find({ key, value_type() }); }
    const_iterator find(const_key_reference key) const { return table_.find({ key, value_type() }); }
    
    bool contains(const_key_reference key) { return table_.contains({ key, value_type() }); }

    iterator begin() { return table_.begin(); }
    iterator end()   { return table_.end(); }
    
    const_iterator begin() const { return table_.begin(); }
    const_iterator end()   const { return table_.end(); }

    size_t size() const { return table_.size(); }

// Variables ===========================================================================================================

private:
    table_type table_;
};

template<typename Key, typename Value, class Alloc>
void map<Key, Value, Alloc>::insert(const_key_reference key, const_value_reference value)
{
    iterator it = find({ key, value });
    if(it != end()) table_.insert({ key, value });
}

template<typename Key, typename Value, class Alloc>
void map<Key, Value, Alloc>::erase(const_key_reference key)
{
    table_.erase({ key, value_type() });
}

template<typename Key, typename Value, class Alloc>
typename map<Key, Value, Alloc>::value_reference map<Key, Value, Alloc>::operator[](const_key_reference key)
{
    iterator it = table_.find({ key, value_type() });
    if(it == table_.end())
    {
        table_.insert(key);
        it = table_.find({ key, value_type() });
    }
    return it->value;
}

template<typename Key, typename Value, class Alloc>
typename map<Key, Value, Alloc>::const_value_reference map<Key, Value, Alloc>::get(const_key_reference key,
    const_value_reference def) const
{
    auto it = find(key);
    if(it == end()) return def;
    return it->value;
}
    
}

#endif // OPEN_CPP_UTILS_MAP_H
