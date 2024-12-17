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

#ifndef VECTORMATHS_H
#define VECTORMATHS_H

#include <Graph/Nodes/Math/Common.h>

namespace OpenShaderDesigner::Nodes::VectorMath
{
    
// Header Colors =======================================================================================================

inline static constexpr ImColor HeaderColor        = ImColor(0xA7, 0x62, 0x53);
inline static constexpr ImColor HeaderHoveredColor = ImColor(0xC5, 0x79, 0x67);
inline static constexpr ImColor HeaderActiveColor  = ImColor(0x82, 0x4C, 0x40);

inline static const std::string HeaderMarker       = "\uF3B9 ";
    
// =====================================================================================================================
// Vector Utilities
// =====================================================================================================================


// Make Vector ---------------------------------------------------------------------------------------------------------

struct MakeVector : public Node
{
    MakeVector(ShaderGraph& graph, ImVec2 pos);
    ~MakeVector() override = default;

    [[nodiscard]] Node* Copy(ShaderGraph& graph) const override;

    std::string GetCode() const override;
};


// Break Vector ---------------------------------------------------------------------------------------------------------

struct BreakVector : public Node
{
    BreakVector(ShaderGraph& graph, ImVec2 pos);
    ~BreakVector() override = default;

    [[nodiscard]] Node* Copy(ShaderGraph& graph) const override;

    std::string GetCode() const override;
};
    
}

#endif //VECTORMATHS_H
