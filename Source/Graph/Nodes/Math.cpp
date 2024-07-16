//
// Created by Maddie on 7/11/2024.
//

#include <Graph/Nodes/Math.h>

using namespace OpenShaderDesigner;

Add::Add(ShaderGraph& graph, ImVec2 pos)
	: Node(
		graph, pos
	,	"Add", ImColor(0x92, 0x16, 0x16)
	,	{ { "A",   Pin::ANY, Pin::INPUT }, { "B", Pin::ANY, Pin::INPUT } }, true
	,	{ { "Out", Pin::ANY, Pin::OUTPUT } }
	)
{ }

Node* Add::Copy(ShaderGraph& graph) const
{
	return new Add(graph, Position);
}
