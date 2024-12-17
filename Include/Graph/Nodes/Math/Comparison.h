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

#ifndef COMPARISON_H
#define COMPARISON_H

#include <Graph/Nodes/Math/Common.h>

namespace OpenShaderDesigner::Nodes::Math
{
	
// =====================================================================================================================
// Comparison
// =====================================================================================================================


// Minimum -------------------------------------------------------------------------------------------------------------

struct Minimum : public MathOp
{
	Minimum(ShaderGraph& graph, ImVec2 pos);
	~Minimum() override = default;

	[[nodiscard]] Node* Copy(ShaderGraph& graph) const override;

	std::string GetCode() const override;
};


// Maximum -------------------------------------------------------------------------------------------------------------

struct Maximum : public MathOp
{
	Maximum(ShaderGraph& graph, ImVec2 pos);
	~Maximum() override = default;

	[[nodiscard]] Node* Copy(ShaderGraph& graph) const override;

	std::string GetCode() const override;
};


// Clamp -------------------------------------------------------------------------------------------------------------

struct Clamp : public MathOp
{
	Clamp(ShaderGraph& graph, ImVec2 pos);
	~Clamp() override = default;

	[[nodiscard]] Node* Copy(ShaderGraph& graph) const override;

	std::string GetCode() const override;
};


// Saturate -------------------------------------------------------------------------------------------------------------

struct Saturate : public MathOp
{
	Saturate(ShaderGraph& graph, ImVec2 pos);
	~Saturate() override = default;

	[[nodiscard]] Node* Copy(ShaderGraph& graph) const override;

	std::string GetCode() const override;
};

}

#endif //COMPARISON_H
