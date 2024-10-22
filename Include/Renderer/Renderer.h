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


#ifndef RENDERER_H
#define RENDERER_H

#include <Editor/EditorWindow.h>

#include <Renderer/Assets/Texture.h>

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
    ,   function
    };
    
    Renderer();
    virtual ~Renderer();

    void DrawMenu() override;
    void DrawWindow() override;

    void OpenTexture(Texture* texture);
    
private:
    void DrawTexture();
    void DrawFunction();

    glw::enum_t  Mode_;
    Texture* ViewTexture_;
    glw::shader* Shader_;
};
	
}



#endif //RENDERER_H
