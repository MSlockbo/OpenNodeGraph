//
// Created by Maddie on 7/11/2024.
//

#ifndef MATH_H
#define MATH_H

#include <Graph/ShaderGraph.h>

namespace OpenShaderDesigner
{

	struct Add : public Node
	{
		Add(ShaderGraph& graph, ImVec2 pos);
		[[nodiscard]] Node* Copy(ShaderGraph& graph) const override;
	};

	RegisterNode("Math/Add", Add);
}

#endif //MATH_H
