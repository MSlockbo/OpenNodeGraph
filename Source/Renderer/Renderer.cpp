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

#include <Renderer/Renderer.h>

#include "Core/Console.h"

using namespace OpenShaderDesigner;

Renderer::Renderer()
    : EditorWindow("View", 0)
    , Mode_(view_texture)
    , ViewTexture_(nullptr)
    , Shader_(new glw::shader())
{
    if(not Shader_->attach_source(glw::compute, {
        "#version 430 core\n"
        "\n"
        "layout (local_size_x = 8, local_size_y = 8, local_size_z = 1) in;\n"
        "\n"
        "layout (rgba32f, binding = 0) uniform restrict image2D out_Colour0;\n"
        "\n"
        "void main(void)\n"
        "{\n"
        "    imageStore(out_Colour0, ivec2(gl_GlobalInvocationID.xy), vec4(1, 0, 0, 1));\n"
        "}\n"
    }))
    {
        Console::Log(Console::Error, "Failed to compile shader: \n{}", Shader_->get_error_string());
    }

    if(not Shader_->link())
    {
        Console::Log(Console::Error, "Failed to link shader: \n{}", Shader_->get_error_string());
    }
}

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

    case view_texture:
        DrawTexture();
        return;
    }
}

void Renderer::OpenTexture(Texture* texture)
{
    ViewTexture_ = texture;
    Mode_ = view_texture;
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

void Renderer::DrawFunction()
{
    
}
