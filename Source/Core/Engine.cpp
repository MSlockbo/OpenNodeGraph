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

#include <Renderer/Renderer.h>

#include <FileSystem/FileManager.h>

#include <Graph/ShaderGraph.h>

#include "Project/Project.h"

using namespace OpenShaderDesigner;

void Engine::Start(const Window::Configuration& config)
{
    std::string version = VersionString();
    Console::Log(Console::Severity::Alert, "Starting {} ({})", config.Application.Title, version);

    Console::Log(Console::Message, "Creating Main Window");
    MainWindow = new Window(config);

    Initialize();

    Console::Log(Console::Message, "Starting Main Loop");
    while(MainWindow->IsOpen())
    {
        Update();
    }

    Shutdown();
}

void Engine::Stop()
{
    MainWindow->Close();
}

void Engine::Initialize()
{
    Console::Log(Console::Message, "Initializing Engine");

    Console::Log(Console::Message, "Initializing Editor");
    EditorSystem::Initialize();

    Console::Log(Console::Message, "Opening Console");
    EditorSystem::Open<ConsoleWindow>();

    Console::Log(Console::Message, "Opening Profiler");
    EditorSystem::Open<Profiler>();

    Console::Log(Console::Message, "Opening File Manager");
    EditorSystem::Open<FileManager>();

    Console::Log(Console::Message, "Opening Shader Graph");
    EditorSystem::Open<Inspector>();
    EditorSystem::Open<ShaderGraph>();

    Console::Log(Console::Message, "Opening Renderer");
    EditorSystem::Open<Renderer>();

    Console::Log(Console::Message, "Setting up Project");
    EditorSystem::SetMainMenuBar<Project>();

    
    FileManager* filesystem = EditorSystem::Get<FileManager>();
    filesystem->LoadDirectory("./Test/");
}

void Engine::Shutdown()
{
    EditorSystem::Shutdown();
}

void Engine::Update()
{
    _Delta = Frame.Poll();
    Frame.Reset();

    MainWindow->BeginFrame();

    EditorSystem::Draw();

    MainWindow->EndFrame();
}
