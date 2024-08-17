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


#include <filesystem>
#include <stack>

#include <Core/Console.h>
#include <Editor/EditorSystem.h>

#include <Graph/ShaderGraph.h>

#include <imgui-docking/imgui_internal.h>

#include "imgui-extras/imgui_extras.h"


using namespace OpenShaderDesigner;

ImColor operator*(const ImColor& c, float f)
{
	return ImVec4(c.Value.x * f, c.Value.y * f, c.Value.z * f, c.Value.w);
}

ShaderGraph::GraphState::GraphState(ShaderGraph& parent)
	: Parent(parent)
{
}

ShaderGraph::GraphState::GraphState(const GraphState& other)
	: Parent(other.Parent)
	, Nodes(other.Nodes)
{
    for(Node*& node : Nodes)
    {
        node = node->Copy(Parent);
    }
}

ShaderGraph::GraphState::~GraphState()
{
	for(Node* node : Nodes)
	{
		if(node) delete node;
	}
}

ShaderGraph::GraphState& ShaderGraph::GraphState::operator=(const GraphState& other)
{
    Nodes = other.Nodes;

    for(Node*& node : Nodes) if(node) node = node->Copy(Parent);

    return *this;
}

Node::Node(
	ShaderGraph& graph, ImVec2 pos)
	: Position(pos)
	, Header
	{
		.Title        = "Node"
	,	.Color        = ImColor(0xA7, 0x62, 0x53)
	,   .HoveredColor = ImColor(0xC5, 0x79, 0x67)
	,   .ActiveColor  = ImColor(0x82, 0x4C, 0x40)
	,	.Enabled      = true
	}
	, IO
	{
		.DynamicInputs = false
	}
	, Info
	{
		.Const = false
	}
{ }

void Node::DrawPin(ImGuiID id, Pin& pin, ImPinDirection direction)
{
    ImPinFlags flags = 0;
    if(pin.Flags & PinFlags_NoPadding) flags |= ImPinFlags_NoPadding;

    ImNodeGraph::BeginPin(std::format("{}##{}", pin.Name, id).c_str(), pin.Type, direction, flags);

    bool connected = ImNodeGraph::IsPinConnected();

    if((connected || pin.Type == PinType_Any) && !(pin.Flags & PinFlags_NoCollapse))
    {
        ImGui::Text(pin.Name.c_str());
    }
    else
    {
        switch (pin.Type)
        {
        case PinType_Int:
            ImNodeGraph::PushItemWidth(200.0f);
            ImGui::InputInt(std::format("##in{}{}", pin.Name, id).c_str(), pin.Value); break;
        case PinType_UInt:
            ImNodeGraph::PushItemWidth(200.0f);
            ImGui::InputUInt(std::format("##in{}{}", pin.Name, id).c_str(), pin.Value); break;
        case PinType_Float:
            ImNodeGraph::PushItemWidth(100.0f);
            ImGui::InputFloat(std::format("##in{}{}", pin.Name, id).c_str(), pin.Value); break;
        case PinType_Vector:
            ImGui::BeginGroup();

            // Color Picker
            ImNodeGraph::PushItemWidth(150.0f);
            ImGui::ColorPicker4(
                std::format("##in{}{}", pin.Name, id).c_str(), &pin.Value.get<ImVec4>().x
            ,   ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float
            );

            ImNodeGraph::PushItemWidth(150.0f);
            ImGui::ColorPreview(
                std::format("##invec{}{}", pin.Name, id).c_str(), &pin.Value.get<ImVec4>().x
            ,   ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float
            );

            ImGui::EndGroup();
            break;
        }
    }

    ImNodeGraph::EndPin();
}

void Node::Draw(ImGuiID id)
{
    ImNodeGraph::BeginNode(id, Position);

    if(Header.Enabled)
    {
        ImNodeGraph::BeginNodeHeader(id, Header.Color, Header.HoveredColor, Header.ActiveColor);

        ImGui::Text(Header.Title.c_str());

        ImNodeGraph::EndNodeHeader();
    }

    for(Pin& pin : IO.Inputs)  DrawPin(id, pin, ImPinDirection_Input);
    for(Pin& pin : IO.Outputs) DrawPin(id, pin, ImPinDirection_Output);

    ImNodeGraph::EndNode();
}

ShaderGraph::ShaderGraph()
	: EditorWindow("\uED46 Shader Graph", 0)
    , GrabFocus(false)
	, State(*this)
{
}

ShaderGraph::~ShaderGraph()
{
}

void ShaderGraph::OnOpen()
{
	EditorSystem::Open<Inspector>()->Graph = this;

    GrabFocus = true;
}


void ShaderGraph::DrawWindow()
{
    ImNodeGraph::BeginGraph("ShaderGraph");
    ImNodeGraph::SetPinColors(Pin::Colors);

    if(GrabFocus)
    {
        GrabFocus = false;
        ImGui::SetWindowFocus();
        ImGui::SetNavWindow(ImGui::GetCurrentWindow());
    }

    for(ImGuiID id = 0; id < State.Nodes.size(); ++id)
    {
        if(State.Nodes(id) == false) continue;

        State.Nodes[id]->Draw(id);
    }

    DrawContextMenu();

    ImNodeGraph::EndGraph();

}


void ShaderGraph::DrawContextMenu()
{
    if(ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    {
        ContextMenuPosition = ImNodeGraph::ScreenToGrid(ImGui::GetMousePos());
    }

    if(ImGui::BeginPopupContextWindow())
    {
        if(ImGui::MenuItem("Copy", "Ctrl+C", false, false)) Copy();
        if(ImGui::MenuItem("Cut", "Ctrl+X", false, false))
        {
            Copy();
            Erase();
        }
        if(ImGui::MenuItem("Paste", "Ctrl+V", false, false)) Paste(ContextMenuPosition);

        ImGui::Separator();

        ImGui::Text("Create");

        ImGui::Separator();

        // Create Nodes
        ImVec2 position = ContextMenuPosition;

        std::stack<ContextID> context; context.push(0);

        struct Visitor
        {
            bool operator()(ContextMenuItem& item, ContextID id)
            {
                const auto depth = Graph.ContextMenu.depth(id);
                if(depth > Context.size()) return false;

                while(depth < Context.size())
                {
                    Context.pop();
                    ImGui::EndMenu();
                }

                if(Context.top() != Graph.ContextMenu.parent(id)) return false;
                std::string name = std::format("{}##{}", item.Name, id);

                if(item.Constructor)
                {
                    if(ImGui::MenuItem(item.Name.c_str()))
                    {
                        Graph.State.Nodes.insert(item.Constructor(Graph, Location));
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

        ContextMenu.traverse<ContextMenuHierarchy::pre_order>(MenuVisitor);

        context.pop();
        while(context.empty() == false)
        {
            ImGui::EndMenu();
            context.pop();
        }

        ImGui::EndPopup();
    }
}

void ShaderGraph::Copy() {}
void ShaderGraph::Erase() {}
void ShaderGraph::Paste(ImVec2) {}

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
		ContextID child = ContextMenu.first_child(node);

		while(child)
		{
			if(ContextMenu[child].Name == decomp.top())
			{
				node = child;
				decomp.pop();
				break;
			}

			child = ContextMenu.next_sibling(child);
		}

		if(node == 0 || node != child)
		{
			node = ContextMenu.insert({ decomp.top(), nullptr }, node);
			decomp.pop();
		}
	}

	ContextMenu.insert({ name, constructor }, node);
}

Inspector::Inspector()
	: EditorWindow("Inspector", 0)
	, Graph(nullptr)
{
}

void Inspector::DrawWindow()
{
    /*
	if(Graph->Mouse.Selected.size() != 1)
	{
		ImGui::Text("Selected %d nodes.", Graph->Mouse.Selected.size());
		return;
	}

	Graph->State.Nodes[*Graph->Mouse.Selected.begin()]->Inspect();
	*/
}


/** DEPRECATED
void ShaderGraph::DrawWindow()
{
	HandleInput();
	DrawContextMenu();

	DrawGrid();


	NodeId uid = 0;
	for(Node* node : State.Nodes)
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
			Mouse.Selected.clear();
		}

		Mouse.Locks.clear();
		Mouse.FocusedNode.reset();
		Mouse.DragSelect.clear();
		Mouse.LocksDragged = false;
	}
}


void ShaderGraph::HandleInput()
{

	Focused = ImGui::IsWindowFocused();

	// Calculate Mouse Delta
	Mouse.ClickedSomething = false;
	Mouse.Location = ScreenToGrid(Mouse.ScreenLocation);
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

	// ImGui Vars
	ImDrawList& DrawList = *ImGui::GetWindowDrawList();

	// Draw Vars
	const float HeaderHeight    = Style.FontSize;
	const ImVec2 Padding        = { Style.Grid.Lines.Padding, Style.Grid.Lines.Padding };
	const ImVec2 NodePos        = node.Position + Padding;
	const ImVec2 NodeRoot       = GridToScreen(NodePos);
	const ImVec2 NodeEdge       = GridToScreen(NodePos + node.Info.Size);
	const ImVec2 HeaderEdge     = GridToScreen(NodePos + ImVec2(node.Info.Size.x, HeaderHeight));
	const ImVec2 HeaderText     = GridToScreen(NodePos + ImVec2(Style.Nodes.Rounding, 0) + Padding * 0.5f);
	const ImVec2 InputRoot      = GridToScreen(NodePos + ImVec2(Style.Nodes.Pins.Padding, HeaderHeight));
	const ImVec2 OutputRoot     = GridToScreen(NodePos + ImVec2(node.Info.Size.x - HeaderHeight - Style.Nodes.Pins.Padding, HeaderHeight));
	const bool HasLock          = Mouse.FocusedNode();
	const bool NodeHovered      = ImGui::IsMouseHoveringRect(NodeRoot, NodeEdge);
	const bool HeaderHovered    = ImGui::IsMouseHoveringRect(NodeRoot, HeaderEdge);
	const ImColor HeaderColor   = node.Header.Color * (HeaderHovered || HasLock ? 1.2f : 1.0f) * (HasLock ? 0.8f : 1.0f);
	const float Rounding        = Style.Nodes.Rounding / Camera.Zoom;
	const float PinSpacing      = HeaderHeight / Camera.Zoom;
	const float BorderThickness = Style.Nodes.Border.Thickness / Camera.Zoom;

	const bool Ctrl = ImGui::IsKeyDown(ImGuiKey_ModCtrl);
	const bool Shift = ImGui::IsKeyDown(ImGuiKey_ModShift);

	if(!AABB(CanvasMin, CanvasMax, NodeRoot, NodeEdge)) return;

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

				for(NodeId selected : Mouse.Selected)
				{
					Mouse.Locks.emplace(selected, Mouse.Location - State.Nodes[selected]->Position);
				}
			}
		}
	}

	// Drag selection
	if(ImGui::IsMouseDragging(ImGuiMouseButton_Left) && !Mouse.NewConnection() && Focused)
	{
		// Reset When Dragging a New Node
		if(Mouse.FocusedNode() && Mouse.FocusedNode == id && !Mouse.Selected.contains(id))
		{
			if(!(Ctrl || Shift))
			{
				Mouse.Selected.clear();
				Mouse.Locks.clear();
			}
			Mouse.Selected.insert(id);
		}

		// Begin Dragging
		if(Mouse.FocusedNode() && Mouse.Selected.contains(id) && !(Ctrl || Shift))
		{
			if(Mouse.LocksDragged == false) PushState();

			Mouse.LocksDragged = true;

			for(NodeId selected : Mouse.Selected)
			{
				Mouse.Locks.emplace(selected, Mouse.Location - State.Nodes[selected]->Position);
			}
		}

		// Make sure we aren't dragging nodes
		if(Mouse.LocksDragged == false)
		{
			// Calculate selection bounds
			ImVec2 Drag = ImGui::GetMouseDragDelta() * Camera.Zoom;
			ImVec2 A0 = Mouse.Location - Drag;
			ImVec2 A1 = Mouse.Location;
			ImVec2 B0 = NodePos;
			ImVec2 B1 = NodePos + node.Info.Size;
			bool Intersect = AABB(A0, A1, B0, B1);

			// Clear selection for new selection
			if(Mouse.DragSelect.empty() && !Shift && !Ctrl && !Mouse.FocusedNode())
			{
				Mouse.Selected.clear();
			}

			// Select nodes
			if(Intersect && !Mouse.DragSelect.contains(id))
			{
				if(!Ctrl)
				{
					if(!Mouse.Selected.contains(id))
					{
						Mouse.Selected.insert(id);
						Mouse.DragSelect.insert(id);
					}
				}
				else
				{
					if(Mouse.Selected.contains(id)) Mouse.Selected.erase(id);
					else Mouse.Selected.insert(id);

					Mouse.DragSelect.insert(id);
				}
			}

			// Unselect nodes
			if(!Intersect && Mouse.DragSelect.contains(id))
			{
				Mouse.DragSelect.erase(id);

				if(!Ctrl)
				{
					Mouse.Selected.erase(id);
				}
				else
				{
					if(Mouse.Selected.contains(id)) Mouse.Selected.erase(id);
					else Mouse.Selected.insert(id);
				}
			}
		}
	}

	if(ImGui::IsMouseReleased(ImGuiMouseButton_Left) && HeaderHovered)
	{
		if(Mouse.Selected.contains(id))
		{
			if(Ctrl) Mouse.Selected.erase(id);
			else if(!Shift && !Mouse.LocksDragged)
			{
				Mouse.Selected.clear();
				Mouse.Selected.insert(id);
			}
		}
		else
		{
			if(!(Ctrl || Shift))
				Mouse.Selected.clear();
			Mouse.Selected.insert(id);
		}
	}

	if(Mouse.Locks.contains(id))
	{
		node.Position = Mouse.Location - Mouse.Locks[id];
		node.Position = SnapToGrid(node.Position);
	}

	// Content =========================================================================================================

	DrawList.AddRectFilled(NodeRoot, NodeEdge, Style.Nodes.Content, Rounding + 1);

	// Header ==========================================================================================================

	if(node.Header.Enabled)
	{
		DrawList.PushClipRect(NodeRoot, HeaderEdge, true);
		DrawList.AddRectFilled(NodeRoot, NodeEdge, HeaderColor, Rounding + 1);
		DrawList.AddText(NULL, Style.FontSize / Camera.Zoom, HeaderText, Style.Nodes.Title, node.Header.Title.c_str());
		DrawList.PopClipRect();

		DrawList.AddLine(ImVec2(NodeRoot.x, HeaderEdge.y), HeaderEdge, Style.Nodes.Border.Color, BorderThickness);
	}

	// Border ==========================================================================================================

	if(Mouse.Selected.contains(id)) DrawList.AddRect(NodeRoot, NodeEdge, Style.Nodes.SelectedBorder.Color, Rounding, 0, Style.Nodes.SelectedBorder.Thickness / Camera.Zoom);
	else DrawList.AddRect(NodeRoot, NodeEdge, Style.Nodes.Border.Color, Rounding, 0, BorderThickness);

	// Pins ============================================================================================================

	int i = 0;
	for(Pin& pin : node.IO.Inputs)
	{
		DrawPin(id, pin, i, InputRoot + ImVec2(0, PinSpacing) * (i + 0.5f), true);
		++i;
	}

	int j = 0;
	for(Pin& pin : node.IO.Outputs)
	{
		DrawPin(id, pin, j, OutputRoot + ImVec2(0, PinSpacing) * (j + 0.5f), false);
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
	const ImVec2 Offset = ImVec2(HeaderHeight, HeaderHeight) * 0.5f;
	const ImVec2 PinRoot = location;
	const ImVec2 PinEdge = PinRoot + Offset * 2.0f;
	const ImVec2 PinCenter = PinRoot + Offset;
	const float PinPadding = Style.Nodes.Pins.Padding / Camera.Zoom;
	const float BorderThickness = Style.Nodes.Pins.BorderThickness / Camera.Zoom;
	const float PinRadius = (HeaderHeight - PinPadding - 2.0f * BorderThickness) * 0.5f;
	const bool Hovered = ImGui::IsMouseHoveringRect(PinRoot, PinEdge);
	const bool MouseDown = ImGui::IsMouseDown(ImGuiMouseButton_Left);
	const bool MouseClicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left);

	// Pin =============================================================================================================

	// Input
	if(MouseClicked && Hovered && !Mouse.NewConnection() && !ImGui::IsKeyDown(ImGuiKey_ModAlt))
	{
		StartConnection({ node_id, pin_id, input });
	}

	// Circle
	PinPtr ptr = { node_id, pin_id, input };
	auto it = State.Connections.find(ptr);
	const bool Connected         = (it != State.Connections.end());
	const bool NewConnectionRoot = Mouse.NewConnection() && *Mouse.NewConnection == ptr;
	const bool NewConnectionNext = Mouse.NewConnection() && *Mouse.NewConnection != ptr && Hovered;
	const bool Pressed           = Hovered && MouseDown;
	const bool Filled            = Hovered || Connected || NewConnectionRoot || NewConnectionNext;
	ImColor pinColor             = Pin::Colors[pin.Type];
	ImColor fillColor            = Style.Nodes.Pins.Background;

	if(input)
	{
		if(Connected)              pinColor = Pin::Colors[GetPin(it->second).Type];
		else if(NewConnectionNext)
		{
			Pin& Next = GetPin(*Mouse.NewConnection);
			if(pin.Type == Next.Type || pin.Type == Pin::ANY) pinColor = Pin::Colors[Next.Type];
		}
	}

	if(Pressed && !NewConnectionNext) pinColor = pinColor * 0.8f;
	if(Filled)  fillColor = pinColor;

	DrawList.AddCircleFilled(PinCenter, PinRadius, fillColor);
	DrawList.AddCircle(PinCenter, PinRadius, pinColor, 0, BorderThickness);

	// Text
	const ImVec2 TextOffset = location + ImVec2((input ? HeaderHeight : -ImGui::CalcTextSize(pin.Name.c_str()).x / Camera.Zoom), 0);
	DrawList.AddText(NULL, HeaderHeight, TextOffset, Style.Nodes.Pins.Text, pin.Name.c_str());

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

void ShaderGraph::DrawConnections()
{
	// Vars ============================================================================================================

	// ImGui Vars
	ImDrawList& DrawList = *ImGui::GetWindowDrawList();

	const float HeaderHeight = Style.FontSize / Camera.Zoom;
	const float PinRadius = (Style.FontSize - Style.Nodes.Pins.Padding - 2.0f * Style.Nodes.Pins.BorderThickness) * 0.5f / Camera.Zoom;
	const ImVec2 Padding = { Style.Grid.Lines.Padding, Style.Grid.Lines.Padding };
	const ImVec2 Offset = ImVec2(HeaderHeight, HeaderHeight) * 0.5f;

	// Connections =============================================================================================================

	for(const Connection& connection : State.Connections)
	{
		DrawConnection(connection.first, connection.second);
	}

	if(Mouse.NewConnection())
	{
		const Node& Node = *State.Nodes[Mouse.NewConnection->Node];
		const Pin&  Pin  = Mouse.NewConnection->Input ? Node.IO.Inputs[Mouse.NewConnection->Pin] : Node.IO.Outputs[Mouse.NewConnection->Pin];
		const auto Connection = State.Connections.find(*Mouse.NewConnection);
		const bool Connected  = Connection != State.Connections.end();

		const ImVec2 NodePos = Node.Position + Padding;
		const ImVec2 InputRoot = GridToScreen(NodePos + ImVec2(Style.Nodes.Pins.Padding, HeaderHeight));
		const ImVec2 OutputRoot = GridToScreen(NodePos + ImVec2(Node.Info.Size.x - HeaderHeight - Style.Nodes.Pins.Padding, HeaderHeight));
		const ImVec2 Root = (Mouse.NewConnection->Input ? InputRoot : OutputRoot) + ImVec2(0, HeaderHeight) * (Mouse.NewConnection->Pin + 0.5f);

		const ImVec2 PinLoc = Root + Offset + ImVec2(Mouse.NewConnection->Input ? -PinRadius : PinRadius, 0);
		const ImVec2 MouseLoc = ImGui::GetMousePos();

		const ImVec2 A = Mouse.NewConnection->Input ? MouseLoc : PinLoc;
		const ImVec2 D = Mouse.NewConnection->Input ? PinLoc   : MouseLoc;
		const float Off = BezierOffset(A, D);

		const ImVec2 B = ImVec2(A.x + Off, A.y);
		const ImVec2 C = ImVec2(D.x - Off, D.y);

		const ImColor Color = Mouse.NewConnection->Input && Connected
		                    ? Pin::Colors[GetPin(Connection->second).Type] : Pin::Colors[Pin.Type];

		DrawList.AddBezierCubic(
			A, B, C, D
		,	Color, Style.Nodes.Pins.Connections.Thickness / Camera.Zoom
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

	const Node& OutNode = *State.Nodes[Out.Node];
	const ImVec2 OutSize = OutNode.Info.Size / Camera.Zoom;
	const ImVec2 OutPosition = CanvasCenter - Camera.Location + OutNode.Position / Camera.Zoom + Padding;
	const ImVec2 OutputLoc = OutPosition + ImVec2(OutSize.x - HeaderHeight - Style.Nodes.Pins.Padding / Camera.Zoom, HeaderHeight);

	const Node& InNode = *State.Nodes[In.Node];
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
	Mouse.NewConnection.reset();
}

void ShaderGraph::CreateConnection(const PinPtr& a, const PinPtr& b)
{
	// Check if connection is valid
	if(a.Input == b.Input) return;
	if(a.Node  == b.Node) return;

	const PinPtr& In     = a.Input ? a : b;
	const Node&   InNode = *State.Nodes[In.Node];
	const Pin&    InPin  = InNode.IO.Inputs[In.Pin];

	const PinPtr& Out     = a.Input ? b : a;
	const Node&   OutNode = *State.Nodes[Out.Node];
	const Pin&    OutPin  = OutNode.IO.Outputs[Out.Pin];

	// Make sure valid typing
	if(InPin.Type != OutPin.Type && InPin.Type != Pin::ANY && OutPin.Type != Pin::ANY) return;

	// Break Input Connection
	if(a.Input) EraseConnections(a);
	if(b.Input) EraseConnections(b);

	// Add New Connections
	State.Connections.emplace(a, b);
	State.Connections.emplace(b, a);
}

void ShaderGraph::EraseConnection(const PinPtr& a, const PinPtr& b)
{
	auto range = State.Connections.equal_range(a);
	for(auto it = range.first; it != range.second; ++it)
	{
		if(it->second == b)
		{
			State.Connections.erase(it);
			break;
		}
	}

	range = State.Connections.equal_range(b);
	for(auto it = range.first; it != range.second; ++it)
	{
		if(it->second == a)
		{
			State.Connections.erase(it);
			break;
		}
	}
}

void ShaderGraph::EraseConnections(const PinPtr& a)
{
	auto it = State.Connections.find(a);
	while(it != State.Connections.end())
	{
		auto range = State.Connections.equal_range(it->second);
		for(auto match = range.first; match != range.second; ++match)
		{
			if(match->second == a)
			{
				State.Connections.erase(match);
				break;
			}
		}

		State.Connections.erase(it);
		it = State.Connections.find(a);
	}
}

NodeId ShaderGraph::AddNode(Node* node)
{
	if(State.Erased.empty())
	{
		State.Nodes.push_back(node);
		return static_cast<NodeId>(State.Nodes.size() - 1);
	}

	NodeId id = *State.Erased.begin();
	State.Nodes[id] = node;
	State.Erased.erase(id);
	return id;
}

void ShaderGraph::RemoveNode(NodeId id)
{
	Node* node = State.Nodes[id];
	if(node->Info.Const) return;

	PinId i = 0;
	for(const auto& pin : node->IO.Inputs) EraseConnections({ id, i++, true });

	i = 0;
	for(const auto& pin : node->IO.Outputs) EraseConnections({ id, i++, false });

	State.Erased.insert(id);
	delete node;
	State.Nodes[id] = nullptr;
}

void ShaderGraph::ClearClipboard()
{
	for(auto node : Clipboard.Nodes) delete node;
	Clipboard.Nodes.clear();
	Clipboard.Connections.clear();
}

void ShaderGraph::Copy()
{
	if(Mouse.Selected.empty()) return;

	// Helper for connections
	std::unordered_map<NodeId, NodeId> clipboardTransform;
	ImVec2 min = State.Nodes[*Mouse.Selected.begin()]->Position;

	// Reset Clipboard
	ClearClipboard();
	Clipboard.Nodes.reserve(Mouse.Selected.size());

	// Copy nodes
	for(auto id : Mouse.Selected)
	{
		Node* node = State.Nodes[id];
		clipboardTransform[id] = static_cast<NodeId>(Clipboard.Nodes.size());
		Clipboard.Nodes.push_back(node->Copy(*this));
		min = ImMin(node->Position, min);
	}

	// Offset nodes
	for(auto node : Clipboard.Nodes)
	{
		node->Position -= min;
	}

	// Copy connections
	for(const Connection& connection : State.Connections)
	{
		if(!(Mouse.Selected.contains(connection.first.Node) && Mouse.Selected.contains(connection.second.Node))) continue;

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
	ImVec2 root = SnapToGrid(location);
	Mouse.Selected.clear();

	// Paste the nodes
	NodeId id = 0;
	for(Node* node : Clipboard.Nodes)
	{
		NodeId index = clipboardTransform[id++] = AddNode(node->Copy(*this));
		State.Nodes[index]->Position += root;
		Mouse.Selected.insert(index);
	}

	// Paste the connections
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
	for(auto node : Mouse.Selected)
	{
		RemoveNode(node);
	}
	Mouse.Selected.clear();
}

void ShaderGraph::PushState()
{
	History.push(State);
}

void ShaderGraph::PopState()
{
	State = History.top();
	History.pop();
}

float ShaderGraph::BezierOffset(const ImVec2& out, const ImVec2& in)
{
	const float HeaderHeight = Style.FontSize / Camera.Zoom;
	const float diff_x = out.x - in.x;
	const float diff_y = out.y - in.y;
	return abs(diff_y) * (1 + glm::max(diff_x, 0.0f) / (HeaderHeight + abs(diff_y)));
}

bool ShaderGraph::AABB(const ImVec2& a0, const ImVec2& a1, const ImVec2& b0, const ImVec2& b1)
{
	bool X = glm::max(a0.x, a1.x) >= glm::min(b0.x, b1.x)
		  && glm::max(b0.x, b1.x) >= glm::min(a0.x, a1.x);

	bool Y = glm::max(a0.y, a1.y) >= glm::min(b0.y, b1.y)
		  && glm::max(b0.y, b1.y) >= glm::min(a0.y, a1.y);

	return X && Y;
}

ImVec2 ShaderGraph::GridToScreen(const ImVec2& position)
{
	// Window Info
	const ImVec2 CanvasCenter = ImGui::GetWindowPos()
	                          + (ImGui::GetWindowContentRegionMin() + ImGui::GetWindowContentRegionMax()) * 0.5f;
	return CanvasCenter - Camera.Location + position / Camera.Zoom;
}

ImVec2 ShaderGraph::ScreenToGrid(const ImVec2& position)
{
	const ImVec2 CanvasCenter = ImGui::GetWindowPos()
							  + (ImGui::GetWindowContentRegionMin() + ImGui::GetWindowContentRegionMax()) * 0.5f;
	return (position - CanvasCenter + Camera.Location) * Camera.Zoom;
}

ImVec2 ShaderGraph::SnapToGrid(const ImVec2& position)
{
	const float GridSize = (Style.FontSize + Style.Grid.Lines.Padding);
	return ImFloor(position / GridSize) * GridSize;
}

Pin& ShaderGraph::GetPin(const PinPtr &ptr)
{
	Node* node = State.Nodes[ptr.Node];
	return (ptr.Input ? node->IO.Inputs : node->IO.Outputs)[ptr.Pin];
}
*/