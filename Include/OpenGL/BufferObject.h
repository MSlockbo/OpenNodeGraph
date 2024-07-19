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

#ifndef BUFFEROBJECT_H
#define BUFFEROBJECT_H

#ifndef NULL
#define NULL 0
#endif

#include <algorithm>

#include "Enum.h"
#include "Type.h"

namespace GLW
{
// Definition ==========================================================================================================

	template<BufferType T, BufferUsage U, BufferStorage S = GPU>
	class BufferObject
	{
	public:
		static constexpr BufferType    Type    = T;
		static constexpr BufferUsage   Usage   = U;
		static constexpr BufferStorage Storage = S;

		/**
		 * \brief BufferObject constructor
		 * \param size Size in bytes of the Buffer
		 * \param data Data to be used as the initial contents of the Buffer
		 */
		BufferObject(SizeT size, void* data = nullptr);

		/**
		 * \brief Move Constructor
		 */
		BufferObject(BufferObject&& other);

		/**
		 * \brief Copy Constructor
		 */
		BufferObject(const BufferObject& other);

		/**
		 * \brief Destructor
		 */
		~BufferObject();

		/**
		 * \brief Validity test
		 */
		[[nodiscard]] operator bool() const { return Handle != 0; }

		/**
		 * \brief Copy Assignment
		 */
		BufferObject& operator=(const BufferObject& other);

		/**
		 * \brief Move Assignment
		 */
		BufferObject& operator=(BufferObject&& other) noexcept;

		[[nodiscard]] SizeT Size() const { return Size; }
		void Resize(SizeT size);

	private:
		HandleT Handle;
		SizeT   Size;
		void*   Mapping;
	};

// Constructors ========================================================================================================

	template <BufferType T, BufferUsage U, BufferStorage S>
	BufferObject<T, U, S>::BufferObject(SizeT size, void* data)
		: Handle(NULL)
		, Size(size)
		, Mapping(nullptr)
	{
		glGenBuffers(1, &Handle);

		if(!*this) return;

		glBindBuffer(Type, Handle);
		glBufferStorage(Type, Size, data, Usage);
		glBindBuffer(Type, 0);
	}

	template <BufferType T, BufferUsage U, BufferStorage S>
	BufferObject<T, U, S>::BufferObject(BufferObject&& other)
		: Handle(other.Handle)
		, Size(other.Size)
		, Mapping(other.Size)
	{
		other.Handle  = NULL;
		other.Size    = 0;
		other.Mapping = nullptr;
	}

	template <BufferType T, BufferUsage U, BufferStorage S>
	BufferObject<T, U, S>::BufferObject(const BufferObject& other)
		: BufferObject(other.Size)
	{
		if(Handle == NULL) return;
		if(other.Handle == NULL) return;

		glCopyNamedBufferSubData(other.Handle, Handle, 0, 0, Size);
	}

	template <BufferType T, BufferUsage U, BufferStorage S>
	BufferObject<T, U, S>::~BufferObject()
	{
		glDeleteBuffers(1, &Handle);
	}

// Assignment Operators ================================================================================================

	template <BufferType T, BufferUsage U, BufferStorage S>
	BufferObject<T, U, S>& BufferObject<T, U, S>::operator=(const BufferObject& other)
	{
		BufferObject temp(other);
		return (*this = std::move(temp)); // NOLINT(*-unconventional-assign-operator)
	}

	template <BufferType T, BufferUsage U, BufferStorage S>
	BufferObject<T, U, S>& BufferObject<T, U, S>::operator=(BufferObject&& other) noexcept
	{
		glDeleteBuffers(1, &Handle);

		Handle  = other.Handle;
		Size    = other.Size;
		Mapping = other.Mapping;

		other.Handle  = NULL;
		other.Size    = 0;
		other.Mapping = nullptr;

		return *this;
	}

	template <BufferType T, BufferUsage U, BufferStorage S>
	void BufferObject<T, U, S>::Resize(SizeT size)
	{
		BufferObject temp(size);
		glCopyNamedBufferSubData(Handle, temp.Handle, 0, 0, Size);
		*this = std::move(temp);
	}
}

#endif //BUFFEROBJECT_H
