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

#include <Core/Engine.h>
#include <Editor/EditorSystem.h>
#include <Project/Project.h>

#include <fstream>
#include <Core/Console.h>

#include <portable-file-dialogs/portable-file-dialogs.h>

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/prettywriter.h>

using namespace OpenShaderDesigner;

RegisterAsset("##/Project", Project, ".sgp");

Project::Project()
    : Asset("")
    , ProjectFile_(NULL)
{
}

Project::~Project()
{
    
}

void Project::DrawMenuBar()
{
    FileManager* filesystem = EditorSystem::Get<FileManager>();
    
    if(ImGui::BeginMenu("File"))
    {
        Console::Log(Console::Message, "BeginMenu File");
        if(ImGui::MenuItem("\uecc9 New..."))
        {
            Console::Log(Console::Message, "Creating Project");
            
            bool cancelled = false;
            if(filesystem->AnyDirty())
            {
                auto save = pfd::message("Unsaved Changes", "Would you like to save your changes?", pfd::choice::yes_no_cancel);
                switch(save.result())
                {
                case pfd::button::yes:
                    Console::Log(Console::Message, "Saving Old Project");
                    filesystem->SaveAll();
                    break;
                case pfd::button::cancel:
                    Console::Log(Console::Message, "Cancelled");
                    cancelled = true;
                    break;
                default: case pfd::button::no:
                    break;
                }
            }

            if(not cancelled)
            {
                filesystem->CloseDirectory(filesystem->Parent(ProjectFile_));
                
                Console::Log(Console::Message, "Select Location");
                auto res = pfd::save_file("New", FileManager::GetHomeDirectory().string(), { "Project File (.sgp)", "*.sgp" }).result();

                if(not res.empty())
                {
                    FileManager::Path path = res;
                
                    if(not path.has_extension()) path = res + ".sgp";

                    Console::Log(Console::Alert, "Creating Project {} in {}", path.filename().string(), path.parent_path().string());
                
                    create_directories(path.parent_path());
                    FileManager::FileID root = filesystem->LoadDirectory(path.parent_path());
                    filesystem->CurrentDirectory(root);
                    ProjectFile_ = filesystem->Create(path.filename().string());
                }
            }
        }
        
        if(ImGui::MenuItem("\ued6f Open..."))
        {
            Console::Log(Console::Message, "Opening Project");
            
            bool cancelled = false;
            if(filesystem->AnyDirty())
            {
                auto save = pfd::message("Unsaved Changes", "Would you like to save your changes?", pfd::choice::yes_no_cancel);
                switch(save.result())
                {
                    case pfd::button::yes:
                        Console::Log(Console::Message, "Saving Old Project");
                    filesystem->SaveAll();
                    break;
                    case pfd::button::cancel:
                        Console::Log(Console::Message, "Cancelled");
                    cancelled = true;
                    break;
                    default: case pfd::button::no:
                        break;
                }
            }

            if(not cancelled)
            {
                Console::Log(Console::Message, "Post Dialogue");
                auto project = pfd::open_file("Open", FileManager::GetHomeDirectory().string(), { "Project File (.sgp)", "*.sgp" });

                if(not project.result().empty())
                {
                    Console::Log(Console::Message, "Closing Old Project");
                    filesystem->CloseDirectory(filesystem->Parent(ProjectFile_));
                    
                    Console::Log(Console::Message, "Opening New Project");
                    FileManager::Path   res = FileManager::Path(project.result().front());
                    FileManager::FileID root = filesystem->LoadDirectory(res.parent_path());
                    filesystem->CurrentDirectory(root);
                    
                    Console::Log(Console::Message, "Retrieving Project File {}", res.string());
                    ProjectFile_ = filesystem->Get(res);

                    Console::Log(Console::Message, "Loaded Project");
                }
            }
        }

        ImGui::EndMenu();
        Console::Log(Console::Message, "EndMenu File");
    }
}

void Project::Open()
{
    
}

void Project::Save(const FileManager::Path& path)
{
    // Setup
    rapidjson::Document document;
    document.SetObject();

    // Write to file
    std::ofstream file(path);
    rapidjson::OStreamWrapper out(file);
    rapidjson::PrettyWriter writer(out);
    document.Accept(writer);
    file.close();

    // Clear Dirty Flag
    Asset::Save(path);

    Console::Log(Console::Alert, "Saved Project {} to {}", path.filename().string(), path.parent_path().string());
}

FileManager::Asset* Project::Load(const FileManager::Path& path)
{
    Project* project = EditorSystem::GetMainMenuBar<Project>();
    Console::Log(Console::Message, "Loading Project {}", path.string());
    return project;
}

FileManager::Asset* Project::Import(const FileManager::Path &src,
    const FileManager::Path &dst)
{
    return nullptr;
}

FileManager::Asset* Project::Create(const FileManager::Path &path)
{
    Project* project = EditorSystem::GetMainMenuBar<Project>();
    Console::Log(Console::Message, "Loading Project {}", path.string());

    project->Reset();

    return project;
}

void Project::Reset()
{
    
}
