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

#ifndef SHADERGRAPH_H
#define SHADERGRAPH_H

#include <Editor/EditorWindow.h>

#include <vector>
#include <unordered_map>
#include <filesystem>
#include <unordered_set>
#include <set>
#include <stack>

#include <glm/glm.hpp>

#include <open-cpp-utils/startup.h>
#include <open-cpp-utils/directed_tree.h>
#include <open-cpp-utils/optional.h>

#include <imnode-graph/imnode_graph.h>

#include "open-cpp-utils/any.h"
#include "open-cpp-utils/object_pool.h"

namespace ocu = open_cpp_utils;

#define RegisterNode(Name, Type) \
	Node* Create##Type(ShaderGraph& graph, ImVec2 pos) { return new Type(graph, pos); } \
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

    using PinFlags = unsigned int;
    enum PinFlags_
    {
        PinFlags_None           = 0
    ,   PinFlags_NoCollapse     = 1 << 0
    ,   PinFlags_AlwaysCollapse = 1 << 1
    ,   PinFlags_NoPadding      = 1 << 2
    ,   PinFlags_Ambiguous      = 1 << 3
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

		inline const static std::string TypeNames[PinType_COUNT] = {
			"Unsigned Int"
		,	"Int"
		,	"Float"
		,	"Vector"
		,   "Any"
		};

	    inline const static int TypeWidths[PinType_COUNT] = {
	         1 // Unsigned Int
        ,	 1 // Int
        ,	 1 // Float
        ,	 3 // Vector
        ,   -1 // Any
	    };

	    using Ambiguous = ocu::any<int, unsigned int, float, glm::vec3>;

		std::string Name;
		PinType     Type;
        PinFlags    Flags;
        Ambiguous   Value;
	    ImPinPtr    Ptr;

	    Pin(const std::string& name, PinType type, PinFlags flags = PinFlags_None)
	        : Name(name)
	        , Type(type)
	        , Flags(flags)
	    { }
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
			bool             DynamicInputs;
		} IO;

		struct
		{
			bool   Const;
		} Info;

		Node(ShaderGraph& graph, ImVec2 pos);
		~Node() = default;

	    void DrawPin(int id, Pin& pin, ImPinDirection direction);
	    void Draw(ImGuiID id);

	    virtual bool CheckConnection(Pin*, Pin*) { return false; }
	    virtual void ValidateConnections() { }
		virtual Node* Copy(ShaderGraph& graph) const = 0;
		virtual void Inspect() = 0;
	};

	class ShaderGraph
		: public EditorWindow
	{
	public:
	    struct GraphState
	    {
	        ShaderGraph&            Parent;
	        ocu::object_list<Node*> Nodes;

	        GraphState(ShaderGraph& parent);
	        GraphState(const GraphState& other);
	        ~GraphState();

	        GraphState& operator=(const GraphState& other);
	    };

	private:
		friend Node;

		using ConstructorPtr = Node*(*)(ShaderGraph&, ImVec2);
		struct ContextMenuItem
		{
			std::string Name;
			ConstructorPtr    Constructor;
		};

		using ContextMenuHierarchy = ocu::directed_tree<ContextMenuItem>;
		using ContextID = ContextMenuHierarchy::node;
		inline static ContextMenuHierarchy ContextMenu;


	public:
		ShaderGraph();
		~ShaderGraph();

		void OnOpen() override;
	    void DrawWindow() override;

	    void DrawContextMenu();

	    void Copy();
	    void Erase();
	    void Paste(ImVec2 pos);

	    Node* FindNode(ImPinPtr ptr);
	    Pin&  FindPin(ImPinPtr ptr);

	    static void Register(const std::filesystem::path& path, ConstructorPtr constructor);

	    // History Functionality
	    void        PushState();
	    void        PopState();
	    GraphState& GetState() { return State; }

	private:
        bool GrabFocus;

		GraphState             State;
	    std::stack<GraphState> History;

	    ImVec2 ContextMenuPosition;

		friend class Inspector;
	};

	class Inspector
		: public EditorWindow
	{
	public:
		Inspector();

		void DrawWindow() override;

	private:
		ShaderGraph* Graph;

		friend class ShaderGraph;
	};
}

#endif //SHADERGRAPH_H
