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
    const ShaderGraph& Graph = *GCurrentGraph;

    bool result = false;
    result |= Graph.FindNode(a)->CheckConnection(&Graph.FindPin(a), &Graph.FindPin(b));
    result |= Graph.FindNode(b)->CheckConnection(&Graph.FindPin(b), &Graph.FindPin(a));

    return result;
}



ImColor operator*(const ImColor& c, float f)
{
	return ImVec4(c.Value.x * f, c.Value.y * f, c.Value.z * f, c.Value.w);
}

GraphState::GraphState(ShaderGraph& parent, ShaderAsset* asset)
	: Asset_(asset)
	, Parent_(parent)
{
}

GraphState::GraphState(const GraphState& other)
	: Asset_(other.Asset_)
	, Parent_(other.Parent_)
	, Nodes_(other.Nodes_)
{
    for(Node*& node : Nodes_)
    {
        node = node->Copy(Parent_);
    }
}

GraphState::~GraphState()
{
	for(Node* node : Nodes_)
	{
		delete node;
	}
}

NodeId GraphState::AddNode(Node *node)
{
	Asset_->MakeDirty();
	return Nodes_.insert(node);
}

void GraphState::RemoveNode(NodeId node, bool erase, bool override_flags)
{
	if(not override_flags && Nodes_[node]->Info.Flags & NodeFlags_Const) return;
	if(erase) delete Nodes_[node];
	Nodes_.erase(node);
	Asset_->MakeDirty();
}

void GraphState::Clear()
{
	for(Node* node : Nodes_)
	{
		delete node;
	}

	Nodes_.clear();
}

void GraphState::Draw()
{
	for(ImGuiID id = 0; id < Nodes_.size(); ++id)
	{
		if(Nodes_(id) == false) continue;

		Nodes_[id]->Draw(id);
	}
}

void GraphState::Read(const JsonValue& data)
{
	uint32_t version = data.FindMember("Version")->value.GetInt();

	// Disambiguation function
	Read_(version, data);
}

template<>
void GraphState::Read_<GraphState::Version<GraphState::GraphVersion_0>>(const JsonValue& data)
{
	// Typedefs and necessary locals
	using                        AliasMapping = ShaderGraph::AliasMapping;
	using                ContextMenuHierarchy = ShaderGraph::ContextMenuHierarchy;
	const AliasMapping&              AliasMap = ShaderGraph::AliasMap();
	const ContextMenuHierarchy& NodeRegistrar = ShaderGraph::ContextMenu();
	const JsonValue&                    nodes = data.FindMember("Nodes")->value;
	const JsonValue&              connections = data.FindMember("Connections")->value;
	
	// Read Nodes
	for(uint32_t n = 0; n < nodes.Size(); ++n)
	{
		// Locals
		const JsonValue& node_data = nodes[n];
		const JsonValue&    inputs = node_data.FindMember("Inputs")->value;
		const JsonValue&   outputs = node_data.FindMember("Outputs")->value;
		std::string          alias = node_data.FindMember("Alias")->value.GetString();
		Node*                 node = Asset_->GetSingletonNode(alias);

		if(node == nullptr)
		{
			// Create a node
			node = NodeRegistrar[AliasMap.find(alias)->second].Constructor(Parent_, { 0, 0 });
			node->Info.Alias = alias;
			AddNode(node);
		}
		
		const JsonValue& position = node_data.FindMember("Position")->value;

		node->Position.x   = position.FindMember("X")->value.GetFloat();
		node->Position.y   = position.FindMember("Y")->value.GetFloat();
		node->Header.Title = node_data.FindMember("Title")->value.GetString();

		// Inputs
		for(uint32_t p = 0; p < inputs.Size(); ++p)
		{
			if(node->IO.Inputs.size() <= p) node->IO.Inputs.push_back(Pin("", PinType_Any));
			ReadPin_(node->IO.Inputs[p], inputs[p]);
		}

		// Outputs
		for(uint32_t p = 0; p < outputs.Size(); ++p)
		{
			if(node->IO.Outputs.size() <= p) node->IO.Outputs.push_back(Pin("", PinType_Any));
			ReadPin_(node->IO.Outputs[p], outputs[p]);
		}
	}
	
	// Connections
	for(uint32_t c = 0; c < connections.Size(); ++c)
	{
		const JsonValue& connection = connections[c];
		const JsonValue& a = connection.FindMember("A")->value;
		const JsonValue& b = connection.FindMember("B")->value;
		ImUserPinPtr A, B;

		A.Node      = a.FindMember("Node")->value.GetInt();
		A.Pin       = a.FindMember("Pin")->value.GetInt();
		A.Direction = a.FindMember("Direction")->value.GetBool();
		
		B.Node      = b.FindMember("Node")->value.GetInt();
		B.Pin       = b.FindMember("Pin")->value.GetInt();
		B.Direction = b.FindMember("Direction")->value.GetBool();

		QueuedConnections_.push_back({ A, B });
	}
}

void GraphState::Read_(uint32_t version, const JsonValue& value)
{
	switch (version)
	{
		//case FunctionVersion_0: Read_<FunctionVersion_0>(document); return;
		default: case GraphVersion_Latest: Read_<Version<GraphVersion_Latest>>(value); return;
	}
}

rapidjson::Value GraphState::Write(rapidjson::MemoryPoolAllocator<>& alloc)
{
	JsonValue out; out.SetObject();

	// Node Array
	ocu::map<NodeId, uint32_t> node_map;
	JsonValue nodes,  connections;
	nodes.SetArray(); connections.SetArray();

	for(auto it = Nodes_.begin(); it != Nodes_.end(); ++it)
	{
		const auto& node_data = *it;
		node_map[it.id()] = nodes.Size();
		
		// Setup JSON Objects
		JsonValue node, inputs, outputs, position;
		bool dyn_inputs  = node_data->Info.Flags & NodeFlags_DynamicInputs;
		bool dyn_outputs = node_data->Info.Flags & NodeFlags_DynamicOutputs;

		node.SetObject();
		inputs.SetArray();
		outputs.SetArray();
		position.SetObject();

		position.AddMember("X", node_data->Position.x, alloc);
		position.AddMember("Y", node_data->Position.y, alloc);

		node.AddMember("Alias",    node_data->Info.Alias,   alloc);
		node.AddMember("Title",    node_data->Header.Title, alloc);
		node.AddMember("Position", position,                alloc);

		// Write Input Pins
		for(const Pin& input : node_data->IO.Inputs)
		{
			JsonValue value = WritePin_(input, alloc);
			
			inputs.PushBack(value, alloc);
		}

		// Write Output Pins
		for(Pin& output : node_data->IO.Outputs)
		{
			JsonValue value = WritePin_(output, alloc);
			
			outputs.PushBack(value, alloc);
		}

		node.AddMember("Inputs",  inputs,  alloc);
		node.AddMember("Outputs", outputs, alloc);
		
		nodes.PushBack(node, alloc);
	}

	// Write Connections
	for(const ImPinConnection& conn : ImNodeGraph::GetConnections())
	{
		JsonValue connection, A, B;
		connection.SetObject(); A.SetObject(); B.SetObject();

		A.AddMember("Node",      ImNodeGraph::GetUserID(conn.A.Node), alloc);
		A.AddMember("Pin",       ImNodeGraph::GetUserID(conn.A),      alloc);
		A.AddMember("Direction", conn.A.Direction,                    alloc);

		B.AddMember("Node",      ImNodeGraph::GetUserID(conn.B.Node), alloc);
		B.AddMember("Pin",       ImNodeGraph::GetUserID(conn.B),      alloc);
		B.AddMember("Direction", conn.B.Direction,                    alloc);

		connection.AddMember("A", A, alloc);
		connection.AddMember("B", B, alloc);

		connections.PushBack(connection, alloc);
	}
	
	out.AddMember("Nodes",       nodes,        alloc);
	out.AddMember("Connections", connections,  alloc);

	return out;
}

const Node* GraphState::GetNode(NodeId node) const
{
	return Nodes_[node];
}

const Node* GraphState::FindNode(ImGuiID node) const
{
	return Nodes_[ImNodeGraph::GetUserID("ShaderGraph", node)];
}

const Pin& GraphState::FindPin(ImPinPtr ptr) const
{
	Node* node = Nodes_[ImNodeGraph::GetUserID(ptr.Node)]; // Get the node
	auto& pins = ptr.Direction ? node->IO.Outputs : node->IO.Inputs; // Get the correct set of pins
	int idx = ImNodeGraph::GetUserID(ptr); // Get our id for the pin
	if(ptr.Direction) idx *= -1; // Outputs have negative ids
	idx -= 1; // Offset for indexing
	return pins[idx];
}

Node* GraphState::GetNode(NodeId node)
{
	return Nodes_[node];
}

Node* GraphState::FindNode(ImGuiID node)
{
	return Nodes_[ImNodeGraph::GetUserID("ShaderGraph", node)];
}

Pin& GraphState::FindPin(ImPinPtr ptr)
{
	Node* node = Nodes_[ImNodeGraph::GetUserID(ptr.Node)]; // Get the node
	auto& pins = ptr.Direction ? node->IO.Outputs : node->IO.Inputs; // Get the correct set of pins
	int idx = ImNodeGraph::GetUserID(ptr); // Get our id for the pin
	if(ptr.Direction) idx *= -1; // Outputs have negative ids
	idx -= 1; // Offset for indexing
	return pins[idx];
}

void GraphState::ReadPin_(Pin& pin, const JsonValue& value)
{
	pin.Name  = value.FindMember("Name")->value.GetString();
	pin.Flags = value.FindMember("Flags")->value.GetUint();
	pin.Type  = value.FindMember("Type")->value.GetUint();

	switch(pin.Type)
	{
		case PinType_Int:    pin.Value = value.FindMember("Value")->value.GetInt();   break;
		case PinType_UInt:   pin.Value = value.FindMember("Value")->value.GetUint();  break;
			
		default:
		case PinType_Float:  pin.Value = value.FindMember("Value")->value.GetFloat(); break;
			
		case PinType_Vector:
			const JsonValue& data = value.FindMember("Value")->value;
		glm::vec3& val  = pin.Value;
		val.x = data.FindMember("X")->value.GetFloat();
		val.y = data.FindMember("Y")->value.GetFloat();
		val.z = data.FindMember("Z")->value.GetFloat();
		break;
	}
}

rapidjson::Value GraphState::WritePin_(const Pin &pin, JsonAlloc& alloc)
{
	JsonValue out;
	out.SetObject();

	out.AddMember("Name",  pin.Name, alloc);
	out.AddMember("Flags", pin.Flags, alloc);
	out.AddMember("Type",  pin.Type, alloc);

	switch(pin.Type)
	{
	case PinType_Int:    out.AddMember("Value", static_cast<int32_t>(pin.Value), alloc); break;
	case PinType_UInt:   out.AddMember("Value", static_cast<uint32_t>(pin.Value), alloc); break;
		
	default:
	case PinType_Float:  out.AddMember("Value", static_cast<float>(pin.Value), alloc); break;
		
	case PinType_Vector:
		JsonValue vec; vec.SetObject();
		vec.AddMember("X", pin.Value.get<glm::vec3>().x, alloc);
		vec.AddMember("Y", pin.Value.get<glm::vec3>().y, alloc);
		vec.AddMember("Z", pin.Value.get<glm::vec3>().z, alloc);
		out.AddMember("Value", vec, alloc);
		break;
	}

	return out;
}

bool GraphState::ResolveConnections_()
{
	bool compile = not QueuedConnections_.empty();
	while(QueuedConnections_.empty() == false)
	{
		const auto& connection = QueuedConnections_.back();		
		ImNodeGraph::MakeConnection(connection.first, connection.second);
		QueuedConnections_.pop_back();
	}

	return compile;
}

void ShaderAsset::View(HDRTexture::HandleType *Target)
{
	ImNodeGraph::BeginGraphPostOp("ShaderGraph");

	if(State_.ResolveConnections_()) Compile();

	ImNodeGraph::EndGraphPostOp();
}

GraphState& GraphState::operator=(const GraphState& other)
{
    Nodes_ = other.Nodes_;

    for(Node*& node : Nodes_) if(node) node = node->Copy(Parent_);

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
	, IO { }
	, Info
	{
		.Flags = NodeFlags_None
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
        const ImVector<ImGuiID>&  connections  = ImNodeGraph::GetPinConnections();
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

    if(Info.Flags & NodeFlags_DynamicInputs)
    {
        ImGui::Text("\uEA11");
    }

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

	Shader_->GetState().Draw();

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

    const ImSet<ImGuiID>& Selected = *ImNodeGraph::GetSelected();
    Selected_.reset();
    if(Selected.Size == 1)
    {
        Selected_ = ImNodeGraph::GetUserID(*Selected.cbegin());
    }

    ImNodeGraph::EndGraphPostOp();
}


void ShaderGraph::DrawContextMenu()
{
    ContextMenuHierarchy& ContextMenu = ShaderGraph::ContextMenu_();
    
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
                ContextMenuHierarchy& ContextMenu = ShaderGraph::ContextMenu_();
                const auto depth = ContextMenu.depth(id);

            	// If the node is at a greater depth than the context, skip it
                if(depth > Context.size()) return false;

            	// if the depth is less than the depth of the context, pop the sub menus
                while(depth < Context.size())
                {
                    Context.pop();
                    ImGui::EndMenu();
                }

            	if(item.Name == "##") return false;
            		
            	// If we are in a different sub-tree than the context, skip it
                if(Context.top() != ContextMenu.parent(id)) return false;

            	// Generate a string to identify the node for ImGUI, tack on the id just in case of duplicates
                std::string name = std::format("{}##{}", item.Name, id);

            	// Check if this is a subfolder or a node
                if(item.Constructor)
                {
                	// Clickable menu to create the node
                    if(ImGui::MenuItem(item.Name.c_str()))
                    {
                        NodeId id = Graph.Shader_->GetState().AddNode(item.Constructor(Graph, Location));
                    }
                }
                else
                {
                	// Begin a submenu
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

    	// Push a null node to represent the root
        MenuVisitor.Context.push(0);

    	// Traverser-Visitor Pattern
        ContextMenu.traverse<ContextMenuHierarchy::pre_order>(MenuVisitor);

    	// Pop the context to end the sub-menus
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
    ImSet<ImGuiID>& Selected = *ImNodeGraph::GetSelected();
    for(ImGuiID node : Selected)
    {
        State.RemoveNode(ImNodeGraph::GetUserID(node));
    }
    Selected.Clear();
}

void ShaderGraph::Paste(ImVec2)
{
    
}

void ShaderGraph::Clear()
{
    
}

const Node * ShaderGraph::GetNode(NodeId node) const
{
	return Shader_->GetState().GetNode(node);
}

const Node* ShaderGraph::FindNode(ImPinPtr ptr) const
{
    return Shader_->GetState().FindNode(ptr.Node);
}

const Node* ShaderGraph::FindNode(ImGuiID id) const
{
    return Shader_->GetState().FindNode(id);
}

const Pin& ShaderGraph::FindPin(ImPinPtr ptr) const
{
    return Shader_->GetState().FindPin(ptr);
}

Node * ShaderGraph::GetNode(NodeId node)
{
	return Shader_->GetState().GetNode(node);
}

Node* ShaderGraph::FindNode(ImPinPtr ptr)
{
	return Shader_->GetState().FindNode(ptr.Node);
}

Node* ShaderGraph::FindNode(ImGuiID id)
{
	return Shader_->GetState().FindNode(id);
}

Pin& ShaderGraph::FindPin(ImPinPtr ptr)
{
    return Shader_->GetState().FindPin(ptr);
}

std::string ShaderGraph::GetValue(ImPinPtr ptr)
{
    if(ptr.Direction == ImPinDirection_Output)
    {
        const Pin& pin = FindPin(ptr);

    	// L-Value
    	if(pin.Flags & PinFlags_Literal)
    	{
    		switch(pin.Type)
    		{
    		case PinType_UInt:   return std::to_string(pin.Value.get<glm::uint32>());
    		case PinType_Int:    return std::to_string(pin.Value.get<glm::int32>());
    		case PinType_Float:  return std::to_string(pin.Value.get<glm::float32>());
    		case PinType_Vector:
    		{
    			const glm::vec3& val = pin.Value.get<glm::vec3>();
    			return std::format("vec3({},{},{})", val.x, val.y, val.z);
    		}
    		default: return "0";
    		}
    	}
    
        return pin.GetVarName();
    }
    else
    {
        const ImVector<ImGuiID>& connections = ImNodeGraph::GetPinConnections(ptr);
        const Pin& pin = FindPin(ptr);

        // L-Value
        if(connections.empty())
        {
            switch(pin.Type)
            {
            case PinType_UInt:   return std::to_string(pin.Value.get<glm::uint32>());
            case PinType_Int:    return std::to_string(pin.Value.get<glm::int32>());
            case PinType_Float:  return std::to_string(pin.Value.get<glm::float32>());
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

void ShaderGraph::Register(const std::filesystem::path& path, const std::string &alias, ConstructorPtr constructor)
{
	const std::string            name = path.filename().string();
    ContextMenuHierarchy& ContextMenu = ContextMenu_();
	AliasMapping&            AliasMap = AliasMap_();

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
			node = ContextMenu.insert({ it->string(), alias, nullptr }, node);
			++it;
		}
	}

	ContextMenu[node].Constructor = constructor;
	AliasMap[alias] = node;
}

Inspector::Inspector()
	: EditorWindow("Inspector", ImGuiWindowFlags_None)
	, Graph(nullptr)
{
}

void Inspector::DrawWindow()
{
    if(Graph->Shader_ == nullptr) return;

    if(Graph->Selected_())
    {
        Graph->GetNode(Graph->Selected_)->Inspect();
    }
}