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

#include <Graph/Nodes/Math.h>
#include <imgui-extras/imgui_extras.h>

using namespace OpenShaderDesigner;
using namespace OpenShaderDesigner::Nodes::Math;

Constant::Constant(ShaderGraph& graph, ImVec2 pos)
	: Node(
		graph, pos
	,	"Constant", HeaderColor
	,	{ }, false
	,	{ { "Out", Pin::FLOAT, Pin::OUTPUT } }
	)
{
}

Node* Constant::Copy(ShaderGraph& graph) const
{
	return new Constant(graph, Position);
}

void Constant::Inspect()
{
	Pin::PinType& Type = IO.Outputs[0].Type;

	if(ImGui::BeginCombo("Type", Pin::TypeNames[Type].c_str()))
	{
		for(int i = 0; i < Pin::ANY; ++i)
		{
			Pin::PinType t = static_cast<Pin::PinType>(i);

			if(ImGui::Selectable(Pin::TypeNames[t].c_str(), t == Type))
			{
				Type = t;
			}
		}

		ImGui::EndCombo();
	}

	glm::vec4& v = Value;

	switch(Type)
	{
	case Pin::INT:
		ImGui::InputInt("Value", Value);
		break;

	case Pin::UINT:
		ImGui::InputUInt("Value", Value);
		break;

	case Pin::FLOAT:
		ImGui::InputFloat("Value", Value);
		break;

	case Pin::VECTOR:
		ImGui::ColorEdit4("Value", &v.x);
		break;

	default:
		break;
	}
}

Add::Add(ShaderGraph& graph, ImVec2 pos)
	: Node(
		graph, pos
	,	"Add", HeaderColor
	,	{ { "A",   Pin::ANY, Pin::INPUT }, { "B", Pin::ANY, Pin::INPUT } }, true
	,	{ { "Out", Pin::ANY, Pin::OUTPUT } }
	)
{ }

Node* Add::Copy(ShaderGraph& graph) const
{
	return new Add(graph, Position);
}

void Add::Inspect()
{

}
