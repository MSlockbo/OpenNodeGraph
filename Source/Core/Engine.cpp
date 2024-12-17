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

    Console::Log(Console::Alert, "Starting Main Loop");
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

	Console::Log(Console::Message, "Opening Shader Graph");
	EditorSystem::Open<ShaderGraph>();
	EditorSystem::Open<Inspector>();

    Console::Log(Console::Message, "Opening Renderer");
	EditorSystem::Open<Renderer>();

	Console::Log(Console::Message, "Opening File Manager");
	EditorSystem::Open<FileManager>();

    Console::Log(Console::Message, "Setting up Project");
    EditorSystem::SetMainMenuBar<Project>();

    
    FileManager* filesystem = EditorSystem::Get<FileManager>();
    FileManager::FileID root = filesystem->LoadDirectory("./Test/");
    filesystem->CurrentDirectory(root);

	Time.Reset();
}

void Engine::Shutdown()
{
    EditorSystem::Shutdown();
}

void Engine::Update()
{
    _Runtime = Time.Poll();
    
    _Delta = Frame.Poll();
    Frame.Reset();

    MainWindow->BeginFrame();

    EditorSystem::Draw();

    MainWindow->EndFrame();
}
