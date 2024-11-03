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

#ifndef OSD_SHADERGRAPH_H
#define OSD_SHADERGRAPH_H

#include <Editor/EditorWindow.h>

#include <vector>
#include <filesystem>
#include <unordered_set>
#include <stack>

#include <glm/glm.hpp>
#include <glw/common.h>

#include <open-cpp-utils/startup.h>
#include <open-cpp-utils/directed_tree.h>
#include <open-cpp-utils/any.h>
#include <open-cpp-utils/object_pool.h>

#include <imnode-graph/imnode_graph.h>

#include "FileSystem/FileManager.h"
#include <Renderer/Assets/Texture.h>


namespace ocu = open_cpp_utils;

#define RegisterNode(Name, Type) \
	inline Node* Create##Type(ShaderGraph& graph, ImVec2 pos) { return new Type(graph, pos); } \
	STARTUP(_Register##Type) { ShaderGraph::Register(Name, Create##Type); }

namespace OpenShaderDesigner
{
	class ShaderGraph;

    using PinType = int;
    enum PinType_
    {
        PinType_UInt = 0
    ,	PinType_Int
    ,	PinType_Float
    ,	PinType_Vector

    ,	PinType_Any
    ,	PinType_COUNT
    };

    using FlagT = unsigned int;
    enum PinFlags_
    {
        PinFlags_None           = 0
    ,   PinFlags_NoCollapse     = 1 << 0
    ,   PinFlags_AlwaysCollapse = 1 << 1
    ,   PinFlags_NoPadding      = 1 << 2
    ,   PinFlags_Ambiguous      = 1 << 3
    ,	PinFlags_Literal        = 1 << 4
    };
    
    enum InterpolationType_ : glw::enum_t
    {
        InterpolationType_Flat = 0
    ,   InterpolationType_Screen
    ,   InterpolationType_Smooth
    };

	struct Pin
	{
		inline const static ImColor Colors[PinType_COUNT] = {
			ImColor(0x7A, 0x9F, 0x82) // Unsigned Int
		,	ImColor(0x64, 0x94, 0xAA) // Int
		,	ImColor(0xA6, 0x3D, 0x40) // Float
		,	ImColor(0xE9, 0xB8, 0x72) // Vector
		,	ImColor(0xFF, 0xFF, 0xFF) // Any
		};

		inline static constexpr const char* TypeNames[PinType_COUNT] = {
			"Unsigned Int"
		,	"Int"
		,	"Float"
		,	"Vector"
		,   "Any"
		};

	    inline const static std::string TypeKeywords[PinType_COUNT] = {
	        "uint"
	    ,   "int"
	    ,   "float"
	    ,   "vec3"
	    ,   "vec3"
	    };

	    inline const static int TypeWidths[PinType_COUNT] = {
	         1 // Unsigned Int
        ,	 1 // Int
        ,	 1 // Float
        ,	 3 // Vector
        ,   -1 // Any
	    };

	    using Ambiguous = ocu::any<glm::int32, glm::uint32, glm::float32, glm::vec3>;

		std::string Name;
		PinType     Type;
        FlagT       Flags;
        Ambiguous   Value;
	    ImPinPtr    Ptr;

	    Pin(const std::string& name, PinType type, FlagT flags = PinFlags_None)
	        : Name(name)
	        , Type(type)
	        , Flags(flags)
	    { }

	    std::string GetVarName() const { return std::format("{}_{}", Name, Ptr.Node); }
    };

    enum NodeFlags_
    {
        NodeFlags_None           = 0
    ,   NodeFlags_Const          = 0x0000'0001
    ,   NodeFlags_DynamicInputs  = 0x0000'0002
    ,   NodeFlags_DynamicOutputs = 0x0000'0004
    };

	struct Node
	{
	public:
	    ShaderGraph& Graph;
		ImVec2       Position = { 0, 0 };

		struct
		{
			std::string Title;
			ImColor     Color, HoveredColor, ActiveColor;
			bool        Enabled;
		} Header;

		struct
		{
			std::vector<Pin> Inputs, Outputs;
		} IO;

		struct
		{
		    std::string Alias;
		    FlagT       Flags;
		} Info;

		Node(ShaderGraph& graph, ImVec2 pos);
		virtual ~Node() = default;

	    void DrawPin(int id, Pin& pin, ImPinDirection direction);
	    void Draw(ImGuiID id);

	    inline virtual bool CheckConnection(Pin*, Pin*) { return true; }
	    virtual void ValidateConnections() { }
	    
		virtual Node* Copy(ShaderGraph& graph) const = 0;
		virtual void Inspect() = 0;
	    virtual std::string GetCode() const = 0;
	};
    
    using NodeList = ocu::object_list<Node*>;
    using NodeId = NodeList::uuid_type;

    struct Attribute
    {
        std::string Name;
        glw::enum_t Type;
        glw::enum_t Interpolation;
        glw::size_t Count;
    };

    struct Parameter
    {
        std::string Name;
        glw::enum_t Type;
        glw::size_t Count; // For arrays
    };
    
    struct GraphState
    {
        
        ShaderGraph& Parent;
        NodeList     Nodes;

        GraphState(ShaderGraph& parent);
        GraphState(const GraphState& other);
        ~GraphState();

        NodeId AddNode(Node* node) { return Nodes.insert(node); }
        void RemoveNode(NodeId node) { if(Nodes[node]->Info.Flags & NodeFlags_Const) return; Nodes.erase(node); }

        GraphState& operator=(const GraphState& other);
    };

    class ShaderAsset : public FileManager::Asset
    {
    public:
        inline static const std::string VersionString = "#version 430 core";
        
        ShaderAsset(const FileManager::Path& path, ShaderGraph& graph)
            : Asset(path)
            , State_(graph)
        { }

        void        PushState()            { History_.push(State_); }
        void        PopState()             { State_ = History_.top(); History_.pop();}
        
        GraphState& GetState()             { return State_; }
        const GraphState& GetState() const { return State_; }

        ShaderGraph& GetGraph() { return State_.Parent; }
        const ShaderGraph& GetGraph() const { return State_.Parent; }

        virtual void Compile() = 0;
        virtual void View(HDRTexture::HandleType* Target) = 0;

    protected:
        std::string Code;
        

    private:
        GraphState             State_;
        std::stack<GraphState> History_;
    };

	class ShaderGraph
		: public EditorWindow
	{
	private:
		friend Node;

		using ConstructorPtr = Node*(*)(ShaderGraph&, ImVec2);
		struct ContextMenuItem
		{
			std::string    Name;
			ConstructorPtr Constructor;
		};

		using ContextMenuHierarchy = ocu::directed_tree<ContextMenuItem>;
		using ContextID = ContextMenuHierarchy::node;
	    
		static ContextMenuHierarchy& ContextMenu() { static ContextMenuHierarchy Menu {{ "", nullptr }}; return Menu; }


	public:
		ShaderGraph();
		virtual ~ShaderGraph();

		void OnOpen() override;
	    void DrawMenu() override;
	    void DrawWindow() override;

	    void DrawContextMenu();

	    void Copy();
	    void Erase();
	    void Paste(ImVec2 pos);
	    void Clear();

	    Node* FindNode(ImPinPtr ptr);
	    Node* FindNode(ImGuiID id);
	    Pin&  FindPin(ImPinPtr ptr);

	    std::string GetValue(ImPinPtr ptr);

	    void OpenShader(ShaderAsset* asset) { Shader_ = asset; }

	    static void Register(const std::filesystem::path& path, ConstructorPtr constructor);

	private:
	    // TODO: Make bitfield
        bool GrabFocus_;
	    ShaderAsset* Shader_;
	    ImVec2 ContextMenuPosition_;
	    ocu::optional<NodeId> Selected_;
	    
	    

		friend class Inspector;
	};

	class Inspector
		: public EditorWindow
	{
	public:
        Inspector();
        virtual ~Inspector() = default;

		void DrawWindow() override;

	private:
		ShaderGraph* Graph;

		friend class ShaderGraph;
	};
}

#endif // OSD_SHADERGRAPH_H
