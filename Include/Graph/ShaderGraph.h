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
#include <FileSystem/FileManager.h>
#include <Renderer/Assets/Texture.h>

#include <vector>
#include <filesystem>
#include <functional>
#include <unordered_set>
#include <unordered_map>
#include <stack>

#include <glm/glm.hpp>
#include <glw/common.h>

#include <open-cpp-utils/startup.h>
#include <open-cpp-utils/directed_tree.h>
#include <open-cpp-utils/any.h>
#include <open-cpp-utils/object_pool.h>

#include <imnode-graph/imnode_graph.h>
#include <rapidjson/document.h>


namespace ocu = open_cpp_utils;

#define RegisterNode(Name, Type) \
	inline Node* Create##Type(ShaderGraph& graph, ImVec2 pos) \
	{ Node* node = new Type(graph, pos); node->Info.Alias = #Type; return node; } \
	STARTUP(_RegisterNode##Type) { ShaderGraph::Register(Name, #Type, Create##Type); }

namespace OpenShaderDesigner
{
	class ShaderAsset;
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
		inline static constexpr ImColor Colors[PinType_COUNT] = {
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
		    std::string Alias; // Node Class Alias, do not change, automatically set by the graph
		    FlagT       Flags;
		} Info;

		Node(ShaderGraph& graph, ImVec2 pos);
		virtual ~Node() = default;

	    void DrawPin(int id, Pin& pin, ImPinDirection direction);
	    void Draw(ImGuiID id);

	    inline virtual bool CheckConnection(const Pin*, const Pin*) const { return true; }
	    virtual void ValidateConnections() { }
	    
		virtual Node* Copy(ShaderGraph& graph) const = 0;
		virtual bool Inspect() { return false; };
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
    
    class GraphState
    {
    public:
    	enum GraphVersion_ : uint32_t
    	{
    		GraphVersion_0 = 0

		,	GraphVersion_Count
		,	GraphVersion_Latest = GraphVersion_Count - 1
		};
    	
    	template<uint32_t V> using Version = std::integral_constant<uint32_t, V>;
    	
    private:
    	
    	using ConnectionStack = ocu::dynarray<std::pair<ImUserPinPtr, ImUserPinPtr>>;
    	using JsonValue       = rapidjson::Value;
    	using JsonAlloc       = rapidjson::MemoryPoolAllocator<>;

    public:
        GraphState(ShaderGraph& parent, ShaderAsset* asset);
        GraphState(const GraphState& other);
        ~GraphState();

        NodeId AddNode(Node* node);
        void RemoveNode(NodeId node, bool erase = true, bool override_flags = false);
    	void Clear();

    	void Draw();

    	void Read(const rapidjson::Value& data);
    	rapidjson::Value Write(rapidjson::MemoryPoolAllocator<>& alloc);

    	GraphState& operator=(const GraphState& other);

    	bool Dirty() const { return Dirty_; }
    	void MakeDirty() { Dirty_ = true; }

    	const Node* GetNode(NodeId node) const;
	    const Node* FindNode(ImGuiID node) const;
    	const Pin&  FindPin(ImPinPtr ptr)  const;

    	const NodeList& GetNodes() const { return Nodes_; }

    private:
		Node* GetNode(NodeId node);
    	Node* FindNode(ImGuiID id);
    	Pin&  FindPin(ImPinPtr ptr);
    	
    	static void             ReadPin_(Pin& pin, const JsonValue& value);
    	static rapidjson::Value WritePin_(const Pin& pin, JsonAlloc& alloc);

    	bool ResolveConnections_();

    	void                      Read_(uint32_t version, const JsonValue& data);
    	template<typename V> void Read_(const JsonValue& data);
    	
    	ShaderAsset*    Asset_;
    	ShaderGraph&    Parent_;
    	NodeList        Nodes_;
    	ConnectionStack QueuedConnections_;
    	bool            Dirty_;

    	friend class ShaderAsset;
    	friend class ShaderGraph;
    };

    class ShaderAsset : public FileManager::Asset
    {
    public:
        inline static const std::string VersionString = "#version 430 core";
        
        ShaderAsset(const FileManager::Path& path, ShaderGraph& graph)
            : Asset(path)
            , State_(graph, this)
        { }

        void PushState()  { History_.push(State_); }
        void PopState()   { State_ = History_.top(); History_.pop();}
    	void ClearState() { State_.Clear(); }
        
        GraphState&       GetState()       { return State_; }
        const GraphState& GetState() const { return State_; }

        ShaderGraph&       GetGraph()       { return State_.Parent_; }
        const ShaderGraph& GetGraph() const { return State_.Parent_; }

        virtual void Compile() = 0;
        virtual void View(HDRTexture::HandleType* Target);

    protected:
        std::string Code;

    	virtual Node* GetSingletonNode(const std::string& alias) { return nullptr; }
        

    private:
        GraphState             State_;
        std::stack<GraphState> History_;

    	friend class GraphState;
    };

	class ShaderGraph
		: public EditorWindow
	{
	public:
		friend Node;

		using ConstructorPtr = Node*(*)(ShaderGraph&, ImVec2);
		struct ContextMenuItem
		{
			std::string    Name, Alias;
			ConstructorPtr Constructor;
		};

		using ContextMenuHierarchy = ocu::directed_tree<ContextMenuItem>;
		using ContextID            = ContextMenuHierarchy::node;
		using AliasMapping         = std::unordered_map<std::string, ContextID>;

	private:
		static ContextMenuHierarchy& ContextMenu_() { static ContextMenuHierarchy Menu {{ "", "", nullptr }}; return Menu; }
		static AliasMapping&            AliasMap_() { static AliasMapping Map; return Map; }

	public:
		static const ContextMenuHierarchy& ContextMenu() { return ContextMenu_(); }
		static const AliasMapping&         AliasMap()    { return AliasMap_();    }

	public:
		ShaderGraph();
		virtual ~ShaderGraph();

		void OnOpen() override;
	    void DrawMenu() override;
	    void DrawWindow() override;

	    void DrawContextMenu();

		/**
		 * \brief Copy the selected nodes
		 */
		void Copy();

		/**
		 * \brief Erase the selected nodes
		 */
		void Erase();

		/**
		 * \brief Paste the selected nodes
		 * \param pos Position to place them at
		 */
		void Paste(ImVec2 pos);

		/**
		 * \brief Clear the selection
		 */
		void Clear();

		const Node* GetNode(NodeId node)   const;
	    const Node* FindNode(ImPinPtr ptr) const;
	    const Node* FindNode(ImGuiID id)   const;
	    const Pin&  FindPin(ImPinPtr ptr)  const;

	    std::string GetValue(ImPinPtr ptr);

	    void OpenShader(ShaderAsset* asset) { Shader_ = asset; }

	    static void Register(const std::filesystem::path& path, const std::string& alias, ConstructorPtr constructor);

		ShaderAsset* GetActive() { return Shader_; }

	private:
		Node* GetNode(NodeId node);
		Node* FindNode(ImPinPtr ptr);
		Node* FindNode(ImGuiID id);
		Pin&  FindPin(ImPinPtr ptr);
		
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
