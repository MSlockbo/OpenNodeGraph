//
// Created by Maddie on 7/1/2024.
//

#ifndef SHADERGRAPH_H
#define SHADERGRAPH_H

#include <Editor/EditorWindow.h>
#include <Utility/Startup.h>

#include <vector>
#include <unordered_map>
#include <filesystem>
#include <unordered_set>

#include <Utility/DirectedGraph.h>
#include <Utility/Optional.h>

#define RegisterNode(Name, Type) \
	Node* Create##Type(ShaderGraph& graph, ImVec2 pos) { return new Type(graph, pos); } \
	STARTUP(_Register##Type) { ShaderGraph::Register(Name, Create##Type); }

namespace OpenShaderDesigner
{
	class ShaderGraph;
	using PinId = uint16_t;
	using NodeId = uint32_t;

	struct Pin
	{
		enum PinType
		{
			INT = 0
		,	UINT
		,	FLOAT
		,	VECTOR
		,	TEXTURE
		,	ANY

		,	COUNT
		};

		enum PinDirection
		{
			INPUT
		,	OUTPUT
		};

		inline const static ImColor Colors[COUNT] = {
			ImColor(0xB9, 0xF5, 0x94)
		,	ImColor(0x8C, 0xC0, 0x8C)
		,	ImColor(0x37, 0x95, 0x85)
		,	ImColor(0xE3, 0x7D, 0xDC)
		,	ImColor(0xD2, 0x6E, 0x46)
		,	ImColor(0xD2, 0xD5, 0xD3)
		};

		std::string   Name;
		PinType       Type;
		PinDirection  Direction;
	};

	struct Node
	{
		ImVec2 Position = { 0, 0 };

		struct
		{
			std::string Title = "Node";
			ImColor     Color = Pin::Colors[Pin::VECTOR];
			bool        Enabled = true;
		} Header;

		struct
		{
			std::vector<Pin> Inputs, Outputs;
			bool             DynamicInputs = false;
		} IO;

		struct
		{
			ImVec2 Size;
			bool Const;
		} Info;

		Node(
			ShaderGraph& graph, ImVec2 pos
		,	const std::string& title, ImColor color
		,	const std::vector<Pin>& inputs, bool dyn_inputs
		,	const std::vector<Pin>& outputs
		,	bool constant = false);

		virtual Node* Copy(ShaderGraph& graph) const = 0;
	};

	class ShaderGraph
		: public EditorWindow
	{
	private:
		friend Node;
		struct PinPtr
		{
			struct Hash
			{
				size_t operator()(const PinPtr& p) const
				{
					return p.hash();
				}
			};

			NodeId Node;
			PinId  Pin;
			bool Input;

			size_t hash() const { return (Input ? 0 : 0x8000000) | static_cast<size_t>(Node) << 32 | static_cast<size_t>(Pin & 0x7FFFFFFF); }

			bool operator<(const PinPtr& o) const { return hash() < o.hash(); }
			bool operator==(const PinPtr& o) const { return hash() == o.hash(); }
		};

		using Connection = std::pair<const PinPtr, PinPtr>;
		using ConnectionMap = std::unordered_multimap<PinPtr, PinPtr, PinPtr::Hash>;

		struct Line
		{
			ImColor Color;
			float Thickness;
		};

		using ConstructorPtr = Node*(*)(ShaderGraph&, ImVec2);
		struct ContextMenuItem
		{
			std::string Name;
			ConstructorPtr    Constructor;
		};

		using ContextMenuHierarchy = DirectedGraph<ContextMenuItem>;
		using ContextID = ContextMenuHierarchy::Node;
		inline static ContextMenuHierarchy ContextMenu;

		// Helper functions
		float CalculateWidth(Node& node);
		float CalculateHeight(Node& node);

		// Base Draw and Input functions
		void HandleInput();
		void DrawGrid();
		void DrawNode(Node& node, NodeId id);
		void DrawPin(NodeId node_id, Pin& pin, PinId pin_id, ImVec2 location, bool input);
		void DrawContextMenu();

		// Connection functions
		void DrawConnections();
		void DrawConnection(const PinPtr& a, const PinPtr& b);
		auto StartConnection(const PinPtr& ptr) -> void;
		void StopConnection();
		void CreateConnection(const PinPtr& a, const PinPtr& b);
		void EraseConnection(const PinPtr& a, const PinPtr& b);
		void EraseConnections(const PinPtr& a);

		NodeId AddNode(Node* node);
		void RemoveNode(NodeId id);

		// Clipboard functionality
		void ClearClipboard();
		void Copy();
		void Paste(const ImVec2& location);
		void EraseSelection();

		// Helper functions
		float BezierOffset(const ImVec2& out, const ImVec2& in);
		bool AABB(const ImVec2& a0, const ImVec2& a1, const ImVec2& b0, const ImVec2& b1);

	public:
		ShaderGraph();
		~ShaderGraph();

		void OnOpen() override;
		void DrawWindow() override;

		static void Register(const std::filesystem::path& path, ConstructorPtr constructor);

	private:
		std::unordered_set<NodeId> Selected;
		std::vector<Node*>         Nodes;
		std::unordered_set<PinId>  Erased;
		ConnectionMap              Connections;

		struct
		{
			struct
			{
				ImColor BackgroundColor;

				struct
				{
					Line Thin, Thick;
					float Padding;
				} Lines;
			} Grid;

			struct
			{
				float Rounding;
				Line Border, SelectedBorder;
				ImColor Content;
				ImColor Title;

				struct
				{
					float Padding;
					float BorderThickness;
					ImColor Background;
					ImColor Text;
					Line Connections;
				} Pins;
			} Nodes;

			struct
			{
				ImColor Background;
				Line    Border;
			} Selection;

			float FontSize;
		} Style;

		struct
		{
			struct
			{
				struct
				{
					float Rate, Smoothing;
				} Scroll;
			} Input;
		} Settings;

		struct
		{
			ImVec2           Location, ScreenLocation, Delta;
			float            Scroll;
			bool             ClickedSomething;

			Optional<NodeId> FocusedNode;
			std::unordered_map<NodeId, ImVec2> Locks;
			std::unordered_set<NodeId> DragSelect;
			bool LocksDragged, NodeHovered;
			Optional<PinPtr> NewConnection;
		} Mouse;

		struct
		{
			ImVec2 Location;
			float  Zoom, Scroll;
		} Camera;

		struct
		{
			std::vector<Node*> Nodes;
			ConnectionMap      Connections;
		} Clipboard;

		bool Focused;
		ImVec2 ContextMenuPosition;
	};
}

#endif //SHADERGRAPH_H
