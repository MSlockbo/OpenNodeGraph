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

#include <Editor/EditorSystem.h>
#include <FileSystem/FileManager.h>
#include <Core/Console.h>

#include <imgui-docking/misc/cpp/imgui_stdlib.h>

#include "Project/Project.h"

OpenShaderDesigner::Asset * OpenShaderDesigner::FileManager::load(const Path& path)
{
    Console::Log(Console::Message, "Loading File {}", path.string());
    FileType type = ExtensionMap[path.extension().string()];

    switch (type)
    {
    case FileType_Project:
    {
        Project* project = EditorSystem::GetMainMenuBar<Project>();
        project->Load(path);
        return project;
    }

    default:
        return nullptr;
    }
}

OpenShaderDesigner::Asset * OpenShaderDesigner::FileManager::import(const Path &src, const Path &dst)
{
    return nullptr;
}

OpenShaderDesigner::Asset * OpenShaderDesigner::FileManager::create(const Path &path)
{
    Console::Log(Console::Message, "Creating File {}", path.string());
    FileType type = ExtensionMap[path.extension().string()];

    switch (type)
    {
        case FileType_Project:
        {
            Project* project = EditorSystem::GetMainMenuBar<Project>();
            project->Create(path);
            return project;
        }

        default:
            return nullptr;
    }
}

OpenShaderDesigner::FileManager::FileManager()
	: EditorWindow("File Manager", ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar)
    , CurrentDirectory_(FileSystem::root), Selected_(FileSystem::root)
    , Rename_(false), FocusRename_(false)
{
	//CurrentDirectory_ = Filesystem_.load_directory(std::filesystem::current_path());
}

void OpenShaderDesigner::FileManager::DrawMenu()
{
	
}

void OpenShaderDesigner::FileManager::DrawWindow()
{
    // Directory Hierarchy
	ImGui::BeginGroup();
	if (ImGui::BeginChild("##hierarchy", ImVec2(200, 0),
						  ImGuiChildFlags_ResizeX | ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_Border))
	{
		struct Visitor
		{
			bool operator()(File& file, FileID id)
			{
			    const FileSystem& tree = file.system();
				uint32_t depth = tree.depth(id);

			    // Skip root
			    if(id == FileSystem::root) return false;

			    // Skip if we are in a closed directory
			    if(depth > stack.size()) return false;

			    // Skip non-directories
			    const bool is_dir = file.is_directory();
			    if(not is_dir) return false;

			    // Pop dead paths
			    while(depth < stack.size())
			    {
			        stack.pop();
			        ImGui::TreePop();
			    }

			    // Skip if the top of the stack is not the parent of the file
			    if(stack.top() != tree.parent(id)) return false;

			    // Generate display name
			    const char* icon = "\ued53";
			    std::string name = std::format("{} {}##{}", icon, file.path().stem().string(), id);

			    // Display flags
			    int flags = 0;
			    if(file.has_subdirectory())
			    {
			        flags |= ImGuiTreeNodeFlags_OpenOnArrow;
			        flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
			        flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
			    }
			    else
			    {
			        flags |= ImGuiTreeNodeFlags_Leaf;
			    }

			    if(id == directory) flags |= ImGuiTreeNodeFlags_Selected;

			    if(ImGui::TreeNodeEx(name.c_str(), flags))
			    {
			        stack.push(id);
			    }
			    
			    if(ImGui::IsItemClicked()) directory = file.get_id();

			    return false;
			}

			std::stack<FileID> stack;
		    FileID             directory;
		} visitor;

	    visitor.directory = CurrentDirectory_;
		visitor.stack.push(FileSystem::root);
		Filesystem_.traverse(visitor);

		while(visitor.stack.size() > 1)
		{
			ImGui::TreePop();
			visitor.stack.pop();
		}

	    CurrentDirectory_ = visitor.directory;
	}
	ImGui::EndChild();
	ImGui::EndGroup();

    ImGui::SameLine();

    if(ImGui::BeginTable("##directory", 3, ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_BordersOuter))
    {
        ImGui::TableSetupScrollFreeze(0, 1);
        
        ImGui::TableSetupColumn(" Name");
        ImGui::TableSetupColumn("Type");
        ImGui::TableSetupColumn("Source");

        ImGui::TableHeadersRow();

        for(FileID it = Filesystem_.begin(CurrentDirectory_); it != Filesystem_.end(CurrentDirectory_); it = Filesystem_.next(it))
        {
            File& file = Filesystem_[it];
            ImGui::TableNextRow();

            const bool selected = Selected_ == it;

            ImGui::TableNextColumn();

            // Display Name
            bool dirty = *file ? file->Dirty() : false;
            std::string name = std::format("{}{}", dirty ? "\uea81 " : "", file.path().stem().string());
            if(ImGui::Selectable(
                std::format(" {}##{}", Rename_ && selected ? "" : name.c_str(), file.get_id()).c_str()
            ,   selected, ImGuiSelectableFlags_SpanAllColumns
            ))
            {
                Selected_ = Rename_ ? Selected_ : it;
            }

            // Renaming
            if(selected && Rename_)
            {
                ImGui::SameLine();
                if(FocusRename_) ImGui::SetKeyboardFocusHere();

                if(ImGui::InputText(" ##rename", &RenameBuffer_, ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    if(not RenameBuffer_.empty()) Filesystem_.rename(Selected_, RenameBuffer_);

                    Rename_ = false;
                }

                if(FocusRename_)
                {
                    FocusRename_ = false;
                }
                else if(ImGui::IsItemFocused() == false)
                {
                    Rename_ = false;
                }
            }

            ImGui::TableNextColumn();
            ImGui::Text("%s", file.path().extension().string().c_str());

            ImGui::TableNextColumn();
            ImGui::Text("%s", file.path().string().c_str());
        }

		ImGui::EndTable();
    }
}

bool OpenShaderDesigner::FileManager::AnyDirty()
{
    bool res = false;
    struct Visitor
    {
        bool operator()(File& file, FileID id)
        {
            if(id == FileSystem::root) return false;
            
            res |= file->Dirty();
            return false;
        }

        bool& res;
    } visitor{ res };
    Filesystem_.traverse(visitor);
    return res;
}

void OpenShaderDesigner::FileManager::SaveAll()
{
    
}

OpenShaderDesigner::FileManager::Path OpenShaderDesigner::FileManager::GetHomeDirectory()
{
#ifdef WIN32
    return Path(getenv("HOMEDRIVE")) / getenv("HOMEPATH");
#else
    return getenv("HOME");
#endif
}
