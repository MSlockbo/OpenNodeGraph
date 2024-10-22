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

#ifndef OSD_SHADERS_H
#define OSD_SHADERS_H

#include <glm/vec4.hpp>
#include <Graph/ShaderGraph.h>

namespace OpenShaderDesigner::Nodes::Shaders
{
    
// Header Colors =======================================================================================================

inline static constexpr ImColor HeaderColor        = ImColor(0xA9, 0x85, 0xC1);
inline static constexpr ImColor HeaderHoveredColor = ImColor(0xBB, 0x96, 0xD4);
inline static constexpr ImColor HeaderActiveColor  = ImColor(0x8D, 0x68, 0xA6);

inline static const std::string HeaderMarker = "\uF42E ";

// =====================================================================================================================
// Shaders
// =====================================================================================================================


// Function ------------------------------------------------------------------------------------------------------------

class Function : public Node, public ShaderAsset
{
public:
    Function(const FileManager::Path& path, ShaderGraph& graph);
    ~Function() override = default;

    [[nodiscard]] Node* Copy(ShaderGraph& graph) const override;
    void Inspect() override;

    void Compile() override;
    void Open() override;

    static Asset* Create(const FileManager::Path& path);
    static Asset* Load(const FileManager::Path& path);
    static Asset* Import(const FileManager::Path& src, const FileManager::Path& dst);

    std::string GetCode() const override;

private:
    NodeId ID_;
};

    
}
    
#endif // OSD_SHADERS_H
