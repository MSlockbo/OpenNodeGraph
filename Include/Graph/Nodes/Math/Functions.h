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

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <Graph/Nodes/Math/Common.h>

namespace ocu = open_cpp_utils;

namespace OpenShaderDesigner::Nodes::Math
{
	
// =====================================================================================================================
// Operations
// =====================================================================================================================


// Add -----------------------------------------------------------------------------------------------------------------

struct Add : public MathOp
{
    Add(ShaderGraph& graph, ImVec2 pos);
    ~Add() override = default;

    [[nodiscard]] Node* Copy(ShaderGraph& graph) const override;

    std::string GetCode() const override;
};


// Subtract ------------------------------------------------------------------------------------------------------------

struct Subtract : public MathOp
{
    Subtract(ShaderGraph& graph, ImVec2 pos);
    ~Subtract() override = default;

    [[nodiscard]] Node* Copy(ShaderGraph& graph) const override;

    std::string GetCode() const override;
};

// Multiply ------------------------------------------------------------------------------------------------------------

struct Multiply : public MathOp
{
    Multiply(ShaderGraph& graph, ImVec2 pos);
    ~Multiply() override = default;

    [[nodiscard]] Node* Copy(ShaderGraph& graph) const override;

    std::string GetCode() const override;
};


// Divide --------------------------------------------------------------------------------------------------------------

struct Divide : public MathOp
{
    Divide(ShaderGraph& graph, ImVec2 pos);
    ~Divide() override = default;

    [[nodiscard]] Node* Copy(ShaderGraph& graph) const override;

    std::string GetCode() const override;
};


// Abs --------------------------------------------------------------------------------------------------------------

struct AbsoluteValue : public MathOp
{
	AbsoluteValue(ShaderGraph& graph, ImVec2 pos);
	~AbsoluteValue() override = default;

	[[nodiscard]] Node* Copy(ShaderGraph& graph) const override;

	std::string GetCode() const override;
};

	
// Power --------------------------------------------------------------------------------------------------------------

struct Power : public MathOp
{
	Power(ShaderGraph& graph, ImVec2 pos);
	~Power() override = default;

	[[nodiscard]] Node* Copy(ShaderGraph& graph) const override;

	std::string GetCode() const override;
};


// SquareRoot --------------------------------------------------------------------------------------------------------------

struct SquareRoot : public MathOp
{
	SquareRoot(ShaderGraph& graph, ImVec2 pos);
	~SquareRoot() override = default;

	[[nodiscard]] Node* Copy(ShaderGraph& graph) const override;

	std::string GetCode() const override;
};


// InverseSquareRoot --------------------------------------------------------------------------------------------------------------

struct InverseSquareRoot : public MathOp
{
	InverseSquareRoot(ShaderGraph& graph, ImVec2 pos);
	~InverseSquareRoot() override = default;

	[[nodiscard]] Node* Copy(ShaderGraph& graph) const override;

	std::string GetCode() const override;
};


// Logarithm2 --------------------------------------------------------------------------------------------------------------

struct Logarithm2 : public MathOp
{
	Logarithm2(ShaderGraph& graph, ImVec2 pos);
	~Logarithm2() override = default;

	[[nodiscard]] Node* Copy(ShaderGraph& graph) const override;

	std::string GetCode() const override;
};


// LogarithmE --------------------------------------------------------------------------------------------------------------

struct LogarithmE : public MathOp
{
	LogarithmE(ShaderGraph& graph, ImVec2 pos);
	~LogarithmE() override = default;

	[[nodiscard]] Node* Copy(ShaderGraph& graph) const override;

	std::string GetCode() const override;
};


// Logarithm --------------------------------------------------------------------------------------------------------------

struct Logarithm : public MathOp
{
	Logarithm(ShaderGraph& graph, ImVec2 pos);
	~Logarithm() override = default;

	[[nodiscard]] Node* Copy(ShaderGraph& graph) const override;

	std::string GetCode() const override;
};


// Exponential --------------------------------------------------------------------------------------------------------------

struct Exponential : public MathOp
{
	Exponential(ShaderGraph& graph, ImVec2 pos);
	~Exponential() override = default;

	[[nodiscard]] Node* Copy(ShaderGraph& graph) const override;

	std::string GetCode() const override;
};


}

#endif // FUNCTIONS_H
