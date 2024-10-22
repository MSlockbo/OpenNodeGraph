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
