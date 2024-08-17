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


// =====================================================================================================================
// Integral
// =====================================================================================================================

Integral::Integral(ShaderGraph& graph, ImVec2 pos)
    : Node(graph, pos)
{
    Header.Title        = HeaderMarker + "Integral";
    Header.Color        = HeaderColor;
    Header.HoveredColor = HeaderHoveredColor;
    Header.ActiveColor  = HeaderActiveColor;

    IO.Outputs.emplace_back("Out", PinType_Float, PinFlags_NoCollapse | PinFlags_NoPadding);
}

Node* Integral::Copy(ShaderGraph& graph) const
{
    return new Integral(graph, Position);
}

void Integral::Inspect()
{

}


// =====================================================================================================================
// Scalar
// =====================================================================================================================

UnsignedIntegral::UnsignedIntegral(ShaderGraph& graph, ImVec2 pos)
    : Node(graph, pos)
{
    Header.Title        = HeaderMarker + "Unsigned Integral";
    Header.Color        = HeaderColor;
    Header.HoveredColor = HeaderHoveredColor;
    Header.ActiveColor  = HeaderActiveColor;

    IO.Outputs.emplace_back("Out", PinType_Float, PinFlags_NoCollapse | PinFlags_NoPadding);
}

Node* UnsignedIntegral::Copy(ShaderGraph& graph) const
{
    return new UnsignedIntegral(graph, Position);
}

void UnsignedIntegral::Inspect()
{

}


// =====================================================================================================================
// Scalar
// =====================================================================================================================

Scalar::Scalar(ShaderGraph& graph, ImVec2 pos)
	: Node(graph, pos)
{
    Header.Title        = HeaderMarker + "Scalar";
    Header.Color        = HeaderColor;
    Header.HoveredColor = HeaderHoveredColor;
    Header.ActiveColor  = HeaderActiveColor;

    IO.Outputs.emplace_back("Out", PinType_Float, PinFlags_NoCollapse | PinFlags_NoPadding);
}

Node* Scalar::Copy(ShaderGraph& graph) const
{
	return new Scalar(graph, Position);
}

void Scalar::Inspect()
{

}


// =====================================================================================================================
// Vector
// =====================================================================================================================

Vector::Vector(ShaderGraph &graph, ImVec2 pos)
	: Node(graph, pos)
{
    Header.Title        = HeaderMarker + "Vector";
    Header.Color        = HeaderColor;
    Header.HoveredColor = HeaderHoveredColor;
    Header.ActiveColor  = HeaderActiveColor;

    IO.Outputs.emplace_back("Out", PinType_Vector, PinFlags_NoCollapse | PinFlags_NoPadding);

	IO.Outputs[0].Value.get<ImVec4>() = ImVec4(0, 0, 0, 1);
}

Node* Vector::Copy(ShaderGraph &graph) const
{
	return new Vector(graph, Position);
}

void Vector::Inspect()
{

}


// =====================================================================================================================
// Add
// =====================================================================================================================

Add::Add(ShaderGraph& graph, ImVec2 pos)
	: Node(graph, pos)
{
    Header.Title        = HeaderMarker + "Add";
    Header.Color        = HeaderColor;
    Header.HoveredColor = HeaderHoveredColor;
    Header.ActiveColor  = HeaderActiveColor;

    IO.Inputs.emplace_back("A", PinType_Any);
    IO.Inputs.emplace_back("B", PinType_Any);
    IO.DynamicInputs = true;

    IO.Outputs.emplace_back("B", PinType_Any);
}

Node* Add::Copy(ShaderGraph& graph) const
{
	return new Add(graph, Position);
}

void Add::Inspect()
{

}
