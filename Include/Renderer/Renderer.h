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


#ifndef RENDERER_H
#define RENDERER_H

#include <Editor/EditorWindow.h>
#include <Graph/ShaderGraph.h>

#include "glw/shader.h"

namespace OpenShaderDesigner
{

class Renderer : public EditorWindow
{
public:
    enum mode : glw::enum_t
    {
        none = 0
    ,   view_texture
    ,   shader
    };
    
    Renderer();
    virtual ~Renderer();

    void DrawMenu() override;
    void DrawWindow() override;

    void OpenTexture(Texture* texture);
    void OpenShader(ShaderAsset* shader);

    
private:	
    void DrawTexture();
    void DrawShader();

    glw::enum_t  Mode_;
    Texture* ViewTexture_;
    HDRTexture::HandleType* RenderTarget_;
    ShaderAsset* Shader_;
};
	
}



#endif //RENDERER_H
