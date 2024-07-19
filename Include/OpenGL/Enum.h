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

#ifndef ENUM_H
#define ENUM_H

#include <gl/glew.h>

namespace GLW
{
	enum Primitive : GLenum
	{
		TRIANGLES = GL_TRIANGLES
	,	LINES = GL_LINES
	,	POINTS = GL_POINTS
	};

	enum BufferType : GLenum
	{
		AtomicCounter = GL_ATOMIC_COUNTER_BUFFER
	,	ElementArray = GL_ELEMENT_ARRAY_BUFFER
	,	IndirectCompute = GL_DISPATCH_INDIRECT_BUFFER
	,	IndirectDraw = GL_DRAW_INDIRECT_BUFFER
	,	PixelPack = GL_PIXEL_PACK_BUFFER
	,	PixelUnpack = GL_PIXEL_UNPACK_BUFFER
	,	ShaderStorage = GL_SHADER_STORAGE_BUFFER
	,	TransformFeedback = GL_TRANSFORM_FEEDBACK
	,	Uniform = GL_UNIFORM_BUFFER
	,	VertexArray = GL_ARRAY_BUFFER
	};

	enum BufferUsage : GLenum
	{
		/**
		 * \brief Buffer cannot be read or modified by the CPU after creation
		 */
		STATIC     = GL_NONE

		/**
		 * \brief Buffer can be mapped for Read, Write, or Both
		 */
	,	READ       = GL_MAP_READ_BIT
	,	WRITE      = GL_MAP_WRITE_BIT
	,	READ_WRITE = READ | WRITE

		/**
		 * \brief Buffer can be written to using glBufferSubData
		 */
	,	DYNAMIC = GL_DYNAMIC_STORAGE_BIT

		/**
		 * \brief Buffer may be used while mapped
		 */
	,	PERSISTENT = GL_MAP_PERSISTENT_BIT

		/**
		 * \brief Allows buffer to remain coherent without an explicit barrier
		 */
	,	COHERENT = GL_MAP_PERSISTENT_BIT | PERSISTENT
	};

	enum BufferStorage : GLenum
	{
		/**
		 * \brief Buffer memory will come from the GPU
		 */
		GPU = GL_NONE

		/**
		 * \brief Buffer memory will come from the CPU
		 */
	,	CPU = GL_CLIENT_STORAGE_BIT
	};
}

#endif //ENUM_H
