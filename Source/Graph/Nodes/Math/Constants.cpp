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

#include <Graph/Nodes/Math/Constants.h>

#include <glm/gtc/constants.hpp>

using namespace OpenShaderDesigner;
using namespace OpenShaderDesigner::Nodes::Math;

// User Defined Constants
RegisterNode("Math/Constants/Integer",          Integer);
RegisterNode("Math/Constants/Unsigned Integer", UnsignedInteger);
RegisterNode("Math/Constants/Scalar",           Scalar);
RegisterNode("Math/Constants/Vector",           Vector);

// Numeric Constants
RegisterNode("Math/Constants/π", Pi);
RegisterNode("Math/Constants/e", EulersNumber);
RegisterNode("Math/Constants/φ", GoldenRatio);

// =====================================================================================================================
// User Defined Constants
// =====================================================================================================================


// Integer -------------------------------------------------------------------------------------------------------------

Integer::Integer(ShaderGraph& graph, ImVec2 pos)
    : Node(graph, pos)
{
    Header.Title        = HeaderMarker + "Integer";
    Header.Color        = HeaderColor;
    Header.HoveredColor = HeaderHoveredColor;
    Header.ActiveColor  = HeaderActiveColor;

    IO.Outputs.emplace_back("Out", PinType_Int, PinFlags_NoCollapse | PinFlags_NoPadding | PinFlags_Literal);
}

Node* Integer::Copy(ShaderGraph& graph) const
{
    return new Integer(graph, Position);
}

std::string Integer::GetCode() const
{
    return std::format("const int {} = {};",
        Graph.GetValue(IO.Outputs[0].Ptr)
    ,   IO.Outputs[0].Value.get<int>()
    );
}


// Unsigned Integer ----------------------------------------------------------------------------------------------------

UnsignedInteger::UnsignedInteger(ShaderGraph& graph, ImVec2 pos)
    : Node(graph, pos)
{
    Header.Title        = HeaderMarker + "Unsigned Integer";
    Header.Color        = HeaderColor;
    Header.HoveredColor = HeaderHoveredColor;
    Header.ActiveColor  = HeaderActiveColor;

    IO.Outputs.emplace_back("Out", PinType_UInt, PinFlags_NoCollapse | PinFlags_NoPadding | PinFlags_Literal);
}

Node* UnsignedInteger::Copy(ShaderGraph& graph) const
{
    return new UnsignedInteger(graph, Position);
}

std::string UnsignedInteger::GetCode() const
{
    return std::format("const unsigned int {} = {};",
        Graph.GetValue(IO.Outputs[0].Ptr)
    ,   IO.Outputs[0].Value.get<unsigned int>()
    );
}


// Scalar --------------------------------------------------------------------------------------------------------------

Scalar::Scalar(ShaderGraph& graph, ImVec2 pos)
	: Node(graph, pos)
{
    Header.Title        = HeaderMarker + "Scalar";
    Header.Color        = HeaderColor;
    Header.HoveredColor = HeaderHoveredColor;
    Header.ActiveColor  = HeaderActiveColor;

    IO.Outputs.emplace_back("Out", PinType_Float, PinFlags_NoCollapse | PinFlags_NoPadding | PinFlags_Literal);
}

Node* Scalar::Copy(ShaderGraph& graph) const
{
	return new Scalar(graph, Position);
}

std::string Scalar::GetCode() const
{
    return std::format("const float {} = {};",
        Graph.GetValue(IO.Outputs[0].Ptr)
    ,   IO.Outputs[0].Value.get<float>()
    );
}


// =====================================================================================================================
// Numeric Constants
// =====================================================================================================================


// Pi --------------------------------------------------------------------------------------------------------------

Pi::Pi(ShaderGraph& graph, ImVec2 pos)
	: Scalar(graph, pos)
{
	Header.Title        = HeaderMarker + "π";

	IO.Outputs[0].Value = glm::pi<float>();
	IO.Outputs[0].Name  = "π";
	IO.Outputs[0].Flags = PinFlags_AlwaysCollapse | PinFlags_NoPadding | PinFlags_Literal;
}

Node* Pi::Copy(ShaderGraph& graph) const
{
	return new Scalar(graph, Position);
}

std::string Pi::GetCode() const
{
	return std::format("const float {} = {};",
		Graph.GetValue(IO.Outputs[0].Ptr)
	,   IO.Outputs[0].Value.get<float>()
	);
}


// EulersNumber --------------------------------------------------------------------------------------------------------------

EulersNumber::EulersNumber(ShaderGraph& graph, ImVec2 pos)
	: Scalar(graph, pos)
{
	Header.Title        = HeaderMarker + "e";

	IO.Outputs[0].Value = glm::euler<float>();
	IO.Outputs[0].Name  = "e";
	IO.Outputs[0].Flags = PinFlags_AlwaysCollapse | PinFlags_NoPadding | PinFlags_Literal;
}

Node* EulersNumber::Copy(ShaderGraph& graph) const
{
	return new Scalar(graph, Position);
}

std::string EulersNumber::GetCode() const
{
	return std::format("const float {} = {};",
		Graph.GetValue(IO.Outputs[0].Ptr)
	,   IO.Outputs[0].Value.get<float>()
	);
}


// EulersNumber --------------------------------------------------------------------------------------------------------------

GoldenRatio::GoldenRatio(ShaderGraph& graph, ImVec2 pos)
	: Scalar(graph, pos)
{
	Header.Title        = HeaderMarker + "φ";

	IO.Outputs[0].Value = glm::golden_ratio<float>();
	IO.Outputs[0].Name  = "φ";
	IO.Outputs[0].Flags = PinFlags_AlwaysCollapse | PinFlags_NoPadding | PinFlags_Literal;
}

Node* GoldenRatio::Copy(ShaderGraph& graph) const
{
	return new Scalar(graph, Position);
}

std::string GoldenRatio::GetCode() const
{
	return std::format("const float {} = {};",
		Graph.GetValue(IO.Outputs[0].Ptr)
	,   IO.Outputs[0].Value.get<float>()
	);
}


// Vector --------------------------------------------------------------------------------------------------------------

Vector::Vector(ShaderGraph &graph, ImVec2 pos)
	: Node(graph, pos)
{
    Header.Title        = HeaderMarker + "Vector";
    Header.Color        = HeaderColor;
    Header.HoveredColor = HeaderHoveredColor;
    Header.ActiveColor  = HeaderActiveColor;

    IO.Outputs.emplace_back("Out", PinType_Vector, PinFlags_NoCollapse | PinFlags_NoPadding | PinFlags_Literal);

	IO.Outputs[0].Value.get<glm::vec3>() = glm::vec3(0);
}

Node* Vector::Copy(ShaderGraph &graph) const
{
	return new Vector(graph, Position);
}

std::string Vector::GetCode() const
{
    const glm::vec3& val = IO.Outputs[0].Value.get<glm::vec3>();
    return std::format("const int {} = vec3({},{},{});",
        Graph.GetValue(IO.Outputs[0].Ptr)
    ,   val.x, val.y, val.z
    );
}
