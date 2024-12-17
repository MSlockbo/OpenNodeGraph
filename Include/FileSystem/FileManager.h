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

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <Editor/EditorWindow.h>

#include <open-cpp-utils/filesystem.h>
#include <open-cpp-utils/map.h>
#include <open-cpp-utils/startup.h>

#define RegisterAsset(Name, Type, ...) \
    STARTUP(_RegisterAsset##Type) { FileManager::Register(Name, { __VA_ARGS__ }, ##Type::Create, ##Type::Load, ##Type::Import); }

namespace ocu = open_cpp_utils;

namespace OpenShaderDesigner
{

class FileManager : public EditorWindow
{
public:
    class Asset;
    using FileSystem = ocu::filesystem<Asset, FileManager>;
    using File       = FileSystem::file;
    using Path       = std::filesystem::path;
    using FileID     = FileSystem::file_id;
    using CreateFunc = Asset* (*)(const Path&);
    using LoadFunc   = Asset* (*)(const Path&);
    using ImportFunc = Asset* (*)(const Path&, const Path&);
    friend FileSystem;

private:
    struct AssetDetail
    {
        std::string              Name;
        std::vector<std::string> Extensions;
        CreateFunc               Create;
        LoadFunc                 Load;
        ImportFunc               Import;

        AssetDetail() : Create(nullptr), Load(nullptr), Import(nullptr) { }
        AssetDetail(const std::string& name) : Name(name), Create(nullptr), Load(nullptr), Import(nullptr) {}

        
        AssetDetail(const std::string& name, const std::vector<std::string>& exts,
                    const CreateFunc create, const LoadFunc load, const ImportFunc import)
        : Name(name), Extensions(exts), Create(create), Load(load), Import(import) {}
    };

    using AssetMenuHierarchy = ocu::directed_tree<AssetDetail>;
    using AssetType          = AssetMenuHierarchy::node;
    using ExtensionMapping   = ocu::map<std::string, AssetType>;
    
    static AssetMenuHierarchy& AssetMenu() { static AssetMenuHierarchy Menu; return Menu; }
    static ExtensionMapping& ExtensionMap() { static ExtensionMapping Map; return Map; }
    
public:

    class Asset
    {
    public:
        Asset(const Path& path) : Dirty_(false) { }
        virtual ~Asset() = default;
    
        bool Dirty() const { return Dirty_; }

        virtual void Open() { };
        virtual void Write(const Path& path) { Dirty_ = false; }
    	virtual void Read(const Path& path) { Dirty_ = false; }

        File& GetFile() { return Manager_->Get(File_); }
        FileID GetID() const { return File_; }

    protected:
        void MakeDirty() { Dirty_ = true; }
        FileManager* Parent() const { return Manager_; }

    private:
        FileManager* Manager_;
        FileID File_;
        bool Dirty_;
        friend FileManager;
    };

    struct Folder : Asset
    {
        Folder(const std::filesystem::path& p) : Asset(p) { };
        virtual ~Folder() = default;

        void Open() override { Manager_->CurrentDirectory_ = GetID(); }
    };
    
private:
    static Asset* load(const Path& file, FileID id);
    static Asset* import(const Path& src, const Path& dst, FileID id);
    static Asset* create(const Path& file, FileID id);
    
public:
    FileManager();
    virtual ~FileManager() = default;

    void DrawMenu() override;
    void DrawWindow() override;

    FileID CurrentDirectory() const    { return CurrentDirectory_; }
    void   CurrentDirectory(FileID id) { CurrentDirectory_ = id; }

    FileID Create(const std::string& name) { return Filesystem_.create(name, CurrentDirectory_); }
    FileID Import(const Path& path)        { return Filesystem_.import(path, CurrentDirectory_); }
    FileID LoadDirectory(const Path& path) { return Filesystem_.load_directory(path); }
    void   CloseDirectory(FileID dir)      { Filesystem_.close_directory(dir); }

	void Save(FileID file);
    
    FileID      Get(const Path& path) const { return Filesystem_.find(path); }
    File&       Get(FileID id)              { return Filesystem_[id]; }
    const File& Get(FileID id) const        { return Filesystem_[id]; }

    FileID Parent(FileID id) const { return Filesystem_.parent(id); }

    bool AnyDirty();
    void SaveAll();

    static Path GetHomeDirectory();
	static void Register(const std::filesystem::path& path,
	                     const std::vector<std::string>& extension,
	                     CreateFunc create, LoadFunc load, ImportFunc import);
private:
    FileSystem  Filesystem_;
    FileID      CurrentDirectory_, Selected_;
    bool        Rename_, FocusRename_;
    std::string RenameBuffer_;
};

}


#endif //FILESYSTEM_H
