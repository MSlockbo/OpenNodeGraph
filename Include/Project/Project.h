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
