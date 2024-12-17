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
	void Read(const FileManager::Path &path) override;

    static Asset* Create(const FileManager::Path& path);
    static Asset* Load(const FileManager::Path& path);
    static Asset* Import(const FileManager::Path& src, const FileManager::Path& dst);

    HandleType* operator->() { return Handle_; }
    const HandleType* operator->() const { return Handle_; }
private:
    HandleType* Handle_;
};

class HDRTexture : public FileManager::Asset
{
public:
    using HandleType = glw::texture<glw::texture2D, glw::rgba16f>;

    HDRTexture(const FileManager::Path& path);
    HDRTexture(const FileManager::Path& src, const FileManager::Path& dst);
    ~HDRTexture() override;

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
