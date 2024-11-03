// =====================================================================================================================
//  OpenShaderDesigner, an open source software utility to create materials and shaders.
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

#include <Graph/Nodes/Math/Comparison.h>

using namespace OpenShaderDesigner;
using namespace OpenShaderDesigner::Nodes::Math;

RegisterNode("Math/Comparison/Min",      Minimum);
RegisterNode("Math/Comparison/Max",      Maximum);
RegisterNode("Math/Comparison/Clamp",    Clamp);
RegisterNode("Math/Comparison/Saturate", Saturate);

// =====================================================================================================================
// User Defined Constants
// =====================================================================================================================


// Minimum -----------------------------------------------------------------------------------------------------------------

Minimum::Minimum(ShaderGraph& graph, ImVec2 pos)
	: MathOp(graph, pos)
{
	Info.Alias          = "Minimum";
	Header.Title        = HeaderMarker + "Min";
	Header.Color        = HeaderColor;
	Header.HoveredColor = HeaderHoveredColor;
	Header.ActiveColor  = HeaderActiveColor;

	IO.Inputs.emplace_back("A", PinType_Any, PinFlags_Ambiguous);
	IO.Inputs.emplace_back("B", PinType_Any, PinFlags_Ambiguous);
	Info.Flags |= NodeFlags_DynamicInputs;

	IO.Outputs.emplace_back("Out", PinType_Any, PinFlags_Ambiguous);
}

Node* Minimum::Copy(ShaderGraph& graph) const
{
	return new Minimum(graph, Position);
}

void Minimum::Inspect()
{

}

std::string Minimum::GetCode() const
{
	// TODO: Support more than 2 inputs
	return std::format("const {} {} = min({}, {});",
		Pin::TypeKeywords[IO.Outputs[0].Type]
	,   Graph.GetValue(IO.Outputs[0].Ptr)
	,   Graph.GetValue(IO.Inputs[0].Ptr)
	,   Graph.GetValue(IO.Inputs[1].Ptr)
	);
}


// Maximum -----------------------------------------------------------------------------------------------------------------

Maximum::Maximum(ShaderGraph& graph, ImVec2 pos)
	: MathOp(graph, pos)
{
	Info.Alias          = "Maximum";
	Header.Title        = HeaderMarker + "Max";
	Header.Color        = HeaderColor;
	Header.HoveredColor = HeaderHoveredColor;
	Header.ActiveColor  = HeaderActiveColor;

	IO.Inputs.emplace_back("A", PinType_Any, PinFlags_Ambiguous);
	IO.Inputs.emplace_back("B", PinType_Any, PinFlags_Ambiguous);
	Info.Flags |= NodeFlags_DynamicInputs;

	IO.Outputs.emplace_back("Out", PinType_Any, PinFlags_Ambiguous);
}

Node* Maximum::Copy(ShaderGraph& graph) const
{
	return new Maximum(graph, Position);
}

void Maximum::Inspect()
{

}

std::string Maximum::GetCode() const
{
	return std::format("const {} {} = max({}, {});",
		Pin::TypeKeywords[IO.Outputs[0].Type]
	,   Graph.GetValue(IO.Outputs[0].Ptr)
	,   Graph.GetValue(IO.Inputs[0].Ptr)
	,   Graph.GetValue(IO.Inputs[1].Ptr)
	);
}


// Clamp -----------------------------------------------------------------------------------------------------------------

Clamp::Clamp(ShaderGraph& graph, ImVec2 pos)
	: MathOp(graph, pos)
{
	Info.Alias          = "Clamp";
	Header.Title        = HeaderMarker + "Clamp";
	Header.Color        = HeaderColor;
	Header.HoveredColor = HeaderHoveredColor;
	Header.ActiveColor  = HeaderActiveColor;

	IO.Inputs.emplace_back("X", PinType_Any, PinFlags_Ambiguous);
	IO.Inputs.emplace_back("Min", PinType_Any, PinFlags_Ambiguous);
	IO.Inputs.emplace_back("Max", PinType_Any, PinFlags_Ambiguous);
	Info.Flags |= NodeFlags_DynamicInputs;

	IO.Outputs.emplace_back("Out", PinType_Any, PinFlags_Ambiguous);
}

Node* Clamp::Copy(ShaderGraph& graph) const
{
	return new Clamp(graph, Position);
}

void Clamp::Inspect()
{

}

std::string Clamp::GetCode() const
{
	return std::format("const {} {} = clamp({}, {}, {});",
		Pin::TypeKeywords[IO.Outputs[0].Type]
	,   Graph.GetValue(IO.Outputs[0].Ptr)
	,   Graph.GetValue(IO.Inputs[0].Ptr)
	,   Graph.GetValue(IO.Inputs[1].Ptr)
	,   Graph.GetValue(IO.Inputs[2].Ptr)
	);
}


// Clamp -----------------------------------------------------------------------------------------------------------------

Saturate::Saturate(ShaderGraph& graph, ImVec2 pos)
	: MathOp(graph, pos)
{
	Info.Alias          = "Saturate";
	Header.Title        = HeaderMarker + "Saturate";
	Header.Color        = HeaderColor;
	Header.HoveredColor = HeaderHoveredColor;
	Header.ActiveColor  = HeaderActiveColor;

	IO.Inputs.emplace_back("X", PinType_Any, PinFlags_Ambiguous);
	Info.Flags |= NodeFlags_DynamicInputs;

	IO.Outputs.emplace_back("Out", PinType_Any, PinFlags_Ambiguous);
}

Node* Saturate::Copy(ShaderGraph& graph) const
{
	return new Saturate(graph, Position);
}

void Saturate::Inspect()
{

}

std::string Saturate::GetCode() const
{
	switch(IO.Outputs[0].Type)
	{
	case PinType_Int: case PinType_UInt:
		return std::format("const {} {} = clamp({}, 0, 1);",
			Pin::TypeKeywords[IO.Outputs[0].Type]
		,   Graph.GetValue(IO.Outputs[0].Ptr)
		,   Graph.GetValue(IO.Inputs[0].Ptr)
		);
		
	case PinType_Float: default:
		return std::format("const {} {} = clamp({}, 0.0, 1.0);",
			Pin::TypeKeywords[IO.Outputs[0].Type]
		,   Graph.GetValue(IO.Outputs[0].Ptr)
		,   Graph.GetValue(IO.Inputs[0].Ptr)
		);
		
	case PinType_Vector:
		return std::format("const {} {} = clamp({}, vec3(0.0), vec3(1.0));",
			Pin::TypeKeywords[IO.Outputs[0].Type]
		,   Graph.GetValue(IO.Outputs[0].Ptr)
		,   Graph.GetValue(IO.Inputs[0].Ptr)
		);
	}
}