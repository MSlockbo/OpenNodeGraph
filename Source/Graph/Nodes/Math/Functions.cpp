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

#include <Graph/Nodes/Math/Functions.h>

#include <queue>

using namespace OpenShaderDesigner;
using namespace OpenShaderDesigner::Nodes::Math;

RegisterNode("Math/Functions/Add",      Add);
RegisterNode("Math/Functions/Subtract", Subtract);
RegisterNode("Math/Functions/Multiply", Multiply);
RegisterNode("Math/Functions/Divide",   Divide);

RegisterNode("Math/Functions/Abs",  AbsoluteValue);
RegisterNode("Math/Functions/Sqrt", SquareRoot);
RegisterNode("Math/Functions/Log",  Logarithm);
RegisterNode("Math/Functions/Log2", Logarithm2);
RegisterNode("Math/Functions/LogE", LogarithmE);
RegisterNode("Math/Functions/Pow",  Power);
RegisterNode("Math/Functions/Exp",  Exponential);


// =====================================================================================================================
// Math Operations
// =====================================================================================================================


// Add -----------------------------------------------------------------------------------------------------------------

Add::Add(ShaderGraph& graph, ImVec2 pos)
	: MathOp(graph, pos)
{
    Header.Title        = HeaderMarker + "Add";
    Header.Color        = HeaderColor;
    Header.HoveredColor = HeaderHoveredColor;
    Header.ActiveColor  = HeaderActiveColor;

    IO.Inputs.emplace_back("A", PinType_Any, PinFlags_Ambiguous);
    IO.Inputs.emplace_back("B", PinType_Any, PinFlags_Ambiguous);
    Info.Flags |= NodeFlags_DynamicInputs;

    IO.Outputs.emplace_back("Out", PinType_Any, PinFlags_Ambiguous);
}

Node* Add::Copy(ShaderGraph& graph) const
{
	return new Add(graph, Position);
}

std::string Add::GetCode() const
{
    // TODO: Support more than 2 inputs
    return std::format("const {} {} = {} + {};",
        Pin::TypeKeywords[IO.Outputs[0].Type]
    ,   Graph.GetValue(IO.Outputs[0].Ptr)
    ,   Graph.GetValue(IO.Inputs[0].Ptr)
    ,   Graph.GetValue(IO.Inputs[1].Ptr)
    );
}


// Subtract ------------------------------------------------------------------------------------------------------------

Subtract::Subtract(ShaderGraph& graph, ImVec2 pos)
    : MathOp(graph, pos)
{
    Header.Title        = HeaderMarker + "Subtract";
    Header.Color        = HeaderColor;
    Header.HoveredColor = HeaderHoveredColor;
    Header.ActiveColor  = HeaderActiveColor;

    IO.Inputs.emplace_back("A", PinType_Any, PinFlags_Ambiguous);
    IO.Inputs.emplace_back("B", PinType_Any, PinFlags_Ambiguous);
    Info.Flags |= NodeFlags_DynamicInputs;

    IO.Outputs.emplace_back("Out", PinType_Any, PinFlags_Ambiguous);
}

Node* Subtract::Copy(ShaderGraph& graph) const
{
    return new Subtract(graph, Position);
}

std::string Subtract::GetCode() const
{
    // TODO: Support more than 2 inputs
    return std::format("const {} {} = {} - {};",
        Pin::TypeKeywords[IO.Outputs[0].Type]
    ,   Graph.GetValue(IO.Outputs[0].Ptr)
    ,   Graph.GetValue(IO.Inputs[0].Ptr)
    ,   Graph.GetValue(IO.Inputs[1].Ptr)
    );
}


// Multiply ------------------------------------------------------------------------------------------------------------

Multiply::Multiply(ShaderGraph& graph, ImVec2 pos)
    : MathOp(graph, pos)
{
    Header.Title        = HeaderMarker + "Multiply";
    Header.Color        = HeaderColor;
    Header.HoveredColor = HeaderHoveredColor;
    Header.ActiveColor  = HeaderActiveColor;

    IO.Inputs.emplace_back("A", PinType_Any, PinFlags_Ambiguous);
    IO.Inputs.emplace_back("B", PinType_Any, PinFlags_Ambiguous);
    Info.Flags |= NodeFlags_DynamicInputs;

    IO.Outputs.emplace_back("Out", PinType_Any, PinFlags_Ambiguous);

    Math.Flags |= MathOpFlags_AllowMultipleInputTypes;
}

Node* Multiply::Copy(ShaderGraph& graph) const
{
    return new Multiply(graph, Position);
}

std::string Multiply::GetCode() const
{
    // TODO: Support more than 2 inputs
    return std::format("const {} {} = {} * {};",
        Pin::TypeKeywords[IO.Outputs[0].Type]
    ,   Graph.GetValue(IO.Outputs[0].Ptr)
    ,   Graph.GetValue(IO.Inputs[0].Ptr)
    ,   Graph.GetValue(IO.Inputs[1].Ptr)
    );
}


// Divide --------------------------------------------------------------------------------------------------------------

Divide::Divide(ShaderGraph& graph, ImVec2 pos)
    : MathOp(graph, pos)
{
    Header.Title        = HeaderMarker + "Divide";
    Header.Color        = HeaderColor;
    Header.HoveredColor = HeaderHoveredColor;
    Header.ActiveColor  = HeaderActiveColor;

    IO.Inputs.emplace_back("A", PinType_Any, PinFlags_Ambiguous);
    IO.Inputs.emplace_back("B", PinType_Any, PinFlags_Ambiguous);
    Info.Flags |= NodeFlags_DynamicInputs;

    IO.Outputs.emplace_back("Out", PinType_Any, PinFlags_Ambiguous);

    Math.Flags |= MathOpFlags_AllowMultipleInputTypes;
}

Node* Divide::Copy(ShaderGraph& graph) const
{
    return new Divide(graph, Position);
}

std::string Divide::GetCode() const
{
    // TODO: Support more than 2 inputs
    return std::format("const {} {} = {} / {};"
    ,   Pin::TypeKeywords[IO.Outputs[0].Type]
    ,   Graph.GetValue(IO.Outputs[0].Ptr)
    ,   Graph.GetValue(IO.Inputs[0].Ptr)
    ,   Graph.GetValue(IO.Inputs[1].Ptr)
    );
}


// Abs --------------------------------------------------------------------------------------------------------------

AbsoluteValue::AbsoluteValue(ShaderGraph& graph, ImVec2 pos)
	: MathOp(graph, pos)
{
	Header.Title        = HeaderMarker + "Abs";
	Header.Color        = HeaderColor;
	Header.HoveredColor = HeaderHoveredColor;
	Header.ActiveColor  = HeaderActiveColor;

	IO.Inputs.emplace_back("In",   PinType_Any, PinFlags_Ambiguous);
	IO.Outputs.emplace_back("Out", PinType_Any, PinFlags_Ambiguous);
}

Node* AbsoluteValue::Copy(ShaderGraph& graph) const
{
	return new Divide(graph, Position);
}

std::string AbsoluteValue::GetCode() const
{
	// TODO: Support more than 2 inputs
	return std::format("const {} {} = abs({});"
	,   Pin::TypeKeywords[IO.Outputs[0].Type]
	,   Graph.GetValue(IO.Outputs[0].Ptr)
	,   Graph.GetValue(IO.Inputs[0].Ptr)
	);
}


// Power --------------------------------------------------------------------------------------------------------------

Power::Power(ShaderGraph& graph, ImVec2 pos)
	: MathOp(graph, pos)
{
	Header.Title        = HeaderMarker + "Pow";
	Header.Color        = HeaderColor;
	Header.HoveredColor = HeaderHoveredColor;
	Header.ActiveColor  = HeaderActiveColor;

	IO.Inputs.emplace_back("Base", PinType_Any, PinFlags_Ambiguous);
	IO.Inputs.emplace_back("Exp",  PinType_Any, PinFlags_Ambiguous);
	IO.Outputs.emplace_back("Out", PinType_Any, PinFlags_Ambiguous);
}

Node* Power::Copy(ShaderGraph& graph) const
{
	return new Divide(graph, Position);
}

std::string Power::GetCode() const
{
	// TODO: Support more than 2 inputs
	return std::format("const {} {} = pow({}, {});"
	,   Pin::TypeKeywords[IO.Outputs[0].Type]
	,   Graph.GetValue(IO.Outputs[0].Ptr)
	,   Graph.GetValue(IO.Inputs[0].Ptr)
	,   Graph.GetValue(IO.Inputs[1].Ptr)
	);
}


// Sqrt --------------------------------------------------------------------------------------------------------------

SquareRoot::SquareRoot(ShaderGraph& graph, ImVec2 pos)
	: MathOp(graph, pos)
{
	Info.Alias          = "SquareRoot";
	Header.Title        = HeaderMarker + "Sqrt";
	Header.Color        = HeaderColor;
	Header.HoveredColor = HeaderHoveredColor;
	Header.ActiveColor  = HeaderActiveColor;

	IO.Inputs.emplace_back("In",   PinType_Any, PinFlags_Ambiguous);
	IO.Outputs.emplace_back("Out", PinType_Any, PinFlags_Ambiguous);
}

Node* SquareRoot::Copy(ShaderGraph& graph) const
{
	return new Divide(graph, Position);
}

std::string SquareRoot::GetCode() const
{
	// TODO: Support more than 2 inputs
	return std::format("const {} {} = sqrt({});"
	,   Pin::TypeKeywords[IO.Outputs[0].Type]
	,   Graph.GetValue(IO.Outputs[0].Ptr)
	,   Graph.GetValue(IO.Inputs[0].Ptr)
	);
}


// InverseSquareRoot --------------------------------------------------------------------------------------------------------------

InverseSquareRoot::InverseSquareRoot(ShaderGraph& graph, ImVec2 pos)
	: MathOp(graph, pos)
{
	Header.Title        = HeaderMarker + "RSqrt";
	Header.Color        = HeaderColor;
	Header.HoveredColor = HeaderHoveredColor;
	Header.ActiveColor  = HeaderActiveColor;

	IO.Inputs.emplace_back("In",   PinType_Any, PinFlags_Ambiguous);
	IO.Outputs.emplace_back("Out", PinType_Any, PinFlags_Ambiguous);
}

Node* InverseSquareRoot::Copy(ShaderGraph& graph) const
{
	return new Divide(graph, Position);
}

std::string InverseSquareRoot::GetCode() const
{
	// TODO: Support more than 2 inputs
	return std::format("const {} {} = inversesqrt({});"
	,   Pin::TypeKeywords[IO.Outputs[0].Type]
	,   Graph.GetValue(IO.Outputs[0].Ptr)
	,   Graph.GetValue(IO.Inputs[0].Ptr)
	);
}


// Logarithm --------------------------------------------------------------------------------------------------------------

Logarithm::Logarithm(ShaderGraph& graph, ImVec2 pos)
	: MathOp(graph, pos)
{
	Header.Title        = HeaderMarker + "Log";
	Header.Color        = HeaderColor;
	Header.HoveredColor = HeaderHoveredColor;
	Header.ActiveColor  = HeaderActiveColor;

	IO.Inputs.emplace_back("X",    PinType_Any, PinFlags_Ambiguous);
	IO.Inputs.emplace_back("Base", PinType_Any, PinFlags_Ambiguous);
	IO.Outputs.emplace_back("Out", PinType_Any, PinFlags_Ambiguous);
}

Node* Logarithm::Copy(ShaderGraph& graph) const
{
	return new Divide(graph, Position);
}

std::string Logarithm::GetCode() const
{
	// TODO: Support more than 2 inputs
	return std::format("const {} {} = log({}) / log({});"
	,   Pin::TypeKeywords[IO.Outputs[0].Type]
	,   Graph.GetValue(IO.Outputs[0].Ptr)
	,   Graph.GetValue(IO.Inputs[0].Ptr)
	,   Graph.GetValue(IO.Inputs[1].Ptr)
	);
}


// Logarithm --------------------------------------------------------------------------------------------------------------

Logarithm2::Logarithm2(ShaderGraph& graph, ImVec2 pos)
	: MathOp(graph, pos)
{
	Header.Title        = HeaderMarker + "Log2";
	Header.Color        = HeaderColor;
	Header.HoveredColor = HeaderHoveredColor;
	Header.ActiveColor  = HeaderActiveColor;

	IO.Inputs.emplace_back("X",    PinType_Any, PinFlags_Ambiguous);
	IO.Outputs.emplace_back("Out", PinType_Any, PinFlags_Ambiguous);
}

Node* Logarithm2::Copy(ShaderGraph& graph) const
{
	return new Divide(graph, Position);
}

std::string Logarithm2::GetCode() const
{
	// TODO: Support more than 2 inputs
	return std::format("const {} {} = log2({});"
	,   Pin::TypeKeywords[IO.Outputs[0].Type]
	,   Graph.GetValue(IO.Outputs[0].Ptr)
	,   Graph.GetValue(IO.Inputs[0].Ptr)
	);
}


// Logarithm --------------------------------------------------------------------------------------------------------------

LogarithmE::LogarithmE(ShaderGraph& graph, ImVec2 pos)
	: MathOp(graph, pos)
{
	Header.Title        = HeaderMarker + "LogE";
	Header.Color        = HeaderColor;
	Header.HoveredColor = HeaderHoveredColor;
	Header.ActiveColor  = HeaderActiveColor;

	IO.Inputs.emplace_back("X",    PinType_Any, PinFlags_Ambiguous);
	IO.Outputs.emplace_back("Out", PinType_Any, PinFlags_Ambiguous);
}

Node* LogarithmE::Copy(ShaderGraph& graph) const
{
	return new Divide(graph, Position);
}

std::string LogarithmE::GetCode() const
{
	// TODO: Support more than 2 inputs
	return std::format("const {} {} = log({});"
	,   Pin::TypeKeywords[IO.Outputs[0].Type]
	,   Graph.GetValue(IO.Outputs[0].Ptr)
	,   Graph.GetValue(IO.Inputs[0].Ptr)
	);
}


// Exp --------------------------------------------------------------------------------------------------------------

Exponential::Exponential(ShaderGraph& graph, ImVec2 pos)
	: MathOp(graph, pos)
{
	Header.Title        = HeaderMarker + "Exp";
	Header.Color        = HeaderColor;
	Header.HoveredColor = HeaderHoveredColor;
	Header.ActiveColor  = HeaderActiveColor;

	IO.Inputs.emplace_back("In",   PinType_Any, PinFlags_Ambiguous);
	IO.Outputs.emplace_back("Out", PinType_Any, PinFlags_Ambiguous);
}

Node* Exponential::Copy(ShaderGraph& graph) const
{
	return new Exponential(graph, Position);
}

std::string Exponential::GetCode() const
{
	// TODO: Support more than 2 inputs
	return std::format("const {} {} = exp({});"
	,   Pin::TypeKeywords[IO.Outputs[0].Type]
	,   Graph.GetValue(IO.Outputs[0].Ptr)
	,   Graph.GetValue(IO.Inputs[0].Ptr)
	);
}