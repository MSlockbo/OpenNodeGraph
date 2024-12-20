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

void Engine::Start()
{
	INIFile File = INIFile(PROJECT_NAME ".ini");
	auto& AppConfig = Settings();
	File.read(AppConfig);
	
	std::string Title   = ApplicationName();
    std::string Version = VersionString();
    Console::Log(Console::Severity::Alert, "Starting {} ({})", Title, Version);

	Window::Configuration Config;
	auto& Application = Config.Application;
	auto& Video       = Config.Video;
	
	Application.Title  = Title;
	Video.Fullscreen   = Window::FullScreenTranslateString.get(AppConfig["video"]["fullscreen"]);
	Video.Maximized    = std::stoi(AppConfig["video"]["maximized"]);
	Video.VSync        = Window::VSyncTranslateString.get(AppConfig["video"]["vsync"]);
	Video.Resolution.x = std::stoi(AppConfig["video"]["resolution_x"]);
	Video.Resolution.y = std::stoi(AppConfig["video"]["resolution_y"]);
	Video.Multisamples = Window::MSAATranslateString.get(AppConfig["video"]["msaa"]);
	Video.HDR          = std::stoi(AppConfig["video"]["hdr"]);

    Console::Log(Console::Message, "Creating Main Window");
    MainWindow_ = new Window(Config);

    Initialize();

    Console::Log(Console::Alert, "Starting Main Loop");
    while(MainWindow_->IsOpen())
    {
        Update();
    }

	Update();

    Shutdown();
}

void Engine::Stop()
{
    MainWindow_->Close();
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

	Time_.Reset();
}

void Engine::Shutdown()
{	
	INIFile File = INIFile(PROJECT_NAME ".ini");
    EditorSystem::Shutdown();
	
	auto&       AppConfig   = Settings();
	const auto& Config      = MainWindow_->Config();
	const auto& Application = Config.Application;
	const auto& Video       = Config.Video;
	const bool  Fullscreen  = Video.Fullscreen != Window::FullscreenMode::WINDOWED;
	
	AppConfig["video"]["fullscreen"]   = Window::FullScreenTranslateEnum.get(Video.Fullscreen);
	AppConfig["video"]["maximized"]    = std::to_string(Fullscreen ? false : Video.Maximized);
	AppConfig["video"]["resolution_x"] = std::to_string(Video.Resolution.x);
	AppConfig["video"]["resolution_y"] = std::to_string(Video.Resolution.y);
	AppConfig["video"]["vsync"]        = Window::VSyncTranslateEnum.get(Video.VSync);
	AppConfig["video"]["msaa"]         = Window::MSAATranslateInt.get(Video.Multisamples() ? 1 : Video.Multisamples);
	AppConfig["video"]["hdr"]          = std::to_string(Video.HDR);

	File.write(AppConfig);

	delete MainWindow_;
}

void Engine::Update()
{
    Runtime_ = Time_.Poll();
    
    Delta_ = Frame_.Poll();
    Frame_.Reset();

    MainWindow_->BeginFrame();

    EditorSystem::Draw();

    MainWindow_->EndFrame();
}
