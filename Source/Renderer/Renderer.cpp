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

#include <Renderer/Renderer.h>

#include "Core/Console.h"

using namespace OpenShaderDesigner;

Renderer::Renderer()
    : EditorWindow("View", 0)
    , Mode_(view_texture)
    , ViewTexture_(nullptr)
    , Shader_(nullptr)
    , RenderTarget_(new HDRTexture::HandleType({ 512, 512 }))
{ }

Renderer::~Renderer()
{
    delete ViewTexture_;
    delete Shader_;
}

void Renderer::DrawMenu()
{
    
}

void Renderer::DrawWindow()
{    
    switch(Mode_)
    {
    default: return;

    case view_texture: DrawTexture(); return;
    case shader: DrawShader(); return;
    }
}

void Renderer::OpenTexture(Texture* texture)
{
    
    ViewTexture_ = texture;
    Mode_ = view_texture;
}

void Renderer::OpenShader(ShaderAsset *shader)
{
    Shader_ = shader;
    Mode_ = Renderer::shader;
}

void Renderer::DrawTexture()
{
    if(ViewTexture_ == nullptr) return;

    ImVec2    reg = ImGui::GetContentRegionAvail();
    glm::vec2 size = (*ViewTexture_)->size();
    float     min_r = glm::min(reg.x / size.x, reg.y / size.y);
    
    ImGui::Image(
        reinterpret_cast<ImTextureID>(static_cast<intptr_t>((*ViewTexture_)->handle())),
        { size.x * min_r, size.y * min_r }
    );
}

void Renderer::DrawShader()
{
    Shader_->View(RenderTarget_);
}
