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

#ifndef OSD_SHADERS_H
#define OSD_SHADERS_H

#include <Graph/ShaderGraph.h>

#include <rapidjson/document.h>

#include <glm/vec4.hpp>

#include <glw/shader.h>

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


// Function Inputs -----------------------------------------------------------------------------------------------------

class FunctionInputs : public Node
{
public:
    FunctionInputs(ShaderGraph& graph, ImVec2 pos);
    ~FunctionInputs() override = default;

    [[nodiscard]] Node* Copy(ShaderGraph& graph) const override;
    bool Inspect() override;

    std::string GetCode() const override;
};


// Function ------------------------------------------------------------------------------------------------------------

/**
 * \brief Functions are a Shader function that takes inputs and outputs.
 *
 * Functions are not analagous to Material Functions in UnrealEngine. Functions work strictly on their inputs
 * and outputs without a specified domain.
 */
class Function : public Node, public ShaderAsset
{
public:
	enum FunctionVersion_ : uint32_t
	{
		FunctionVersion_0 = 0

	,	FunctionVersion_Count
	,	FunctionVersion_Latest = FunctionVersion_Count - 1
	};

	template<uint32_t V> using Version = std::integral_constant<uint32_t, V>;
	
	Function(ShaderGraph& graph, ImVec2 pos);
    Function(const FileManager::Path& path, ShaderGraph& graph);
    Function(const FileManager::Path& src, const FileManager::Path& dst, ShaderGraph& graph);
    ~Function() override;

    [[nodiscard]] Node* Copy(ShaderGraph& graph) const override;
    bool Inspect() override;

    void Compile() override;
    void Open() override;

    static Asset* Create(const FileManager::Path& path);
    static Asset* Load(const FileManager::Path& path);
    static Asset* Import(const FileManager::Path& src, const FileManager::Path& dst);

    std::string GetCode() const override;
    void View(HDRTexture::HandleType* Target) override;

	void Write(const FileManager::Path &path) override;
	void Read(const FileManager::Path &path) override;

	Node* GetSingletonNode(const std::string& alias) override;

private:
    using InputMap = ocu::map<int, glw::enum_t>;
	using WriteAlloc = rapidjson::MemoryPoolAllocator<>&;
	using JsonDocument = rapidjson::Document;
	using JsonValue = rapidjson::Value;

    enum FuncInput_ : glw::enum_t
    {
		FuncInput_Custom = 0
    	
    ,   FuncInput_X
    ,   FuncInput_Y
    ,   FuncInput_XY

    ,   FuncInput_U
    ,   FuncInput_V
    ,   FuncInput_UV

    ,   FuncInput_Time
    ,   FuncInput_DeltaTime
    };

    inline static const std::vector<glw::enum_t> InputTypes[] = {
        /* PinType_UInt */   { FuncInput_Custom, FuncInput_X, FuncInput_Y }
    ,   /* PinType_Int */    { FuncInput_Custom, FuncInput_X, FuncInput_Y }
    ,	/* PinType_Float */  { FuncInput_Custom, FuncInput_X, FuncInput_Y, FuncInput_U, FuncInput_V, FuncInput_Time, FuncInput_DeltaTime }
    ,	/* PinType_Vector */ { FuncInput_Custom, FuncInput_XY, FuncInput_UV }
    };

    inline static const std::string InputNames[] = {
    	"Custom..."
    	
    ,   "X"
    ,   "Y"
    ,   "XY"

    ,   "U"
    ,   "V"
    ,   "UV"

    ,   "Time"
    ,   "Delta Time"
    };

    inline static const std::string InputVars[] = {
    	""
    ,   "x"
    ,   "y"
    ,   "xy"
        
    ,   "u"
    ,   "v"
    ,   "uv"

    ,	"t"
    ,	"dt"
    };

    void DrawImage_(HDRTexture::HandleType* Target);
    void DrawInputs_();
    void Render_(HDRTexture::HandleType* Target);
    void CompileDisplayShader_();


	void                      Read_(uint32_t version, const JsonDocument& document);
	template<typename V> void Read_(const JsonDocument& document);

    glw::shader*     Shader_;
    std::string DisplayCode_;
    FunctionInputs   Inputs_;
    NodeId    ID_, InputsID_;
    InputMap    InputValues_;
    uint32_t     DisplayVar_;
};

    
}
    
#endif // OSD_SHADERS_H
