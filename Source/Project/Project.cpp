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

#include "Renderer/Renderer.h"

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
    }

	if(ImGui::BeginMenu("Help"))
	{
		if(ImGui::MenuItem("HDR Calibration..."))
		{
			EditorSystem::OpenHDRCalibration();
		}

		ImGui::EndMenu();
	}
}

void Project::Open()
{
    
}

void Project::Write(const FileManager::Path& path)
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
    Asset::Write(path);

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
