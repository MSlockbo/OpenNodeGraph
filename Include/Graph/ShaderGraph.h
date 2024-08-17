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
        PinType_Int = 0
    ,	PinType_UInt
    ,	PinType_Float
    ,	PinType_Vector

    ,	PinType_Any
    ,	PinType_COUNT
    };

    using PinFlags = unsigned int;
    enum PinFlags_
    {
        PinFlags_None       = 0
    ,   PinFlags_NoCollapse = 1 << 0
    ,   PinFlags_NoPadding  = 1 << 1
    };

	struct Pin
	{
		inline const static ImColor Colors[PinType_COUNT] = {
			ImColor(0x64, 0x94, 0xAA) // Int
		,	ImColor(0x7A, 0x9F, 0x82) // Unsigned Int
		,	ImColor(0xA6, 0x3D, 0x40) // Float
		,	ImColor(0xE9, 0xB8, 0x72) // Vector
		,	ImColor(0xD2, 0xD5, 0xD3) // Any
		};

		inline const static std::string TypeNames[PinType_COUNT] = {
			"Int"
		,	"Unsigned Int"
		,	"Float"
		,	"Vector"
		,   "Any"
		};

	    using Ambiguous = ocu::any<int, unsigned int, float, ImVec4>;

		std::string Name;
		PinType     Type;
        PinFlags    Flags;
        Ambiguous   Value;

	    Pin(const std::string& name, PinType type, PinFlags flags = PinFlags_None)
	        : Name(name)
	        , Type(type)
	        , Flags(flags)
	    { }
    };

	struct Node
	{
	public:
		ImVec2 Position = { 0, 0 };

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

	    void DrawPin(ImGuiID id, Pin& pin, ImPinDirection direction);
	    void Draw(ImGuiID id);

		virtual Node* Copy(ShaderGraph& graph) const = 0;
		virtual void Inspect() = 0;
	};

	class ShaderGraph
		: public EditorWindow
	{
	private:
		friend Node;

		using ConstructorPtr = Node*(*)(ShaderGraph&, ImVec2);
		struct ContextMenuItem
		{
			std::string Name;
			ConstructorPtr    Constructor;
		};

		struct GraphState
		{
			ShaderGraph&            Parent;
		    ocu::object_list<Node*> Nodes;

			GraphState(ShaderGraph& parent);
			GraphState(const GraphState& other);
			~GraphState();

			GraphState& operator=(const GraphState& other);
		};

		using ContextMenuHierarchy = ocu::directed_tree<ContextMenuItem>;
		using ContextID = ContextMenuHierarchy::node;
		inline static ContextMenuHierarchy ContextMenu;

		// History Functionality
		void PushState();
		void PopState();

	public:
		ShaderGraph();
		~ShaderGraph();

		void OnOpen() override;
	    void DrawWindow() override;

	    void DrawContextMenu();

	    void Copy();
	    void Erase();
	    void Paste(ImVec2 pos);

		static void Register(const std::filesystem::path& path, ConstructorPtr constructor);

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
