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

#include <Graph/Nodes/Math/Trigonometry.h>

using namespace OpenShaderDesigner;
using namespace OpenShaderDesigner::Nodes::Math;

RegisterNode("Math/Trigonometry/Sin", Sine);
RegisterNode("Math/Trigonometry/Cos", Cosine);
RegisterNode("Math/Trigonometry/Tan", Tangent);

RegisterNode("Math/Trigonometry/ASin", ArcSine);
RegisterNode("Math/Trigonometry/ACos", ArcCosine);
RegisterNode("Math/Trigonometry/ATan", ArcTangent);


// =====================================================================================================================
// Trigonometry
// =====================================================================================================================


// Sine ----------------------------------------------------------------------------------------------------------------
 
Sine::Sine(ShaderGraph &graph, ImVec2 pos)
    : MathOp(graph, pos)
{
    Info.Alias          = "Sine";
    Header.Title        = HeaderMarker + "Sin";
    Header.Color        = HeaderColor;
    Header.HoveredColor = HeaderHoveredColor;
	Header.ActiveColor  = HeaderActiveColor;

	IO.Inputs.emplace_back("In",   PinType_Any, PinFlags_Ambiguous);
	IO.Outputs.emplace_back("Out", PinType_Any, PinFlags_Ambiguous);
}

Node* Sine::Copy(ShaderGraph &graph) const
{
    return new Sine(graph, Position);
}

void Sine::Inspect()
{
	
}

std::string Sine::GetCode() const
{
	// TODO: Support more than 2 inputs
	return std::format("const {} {} = sin({});"
	,   Pin::TypeKeywords[IO.Outputs[0].Type]
	,   Graph.GetValue(IO.Outputs[0].Ptr)
	,   Graph.GetValue(IO.Inputs[0].Ptr)
	);
}


// Cosine ----------------------------------------------------------------------------------------------------------------
 
Cosine::Cosine(ShaderGraph &graph, ImVec2 pos)
	: MathOp(graph, pos)
{
	Info.Alias          = "Cosine";
	Header.Title        = HeaderMarker + "Cos";
	Header.Color        = HeaderColor;
	Header.HoveredColor = HeaderHoveredColor;
	Header.ActiveColor  = HeaderActiveColor;

	IO.Inputs.emplace_back("In",   PinType_Any, PinFlags_Ambiguous);
	IO.Outputs.emplace_back("Out", PinType_Any, PinFlags_Ambiguous);
}

Node* Cosine::Copy(ShaderGraph &graph) const
{
	return new Cosine(graph, Position);
}

void Cosine::Inspect()
{
	
}

std::string Cosine::GetCode() const
{
	return std::format("const {} {} = cos({});"
	,   Pin::TypeKeywords[IO.Outputs[0].Type]
	,   Graph.GetValue(IO.Outputs[0].Ptr)
	,   Graph.GetValue(IO.Inputs[0].Ptr)
	);
}


// Tangent ----------------------------------------------------------------------------------------------------------------
 
Tangent::Tangent(ShaderGraph &graph, ImVec2 pos)
	: MathOp(graph, pos)
{
	Info.Alias          = "Tangent";
	Header.Title        = HeaderMarker + "Tan";
	Header.Color        = HeaderColor;
	Header.HoveredColor = HeaderHoveredColor;
	Header.ActiveColor  = HeaderActiveColor;

	IO.Inputs.emplace_back("In",   PinType_Any, PinFlags_Ambiguous);
	IO.Outputs.emplace_back("Out", PinType_Any, PinFlags_Ambiguous);
}

Node* Tangent::Copy(ShaderGraph &graph) const
{
	return new Tangent(graph, Position);
}

void Tangent::Inspect()
{
	
}

std::string Tangent::GetCode() const
{
	return std::format("const {} {} = tan({});"
	,   Pin::TypeKeywords[IO.Outputs[0].Type]
	,   Graph.GetValue(IO.Outputs[0].Ptr)
	,   Graph.GetValue(IO.Inputs[0].Ptr)
	);
}


// ArcSine ----------------------------------------------------------------------------------------------------------------
 
ArcSine::ArcSine(ShaderGraph &graph, ImVec2 pos)
    : MathOp(graph, pos)
{
    Info.Alias          = "ArcSine";
    Header.Title        = HeaderMarker + "ASin";
    Header.Color        = HeaderColor;
    Header.HoveredColor = HeaderHoveredColor;
	Header.ActiveColor  = HeaderActiveColor;

	IO.Inputs.emplace_back("In",   PinType_Any, PinFlags_Ambiguous);
	IO.Outputs.emplace_back("Out", PinType_Any, PinFlags_Ambiguous);
}

Node* ArcSine::Copy(ShaderGraph &graph) const
{
    return new ArcSine(graph, Position);
}

void ArcSine::Inspect()
{
	
}

std::string ArcSine::GetCode() const
{
	// TODO: Support more than 2 inputs
	return std::format("const {} {} = asin({});"
	,   Pin::TypeKeywords[IO.Outputs[0].Type]
	,   Graph.GetValue(IO.Outputs[0].Ptr)
	,   Graph.GetValue(IO.Inputs[0].Ptr)
	);
}


// Cosine ----------------------------------------------------------------------------------------------------------------
 
ArcCosine::ArcCosine(ShaderGraph &graph, ImVec2 pos)
	: MathOp(graph, pos)
{
	Info.Alias          = "ArcCosine";
	Header.Title        = HeaderMarker + "ACos";
	Header.Color        = HeaderColor;
	Header.HoveredColor = HeaderHoveredColor;
	Header.ActiveColor  = HeaderActiveColor;

	IO.Inputs.emplace_back("In",   PinType_Any, PinFlags_Ambiguous);
	IO.Outputs.emplace_back("Out", PinType_Any, PinFlags_Ambiguous);
}

Node* ArcCosine::Copy(ShaderGraph &graph) const
{
	return new Cosine(graph, Position);
}

void ArcCosine::Inspect()
{
	
}

std::string ArcCosine::GetCode() const
{
	return std::format("const {} {} = cos({});"
	,   Pin::TypeKeywords[IO.Outputs[0].Type]
	,   Graph.GetValue(IO.Outputs[0].Ptr)
	,   Graph.GetValue(IO.Inputs[0].Ptr)
	);
}


// Tangent ----------------------------------------------------------------------------------------------------------------
 
ArcTangent::ArcTangent(ShaderGraph &graph, ImVec2 pos)
	: MathOp(graph, pos)
{
	Info.Alias          = "ArcTangent";
	Header.Title        = HeaderMarker + "ATan";
	Header.Color        = HeaderColor;
	Header.HoveredColor = HeaderHoveredColor;
	Header.ActiveColor  = HeaderActiveColor;

	IO.Inputs.emplace_back("In",   PinType_Any, PinFlags_Ambiguous);
	IO.Outputs.emplace_back("Out", PinType_Any, PinFlags_Ambiguous);
}

Node* ArcTangent::Copy(ShaderGraph &graph) const
{
	return new ArcTangent(graph, Position);
}

void ArcTangent::Inspect()
{
	
}

std::string ArcTangent::GetCode() const
{
	return std::format("const {} {} = tan({});"
	,   Pin::TypeKeywords[IO.Outputs[0].Type]
	,   Graph.GetValue(IO.Outputs[0].Ptr)
	,   Graph.GetValue(IO.Inputs[0].Ptr)
	);
}

