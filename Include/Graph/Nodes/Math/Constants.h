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

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <Graph/Nodes/Math/Common.h>

namespace OpenShaderDesigner::Nodes::Math
{
	
// =====================================================================================================================
// Constants
// =====================================================================================================================


// Integer -------------------------------------------------------------------------------------------------------------

struct Integer : public Node
{
	Integer(ShaderGraph& graph, ImVec2 pos);
	~Integer() override = default;

	[[nodiscard]] Node* Copy(ShaderGraph& graph) const override;
	void Inspect() override;

	std::string GetCode() const override;
};


// Unsigned Integer ----------------------------------------------------------------------------------------------------

struct UnsignedInteger : public Node
{
	using ValueType = ocu::any<int, unsigned int, float, glm::vec4>;

	UnsignedInteger(ShaderGraph& graph, ImVec2 pos);
	~UnsignedInteger() override = default;

	[[nodiscard]] Node* Copy(ShaderGraph& graph) const override;
	void Inspect() override;

	std::string GetCode() const override;
};


// Scalar --------------------------------------------------------------------------------------------------------------

struct Scalar : public Node
{
	using ValueType = ocu::any<int, unsigned int, float, glm::vec4>;

	Scalar(ShaderGraph& graph, ImVec2 pos);
	~Scalar() override = default;

	[[nodiscard]] Node* Copy(ShaderGraph& graph) const override;
	void Inspect() override;

	std::string GetCode() const override;
};


// Pi --------------------------------------------------------------------------------------------------------------

struct Pi : public Scalar
{
	using ValueType = ocu::any<int, unsigned int, float, glm::vec4>;

	Pi(ShaderGraph& graph, ImVec2 pos);
	~Pi() override = default;

	[[nodiscard]] Node* Copy(ShaderGraph& graph) const override;
	void Inspect() override;

	std::string GetCode() const override;
};


// EulersNumber --------------------------------------------------------------------------------------------------------------

struct EulersNumber : public Scalar
{
	using ValueType = ocu::any<int, unsigned int, float, glm::vec4>;

	EulersNumber(ShaderGraph& graph, ImVec2 pos);
	~EulersNumber() override = default;

	[[nodiscard]] Node* Copy(ShaderGraph& graph) const override;
	void Inspect() override;

	std::string GetCode() const override;
};


// EulersNumber --------------------------------------------------------------------------------------------------------------

struct GoldenRatio : public Scalar
{
	using ValueType = ocu::any<int, unsigned int, float, glm::vec4>;

	GoldenRatio(ShaderGraph& graph, ImVec2 pos);
	~GoldenRatio() override = default;

	[[nodiscard]] Node* Copy(ShaderGraph& graph) const override;
	void Inspect() override;

	std::string GetCode() const override;
};


// Vector --------------------------------------------------------------------------------------------------------------

struct Vector : public Node
{
	using ValueType = ocu::any<int, unsigned int, float, glm::vec4>;

	Vector(ShaderGraph& graph, ImVec2 pos);
	~Vector() override = default;

	[[nodiscard]] Node* Copy(ShaderGraph& graph) const override;
	void Inspect() override;

	std::string GetCode() const override;
};

}

#endif //CONSTANTS_H
