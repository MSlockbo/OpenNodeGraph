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

#include <Graph/Nodes/Math/Common.h>

using namespace OpenShaderDesigner;
using namespace OpenShaderDesigner::Nodes::Math;

// Math Operation Interface --------------------------------------------------------------------------------------------

MathOp::MathOp(ShaderGraph &graph, ImVec2 pos)
	: Node(graph, pos)
	, Math {
		.Flags = false
	}
{

}

bool MathOp::CheckConnection(Pin* a, Pin* b)
{
	if(a->Type == b->Type)                               return true;
	if(Math.Flags & MathOpFlags_AllowMultipleInputTypes) return true;
	if(a->Type == PinType_Any || b->Type == PinType_Any) return true;

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
