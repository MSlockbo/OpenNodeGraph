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

static ShaderGraph* GCurrentGraph = nullptr;

static bool ValidateConnection(ImPinPtr a, ImPinPtr b)
{
    ShaderGraph& Graph = *GCurrentGraph;

    bool result = false;
    result |= Graph.FindNode(a)->CheckConnection(&Graph.FindPin(a), &Graph.FindPin(b));
    result |= Graph.FindNode(b)->CheckConnection(&Graph.FindPin(b), &Graph.FindPin(a));

    return result;
}



ImColor operator*(const ImColor& c, float f)
{
	return ImVec4(c.Value.x * f, c.Value.y * f, c.Value.z * f, c.Value.w);
}

GraphState::GraphState(ShaderGraph& parent)
	: Parent(parent)
{
}

GraphState::GraphState(const GraphState& other)
	: Parent(other.Parent)
	, Nodes(other.Nodes)
{
    for(Node*& node : Nodes)
    {
        node = node->Copy(Parent);
    }
}

GraphState::~GraphState()
{
	for(Node* node : Nodes)
	{
		if(node) delete node;
	}
}

GraphState& GraphState::operator=(const GraphState& other)
{
    Nodes = other.Nodes;

    for(Node*& node : Nodes) if(node) node = node->Copy(Parent);

    return *this;
}

Node::Node(ShaderGraph& graph, ImVec2 pos)
	: Graph(graph)
    , Position(pos)
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

void Node::DrawPin(int id, Pin& pin, ImPinDirection direction)
{
    ImPinFlags flags = 0;
    if(pin.Flags & PinFlags_NoPadding) flags |= ImPinFlags_NoPadding;

    bool res = ImNodeGraph::BeginPin(id, pin.Type, direction, flags);
    pin.Ptr = ImNodeGraph::GetPinPtr();
    if(res)
    {
        const ImVector<ImGuiID>&  connections  = ImNodeGraph::GetConnections();
        const ImVector<ImPinPtr>& new_conns    = ImNodeGraph::GetNewConnections();

        if(pin.Flags & PinFlags_Ambiguous)
        {
            if(connections.size() == new_conns.size() && new_conns.size() > 0)
            {
                Pin& first = Graph.FindPin(new_conns.front());
                if(first.Type != PinType_Any) pin.Type = first.Type;
            }

            if(connections.size() == 0) pin.Type = PinType_Any;
        }

        ValidateConnections();
    }

    const bool connected      = ImNodeGraph::IsPinConnected();
    const bool any            = pin.Type == PinType_Any;
    const bool force_collapse = pin.Flags & PinFlags_AlwaysCollapse;
    const bool no_collapse    = pin.Flags & PinFlags_NoCollapse;

    if((connected || any || direction || force_collapse) && !no_collapse)
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
            ImGui::ColorPicker3(
                std::format("##in{}{}", pin.Name, id).c_str(), &pin.Value.get<glm::vec3>().x
            ,   ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float
            );

            ImNodeGraph::PushItemWidth(150.0f);
            ImGui::ColorPreview3(
                std::format("##invec{}{}", pin.Name, id).c_str(), &pin.Value.get<glm::vec3>().x
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

    ImGuiID pid = 0;
    for(Pin& pin : IO.Inputs)  DrawPin(++pid, pin, ImPinDirection_Input);

    ImVec2 cursor = ImGui::GetCursorPos();

    pid = 0;
    for(Pin& pin : IO.Outputs) DrawPin(--pid, pin, ImPinDirection_Output);

    ImGui::SetCursorPos(cursor);

    //if(IO.DynamicInputs)
    //{
    //    ImGui::Text("\uEA11");
    //}

    ImNodeGraph::EndNode();
}

ShaderGraph::ShaderGraph()
	: EditorWindow("\uED46 Shader Graph", ImGuiWindowFlags_MenuBar)
    , GrabFocus_(false)
	, Shader_(nullptr)
{
}

ShaderGraph::~ShaderGraph()
{
}

void ShaderGraph::OnOpen()
{
	EditorSystem::Open<Inspector>()->Graph = this;

    GrabFocus_ = true;
}

void ShaderGraph::DrawMenu()
{
    if(ImGui::MenuItem("\uf455 Compile"))
    {
        ImNodeGraph::BeginGraphPostOp("ShaderGraph");

        Shader_->Compile();
        Console::Log(Console::Message, "{}", Shader_->GetCode());

        ImNodeGraph::EndGraphPostOp();
    }
}


void ShaderGraph::DrawWindow()
{
    GCurrentGraph = this;

    if(Shader_ == nullptr) return;

    ImNodeGraph::BeginGraph("ShaderGraph");
    ImNodeGraph::SetPinColors(Pin::Colors);

    ImNodeGraph::SetGraphValidation(ValidateConnection);

    if(GrabFocus_)
    {
        GrabFocus_ = false;
        ImGui::SetWindowFocus();
        ImGui::SetNavWindow(ImGui::GetCurrentWindow());
    }

    GraphState& State = Shader_->GetState();
    for(ImGuiID id = 0; id < State.Nodes.size(); ++id)
    {
        if(State.Nodes(id) == false) continue;

        State.Nodes[id]->Draw(id);
    }

    DrawContextMenu();

    ImNodeGraph::EndGraph();


    ImNodeGraph::BeginGraphPostOp("ShaderGraph");

    if(ImGui::IsKeyPressed(ImGuiKey_Delete)) Erase();

    ImGuiIO& io = ImGui::GetIO();

    if(!(io.KeyMods & ~ImGuiMod_Ctrl))
    {
        if(ImGui::IsKeyPressed(ImGuiKey_C)) Copy();
        if(ImGui::IsKeyPressed(ImGuiKey_P)) Paste(ImGui::GetMousePos());
        if(ImGui::IsKeyPressed(ImGuiKey_X)) { Copy(); Erase(); }
    }

    ImNodeGraph::EndGraphPostOp();
}


void ShaderGraph::DrawContextMenu()
{
    ContextMenuHierarchy& ContextMenu = ShaderGraph::ContextMenu();
    
    if(ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    {
        ContextMenuPosition_ = ImNodeGraph::ScreenToGrid(ImGui::GetMousePos());
    }

    if(ImGui::BeginPopupContextWindow("graph_context"))
    {
        if(ImGui::MenuItem("Copy", "Ctrl+C", false, false)) Copy();
        if(ImGui::MenuItem("Cut", "Ctrl+X", false, false))
        {
            Copy();
            Erase();
        }
        if(ImGui::MenuItem("Paste", "Ctrl+V", false, false)) Paste(ContextMenuPosition_);

        ImGui::Separator();

        ImGui::Text("Create");

        ImGui::Separator();

        // Create Nodes
        ImVec2 position = ContextMenuPosition_;

        struct Visitor
        {
            bool operator()(ContextMenuItem& item, ContextID id)
            {
                ContextMenuHierarchy& ContextMenu = ShaderGraph::ContextMenu();
                const auto depth = ContextMenu.depth(id);
                if(depth > Context.size()) return false;

                while(depth < Context.size())
                {
                    Context.pop();
                    ImGui::EndMenu();
                }

                if(Context.top() != ContextMenu.parent(id)) return false;
                std::string name = std::format("{}##{}", item.Name, id);

                if(item.Constructor)
                {
                    if(ImGui::MenuItem(item.Name.c_str()))
                    {
                        Graph.Shader_->GetState().AddNode(item.Constructor(Graph, Location));
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
            std::stack<ContextID> Context;
        } MenuVisitor
        {
            .Graph = *this
        ,	.Location = position
        };

        MenuVisitor.Context.push(0);

        ContextMenu.traverse<ContextMenuHierarchy::pre_order>(MenuVisitor);

        MenuVisitor.Context.pop();
        while(MenuVisitor.Context.empty() == false)
        {
            ImGui::EndMenu();
            MenuVisitor.Context.pop();
        }

        ImGui::EndPopup();
    }
}

void ShaderGraph::Copy()
{
    
}

void ShaderGraph::Erase()
{
    GraphState& State = Shader_->GetState();
    auto& Selected = ImNodeGraph::GetSelected();
    for(ImGuiID node : Selected)
    {
        State.RemoveNode(ImNodeGraph::GetUserID(node).Int);
    }
    Selected.Clear();
}

void ShaderGraph::Paste(ImVec2)
{
    
}

void ShaderGraph::Clear()
{
    
}

Node* ShaderGraph::FindNode(ImPinPtr ptr)
{
    return Shader_->GetState().Nodes[ImNodeGraph::GetUserID(ptr.Node).Int];
}

Pin& ShaderGraph::FindPin(ImPinPtr ptr)
{
    Node* node = Shader_->GetState().Nodes[ImNodeGraph::GetUserID(ptr.Node).Int];
    auto& pins = ptr.Direction ? node->IO.Outputs : node->IO.Inputs;
    int idx = ImNodeGraph::GetUserID(ptr).Int;
    if(ptr.Direction) idx *= -1;
    idx -= 1;
    return pins[idx];
}

std::string ShaderGraph::GetValue(ImPinPtr ptr)
{
    if(ptr.Direction == ImPinDirection_Output)
    {
        NodeId node_id = ImNodeGraph::GetUserID(ptr.Node).Int;
        Node* node = Shader_->GetState().Nodes[node_id];
    
        NodeId pin_id = ImNodeGraph::GetUserID(ptr).Int;
        Pin& pin = FindPin(ptr);
    
        return std::format("{}{}_{}", node->Info.Alias, pin.Name, node_id);
    }
    else
    {
        const ImVector<ImGuiID>& connections = ImNodeGraph::GetConnections(ptr);

        // L-Value
        if(connections.empty())
        {
            Pin& pin = FindPin(ptr);

            switch(pin.Type)
            {
            case PinType_UInt:   return std::to_string(pin.Value.get<unsigned int>());
            case PinType_Int:    return std::to_string(pin.Value.get<int>());
            case PinType_Float:  return std::to_string(pin.Value.get<float>());
            case PinType_Vector:
            {
                const glm::vec3& val = pin.Value.get<glm::vec3>();
                return std::format("vec3({},{},{})", val.x, val.y, val.z);
            }
            default: return "0";
            }
        }

        // Variable
        ImPinConnection connection = ImNodeGraph::GetConnection(connections[0]);
        if(connection.A == ptr) return GetValue(connection.B);
        return GetValue(connection.A);
    }
}

void ShaderGraph::Register(const std::filesystem::path& path, ConstructorPtr constructor)
{
	const std::string name = path.filename().string();
    ContextMenuHierarchy& ContextMenu = ShaderGraph::ContextMenu();

	ContextID node = 0;
	for(auto it = path.begin(); it != path.end();)
	{
		ContextID child = ContextMenu.first_child(node);

		while(child)
		{
			if(ContextMenu[child].Name == it->string())
			{
				node = child;
			    ++it;
				break;
			}

			child = ContextMenu.next_sibling(child);
		}

		if(node == 0 || node != child)
		{
			node = ContextMenu.insert({ it->string(), nullptr }, node);
			++it;
		}
	}

	ContextMenu[node].Constructor = constructor;
}

Inspector::Inspector()
	: EditorWindow("Inspector", 0)
	, Graph(nullptr)
{
}

void Inspector::DrawWindow()
{
}