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

#include <Renderer/Assets/Texture.h>
#include <FileSystem/FileManager.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Core/Console.h"
#include "Editor/EditorSystem.h"
#include "Renderer/Renderer.h"

using namespace OpenShaderDesigner;

RegisterAsset("##/Texture", Texture, ".png", ".jpg", ".bmp");

Texture::Texture(const FileManager::Path &path)
    : Asset(path)
    , Handle_(nullptr)
{
    int width, height, channels;
    uint8_t* pixels = stbi_load(path.string().c_str(), &width, &height, &channels, 0);
    Handle_ = new HandleType({ width, height });
    glw::enum_t layout;

    switch (channels)
    {
    case 1: layout = glw::r; break;
    case 2: layout = glw::rg; break;
    case 3: layout = glw::rgb; break;
    case 4: layout = glw::rgba; break;
    default: layout = glw::r; break;
    }
    
    Handle_->upload(pixels, { width, height }, { 0, 0 }, 0, layout, glw::uint8);
    Handle_->generate_mipmaps();
    stbi_image_free(pixels);
}

Texture::Texture(const FileManager::Path &src, const FileManager::Path &dst)
    : Asset(dst)
    , Handle_(nullptr)
{
    int width, height, channels;
    uint8_t* pixels = stbi_load(src.string().c_str(), &width, &height, &channels, 0);
    Handle_ = new HandleType({ width, height });
    glw::enum_t layout;

    switch (channels)
    {
    case 1:  layout = glw::r8_i;    break;
    case 2:  layout = glw::rg8_i;   break;
    case 3:  layout = glw::rgb8_i;  break;
    case 4:  layout = glw::rgba8_i; break;
    default: layout = glw::r8_i;    break;
    }
    
    
    Handle_->upload(pixels, { width, height }, { 0, 0 }, 0, layout, glw::uint8);
    Handle_->generate_mipmaps();
    stbi_image_free(pixels);
}

Texture::~Texture()
{
    delete Handle_;
}

void Texture::Open()
{
    EditorSystem::Get<Renderer>()->OpenTexture(this);
}

FileManager::Asset* Texture::Create(const FileManager::Path &path)
{
    return nullptr;
}

FileManager::Asset* Texture::Load(const FileManager::Path &path)
{
    Console::Log(Console::Alert, "Loading {}", path.string());
    return new Texture(path);
}

FileManager::Asset * Texture::Import(const FileManager::Path &src, const FileManager::Path &dst)
{
    return new Texture(src, dst);
}
