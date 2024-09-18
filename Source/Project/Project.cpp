//
// Created by Maddie on 9/14/2024.
//

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

OpenShaderDesigner::Project::Project()
    : ProjectFile_(NULL)
{
    
}

OpenShaderDesigner::Project::~Project()
{
    
}

void OpenShaderDesigner::Project::DrawMenuBar()
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
                    
                    Console::Log(Console::Message, "Closing New Project");
                    FileManager::Path   res = FileManager::Path(project.result().front());
                    FileManager::FileID root = filesystem->LoadDirectory(res.parent_path());
                    filesystem->CurrentDirectory(root);
                    
                    Console::Log(Console::Message, "Retrieving Project File");
                    ProjectFile_ = filesystem->Get(res);
                }
            }
        }

        ImGui::EndMenu();
    }
}

void OpenShaderDesigner::Project::Open()
{
    
}

void OpenShaderDesigner::Project::Load(const FileManager::Path& path)
{
    Console::Log(Console::Message, "Loading Project {}", path.string());
}

void OpenShaderDesigner::Project::Save(const FileManager::Path& path)
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

void OpenShaderDesigner::Project::Create(const FileManager::Path &path)
{
    Console::Log(Console::Message, "Setting Up New Project");
    Reset();
    Save(path);
}

void OpenShaderDesigner::Project::Reset()
{
    
}
