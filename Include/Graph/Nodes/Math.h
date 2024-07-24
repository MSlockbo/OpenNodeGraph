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

#ifndef MATH_H
#define MATH_H

#include <glm/vec4.hpp>
#include <Graph/ShaderGraph.h>
#include <Utility/Any.h>

namespace OpenShaderDesigner::Nodes::Math
{
	inline static constexpr ImColor HeaderColor = ImColor(0x92, 0x16, 0x16);

	struct Constant : public Node
	{
		using ValueType = Any<int, unsigned int, float, glm::vec4>;

		Constant(ShaderGraph& graph, ImVec2 pos);
		virtual ~Constant() = default;

		[[nodiscard]] Node* Copy(ShaderGraph& graph) const override;
		void Inspect() override;

		ValueType    Value;
	};

	RegisterNode("Math/Constant", Constant);

	struct Add : public Node
	{
		Add(ShaderGraph& graph, ImVec2 pos);
		virtual ~Add() = default;

		[[nodiscard]] Node* Copy(ShaderGraph& graph) const override;
		void Inspect() override;
	};

	RegisterNode("Math/Add", Add);
}

#endif //MATH_H
