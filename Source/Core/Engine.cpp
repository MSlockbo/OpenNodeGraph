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
