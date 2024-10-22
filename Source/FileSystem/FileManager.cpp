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

using namespace OpenShaderDesigner;

FileManager::Asset* FileManager::load(const Path& path, FileID id)
{
    Console::Log(Console::Message, "Loading File {}", path.string());

    // Create Folder dummy Asset for Folders
    if(is_directory(path))
    {
        Asset* asset = new Folder(path);
        asset->Manager_ = EditorSystem::Get<FileManager>();
        asset->File_ = id;
        return asset;
    }

    // Get the type and load the asset
    AssetType type = ExtensionMap()[path.extension().string()];
    Asset* asset = AssetMenu()[type].Load(path);
    
    // Set the ID and Manager of the file
    if(asset)
    {
        asset->Manager_ = EditorSystem::Get<FileManager>();
        asset->File_ = id;
    }
    return asset;
}

FileManager::Asset* FileManager::import(const Path &src, const Path &dst, FileID id)
{
    Console::Log(Console::Message, "Importing File {} to {}", src.string(), dst.string());
    
    // Get the type and load the asset
    AssetType type = ExtensionMap()[src.extension().string()];
    Asset* asset = AssetMenu()[type].Import(src, dst);
    
    if(asset)
    {
        asset->Manager_ = EditorSystem::Get<FileManager>();
        asset->File_ = id;
    }
    
    return asset;
}

FileManager::Asset* FileManager::create(const Path &path, FileID id)
{
    Console::Log(Console::Message, "Creating File {}", path.string());

    // Create Folder dummy Asset for Folders
    if(is_directory(path))
    {
        Asset* asset = new Folder(path);
        asset->Manager_ = EditorSystem::Get<FileManager>();
        return asset;
    }
    
    // Get the type and load the asset
    AssetType type = ExtensionMap()[path.extension().string()];
    Asset* asset = AssetMenu()[type].Create(path);
    
    if(asset)
    {
        asset->Manager_ = EditorSystem::Get<FileManager>();
        asset->File_ = id;
    }
    
    return asset;
}

FileManager::FileManager()
	: EditorWindow("File Manager", ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar)
    , CurrentDirectory_(FileSystem::root), Selected_(FileSystem::root)
    , Rename_(false), FocusRename_(false)
{
	//CurrentDirectory_ = Filesystem_.load_directory(std::filesystem::current_path());
}

void FileManager::DrawMenu()
{
	if(ImGui::BeginMenu("File"))
	{
        if(ImGui::BeginMenu("Create"))
        {
            struct Visitor
            {
                bool operator()(AssetDetail& item, AssetType id)
                {
                    AssetMenuHierarchy& AssetMenu = FileManager::AssetMenu();
                    const auto depth = AssetMenu.depth(id);
                    if(depth > Context.size()) return false;
                    if(item.Name == "##") return false;

                    while(depth < Context.size())
                    {
                        Context.pop();
                        ImGui::EndMenu();
                    }

                    if(Context.top() != AssetMenu.parent(id)) return false;
                    std::string name = std::format("{}##{}", item.Name, id);

                    if(item.Create != nullptr)
                    {
                        if(ImGui::MenuItem(name.c_str()))
                        {
                            Manager.Create(std::format("{}{}", item.Name, item.Extensions[0]));
                        }
                    }
                    else
                    {
                        if(ImGui::BeginMenu(name.c_str()))
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

                FileManager& Manager;
                std::stack<AssetType> Context;
            } MenuVisitor
            {
                .Manager = *this
            };

            MenuVisitor.Context.push(0);

            AssetMenu().traverse<AssetMenuHierarchy::pre_order>(MenuVisitor);

            MenuVisitor.Context.pop();
            while(MenuVisitor.Context.empty() == false)
            {
                ImGui::EndMenu();
                MenuVisitor.Context.pop();
            }

            ImGui::EndPopup();
        }
	    
	    ImGui::EndMenu();
	}
}

void FileManager::DrawWindow()
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
			    
			    if(id == FileSystem::root) return false; // Skip root
			    if(*file == nullptr) return false; // Skip files without assets

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
            
            if(ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && selected)
            {
                File& file = Filesystem_[Selected_];
                Asset* asset = *file;
                asset->Open();
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

bool FileManager::AnyDirty()
{
    bool res = false;
    struct Visitor
    {
        bool operator()(File& file, FileID id)
        {
            if(id == FileSystem::root) return false; // Continue if root node
            if(*file == nullptr) return false; // Continue if no asset associated with file
            
            res |= file->Dirty();
            return false;
        }

        bool& res;
    } visitor{ res };
    Filesystem_.traverse(visitor);
    return res;
}

void FileManager::SaveAll()
{
    
}

FileManager::Path FileManager::GetHomeDirectory()
{
#ifdef WIN32
    return Path(getenv("HOMEDRIVE")) / getenv("HOMEPATH");
#else
    return getenv("HOME");
#endif
}

void FileManager::Register(const std::filesystem::path& path,
                           const std::vector<std::string>& extensions,
                           CreateFunc create, LoadFunc load, ImportFunc import)
{
    const std::string name = path.filename().string();
    AssetMenuHierarchy&  AssetMenu = FileManager::AssetMenu();
    ExtensionMapping& ExtensionMap = FileManager::ExtensionMap();

    AssetType node = 0;
    for(auto it = path.begin(); it != path.end();)
    {
        AssetType child = AssetMenu.first_child(node);

        while(child)
        {
            if(AssetMenu[child].Name == it->filename())
            {
                node = child;
                ++it;
                break;
            }

            child = AssetMenu.next_sibling(child);
        }

        if(node == 0 || node != child)
        {
            node = AssetMenu.insert({ it->string(), {}, nullptr, nullptr, nullptr }, node);
            ++it;
        }
    }

    AssetMenu[node].Create = create;
    AssetMenu[node].Import = import;
    AssetMenu[node].Load   = load;
    AssetMenu[node].Extensions = extensions;
    
    for(const auto& ext : extensions)
    {
        ExtensionMap[ext] = node;
    }
}






























































