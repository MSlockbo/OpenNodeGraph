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

#ifndef TEXTURE_H
#define TEXTURE_H

#include <glw/texture.h>

#include "FileSystem/FileManager.h"

namespace OpenShaderDesigner
{

class Texture : public FileManager::Asset
{
public:
    using HandleType = glw::texture<glw::texture2D, glw::rgba8>;
    
    Texture(const FileManager::Path& path);
    Texture(const FileManager::Path& src, const FileManager::Path& dst);
    ~Texture() override;

    void Open() override;

    static Asset* Create(const FileManager::Path& path);
    static Asset* Load(const FileManager::Path& path);
    static Asset* Import(const FileManager::Path& src, const FileManager::Path& dst);

    HandleType* operator->() { return Handle_; }
    const HandleType* operator->() const { return Handle_; }
private:
    HandleType* Handle_;
};
    
}

#endif //TEXTURE_H
