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

#include <Graph/Nodes/Shaders/Function.h>

#include <Core/Console.h>
#include <Core/Engine.h>
#include <Editor/EditorSystem.h>
#include <Renderer/Renderer.h>

#include <imgui-docking/misc/cpp/imgui_stdlib.h>
#include <imgui-extras/imgui_extras.h>

#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/prettywriter.h>

#include <queue>
#include <numeric>
#include <fstream>
#include <type_traits>

#include "imnode-graph/imnode_graph_internal.h"


using namespace OpenShaderDesigner;
using namespace OpenShaderDesigner::Nodes::Shaders;

namespace rjson = rapidjson;

RegisterAsset("Shaders/Function", Function, ".sf");
RegisterNode("##/Function", Function);

// =====================================================================================================================
// Shaders
// =====================================================================================================================


// Function ------------------------------------------------------------------------------------------------------------


Function::Function(const FileManager::Path& path, ShaderGraph& graph)
    : Node(graph, { 500, 0 })
    , ShaderAsset(path, graph)
    , Shader_(nullptr)
    , Inputs_(graph, { 0, 0 })
    , DisplayVar_(0)
{
    Info.Flags |= NodeFlags_Const;
	Info.Alias = "Function";

    Header.Title = HeaderMarker + "Outputs";
    Header.Color = HeaderColor;
    Header.HoveredColor = HeaderHoveredColor;
    Header.ActiveColor = HeaderActiveColor;

    ID_ = GetState().AddNode(this);
    InputsID_ = GetState().AddNode(&Inputs_);

	IO.Inputs.push_back({ "Out", PinType_Vector });
	Inputs_.IO.Outputs.push_back({ "In", PinType_Vector });

    MakeDirty();

	if(exists(path)) Function::Read(path);
}

Function::Function(const FileManager::Path& src, const FileManager::Path& dst, ShaderGraph& graph)
	: Node(graph, { 500, 0 })
	, ShaderAsset(dst, graph)
	, Shader_(nullptr)
	, Inputs_(graph, { 0, 0 })
	, DisplayVar_(0)
{
	Info.Flags |= NodeFlags_Const;
	Info.Alias = "Function";

	Header.Title = HeaderMarker + "Outputs";
	Header.Color = HeaderColor;
	Header.HoveredColor = HeaderHoveredColor;
	Header.ActiveColor = HeaderActiveColor;

	ID_ = GetState().AddNode(this);
	InputsID_ = GetState().AddNode(&Inputs_);

	IO.Inputs.push_back({ "Out", PinType_Vector });
	Inputs_.IO.Outputs.push_back({ "In", PinType_Vector });

	MakeDirty();

	if(exists(src)) Function::Read(src);
}

Function::Function(ShaderGraph &graph, ImVec2 pos)
	: Node(graph, pos)
	, ShaderAsset("", graph)
	, Shader_(nullptr)
	, Inputs_(graph, pos)
	, DisplayVar_(0)
	, ID_(0)
	, InputsID_(0)
{
	
}

Function::~Function()
{
    delete Shader_;
}

Node* Function::Copy(ShaderGraph &graph) const
{
    return nullptr; // Non Copyable
}

bool Function::Inspect()
{
	bool  Changed = false;
	auto& Outputs = IO.Inputs;
	
    for(int i = 0; i < Outputs.size(); ++i)
    {
    	const float avail_x = ImGui::GetContentRegionAvail().x;

    	ImGui::PushItemWidth(avail_x * 0.625f);
    	Changed |= ImGui::InputText(std::format("##{}_name", i).c_str(), &Outputs[i].Name, ImGuiInputTextFlags_AutoSelectAll);
    	ImGui::PopItemWidth();
    	
        ImGui::SameLine();
    	
    	ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
        Changed |= ImGui::Combo(std::format("##{}_type", i).c_str(), &Outputs[i].Type, Pin::TypeNames, PinType_Any);
    	ImGui::PopItemWidth();
    }

    if(ImGui::TextLink("\uEA11"))
    {
        PinType type = PinType_Vector;

        if(not Outputs.empty()) type = Outputs.back().Type;
        
        Outputs.push_back({
            std::format("Out{}", Outputs.size()), type
        });

    	Changed |= true;
    }

	return Changed;
}

void Function::Compile()
{
    // Get Static Objects
    const ShaderGraph&  Graph = *EditorSystem::Get<ShaderGraph>();
    GraphState&         State = GetState();
    const auto&        Inputs = Inputs_.IO.Outputs;
    const auto&       Outputs = IO.Inputs;
	const auto&         Nodes = State.GetNodes();

    if(Outputs.size() < 1)
    {
        Console::Log(Console::Error, "Compilation Error; \"No Outputs Specified\"");
        return;
    }
    
    // Generate node priorities
    ocu::dynarray Priority(Nodes.capacity(), -1);

    int p = 0;
    std::deque VisitQueue { Outputs[0].Ptr };

	// The goal is to generate a priority for each node so that we can compile them to glsl in the correct order
	// Each branch has the potential to contain the same node as another branch
	// For this reason, we use a map first so that we don't have to spend a lot of time updating a priority queue
    while(not VisitQueue.empty())
    {
        ImPinPtr pin = VisitQueue.front(); VisitQueue.pop_front();
        NodeId node = ImNodeGraph::GetUserID(pin.Node);
        
        Priority[node] = p++;
        
        for(Pin& pin : Nodes[node]->IO.Inputs)
        {
            const ImVector<ImGuiID>& connections = ImNodeGraph::GetPinConnections(pin.Ptr);
            if(connections.empty()) continue;
            ImPinConnection connection = ImNodeGraph::GetConnection(connections[0]);

            if(connection.A == pin.Ptr)
            {
            	// Skip Literals
            	if(Graph.FindPin(connection.B).Flags & PinFlags_Literal) continue;
            	
                VisitQueue.push_back(connection.B);
            }
            else
            {
            	// Skip Literals
            	if(Graph.FindPin(connection.A).Flags & PinFlags_Literal) continue;
            	
                VisitQueue.push_back(connection.A);
            }
        }
    }

	// Here the priority map dumps each node into an order array with a nodes priority being repsective to its id
	// We use optional as a shortcut to prevent any "real" sorting operation being done
	// This allows us to keep these operations at linear time complexity
    ocu::dynarray<ocu::optional<NodeId>> Order(p);
    for(NodeId node = 0; node < Priority.size(); ++node)
    {
    	if(Priority[node] == -1) continue;
    	
    	Order[Priority[node]] = node;
    }

    // Write out the code
    std::stringstream Out;

    // Function Signature
    Out << std::format("void {}(", GetFile().path().stem().string());

    for(int i = 0; i < Inputs.size(); ++i)
    {
        if(i > 0)
            Out << ", ";
        
        Out << std::format("in {} {}", Pin::TypeKeywords[Inputs[i].Type], Inputs[i].GetVarName());
    }

    for(int i = 0; i < Outputs.size(); ++i)
    {
        if(not Inputs.empty() || i > 0)
            Out << ", ";
        
        Out << std::format("out {} {}", Pin::TypeKeywords[Outputs[i].Type], Outputs[i].GetVarName());
    }

    Out << ")" << std::endl;
    Out << "{" << std::endl;

    // Function Code
    for(int i = Order.size() - 1; i >= 0; --i)
    {
		if(not Order[i]()) continue;
    	
        NodeId id = Order[i];
        if(id == InputsID_) continue;
        
        Out << Nodes[id]->GetCode();

        if(i > 0) Out << std::endl;
    }

    Out << "}" << std::endl;

    Code = Out.str();
    
    Console::Log(Console::Message, "{}", Code);
    
    CompileDisplayShader_();
}

void Function::Open()
{
    EditorSystem::Get<ShaderGraph>()->OpenShader(this);
    EditorSystem::Get<Renderer>()->OpenShader(this);
}

FileManager::Asset* Function::Create(const FileManager::Path &path)
{
    return new Function(path, *EditorSystem::Get<ShaderGraph>());
}

FileManager::Asset* Function::Load(const FileManager::Path &path)
{
    return new Function(path, *EditorSystem::Get<ShaderGraph>());
}

FileManager::Asset* Function::Import(const FileManager::Path &src, const FileManager::Path &dst)
{
    return new Function(src, dst, *EditorSystem::Get<ShaderGraph>());
}

std::string Function::GetCode() const
{
    std::stringstream Out;
    const auto& Outputs = IO.Inputs;
    
    for(int i = 0; i < Outputs.size(); ++i)
    {
        Out << std::format("{} = {};", Outputs[i].GetVarName(), Graph.GetValue(Outputs[0].Ptr)) << std::endl;
    }
    
    return Out.str();
}

void Function::View(HDRTexture::HandleType* Target)
{
    DrawImage_(Target);
	DrawInputs_();

	ShaderAsset::View(Target);
}

void Function::Write(const FileManager::Path &path)
{
	ImNodeGraph::BeginGraphPostOp("ShaderGraph");
	
	JsonDocument document;
	auto&       allocator = document.GetAllocator();
	JsonValue debug_values;
	
	document.SetObject();
	debug_values.SetArray();

	// Version
	document.AddMember("Version", FunctionVersion_Latest, allocator);
	document.AddMember("DisplayVar", DisplayVar_, allocator);

	JsonValue state = GetState().Write(allocator);
	document.AddMember("GraphState", state, allocator);

	for(int p = 0; p < Inputs_.IO.Outputs.size(); ++p)
	{
		debug_values.PushBack(InputValues_[p], allocator);
	}

	// Write Nodes and Connections
	document.AddMember("DebugValues", debug_values, allocator);

	// Write to file
	std::ofstream out(GetFile().path());
	rjson::OStreamWrapper wrapper(out);
	rjson::PrettyWriter writer(wrapper);
	document.Accept(writer);
	out.close();

	// Clear Dirty Flag
	Asset::Write(path);

	ImNodeGraph::EndGraphPostOp();
}

void Function::Read(const FileManager::Path &path)
{
	// Setup streams
	std::ifstream in(path);
	rjson::IStreamWrapper wrapper(in);
	rapidjson::Document document;

	// Parse File
	document.ParseStream(wrapper);
	in.close();

	// Read in version
	uint32_t version = document.FindMember("Version")->value.GetUint();

	// Disambiguation function
	Read_(version, document);
	
	ShaderAsset::Read(path);
}

Node* Function::GetSingletonNode(const std::string &alias)
{
	if(alias == "Function")       return this;
	if(alias == "FunctionInputs") return &Inputs_;

	return nullptr;
}

template<>
void Function::Read_<Function::Version<Function::FunctionVersion_0>>(const rapidjson::Document &document)
{
	// Typedefs and necessary locals
	auto&                   State = GetState();
	const JsonValue& debug_values = document.FindMember("DebugValues")->value;

	DisplayVar_ = document.FindMember("DisplayVar")->value.GetUint();

	State.RemoveNode(ID_, false, true);
	State.RemoveNode(InputsID_, false, true);

	State.Clear();

	ID_ = State.AddNode(this);
	InputsID_ = State.AddNode(&Inputs_);

	IO.Inputs.clear();
	Inputs_.IO.Outputs.clear();

	State.Read(document.FindMember("GraphState")->value);

	for(int p = 0; p < Inputs_.IO.Outputs.size(); ++p)
	{
		InputValues_[p] = debug_values[p].GetUint();
	}
}

void Function::Read_(uint32_t version, const rapidjson::Document &document)
{
	switch (version)
	{
	//case FunctionVersion_0: Read_<FunctionVersion_0>(document); return;
	default: case FunctionVersion_Latest: Read_<Version<FunctionVersion_Latest>>(document); return;
	}
}

void Function::DrawImage_(HDRTexture::HandleType* Target)
{
    if(Target == nullptr) return;

    ImGui::BeginChild("##view", { 0, 0 }, ImGuiChildFlags_Border | ImGuiChildFlags_ResizeY);

    ImVec2     reg = ImGui::GetContentRegionAvail();
    glm::vec2 size = Target->size();
    float    min_r = glm::min(reg.x / size.x, reg.y / size.y);
              size = size * min_r;

    Target->resize({ size.x, size.y });
    size = Target->size();
    
    Render_(Target);

	if(reg.x > reg.y)
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (reg.x - size.x) / 2.0f);
    
    ImGui::Image(
    	reinterpret_cast<ImTextureID>(static_cast<intptr_t>(Target->handle()))
    ,	{ size.x, size.y }
    ,	{ 0.5f / size.x, 0.5f / size.x }, { (size.x - 0.5f) / size.x, (size.y - 0.5f) / size.y });

    ImGui::EndChild();
}

void Function::DrawInputs_()
{
    auto&       Inputs = Inputs_.IO.Outputs;
    const auto& Outputs = IO.Inputs;
    const float avail_x = ImGui::GetContentRegionAvail().x;
    
    if(ImGui::TreeNodeEx("Inputs"))
    {
        
    for(int i = 0; i < Inputs.size(); ++i)
    {
        int flags = ImGuiInputTextFlags_EnterReturnsTrue;
    	auto& pin = Inputs[i];
    	const auto& items = InputTypes[Inputs[i].Type];
    	auto&       value = InputValues_[i];

    	ImGui::Text(pin.Name.c_str());

    	// Dropdown for pin debug value
		ImGui::SameLine();
		ImGui::PushItemWidth(avail_x * 0.25f);
    	if(ImGui::BeginCombo(std::format("##{}_var", pin.Name).c_str(), InputNames[value].c_str()))
    	{
    		for(int j = 0; j < items.size(); ++j)
    		{
    			glw::enum_t t = items[j];
    			if(ImGui::Selectable(InputNames[t].c_str(), t == value))
    			{
    				value = t;
    				CompileDisplayShader_();
    			}
    		}

    		ImGui::EndCombo();
    	}
		ImGui::PopItemWidth();

    	// Handle input for custom debug values
		if(value == FuncInput_Custom)
		{
			ImGui::SameLine();
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
			bool changed = false;
			switch(pin.Type)
			{
			case PinType_Int:   changed |= ImGui::InputInt(std::format("##{}_val", pin.Name).c_str(), pin.Value); break;
			case PinType_UInt:  changed |= ImGui::InputUInt(std::format("##{}_val", pin.Name).c_str(), pin.Value); break;

			default:
			case PinType_Float: changed |= ImGui::InputFloat(std::format("##{}_val", pin.Name).c_str(), pin.Value); break;

			case PinType_Vector:
				ImGui::BeginGroup();
    
				changed |= ImGui::ColorPicker3(
					std::format("##{}_val", pin.Name).c_str(), &pin.Value.get<glm::vec3>().x
				,   ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float
				);
    	
				changed |= ImGui::ColorPreview3(
					std::format("##vec{}_val", pin.Name).c_str(), &pin.Value.get<glm::vec3>().x
				,   ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float
				);

				ImGui::EndGroup();
				break;
			}
			ImGui::PopItemWidth();

			if(changed) CompileDisplayShader_();
		}

    	/*if(ImGui::InputText(Inputs[i].Name.c_str(), &InputValues_[i], flags))
        {
            CompileDisplayShader_();
        }*/
    }
    
    ImGui::TreePop();
    
    }
    
    if(ImGui::BeginCombo("Output", Outputs.empty() ? "" : Outputs[DisplayVar_].Name.c_str()))
    {
        
    for(int i = 0; i < Outputs.size(); ++i)
    {
        if(ImGui::Selectable(Outputs[i].Name.c_str(), DisplayVar_ == i))
        {
            DisplayVar_ = i;
            CompileDisplayShader_();
        }
    }

    ImGui::EndCombo();
    
    }
}

void Function::Render_(HDRTexture::HandleType *Target)
{
    if(Shader_ == nullptr)
    {
        glm::vec4 fill(0, 0, 0, 1);
        Target->clear(Target->size(), { 0, 0 }, 0, &fill, glw::rgba, glw::float32);
        return;
    }
    
    Shader_->bind();

    Shader_->operator[]("t")  = static_cast<float>(Engine::Runtime);
    Shader_->operator[]("dt") = static_cast<float>(Engine::Delta);

    Target->bind_image(0, glw::access_write);

    auto size = Target->size();
    Shader_->dispatch(size.x, size.y, 1);
}

void Function::CompileDisplayShader_()
{
    const auto&  Inputs = Inputs_.IO.Outputs;
    const auto& Outputs = IO.Inputs;
    
    std::stringstream Out;

    // Print Version String and Group Sizes
    Out << VersionString << std::endl;
    Out << glw::shader::group_size(8, 8, 1) << std::endl;
    Out << std::endl;
    Out << glw::shader::built_ins_compute << std::endl;
    Out << std::endl;

    Out << "uniform float t;"  << std::endl;
    Out << "uniform float dt;" << std::endl;

    // Print Function Code
    Out << Code << std::endl << std::endl;

	// Output Texture
    Out << "layout (rgba16f, binding = 0) writeonly restrict uniform image2D out_Color0;" << std::endl;

    // Print driver
    Out << "void main(void)"                                                            << std::endl
        << "{"                                                                          << std::endl
        << std::format("    vec3 size = vec3({}.xy, 1);", glw::shader::glw_RequestedInvocations) << std::endl
        << std::format("    vec3 xy   = vec3({}.xy, 0);", glw::shader::gl_GlobalInvocationID)    << std::endl
        << std::format("    vec3 uv   = xy / size;")                                    << std::endl
        <<             "    float x = xy.x, y = xy.y, u = uv.x, v = uv.y;"              << std::endl;

    // Output vars
    for(int i = 0; i < Outputs.size(); ++i)
    {
        Out << std::format("    {} out_{};", Pin::TypeKeywords[Outputs[i].Type], Outputs[i].Name) << std::endl;
    }

    // Function Call
    Out << std::format("    {}(", GetFile().path().stem().string());
    for(int i = 0; i < Inputs.size(); ++i)
    {
        if(i > 0)
            Out << ", ";

    	glw::enum_t value = InputValues_.get(i, FuncInput_Custom);

    	if(value == FuncInput_Custom)
    	{
    		switch(Inputs[i].Type)
    		{
    		case PinType_UInt:   Out << Inputs[i].Value.get<glm::int32>();   break;
    		case PinType_Int:    Out << Inputs[i].Value.get<glm::uint32>();  break;

    		default:
    		case PinType_Float:  Out << Inputs[i].Value.get<glm::float32>(); break;
    			
    		case PinType_Vector:
    			const auto& v = Inputs[i].Value.get<glm::vec3>();
    			Out << std::format("vec3({}, {}, {})", v.x, v.y, v.z);
    			break;
    		}
    	}
    	else
    	{
    		Out << InputVars[value];
    	}
    }

    for(int i = 0; i < Outputs.size(); ++i)
    {
        if(not Inputs.empty() || i > 0)
            Out << ", ";
        
        Out << "out_" << Outputs[i].Name;
    }
    Out << ");" << std::endl;

    // Result
	if(Outputs.empty())
	{
		Console::Log(Console::Alert, "Attempted to compile function with no outputs.");
		return;
	}
	
    switch(Outputs[DisplayVar_].Type)
    {
    case PinType_Float:
    case PinType_Int:
    case PinType_UInt:
        Out << std::format("    vec3 result = vec3(out_{});", IO.Inputs[DisplayVar_].Name) << std::endl; break;
        
    case PinType_Vector:
    default:
        Out << std::format("    vec3 result = out_{};",       IO.Inputs[DisplayVar_].Name) << std::endl; break;
    }

    // Set the pixel in the render target
    Out << "    imageStore(out_Color0, ivec2(xy), vec4(result, 1));" << std::endl;
    Out << "}" << std::endl;

    // Get the code string
    DisplayCode_ = Out.str();

    // Generate the shader
    delete Shader_;
    Shader_ = new glw::shader();

    Console::Log(Console::Message, "Generated GLSL:\n{}", DisplayCode_);

    if(not Shader_->attach_source(glw::compute, { DisplayCode_ }))
    {
        Console::Log(Console::Error, "{}", Shader_->get_error_string());
        delete Shader_; Shader_ = nullptr;
        return;
    }

    if(not Shader_->link())
    {
        Console::Log(Console::Error, "{}", Shader_->get_error_string());
        delete Shader_; Shader_ = nullptr;
        return;
    }
}


// Function Inputs -----------------------------------------------------------------------------------------------------

FunctionInputs::FunctionInputs(ShaderGraph &graph, ImVec2 pos)
    : Node(graph, pos)
{
    Info.Flags |= NodeFlags_Const;
	Info.Alias = "FunctionInputs";
    
    Header.Title        = HeaderMarker + "Inputs";
    Header.Color        = HeaderColor;
    Header.HoveredColor = HeaderHoveredColor;
    Header.ActiveColor  = HeaderActiveColor;
}

Node* FunctionInputs::Copy(ShaderGraph &graph) const
{
    return new FunctionInputs(graph, { 0, 0 });
}

bool FunctionInputs::Inspect()
{
	bool  Changed = false;
    auto& Inputs  = IO.Outputs;
	
    for(int i = 0; i < Inputs.size(); ++i)
    {
    	const float avail_x = ImGui::GetContentRegionAvail().x;

    	ImGui::PushItemWidth(avail_x * 0.625f);
    	Changed |= ImGui::InputText(std::format("##{}_name", i).c_str(), &Inputs[i].Name, ImGuiInputTextFlags_AutoSelectAll);
    	ImGui::PopItemWidth();
    	
        ImGui::SameLine();
    	
    	ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
        Changed |= ImGui::Combo(std::format("##{}_type", i).c_str(), &Inputs[i].Type, Pin::TypeNames, PinType_Any);
    	ImGui::PopItemWidth();
    }

    if(ImGui::TextLink("\uEA11"))
    {
        PinType type = PinType_Vector;

        if(not Inputs.empty()) type = Inputs.back().Type;

    	Inputs.push_back({
			std::format("In{}", Inputs.size()), type
		});

    	Changed |= true;
    }

	return Changed;
}

std::string FunctionInputs::GetCode() const
{
    return "";
}
