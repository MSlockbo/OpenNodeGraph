// =====================================================================================================================
// Copyright 2024 Medusa Slockbower
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// =====================================================================================================================

#ifndef OPTIONAL_H
#define OPTIONAL_H

template<typename T>
class Optional
{
public:
	using Type = T;

	Optional() : Data(), Valid(false) { }
	Optional(const Type& data) : Data(data), Valid(true) { }
	Optional(Type&& data) : Data(data), Valid(true) { }
	Optional(const Optional& other) = default;
	Optional(Optional&& other) = default;

	Optional& operator=(const Optional& other) = default;
	Optional& operator=(Optional&& other) = default;

	Type& operator=(const Type& data) { Data = data; Valid = true; return Data; }
	Type& operator=(Type&& data) { Data = data; Valid = true; return Data; }

	Type& operator+=(const Type& data) { assert(Valid); Data += data; return Data; }
	Type& operator-=(const Type& data) { assert(Valid); Data += data; return Data; }
	Type& operator*=(const Type& data) { assert(Valid); Data += data; return Data; }
	Type& operator/=(const Type& data) { assert(Valid); Data += data; return Data; }
	Type& operator%=(const Type& data) { assert(Valid); Data += data; return Data; }

	Type& operator<<=(const Type& data) { assert(Valid); Data <<= data; return Data; }
	Type& operator>>=(const Type& data) { assert(Valid); Data >>= data; return Data; }
	Type& operator|=(const Type& data)  { assert(Valid); Data |= data;  return Data; }
	Type& operator&=(const Type& data)  { assert(Valid); Data &= data;  return Data; }
	Type& operator^=(const Type& data)  { assert(Valid); Data ^= data;  return Data; }

	[[nodiscard]] bool operator()() const { return Valid; }

	operator Type&() { assert(Valid); return Data; }
	operator const Type&() const { assert(Valid); return Data; }

	Type* operator->() { assert(Valid); return &Data; }
	const Type* operator->() const { assert(Valid); return &Data; }

	Type& operator*()  { assert(Valid); return Data; }
	const Type& operator*() const  { assert(Valid); return Data; }

	void Reset() { Valid = false; }

private:
	Type Data;
	bool Valid;
};

#endif //OPTIONAL_H
