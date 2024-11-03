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

#ifndef PROJECT_H
#define PROJECT_H

#include <Editor/MainMenuBar.h>
#include <FileSystem/FileManager.h>

namespace OpenShaderDesigner
{
    
class Project : public MainMenuBar, public FileManager::Asset
{
public:
    Project();
    virtual ~Project();

    void DrawMenuBar() override;

    void Open() override;
    void Save(const FileManager::Path& path) override;

    static Asset* Create(const FileManager::Path& path);
    static Asset* Load(const FileManager::Path& path);
    static Asset* Import(const FileManager::Path& src, const FileManager::Path& dst);

private:
    void Reset();
    
    FileManager::FileID ProjectFile_;
};
    
}

#endif //PROJECT_H
