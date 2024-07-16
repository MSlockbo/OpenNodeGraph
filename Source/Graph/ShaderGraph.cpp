//
// Created by Maddie on 7/1/2024.
//

#include <filesystem>
#include <stack>

#include <Core/Engine.h>
#include <Core/Console.h>

#include <glm/common.hpp>
#include <Graph/ShaderGraph.h>
#include <imgui-docking/imgui_internal.h>

using namespace OpenShaderDesigner;

ImColor operator*(const ImColor& c, float f)
{
	return ImVec4(c.Value.x * f, c.Value.y * f, c.Value.z * f, c.Value.w);
}

float ShaderGraph::CalculateWidth(Node& node)
{
	const float GridSize = Style.FontSize + Style.Grid.Lines.Padding;
	const float HeaderHeight = Style.FontSize;
	const float HeaderWidth = ImGui::CalcTextSize(node.Header.Title.c_str()).x;
	float InputWidth = 0.0f, OutputWidth = 0.0f;

	// Longest Input Pin
	for(const Pin& pin : node.IO.Inputs)
	{
		InputWidth = glm::max(InputWidth, HeaderHeight + ImGui::CalcTextSize(pin.Name.c_str()).x);
	}

	// Longest Output Pin
	for(const Pin& pin : node.IO.Outputs)
	{
		OutputWidth = glm::max(OutputWidth, HeaderHeight + ImGui::CalcTextSize(pin.Name.c_str()).x);
	}

	float Width = glm::max(InputWidth, HeaderWidth) + OutputWidth + 1 * HeaderHeight;
	Width += GridSize - std::fmod(1.0f + Style.Grid.Lines.Padding + Width, GridSize);

	return Width;
}

float ShaderGraph::CalculateHeight(Node& node)
{
	const float HeaderHeight = Style.FontSize;
	const float PinHeight = HeaderHeight * static_cast<float>(1 + glm::max(node.IO.Inputs.size(), node.IO.Outputs.size()));
	return glm::max(HeaderHeight + PinHeight, 2 * HeaderHeight);
}

Node::Node(
	ShaderGraph& graph, ImVec2 pos
,	const std::string& title, ImColor color
,	const std::vector<Pin>& inputs, bool dyn_inputs
,	const std::vector<Pin>& outputs, bool constant)
	: Position(pos)
	, Header
	{
		.Title = title
	,	.Color = color
	,	.Enabled = true
	}
	, IO
	{
		.Inputs = inputs
	,	.Outputs = outputs
	,	.DynamicInputs = dyn_inputs
	}
	, Info
	{
		.Size = ImVec2(graph.CalculateWidth(*this), graph.CalculateHeight(*this))
	,	.Const = constant
	}
{ }

ShaderGraph::ShaderGraph()
	: EditorWindow("\uED46 Shader Graph", 0)
	, Style
	{
		.Grid
		{
			.BackgroundColor = ImColor(0x11, 0x11, 0x11)
		,	.Lines
			{
				.Thin
				{
					.Color = ImColor(0x44, 0x44, 0x44)
				,	.Thickness = 1.0
				}
			,	.Thick
				{
					.Color = ImColor(0x88, 0x88, 0x88)
				,	.Thickness = 2.0
				}
			,	.Padding = 2.0f
			}
		}
	,	.Nodes
		{
			.Rounding = 5.0f
		,	.Border = { ImColor(0x33, 0x33, 0x33), 2.0f }
		,	.SelectedBorder = { ImColor(0xEF, 0xAE, 0x4B), 4.0f }
		,	.Content = ImColor(0x88, 0x88, 0x88)
		,	.Title = ImColor(0xCC, 0xCC, 0xCC)
		,	.Pins
			{
				.Padding = 2.0f
			,	.BorderThickness = 3.0f
			,	.Background = ImColor(0x22, 0x22, 0x22)
			,	.Text = ImColor(0x22, 0x22, 0x22)
			,	.Connections
				{
					.Color = ImColor(0x00, 0x00, 0x00)
				,	.Thickness = 2.0f
				}
			}
		}
	,	.Selection
		{
			.Background = ImColor(0xC9, 0x8E, 0x36, 0x44)
		,	.Border
			{
				.Color = ImColor(0xEF, 0xAE, 0x4B, 0xBB)
			,	.Thickness = 2.0f
			}
		}
	,	.FontSize = 20.0f
	}
	, Settings
	{
		.Input
		{
			.Scroll
			{
				.Rate = 0.2f
			,	.Smoothing = 8.0f
			}
		}
	}
	, Mouse({ 0, 0 }, { 0, 0 })
	, Camera({ 0, 0 }, 1)
	, Focused(false)
{
}

ShaderGraph::~ShaderGraph()
{
}

void ShaderGraph::OnOpen()
{
	Mouse.Location = ImGui::GetMousePos();
	Camera.Scroll = Camera.Zoom = 1.0f;
}

void ShaderGraph::DrawWindow()
{
	HandleInput();
	DrawContextMenu();

	DrawGrid();

	NodeId uid = 0;
	for(Node* node : Nodes)
	{
		if(node == nullptr) { ++uid; continue; }
		DrawNode(*node, uid++);
	}

	DrawConnections();

	ImDrawList& DrawList = *ImGui::GetWindowDrawList();

	const bool Ctrl = ImGui::IsKeyDown(ImGuiKey_ModCtrl);
	const bool Shift = ImGui::IsKeyDown(ImGuiKey_ModShift);
	const ImVec2 Drag = ImGui::GetMouseDragDelta();
	if((Drag.x != 0 || Drag.y != 0) && !Mouse.FocusedNode() && !Mouse.NewConnection() && Focused)
	{
		const ImVec2 A = ImGui::GetMousePos();
		const ImVec2 B = A - Drag;

		const ImVec2 Min = ImMin(A, B);
		const ImVec2 Max = ImMax(A, B);

		DrawList.AddRectFilled(Min, Max, Style.Selection.Background);
		DrawList.AddRect(Min, Max, Style.Selection.Border.Color, Style.Selection.Border.Thickness);
	}

	DrawList.PopClipRect();

	if(ImGui::IsMouseReleased(ImGuiMouseButton_Left) && Focused)
	{
		if(Drag.x == 0 && Drag.y == 0 && !Mouse.NodeHovered && !(Ctrl || Shift))
		{
			Selected.clear();
		}

		Mouse.Locks.clear();
		Mouse.FocusedNode.Reset();
		Mouse.DragSelect.clear();
		Mouse.LocksDragged = false;
	}
}

void ShaderGraph::HandleInput()
{
	// Window Info
	const ImVec2 CanvasMin = ImGui::GetWindowPos() + ImGui::GetWindowContentRegionMin();
	const ImVec2 CanvasMax = ImGui::GetWindowPos() + ImGui::GetWindowContentRegionMax();
	const ImVec2 CanvasSize = CanvasMax - CanvasMin;
	const ImVec2 CanvasCenter = CanvasMin + CanvasSize * 0.5f;

	Focused = ImGui::IsWindowFocused();

	// Calculate Mouse Delta
	Mouse.ClickedSomething = false;
	Mouse.Location = (Mouse.ScreenLocation - CanvasCenter + Camera.Location) * Camera.Zoom;
	Mouse.Delta = ImGui::GetMousePos() - Mouse.ScreenLocation;
	Mouse.ScreenLocation = ImGui::GetMousePos();
	Mouse.Scroll = ImGui::GetIO().MouseWheel;
	Mouse.NodeHovered = false;

	// Dragging the Grid
	if(ImGui::IsMouseDragging(ImGuiMouseButton_Middle) && Focused)
	{
		Camera.Location -= Mouse.Delta;
	}

	// Zooming
	if(Focused)
	{
		Camera.Scroll -= Mouse.Scroll * Settings.Input.Scroll.Rate;
		Camera.Scroll = glm::clamp(Camera.Scroll, 1.0f, 4.0f);
	}

	// Smooth out zoom
	const float ZoomPrev = Camera.Zoom;
	Camera.Zoom = glm::mix(Camera.Zoom, Camera.Scroll, Engine::Delta * Settings.Input.Scroll.Smoothing);
	Camera.Location *= (1.0f - (Camera.Zoom - ZoomPrev) / Camera.Zoom);

	// Delete nodes
	if(ImGui::IsKeyPressed(ImGuiKey_Delete))
	{
		EraseSelection();
	}

	if(ImGui::IsKeyPressed(ImGuiKey_C) && ImGui::IsKeyDown(ImGuiKey_ModCtrl))
	{
		Copy();
	}

	if(ImGui::IsKeyPressed(ImGuiKey_X) && ImGui::IsKeyDown(ImGuiKey_ModCtrl))
	{
		Copy();
		EraseSelection();
	}

	if(ImGui::IsKeyPressed(ImGuiKey_V) && ImGui::IsKeyDown(ImGuiKey_ModCtrl))
	{
		Paste(Mouse.Location);
	}
}

void ShaderGraph::DrawGrid()
{
	// Vars ============================================================================================================

	// ImGui Vars
	ImDrawList& DrawList = *ImGui::GetWindowDrawList();

	// Style Vars
	const ImColor BackgroundColor = Style.Grid.BackgroundColor;

	const ImColor ThinLineColor = Style.Grid.Lines.Thin.Color;
	const float ThinLineSpacing = (Style.FontSize + Style.Grid.Lines.Padding);
	const float ThinLineThickness = Style.Grid.Lines.Thin.Thickness;

	const ImColor ThickLineColor = Style.Grid.Lines.Thick.Color;
	const float ThickLineSpacing = 5.0f * (Style.FontSize + Style.Grid.Lines.Padding);
	const float ThickLineThickness = Style.Grid.Lines.Thick.Thickness;

	// Window Info
	const ImVec2 CanvasMin = ImGui::GetWindowPos() + ImGui::GetWindowContentRegionMin();
	const ImVec2 CanvasMax = ImGui::GetWindowPos() + ImGui::GetWindowContentRegionMax();
	const ImVec2 CanvasSize = CanvasMax - CanvasMin;

	// Grid Vars
	ImVec2 TopLeftReal = Camera.Location - CanvasSize * 0.5f;
	ImVec2 ThinGridStart = CanvasMin - ImVec2(glm::mod(TopLeftReal.x, ThinLineSpacing / Camera.Zoom), glm::mod(TopLeftReal.y, ThinLineSpacing / Camera.Zoom));
	ImVec2 ThickGridStart = CanvasMin - ImVec2(glm::mod(TopLeftReal.x, ThickLineSpacing / Camera.Zoom), glm::mod(TopLeftReal.y, ThickLineSpacing / Camera.Zoom));


	// Drawing =========================================================================================================

	DrawList.PushClipRect(CanvasMin, CanvasMax, false);

	// Background
	DrawList.AddRectFilled(CanvasMin, CanvasMax, BackgroundColor);


	// Thin Grid
	for(float x = ThinGridStart.x; x <= ThinGridStart.x + (CanvasSize.x + ThinLineSpacing) * Camera.Zoom; x += ThinLineSpacing / Camera.Zoom)
	{
		DrawList.AddLine({ x, CanvasMin.y }, { x, CanvasMax.y}, ThinLineColor, ThinLineThickness);
	}

	for(float y = ThinGridStart.y; y <= ThinGridStart.y + (CanvasSize.y + ThinLineSpacing) * Camera.Zoom; y += ThinLineSpacing / Camera.Zoom)
	{
		DrawList.AddLine({ CanvasMin.x, y }, { CanvasMax.x, y }, ThinLineColor, ThinLineThickness);
	}


	// Thick Grid
	for(float x = ThickGridStart.x; x <= ThickGridStart.x + (CanvasSize.x + ThickLineSpacing) * Camera.Zoom; x += ThickLineSpacing / Camera.Zoom)
	{
		DrawList.AddLine({ x, CanvasMin.y }, { x, CanvasMax.y}, ThickLineColor, ThickLineThickness);
	}

	for(float y = ThickGridStart.y; y <= ThickGridStart.y + (CanvasSize.y + ThickLineSpacing) * Camera.Zoom; y += ThickLineSpacing / Camera.Zoom)
	{
		DrawList.AddLine({ CanvasMin.x, y }, { CanvasMax.x, y }, ThickLineColor, ThickLineThickness);
	}
}

void ShaderGraph::DrawNode(Node& node, NodeId id)
{
	// Vars ============================================================================================================

	// Window Info
	const ImVec2 CanvasMin = ImGui::GetWindowPos() + ImGui::GetWindowContentRegionMin();
	const ImVec2 CanvasMax = ImGui::GetWindowPos() + ImGui::GetWindowContentRegionMax();
	const ImVec2 CanvasSize = CanvasMax - CanvasMin;
	const ImVec2 CanvasCenter = CanvasMin + CanvasSize * 0.5f;

	// ImGui Vars
	ImDrawList& DrawList = *ImGui::GetWindowDrawList();

	// Draw Vars
	const ImVec2 Padding = { Style.Grid.Lines.Padding, Style.Grid.Lines.Padding };
	const float HeaderHeight = Style.FontSize / Camera.Zoom;
	const ImVec2 Size = node.Info.Size / Camera.Zoom;
	const ImVec2 Position = CanvasCenter - Camera.Location + node.Position / Camera.Zoom + Padding;
	const bool NodeHovered = ImGui::IsMouseHoveringRect(Position, Position + Size);
	const bool HeaderHovered = ImGui::IsMouseHoveringRect(Position, Position + ImVec2(Size.x, HeaderHeight));
	const bool HasLock = Mouse.FocusedNode();
	const ImColor HeaderColor = node.Header.Color * (HeaderHovered || HasLock ? 1.2f : 1.0f) * (HasLock ? 0.8f : 1.0f);
	const ImVec2 InputLoc = Position + ImVec2(Style.Nodes.Pins.Padding / Camera.Zoom, HeaderHeight);
	const ImVec2 OutputLoc = Position + ImVec2(Size.x - HeaderHeight - Style.Nodes.Pins.Padding / Camera.Zoom, HeaderHeight);
	const float GridSize = (Style.FontSize + Style.Grid.Lines.Padding);
	const bool Ctrl = ImGui::IsKeyDown(ImGuiKey_ModCtrl);
	const bool Shift = ImGui::IsKeyDown(ImGuiKey_ModShift);

	// Input ===========================================================================================================

	Mouse.NodeHovered = Mouse.NodeHovered ? true : NodeHovered;

	// Clicking nodes
	if(ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		Mouse.ClickedSomething = NodeHovered ? true : Mouse.ClickedSomething;

		if(HeaderHovered)
		{
			if(!Mouse.FocusedNode() && !(Ctrl || Shift))
			{
				Mouse.FocusedNode  = id;
				Mouse.LocksDragged = false;

				for(NodeId selected : Selected)
				{
					Mouse.Locks.emplace(selected, Mouse.Location - Nodes[selected]->Position);
				}
			}
		}
	}

	// Drag selection
	if(ImGui::IsMouseDragging(ImGuiMouseButton_Left) && !Mouse.NewConnection() && Focused)
	{
		// Reset When Dragging a New Node
		if(Mouse.FocusedNode() && Mouse.FocusedNode == id && !Selected.contains(id))
		{
			if(!(Ctrl || Shift))
			{
				Selected.clear();
				Mouse.Locks.clear();
			}
			Selected.insert(id);
		}

		// Begin selection
		if(Mouse.FocusedNode() && Selected.contains(id) && !(Ctrl || Shift))
		{
			Mouse.LocksDragged = true;

			for(NodeId selected : Selected)
			{
				Mouse.Locks.emplace(selected, Mouse.Location - Nodes[selected]->Position);
			}
		}

		// Make sure we aren't dragging nodes
		if(Mouse.LocksDragged == false)
		{
			// Calculate selection bounds
			ImVec2 Drag = ImGui::GetMouseDragDelta() * Camera.Zoom;
			ImVec2 A0 = Mouse.Location - Drag;
			ImVec2 A1 = Mouse.Location;
			ImVec2 B0 = node.Position;
			ImVec2 B1 = node.Position + node.Info.Size;
			bool Intersect = AABB(A0, A1, B0, B1);

			// Clear selection for new selection
			if(Mouse.DragSelect.empty() && !Shift && !Ctrl && !Mouse.FocusedNode())
			{
				Selected.clear();
			}

			// Select nodes
			if(Intersect && !Mouse.DragSelect.contains(id))
			{
				if(!Ctrl)
				{
					if(!Selected.contains(id))
					{
						Selected.insert(id);
						Mouse.DragSelect.insert(id);
					}
				}
				else
				{
					if(Selected.contains(id)) Selected.erase(id);
					else Selected.insert(id);

					Mouse.DragSelect.insert(id);
				}
			}

			// Unselect nodes
			if(!Intersect && Mouse.DragSelect.contains(id))
			{
				Mouse.DragSelect.erase(id);

				if(!Ctrl)
				{
					Selected.erase(id);
				}
				else
				{
					if(Selected.contains(id)) Selected.erase(id);
					else Selected.insert(id);
				}
			}
		}
	}

	if(ImGui::IsMouseReleased(ImGuiMouseButton_Left) && HeaderHovered)
	{
		if(Selected.contains(id))
		{
			if(Ctrl) Selected.erase(id);
			else if(!Shift && !Mouse.LocksDragged)
			{
				Selected.clear();
				Selected.insert(id);
			}
		}
		else
		{
			if(!(Ctrl || Shift))
				Selected.clear();
			Selected.insert(id);
		}
	}

	if(Mouse.Locks.contains(id))
	{
		node.Position = Mouse.Location - Mouse.Locks[id];
		node.Position = ImFloor(node.Position / GridSize + ImVec2(0.5f, 0.5f)) * GridSize;
	}

	// Content =========================================================================================================

	DrawList.AddRectFilled(Position, Position + Size, Style.Nodes.Content, Style.Nodes.Rounding / Camera.Zoom + 1);

	// Header ==========================================================================================================

	if(node.Header.Enabled)
	{
		DrawList.PushClipRect(Position, Position + ImVec2(Size.x, HeaderHeight), true);
		DrawList.AddRectFilled(Position, Position + Size, HeaderColor, Style.Nodes.Rounding / Camera.Zoom + 1);
		DrawList.AddText(NULL, Style.FontSize / Camera.Zoom, Position + ImVec2(Style.Nodes.Rounding / Camera.Zoom, 0) + Padding / 2.0f, Style.Nodes.Title, node.Header.Title.c_str());
		DrawList.PopClipRect();

		DrawList.AddLine(InputLoc, Position + ImVec2(Size.x, HeaderHeight), Style.Nodes.Border.Color, Style.Nodes.Border.Thickness / Camera.Zoom);
	}

	// Border ==========================================================================================================

	if(Selected.contains(id)) DrawList.AddRect(Position, Position + Size, Style.Nodes.SelectedBorder.Color, Style.Nodes.Rounding / Camera.Zoom, 0, Style.Nodes.SelectedBorder.Thickness / Camera.Zoom);
	else DrawList.AddRect(Position, Position + Size, Style.Nodes.Border.Color, Style.Nodes.Rounding / Camera.Zoom, 0, Style.Nodes.Border.Thickness / Camera.Zoom);

	// Pins ============================================================================================================

	int i = 0;
	for(Pin& pin : node.IO.Inputs)
	{
		DrawPin(id, pin, i, InputLoc + ImVec2(0, HeaderHeight) * (i + 0.5f), true);
		++i;
	}

	int j = 0;
	for(Pin& pin : node.IO.Outputs)
	{
		DrawPin(id, pin, j, OutputLoc + ImVec2(0, HeaderHeight) * (j + 0.5f), false);
		++j;
	}
}

void ShaderGraph::DrawPin(NodeId node_id, Pin& pin, PinId pin_id, ImVec2 location, bool input)
{
	// Vars ============================================================================================================

	// ImGui Vars
	ImDrawList& DrawList = *ImGui::GetWindowDrawList();

	// Draw Vars
	const float HeaderHeight = Style.FontSize / Camera.Zoom;
	const float PinRadius = (Style.FontSize - Style.Nodes.Pins.Padding - 2.0f * Style.Nodes.Pins.BorderThickness) * 0.5f / Camera.Zoom;
	const ImVec2 Offset = ImVec2(HeaderHeight, HeaderHeight) * 0.5f;
	const bool Hovered = ImGui::IsMouseHoveringRect(location, location + ImVec2(HeaderHeight, HeaderHeight) / Camera.Zoom);

	// Pin =============================================================================================================

	// Input
	if(ImGui::IsMouseClicked(ImGuiMouseButton_Left) && Hovered && !Mouse.NewConnection() && !ImGui::IsKeyDown(ImGuiKey_ModAlt))
	{
		StartConnection({ node_id, pin_id, input });
	}

	// Circle
	PinPtr ptr = { node_id, pin_id, input };
	auto it = Connections.find(ptr);
	if(Mouse.NewConnection() && *Mouse.NewConnection == ptr) DrawList.AddCircleFilled(location + Offset, PinRadius, Pin::Colors[pin.Type]);
	else if(!input && it != Connections.end())               DrawList.AddCircleFilled(location + Offset, PinRadius, Pin::Colors[pin.Type] * (Hovered ? 0.8f : 1.0f));
	else if(it != Connections.end())                         DrawList.AddCircleFilled(location + Offset, PinRadius, Pin::Colors[Nodes[it->second.Node]->IO.Outputs[it->second.Pin].Type] * (Hovered ? 0.8f : 1.0f));
	else if(Hovered)                                         DrawList.AddCircleFilled(location + Offset, PinRadius, Pin::Colors[pin.Type]);
	else                                                     DrawList.AddCircleFilled(location + Offset, PinRadius, Style.Nodes.Pins.Background);
	DrawList.AddCircle(location + Offset, PinRadius, Pin::Colors[pin.Type], 0, Style.Nodes.Pins.BorderThickness / Camera.Zoom);

	// Text
	const ImVec2 TextOffset = location + ImVec2((input ? HeaderHeight : -ImGui::CalcTextSize(pin.Name.c_str()).x / Camera.Zoom), 0);
	DrawList.AddText(NULL, Style.FontSize / Camera.Zoom, TextOffset, Style.Nodes.Pins.Text, pin.Name.c_str());

	// Input ===========================================================================================================

	if(ImGui::IsMouseReleased(ImGuiMouseButton_Left))
	{
		if(Hovered)
		{
			if(Mouse.NewConnection()) CreateConnection({ node_id, pin_id, input }, Mouse.NewConnection);
			else if(ImGui::IsKeyDown(ImGuiKey_ModAlt)) EraseConnections({ node_id, pin_id, input });
		}
	}
}

void ShaderGraph::DrawContextMenu()
{
	const float GridSize = (Style.FontSize + Style.Grid.Lines.Padding);

	if(ImGui::IsMouseClicked(ImGuiMouseButton_Right))
	{
		ContextMenuPosition = Mouse.Location;
	}

	if(ImGui::BeginPopupContextWindow())
	{
		if(ImGui::MenuItem("Copy", "Ctrl+C", false, !Selected.empty())) Copy();
		if(ImGui::MenuItem("Cut", "Ctrl+X", false, !Selected.empty()))
		{
			Copy();
			EraseSelection();
		}
		if(ImGui::MenuItem("Paste", "Ctrl+V", false, !Clipboard.Nodes.empty())) Paste(ContextMenuPosition);

		ImGui::Separator();

		// Create Nodes
		ImVec2 position = ContextMenuPosition;
		position = ImFloor(position / GridSize) * GridSize;

		std::stack<ContextID> context; context.push(0);

		struct Visitor
		{
			bool operator()(ContextMenuItem& item, ContextID id)
			{
				const auto depth = Graph.ContextMenu.Depth(id);
				if(depth > Context.size()) return false;

				while(depth < Context.size())
				{
					Context.pop();
					ImGui::EndMenu();
				}

				if(Context.top() != Graph.ContextMenu.Parent(id)) return false;
				std::string name = std::format("{}##{}", item.Name, id);

				if(item.Constructor)
				{
					if(ImGui::MenuItem(item.Name.c_str()))
					{
						Graph.AddNode(item.Constructor(Graph, Location));
					}
				}
				else
				{
					if(ImGui::BeginMenu(item.Name.c_str()))
					{
						Context.push(id);
					}
					else
					{
						return false;
					}
				}

				return false;
			}

			ShaderGraph& Graph;
			const ImVec2 Location;
			std::stack<ContextID>& Context;
		} MenuVisitor
		{
			.Graph = *this
		,	.Location = position
		,	.Context = context
		};

		ContextMenu.Traverse<Visitor, ContextMenuHierarchy::PreOrder>(MenuVisitor);

		context.pop();
		while(context.empty() == false)
		{
			ImGui::EndMenu();
			context.pop();
		}

		ImGui::EndPopup();
	}
}

void ShaderGraph::DrawConnections()
{
	// Vars ============================================================================================================

	// ImGui Vars
	ImDrawList& DrawList = *ImGui::GetWindowDrawList();

	const float HeaderHeight = Style.FontSize / Camera.Zoom;
	const float PinRadius = (Style.FontSize - Style.Nodes.Pins.Padding - 2.0f * Style.Nodes.Pins.BorderThickness) * 0.5f / Camera.Zoom;
	const ImVec2 Padding = { Style.Grid.Lines.Padding, Style.Grid.Lines.Padding };
	const ImVec2 CanvasMin = ImGui::GetWindowPos() + ImGui::GetWindowContentRegionMin();
	const ImVec2 CanvasMax = ImGui::GetWindowPos() + ImGui::GetWindowContentRegionMax();
	const ImVec2 CanvasSize = CanvasMax - CanvasMin;
	const ImVec2 CanvasCenter = CanvasMin + CanvasSize * 0.5f;
	const ImVec2 Offset = ImVec2(HeaderHeight, HeaderHeight) * 0.5f;

	// Connections =============================================================================================================

	for(const Connection& connection : Connections)
	{
		DrawConnection(connection.first, connection.second);
	}

	if(Mouse.NewConnection())
	{
		const Node& Node = *Nodes[Mouse.NewConnection->Node];
		const Pin&  Pin  = Mouse.NewConnection->Input ? Node.IO.Inputs[Mouse.NewConnection->Pin] : Node.IO.Outputs[Mouse.NewConnection->Pin];

		const ImVec2 Size = Node.Info.Size / Camera.Zoom;
		const ImVec2 Position = CanvasCenter - Camera.Location + Node.Position / Camera.Zoom + Padding;
		const ImVec2 InputLoc = Position + ImVec2(Style.Nodes.Pins.Padding / Camera.Zoom, HeaderHeight);
		const ImVec2 OutputLoc = Position + ImVec2(Size.x - HeaderHeight - Style.Nodes.Pins.Padding / Camera.Zoom, HeaderHeight);

		const ImVec2 Root = (Mouse.NewConnection->Input ? InputLoc : OutputLoc) + ImVec2(0, HeaderHeight) * (Mouse.NewConnection->Pin + 0.5f);

		const ImVec2 A = Root + Offset + ImVec2(Mouse.NewConnection->Input ? -PinRadius : PinRadius, 0);
		const ImVec2 D = ImGui::GetMousePos();
		const float Off = BezierOffset(A, D);

		const ImVec2 B = ImVec2(A.x + (Mouse.NewConnection->Input ? -Off :  Off), A.y);
		const ImVec2 C = ImVec2(D.x + (Mouse.NewConnection->Input ?  Off : -Off), D.y);

		DrawList.AddBezierCubic(
			A, B, C, D
		,	Pin::Colors[Pin.Type], Style.Nodes.Pins.Connections.Thickness / Camera.Zoom
		);
	}

	// Input ===========================================================================================================

	if(ImGui::IsMouseReleased(ImGuiMouseButton_Left))
	{
		StopConnection();
	}
}

void ShaderGraph::DrawConnection(const PinPtr& a, const PinPtr& b)
{
	// Vars ============================================================================================================

	// ImGui Vars
	ImDrawList& DrawList = *ImGui::GetWindowDrawList();

	// Draw Vars
	const float HeaderHeight = Style.FontSize / Camera.Zoom;
	const float PinRadius = (Style.FontSize - Style.Nodes.Pins.Padding - 2.0f * Style.Nodes.Pins.BorderThickness) * 0.5f / Camera.Zoom;
	const ImVec2 Padding = { Style.Grid.Lines.Padding, Style.Grid.Lines.Padding };
	const ImVec2 CanvasMin = ImGui::GetWindowPos() + ImGui::GetWindowContentRegionMin();
	const ImVec2 CanvasMax = ImGui::GetWindowPos() + ImGui::GetWindowContentRegionMax();
	const ImVec2 CanvasCenter = (CanvasMin + CanvasMax) * 0.5f;
	const ImVec2 Offset = ImVec2(HeaderHeight, HeaderHeight) * 0.5f;
	const ImVec2 Radius = ImVec2(PinRadius, 0);

	const PinPtr& In = a.Input ? a : b;
	const PinPtr& Out = a.Input ? b : a;

	const Node& OutNode = *Nodes[Out.Node];
	const ImVec2 OutSize = OutNode.Info.Size / Camera.Zoom;
	const ImVec2 OutPosition = CanvasCenter - Camera.Location + OutNode.Position / Camera.Zoom + Padding;
	const ImVec2 OutputLoc = OutPosition + ImVec2(OutSize.x - HeaderHeight - Style.Nodes.Pins.Padding / Camera.Zoom, HeaderHeight);

	const Node& InNode = *Nodes[In.Node];
	const ImVec2 InPosition = CanvasCenter - Camera.Location + InNode.Position / Camera.Zoom + Padding;
	const ImVec2 InputLoc = InPosition + ImVec2(Style.Nodes.Pins.Padding / Camera.Zoom, HeaderHeight);

	const ImVec2 A = OutputLoc + ImVec2(0, HeaderHeight) * (static_cast<float>(Out.Pin) + 0.5f) + Offset + Radius;
	const ImVec2 D = InputLoc  + ImVec2(0, HeaderHeight) * (static_cast<float>(In.Pin)  + 0.5f) + Offset - Radius;
	const float Off = BezierOffset(A, D);

	const ImVec2 B = ImVec2(A.x + Off, A.y);
	const ImVec2 C = ImVec2(D.x - Off, D.y);

	const Pin& OutPin = OutNode.IO.Outputs[Out.Pin];

	// Draw =============================================================================================================

	DrawList.AddBezierCubic(
		A, B, C, D
	,	Pin::Colors[OutPin.Type], Style.Nodes.Pins.Connections.Thickness / Camera.Zoom
	);
}

void ShaderGraph::StartConnection(const PinPtr& ptr)
{
	Mouse.NewConnection = ptr;
}

void ShaderGraph::StopConnection()
{
	Mouse.NewConnection.Reset();
}

void ShaderGraph::CreateConnection(const PinPtr& a, const PinPtr& b)
{
	// Check if connection is valid
	if(a.Input == b.Input) return;
	if(a.Node  == b.Node) return;

	const PinPtr& In     = a.Input ? a : b;
	const Node&   InNode = *Nodes[In.Node];
	const Pin&    InPin  = InNode.IO.Inputs[In.Pin];

	const PinPtr& Out     = a.Input ? b : a;
	const Node&   OutNode = *Nodes[Out.Node];
	const Pin&    OutPin  = OutNode.IO.Outputs[Out.Pin];

	// Make sure valid typing
	if(InPin.Type != OutPin.Type && InPin.Type != Pin::ANY && OutPin.Type != Pin::ANY) return;

	// Break Input Connection
	if(a.Input) EraseConnections(a);
	if(b.Input) EraseConnections(b);

	// Add New Connections
	Connections.emplace(a, b);
	Connections.emplace(b, a);
}

void ShaderGraph::EraseConnection(const PinPtr& a, const PinPtr& b)
{
	auto range = Connections.equal_range(a);
	for(auto it = range.first; it != range.second; ++it)
	{
		if(it->second == b)
		{
			Connections.erase(it);
			break;
		}
	}

	range = Connections.equal_range(b);
	for(auto it = range.first; it != range.second; ++it)
	{
		if(it->second == a)
		{
			Connections.erase(it);
			break;
		}
	}
}

void ShaderGraph::EraseConnections(const PinPtr& a)
{
	auto it = Connections.find(a);
	while(it != Connections.end())
	{
		auto range = Connections.equal_range(it->second);
		for(auto match = range.first; match != range.second; ++match)
		{
			if(match->second == a)
			{
				Connections.erase(match);
				break;
			}
		}

		Connections.erase(it);
		it = Connections.find(a);
	}
}

NodeId ShaderGraph::AddNode(Node* node)
{
	if(Erased.empty())
	{
		Nodes.push_back(node);
		return Nodes.size() - 1;
	}

	NodeId id = *Erased.begin();
	Nodes[id] = node;
	Erased.erase(id);
	return id;
}

void ShaderGraph::RemoveNode(NodeId id)
{
	Node* node = Nodes[id];

	PinId i = 0;
	for(const auto& pin : node->IO.Inputs) EraseConnections({ id, i++, true });

	i = 0;
	for(const auto& pin : node->IO.Outputs) EraseConnections({ id, i++, false });

	Erased.insert(id);
	delete node;
	Nodes[id] = nullptr;
}

void ShaderGraph::ClearClipboard()
{
	for(auto node : Clipboard.Nodes) delete node;
	Clipboard.Nodes.clear();
	Clipboard.Connections.clear();
}

void ShaderGraph::Copy()
{
	// Helper for connections
	std::unordered_map<NodeId, NodeId> clipboardTransform;
	ImVec2 min = Nodes[*Selected.begin()]->Position;

	// Reset Clipboard
	ClearClipboard();
	Clipboard.Nodes.reserve(Selected.size());

	// Copy nodes
	for(auto id : Selected)
	{
		Node* node = Nodes[id];
		clipboardTransform[id] = Clipboard.Nodes.size();
		Clipboard.Nodes.push_back(node->Copy(*this));
		min = ImMin(node->Position, min);
	}

	// Offset nodes
	for(auto node : Clipboard.Nodes)
	{
		node->Position -= min;
	}

	// Copy connections
	for(const Connection& connection : Connections)
	{
		if(!(Selected.contains(connection.first.Node) && Selected.contains(connection.second.Node))) continue;

		Connection copy = {
			{ clipboardTransform[connection.first.Node], connection.first.Pin, connection.first.Input }
		,	{ clipboardTransform[connection.second.Node], connection.second.Pin, connection.second.Input }
		};

		Clipboard.Connections.insert(copy);
	}
}

void ShaderGraph::Paste(const ImVec2& location)
{
	// Helper for connections
	const float GridSize = (Style.FontSize + Style.Grid.Lines.Padding);
	std::unordered_map<NodeId, NodeId> clipboardTransform;
	ImVec2 root = ImFloor(location / GridSize + ImVec2(0.5f, 0.5f)) * GridSize;

	NodeId id = 0;
	for(Node* node : Clipboard.Nodes)
	{
		node->Position += root;
		clipboardTransform[id++] = AddNode(node->Copy(*this));
	}

	for(const Connection& connection : Clipboard.Connections)
	{
		CreateConnection(
			{ clipboardTransform[connection.first.Node], connection.first.Pin, connection.first.Input }
		,	{ clipboardTransform[connection.second.Node], connection.second.Pin, connection.second.Input }
		);
	}
}

void ShaderGraph::EraseSelection()
{
	for(auto node : Selected)
	{
		RemoveNode(node);
	}
	Selected.clear();
}

float ShaderGraph::BezierOffset(const ImVec2& out, const ImVec2& in)
{
	const float HeaderHeight = Style.FontSize / Camera.Zoom;
	const float diff_x = out.x - in.x;
	const float diff_y = out.y - in.y;
	return abs(diff_x) * (abs(diff_y) / abs(diff_x)) * (1 + glm::max(diff_x, 0.0f) / (HeaderHeight + abs(diff_y)));
}

bool ShaderGraph::AABB(const ImVec2& a0, const ImVec2& a1, const ImVec2& b0, const ImVec2& b1)
{
	bool X = glm::max(a0.x, a1.x) >= glm::min(b0.x, b1.x)
		  && glm::max(b0.x, b1.x) >= glm::min(a0.x, a1.x);

	bool Y = glm::max(a0.y, a1.y) >= glm::min(b0.y, b1.y)
		  && glm::max(b0.y, b1.y) >= glm::min(a0.y, a1.y);

	return X && Y;
}

void ShaderGraph::Register(const std::filesystem::path& path, ConstructorPtr constructor)
{
	const std::string name = path.filename().string();

	std::stack<std::string> decomp;
	std::filesystem::path current = path.parent_path();
	while(current.empty() == false)
	{
		decomp.push(current.filename().string());
		current = current.parent_path();
	}

	ContextID node = 0;
	while(decomp.empty() == false)
	{
		ContextID child = ContextMenu.FirstChild(node);

		while(child)
		{
			if(ContextMenu[child].Name == decomp.top())
			{
				node = child;
				decomp.pop();
				break;
			}

			child = ContextMenu.NextSibling(child);
		}

		if(node == 0 || node != child)
		{
			node = ContextMenu.Insert({ decomp.top(), nullptr }, node);
			decomp.pop();
		}
	}

	ContextMenu.Insert({ name, constructor }, node);
}
