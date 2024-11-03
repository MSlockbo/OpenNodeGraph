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

#ifndef TRIGONOMETRY_H
#define TRIGONOMETRY_H

#include <Graph/Nodes/Math/Common.h>

namespace OpenShaderDesigner::Nodes::Math
{

// =====================================================================================================================
// Trigonometry
// =====================================================================================================================


// Sine ----------------------------------------------------------------------------------------------------------------
    
struct Sine : public MathOp
{
    Sine(ShaderGraph& graph, ImVec2 pos);
    ~Sine() override = default;

    [[nodiscard]] Node* Copy(ShaderGraph& graph) const override;
    void Inspect() override;

    std::string GetCode() const override;
};

	
// Cosine ----------------------------------------------------------------------------------------------------------------
    
struct Cosine : public MathOp
{
	Cosine(ShaderGraph& graph, ImVec2 pos);
	~Cosine() override = default;

	[[nodiscard]] Node* Copy(ShaderGraph& graph) const override;
	void Inspect() override;

	std::string GetCode() const override;
};

	
// Tangent ----------------------------------------------------------------------------------------------------------------

struct Tangent : public MathOp
{
	Tangent(ShaderGraph& graph, ImVec2 pos);
	~Tangent() override = default;

	[[nodiscard]] Node* Copy(ShaderGraph& graph) const override;
	void Inspect() override;

	std::string GetCode() const override;
};


// ArcSine ----------------------------------------------------------------------------------------------------------------

struct ArcSine : public MathOp
{
	ArcSine(ShaderGraph& graph, ImVec2 pos);
	~ArcSine() override = default;

	[[nodiscard]] Node* Copy(ShaderGraph& graph) const override;
	void Inspect() override;

	std::string GetCode() const override;
};


// ArcCosine ----------------------------------------------------------------------------------------------------------------

struct ArcCosine : public MathOp
{
	ArcCosine(ShaderGraph& graph, ImVec2 pos);
	~ArcCosine() override = default;

	[[nodiscard]] Node* Copy(ShaderGraph& graph) const override;
	void Inspect() override;

	std::string GetCode() const override;
};


// ArcTangent ----------------------------------------------------------------------------------------------------------------

struct ArcTangent : public MathOp
{
	ArcTangent(ShaderGraph& graph, ImVec2 pos);
	~ArcTangent() override = default;

	[[nodiscard]] Node* Copy(ShaderGraph& graph) const override;
	void Inspect() override;

	std::string GetCode() const override;
};

}

#endif //TRIGONOMETRY_H
