//
// Created by Maddie on 6/18/2024.
//
#include <iostream>
#include <Core/Engine.h>

int main(int, char**)
{
    using namespace OpenShaderDesigner;

    Window::Configuration config;
    config.Application.Title = "OpenShaderDesigner";
    config.Video.HDR = true;

    Engine::Start(config);

    return 0;
}