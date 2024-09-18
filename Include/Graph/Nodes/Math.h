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

#ifndef MATH_H
#define MATH_H

#include <glm/vec4.hpp>
#include <Graph/ShaderGraph.h>
#include <open-cpp-utils/any.h>

namespace ocu = open_cpp_utils;

namespace OpenShaderDesigner::Nodes::Math
{
// Header Colors =======================================================================================================

inline static constexpr ImColor HeaderColor        = ImColor(0xA7, 0x62, 0x53);
inline static constexpr ImColor HeaderHoveredColor = ImColor(0xC5, 0x79, 0x67);
inline static constexpr ImColor HeaderActiveColor  = ImColor(0x82, 0x4C, 0x40);

inline static const std::string HeaderMarker     = "\uF3B9 ";



// =====================================================================================================================
// Constants
// =====================================================================================================================


// Integer -------------------------------------------------------------------------------------------------------------

struct Integer : public Node
{
    using ValueType = ocu::any<int, unsigned int, float, glm::vec4>;

    Integer(ShaderGraph& graph, ImVec2 pos);
    virtual ~Integer() = default;

    [[nodiscard]] Node* Copy(ShaderGraph& graph) const override;
    void Inspect() override;
};

RegisterNode("Math/Constants/Integer", Integer);


// Unsigned Integer ----------------------------------------------------------------------------------------------------

struct UnsignedInteger : public Node
{
    using ValueType = ocu::any<int, unsigned int, float, glm::vec4>;

    UnsignedInteger(ShaderGraph& graph, ImVec2 pos);
    virtual ~UnsignedInteger() = default;

    [[nodiscard]] Node* Copy(ShaderGraph& graph) const override;
    void Inspect() override;
};

RegisterNode("Math/Constants/Unsigned Integer", UnsignedInteger);


// Scalar --------------------------------------------------------------------------------------------------------------

struct Scalar : public Node
{
    using ValueType = ocu::any<int, unsigned int, float, glm::vec4>;

    Scalar(ShaderGraph& graph, ImVec2 pos);
    virtual ~Scalar() = default;

    [[nodiscard]] Node* Copy(ShaderGraph& graph) const override;
    void Inspect() override;
};

RegisterNode("Math/Constants/Scalar", Scalar);


// Vector --------------------------------------------------------------------------------------------------------------

struct Vector : public Node
{
    using ValueType = ocu::any<int, unsigned int, float, glm::vec4>;

    Vector(ShaderGraph& graph, ImVec2 pos);
    virtual ~Vector() = default;

    [[nodiscard]] Node* Copy(ShaderGraph& graph) const override;
    void Inspect() override;
};

RegisterNode("Math/Constants/Vector", Vector);



// =====================================================================================================================
// Operations
// =====================================================================================================================


// Math Op Prototype ---------------------------------------------------------------------------------------------------

struct MathOp : public Node
{
    MathOp(ShaderGraph& graph, ImVec2 pos);
    virtual ~MathOp() = default;

    virtual bool CheckConnection(Pin *, Pin *) override;
    virtual void ValidateConnections() override;

    bool AllowMultiWidthInputs;
};


// Add -----------------------------------------------------------------------------------------------------------------

struct Add : public MathOp
{
    Add(ShaderGraph& graph, ImVec2 pos);
    virtual ~Add() = default;

    [[nodiscard]] Node* Copy(ShaderGraph& graph) const override;
    void Inspect() override;
};

RegisterNode("Math/Operators/Add", Add);


// Subtract ------------------------------------------------------------------------------------------------------------

struct Subtract : public MathOp
{
    Subtract(ShaderGraph& graph, ImVec2 pos);
    virtual ~Subtract() = default;

    [[nodiscard]] Node* Copy(ShaderGraph& graph) const override;
    void Inspect() override;
};

RegisterNode("Math/Operators/Subtract", Subtract);


// Multiply ------------------------------------------------------------------------------------------------------------

struct Multiply : public MathOp
{
    Multiply(ShaderGraph& graph, ImVec2 pos);
    virtual ~Multiply() = default;

    [[nodiscard]] Node* Copy(ShaderGraph& graph) const override;
    void Inspect() override;
};

RegisterNode("Math/Operators/Multiply", Multiply);


// Divide --------------------------------------------------------------------------------------------------------------

struct Divide : public MathOp
{
    Divide(ShaderGraph& graph, ImVec2 pos);
    virtual ~Divide() = default;

    [[nodiscard]] Node* Copy(ShaderGraph& graph) const override;
    void Inspect() override;
};

RegisterNode("Math/Operators/Divide", Divide);



// =====================================================================================================================
// Utilities
// =====================================================================================================================


// Make Vector ---------------------------------------------------------------------------------------------------------

struct MakeVector : public Node
{
    MakeVector(ShaderGraph& graph, ImVec2 pos);
    virtual ~MakeVector() = default;

    [[nodiscard]] Node* Copy(ShaderGraph& graph) const override;
    void Inspect() override;
};

    RegisterNode("Math/Utilities/Make Vector", MakeVector);


// Break Vector ---------------------------------------------------------------------------------------------------------

struct BreakVector : public Node
{
    BreakVector(ShaderGraph& graph, ImVec2 pos);
    virtual ~BreakVector() = default;

    [[nodiscard]] Node* Copy(ShaderGraph& graph) const override;
    void Inspect() override;
};

RegisterNode("Math/Utilities/Break Vector", BreakVector);

}

#endif //MATH_H
