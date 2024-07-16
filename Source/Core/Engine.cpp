//
// Created by Maddie on 6/20/2024.
//

#include <Core/Console.h>
#include <Core/Engine.h>
#include <Editor/EditorSystem.h>

#include <Editor/ConsoleWindow.h>
#include <Editor/Profiler.h>

#include <Graph/ShaderGraph.h>

void OpenShaderDesigner::Engine::Start(const Window::Configuration& config)
{
    Console::Log(Console::Severity::ALERT, "Starting {}", config.Application.Title);

    Console::Log(Console::Severity::MESSAGE, "Creating Main Window");
    MainWindow = new Window(config);

    Initialize();

    Console::Log(Console::Severity::MESSAGE, "Starting Main Loop");
    while(MainWindow->IsOpen())
    {
        Update();
    }

    Shutdown();
}

void OpenShaderDesigner::Engine::Stop()
{
    MainWindow->Close();
}

void OpenShaderDesigner::Engine::Initialize()
{
    Console::Log(Console::Severity::MESSAGE, "Initializing Engine");

    Console::Log(Console::Severity::MESSAGE, "Initializing Editor");
    EditorSystem::Initialize();

    Console::Log(Console::Severity::MESSAGE, "Opening Console");
    EditorSystem::Open<ConsoleWindow>();

    Console::Log(Console::Severity::MESSAGE, "Opening Profiler");
    EditorSystem::Open<Profiler>();

    Console::Log(Console::Severity::MESSAGE, "Opening Shader Graph");
    EditorSystem::Open<ShaderGraph>();
}

void OpenShaderDesigner::Engine::Shutdown()
{
    EditorSystem::Shutdown();
}

void OpenShaderDesigner::Engine::Update()
{
    _Delta = Frame.Poll();
    Frame.Reset();

    MainWindow->BeginFrame();

    EditorSystem::Draw();

    MainWindow->EndFrame();
}
