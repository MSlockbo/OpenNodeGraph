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

#include <Graph/Nodes/Math/Vector.h>

using namespace OpenShaderDesigner;
using namespace OpenShaderDesigner::Nodes::VectorMath;

RegisterNode("Math/Vector/Make Vector",  MakeVector);
RegisterNode("Math/Vector/Break Vector", BreakVector);

// =====================================================================================================================
// Vector Utilities
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
