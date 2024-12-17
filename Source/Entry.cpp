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

#include <iostream>
#include <Core/Engine.h>
#include <Editor/EditorSystem.h>

int main(int, char**)
{
    using namespace OpenShaderDesigner;

    Window::Configuration config;
    config.Application.Title = "OpenShaderDesigner";
    //config.Video.Multisamples = 16;
    config.Video.Fullscreen = Window::FullscreenMode::FULLSCREEN;
    //config.Video.Resolution = { 1280, 720 };
    config.Video.HDR = true;

    Engine::Start(config);

    return 0;
}
