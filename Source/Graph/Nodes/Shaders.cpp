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

#include <queue>
#include <Graph/Nodes/Shaders.h>

#include "Editor/EditorSystem.h"

using namespace OpenShaderDesigner;
using namespace OpenShaderDesigner::Nodes::Shaders;

RegisterAsset("Shaders/Function", Function, ".sf");

// =====================================================================================================================
// Shaders
// =====================================================================================================================


// Function ------------------------------------------------------------------------------------------------------------


Function::Function(const FileManager::Path& path, ShaderGraph& graph)
    : Node(graph, { 0, 0 })
    , ShaderAsset(path, graph)
{
    Info.Const = true;
    
    Header.Title        = HeaderMarker + "Function";
    Header.Color        = HeaderColor;
    Header.HoveredColor = HeaderHoveredColor;
    Header.ActiveColor  = HeaderActiveColor;

    IO.Inputs.emplace_back("Out", PinType_Vector, PinFlags_NoCollapse | PinFlags_NoPadding | PinFlags_Ambiguous);

    GetState().AddNode(this);

    MakeDirty();
}

Node* Function::Copy(ShaderGraph &graph) const
{
    return nullptr; // Non Copyable
}

void Function::Inspect()
{
    
}

void Function::Compile()
{
    // Get Static Objects
    ShaderGraph& Graph = *EditorSystem::Get<ShaderGraph>();
    GraphState&  State = GetState();
    
    // Generate node priorities
    ocu::map<NodeId, int> Priority;
    ocu::set<int> Skip;

    int p = 0;
    std::deque VisitQueue { IO.Inputs[0].Ptr };

    while(not VisitQueue.empty())
    {
        ImPinPtr pin = VisitQueue.front(); VisitQueue.pop_front();
        NodeId node = ImNodeGraph::GetUserID(pin.Node).Int;
        
        if(Priority.contains(node))
            Skip.insert(Priority[node]);
        
        Priority[node] = p++;

        
        for(Pin& pin : State.Nodes[node]->IO.Inputs)
        {
            const ImVector<ImGuiID>& connections = ImNodeGraph::GetConnections(pin.Ptr);
            if(connections.empty()) continue;
            ImPinConnection connection = ImNodeGraph::GetConnection(connections[0]);

            if(connection.A == pin.Ptr)
                VisitQueue.push_back(connection.B);
            else
                VisitQueue.push_back(connection.A);
        }
    }

    ocu::dynarray<NodeId> Order(p, 0);
    for(auto it = Priority.begin(); it != Priority.end(); ++it)
    {
        Order[it->value] = it->key;
    }

    // Write out the code
    std::stringstream Out;

    Out << std::format("{} {}()",
        Pin::TypeKeywords[IO.Inputs[0].Type] // Return Type
    ,   GetFile().path().stem().string()
    ) << std::endl;

    Out << "{" << std::endl;

    for(int i = 0; i < p; ++i)
    {
        if(Skip.contains(i)) continue;
        
        Out << State.Nodes[Order[p - i - 1]]->GetCode() << std::endl;
    }

    Out << "}" << std::endl;

    Code = Out.str();
}

void Function::Open()
{
    EditorSystem::Get<ShaderGraph>()->OpenShader(this);
}

FileManager::Asset* Function::Create(const FileManager::Path &path)
{
    return new Function(path, *EditorSystem::Get<ShaderGraph>());
}

FileManager::Asset* Function::Load(const FileManager::Path &path)
{
    return nullptr;
}

FileManager::Asset* Function::Import(const FileManager::Path &src, const FileManager::Path &dst)
{
    return nullptr;
}

std::string Function::GetCode() const
{
    return std::format("return {};", Graph.GetValue(IO.Inputs[0].Ptr));
}
