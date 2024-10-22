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

#include <Graph/Nodes/Maths.h>

#include <queue>

using namespace OpenShaderDesigner;
using namespace OpenShaderDesigner::Nodes::Math;

RegisterNode("Math/Constants/Integer", Integer);
RegisterNode("Math/Constants/Unsigned Integer", UnsignedInteger);
RegisterNode("Math/Constants/Scalar", Scalar);
RegisterNode("Math/Constants/Vector", Vector);
RegisterNode("Math/Operators/Add", Add);
RegisterNode("Math/Operators/Subtract", Subtract);
RegisterNode("Math/Operators/Multiply", Multiply);
RegisterNode("Math/Operators/Divide", Divide);
RegisterNode("Math/Utilities/Make Vector", MakeVector);
RegisterNode("Math/Utilities/Break Vector", BreakVector);

// =====================================================================================================================
// Constants
// =====================================================================================================================


// Integer -------------------------------------------------------------------------------------------------------------

Integer::Integer(ShaderGraph& graph, ImVec2 pos)
    : Node(graph, pos)
{
    Info.Alias          = "Integer";
    Header.Title        = HeaderMarker + "Integer";
    Header.Color        = HeaderColor;
    Header.HoveredColor = HeaderHoveredColor;
    Header.ActiveColor  = HeaderActiveColor;

    IO.Outputs.emplace_back("Out", PinType_Int, PinFlags_NoCollapse | PinFlags_NoPadding);
}

Node* Integer::Copy(ShaderGraph& graph) const
{
    return new Integer(graph, Position);
}

void Integer::Inspect()
{

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
    Info.Alias          = "UnsignedInteger";
    Header.Title        = HeaderMarker + "Unsigned Integer";
    Header.Color        = HeaderColor;
    Header.HoveredColor = HeaderHoveredColor;
    Header.ActiveColor  = HeaderActiveColor;

    IO.Outputs.emplace_back("Out", PinType_UInt, PinFlags_NoCollapse | PinFlags_NoPadding);
}

Node* UnsignedInteger::Copy(ShaderGraph& graph) const
{
    return new UnsignedInteger(graph, Position);
}

void UnsignedInteger::Inspect()
{

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
    Info.Alias          = "Scalar";
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

std::string Scalar::GetCode() const
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
    Info.Alias          = "Vector";
    Header.Title        = HeaderMarker + "Vector";
    Header.Color        = HeaderColor;
    Header.HoveredColor = HeaderHoveredColor;
    Header.ActiveColor  = HeaderActiveColor;

    IO.Outputs.emplace_back("Out", PinType_Vector, PinFlags_NoCollapse | PinFlags_NoPadding);

	IO.Outputs[0].Value.get<glm::vec3>() = glm::vec3(0);
}

Node* Vector::Copy(ShaderGraph &graph) const
{
	return new Vector(graph, Position);
}

void Vector::Inspect()
{

}

std::string Vector::GetCode() const
{
    const glm::vec3& val = IO.Outputs[0].Value.get<glm::vec3>();
    return std::format("const int {} = vec3({},{},{});",
        Graph.GetValue(IO.Outputs[0].Ptr)
    ,   val.x, val.y, val.z
    );
}


// =====================================================================================================================
// Math Operations
// =====================================================================================================================


// Math Operation Interface --------------------------------------------------------------------------------------------

MathOp::MathOp(ShaderGraph &graph, ImVec2 pos)
    : Node(graph, pos)
    , AllowMultiWidthInputs(false)
{

}

bool MathOp::CheckConnection(Pin* a, Pin* b)
{
    if(AllowMultiWidthInputs) return false;

    for(Pin& pin : IO.Inputs)
    {
        if(&pin == a)                                     continue;
        if((pin.Flags & PinFlags_Ambiguous) == false)     continue;
        if(ImNodeGraph::IsPinConnected(pin.Ptr) == false) continue;
        if(pin.Type == PinType_Any)                       continue;

        if(Pin::TypeWidths[pin.Type] != Pin::TypeWidths[b->Type]) return true;
    }

    return false;
}

void MathOp::ValidateConnections()
{
    std::set<PinType> present;

    for(Pin& pin : IO.Inputs)
    {
        if((pin.Flags & PinFlags_Ambiguous) == false)     continue;
        if(ImNodeGraph::IsPinConnected(pin.Ptr) == false) continue;
        if(pin.Type == PinType_Any)                       continue;

        present.insert(pin.Type);
    }

    PinType type = present.empty() ? PinType_Any : *present.rbegin();

    for(Pin& pin : IO.Inputs)
    {
        if((pin.Flags & PinFlags_Ambiguous) == false) continue;
        if(ImNodeGraph::IsPinConnected(pin.Ptr))      continue;

        pin.Type = type;
    }

    for(Pin& pin : IO.Outputs)
    {
        if((pin.Flags & PinFlags_Ambiguous) == false) continue;
        if(ImNodeGraph::IsPinConnected(pin.Ptr))      continue;

        pin.Type = type;
    }
}


// Add -----------------------------------------------------------------------------------------------------------------

Add::Add(ShaderGraph& graph, ImVec2 pos)
	: MathOp(graph, pos)
{
    Info.Alias          = "Add";
    Header.Title        = HeaderMarker + "Add";
    Header.Color        = HeaderColor;
    Header.HoveredColor = HeaderHoveredColor;
    Header.ActiveColor  = HeaderActiveColor;

    IO.Inputs.emplace_back("A", PinType_Any, PinFlags_Ambiguous);
    IO.Inputs.emplace_back("B", PinType_Any, PinFlags_Ambiguous);
    IO.DynamicInputs = true;

    IO.Outputs.emplace_back("Out", PinType_Any, PinFlags_Ambiguous);
}

Node* Add::Copy(ShaderGraph& graph) const
{
	return new Add(graph, Position);
}

void Add::Inspect()
{

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
    Info.Alias          = "Subtract";
    Header.Title        = HeaderMarker + "Subtract";
    Header.Color        = HeaderColor;
    Header.HoveredColor = HeaderHoveredColor;
    Header.ActiveColor  = HeaderActiveColor;

    IO.Inputs.emplace_back("A", PinType_Any, PinFlags_Ambiguous);
    IO.Inputs.emplace_back("B", PinType_Any, PinFlags_Ambiguous);
    IO.DynamicInputs = true;

    IO.Outputs.emplace_back("Out", PinType_Any, PinFlags_Ambiguous);
}

Node* Subtract::Copy(ShaderGraph& graph) const
{
    return new Subtract(graph, Position);
}

void Subtract::Inspect()
{

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
    Info.Alias          = "Multiply";
    Header.Title        = HeaderMarker + "Multiply";
    Header.Color        = HeaderColor;
    Header.HoveredColor = HeaderHoveredColor;
    Header.ActiveColor  = HeaderActiveColor;

    IO.Inputs.emplace_back("A", PinType_Any, PinFlags_Ambiguous);
    IO.Inputs.emplace_back("B", PinType_Any, PinFlags_Ambiguous);
    IO.DynamicInputs = true;

    IO.Outputs.emplace_back("Out", PinType_Any, PinFlags_Ambiguous);

    AllowMultiWidthInputs = true;
}

Node* Multiply::Copy(ShaderGraph& graph) const
{
    return new Multiply(graph, Position);
}

void Multiply::Inspect()
{

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
    Info.Alias          = "Divide";
    Header.Title        = HeaderMarker + "Divide";
    Header.Color        = HeaderColor;
    Header.HoveredColor = HeaderHoveredColor;
    Header.ActiveColor  = HeaderActiveColor;

    IO.Inputs.emplace_back("A", PinType_Any, PinFlags_Ambiguous);
    IO.Inputs.emplace_back("B", PinType_Any, PinFlags_Ambiguous);
    IO.DynamicInputs = true;

    IO.Outputs.emplace_back("Out", PinType_Any, PinFlags_Ambiguous);

    AllowMultiWidthInputs = true;
}

Node* Divide::Copy(ShaderGraph& graph) const
{
    return new Divide(graph, Position);
}

void Divide::Inspect()
{

}

std::string Divide::GetCode() const
{
    // TODO: Support more than 2 inputs
    return std::format("const {} {} = {} / {};",
        Pin::TypeKeywords[IO.Outputs[0].Type]
    ,   Graph.GetValue(IO.Outputs[0].Ptr)
    ,   Graph.GetValue(IO.Inputs[0].Ptr)
    ,   Graph.GetValue(IO.Inputs[1].Ptr)
    );
}



// =====================================================================================================================
// Utilities
// =====================================================================================================================


// Make Vector ---------------------------------------------------------------------------------------------------------

MakeVector::MakeVector(ShaderGraph& graph, ImVec2 pos)
    : Node(graph, pos)
{
    Info.Alias          = "MakeVector";
    Header.Title        = HeaderMarker + "Make Vector";
    Header.Color        = HeaderColor;
    Header.HoveredColor = HeaderHoveredColor;
    Header.ActiveColor  = HeaderActiveColor;

    IO.Inputs.emplace_back("X", PinType_Float, PinFlags_AlwaysCollapse);
    IO.Inputs.emplace_back("Y", PinType_Float, PinFlags_AlwaysCollapse);
    IO.Inputs.emplace_back("Z", PinType_Float, PinFlags_AlwaysCollapse);

    IO.Outputs.emplace_back("Out", PinType_Vector);
}

Node* MakeVector::Copy(ShaderGraph& graph) const
{
    return new MakeVector(graph, Position);
}

void MakeVector::Inspect()
{

}

std::string MakeVector::GetCode() const
{
    // TODO: Support more than 2 inputs
    return std::format("const vec3 {} = vec3({}, {}, {});",
        Graph.GetValue(IO.Outputs[0].Ptr)
    ,   Graph.GetValue(IO.Inputs[0].Ptr)
    ,   Graph.GetValue(IO.Inputs[1].Ptr)
    ,   Graph.GetValue(IO.Inputs[2].Ptr)
    );
}


// Break Vector ---------------------------------------------------------------------------------------------------------

BreakVector::BreakVector(ShaderGraph& graph, ImVec2 pos)
    : Node(graph, pos)
{
    Info.Alias          = "BreakVector";
    Header.Title        = HeaderMarker + "Break Vector";
    Header.Color        = HeaderColor;
    Header.HoveredColor = HeaderHoveredColor;
    Header.ActiveColor  = HeaderActiveColor;

    IO.Inputs.emplace_back("In", PinType_Vector, PinFlags_AlwaysCollapse);

    IO.Outputs.emplace_back("X", PinType_Float);
    IO.Outputs.emplace_back("Y", PinType_Float);
    IO.Outputs.emplace_back("Z", PinType_Float);
}

Node* BreakVector::Copy(ShaderGraph& graph) const
{
    return new MakeVector(graph, Position);
}

void BreakVector::Inspect()
{

}

std::string BreakVector::GetCode() const
{
    // TODO: Support more than 2 inputs
    return std::format(
        "const float {} = {}.x;\n"
        "const float {} = {}.y;\n"
        "const float {} = {}.z;"
    ,   Graph.GetValue(IO.Outputs[0].Ptr)
    ,   Graph.GetValue(IO.Inputs[0].Ptr)
    
    ,   Graph.GetValue(IO.Outputs[1].Ptr)
    ,   Graph.GetValue(IO.Inputs[0].Ptr)
    
    ,   Graph.GetValue(IO.Outputs[2].Ptr)
    ,   Graph.GetValue(IO.Inputs[0].Ptr)
    );
}
