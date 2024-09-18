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

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <Editor/EditorWindow.h>
#include <FileSystem/Asset.h>

#include <open-cpp-utils/filesystem.h>
#include <open-cpp-utils/map.h>


namespace ocu = open_cpp_utils;

namespace OpenShaderDesigner
{

class FileManager : public EditorWindow
{
public:
    using FileSystem = ocu::filesystem<Asset, FileManager>;
    using File       = FileSystem::file;
    using Path       = std::filesystem::path;
    using FileID     = FileSystem::file_id;
    using FileType   = uint32_t;
    friend FileSystem;

    enum FileType_
    {
        FileType_Folder = 0
    ,   FileType_Project
    };

    inline static ocu::map<std::string, FileType> ExtensionMap = {
        { "",     FileType_Folder }
    ,   { ".sgp", FileType_Project }
    };
    
private:
    static Asset* load(const Path& file);
    static Asset* import(const Path& src, const Path& dst);
    static Asset* create(const Path& file);
    
public:
    FileManager();

    void DrawMenu() override;
    void DrawWindow() override;

    FileID CurrentDirectory() const    { return CurrentDirectory_; }
    void   CurrentDirectory(FileID id) { CurrentDirectory_ = id; }

    FileID Create(const std::string& name) { return Filesystem_.create(name, CurrentDirectory_); }
    FileID Import(const Path& path) { return Filesystem_.import(path, CurrentDirectory_); }
    FileID LoadDirectory(const Path& path) { return Filesystem_.load_directory(path); }
    void   CloseDirectory(FileID dir) { Filesystem_.close_directory(dir); }
    
    FileID Get(const Path& path) const { return Filesystem_.find(path); }
    File&  Get(FileID id) { return Filesystem_[id]; }
    const File& Get(FileID id) const { return Filesystem_[id]; }

    FileID Parent(FileID id) const { return Filesystem_.parent(id); }

    bool AnyDirty();
    void SaveAll();

    static Path GetHomeDirectory();
private:
    FileSystem  Filesystem_;
    FileID      CurrentDirectory_, Selected_;
    bool        Rename_, FocusRename_;
    std::string RenameBuffer_;
};

}


#endif //FILESYSTEM_H
