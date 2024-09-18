//
// Created by Maddie on 9/14/2024.
//

#ifndef MAINMENUBAR_H
#define MAINMENUBAR_H

#include <imgui-docking/imgui.h>

namespace OpenShaderDesigner
{

class MainMenuBar
{
public:
    virtual void DrawMenuBar() = 0;

    friend class EditorSystem;

private:
    void Draw() { ImGui::BeginMainMenuBar(); DrawMenuBar(); ImGui::EndMainMenuBar(); }
};
    
}

#endif //MAINMENUBAR_H
