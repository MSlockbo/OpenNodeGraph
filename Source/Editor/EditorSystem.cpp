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
#include <Editor/EditorSystem.h>
#include <Core/Engine.h>

#include <imgui-docking/imgui_internal.h>
#include <imgui-docking/backends/imgui_impl_sdl3.h>
#include <imgui-docking/backends/imgui_impl_opengl3.h>
#include <imgui-docking/misc/freetype/imgui_freetype.h>

#include <imnode-graph/imnode_graph.h>

#define END_LINE "\n"

using namespace OpenShaderDesigner;

void EditorSystem_ReadOpen(ImGuiContext*, ImGuiSettingsHandler*, const char* name)
{
	
}

void EditorSystem::Initialize()
{
	Window& Window = Engine::GetMainWindow();

	// Initialize ImGui
	Console::Log(Console::Severity::Alert, "Creating ImGui Context");
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Set Style
	Console::Log(Console::Message, "Setting Editor Style");
	
	// https://github.com/ocornut/imgui/issues/707#issuecomment-917151020
	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg]               = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg]                = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
	colors[ImGuiCol_Border]                 = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
	colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
	colors[ImGuiCol_FrameBg]                = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
	colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
	colors[ImGuiCol_FrameBgActive]          = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
	colors[ImGuiCol_TitleBg]                = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TitleBgActive]          = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
	colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
	colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
	colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
	colors[ImGuiCol_CheckMark]              = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
	colors[ImGuiCol_SliderGrab]             = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
	colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
	colors[ImGuiCol_Button]                 = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
	colors[ImGuiCol_ButtonHovered]          = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
	colors[ImGuiCol_ButtonActive]           = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
	colors[ImGuiCol_Header]                 = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_HeaderHovered]          = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
	colors[ImGuiCol_HeaderActive]           = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
	colors[ImGuiCol_Separator]              = ImVec4(0.40f, 0.40f, 0.40f, 0.29f);
	colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
	colors[ImGuiCol_SeparatorActive]        = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
	colors[ImGuiCol_ResizeGrip]             = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
	colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
	colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
	colors[ImGuiCol_Tab]                    = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TabHovered]             = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_TabActive]              = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
	colors[ImGuiCol_TabUnfocused]           = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_DockingPreview]         = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
	colors[ImGuiCol_DockingEmptyBg]         = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotLines]              = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogram]          = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TableBorderLight]       = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
	colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
	colors[ImGuiCol_DragDropTarget]         = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
	colors[ImGuiCol_NavHighlight]           = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowPadding                     = ImVec2(8.00f, 8.00f);
	style.FramePadding                      = ImVec2(5.00f, 2.00f);
	style.CellPadding                       = ImVec2(6.00f, 6.00f);
	style.ItemSpacing                       = ImVec2(6.00f, 6.00f);
	style.ItemInnerSpacing                  = ImVec2(6.00f, 6.00f);
	style.TouchExtraPadding                 = ImVec2(0.00f, 0.00f);
	style.IndentSpacing                     = 25;
	style.ScrollbarSize                     = 15;
	style.GrabMinSize                       = 10;
	style.WindowBorderSize                  = 1;
	style.ChildBorderSize                   = 1;
	style.PopupBorderSize                   = 1;
	style.FrameBorderSize                   = 1;
	style.TabBorderSize                     = 1;
	style.WindowRounding                    = 7;
	style.ChildRounding                     = 4;
	style.FrameRounding                     = 3;
	style.PopupRounding                     = 4;
	style.ScrollbarRounding                 = 9;
	style.GrabRounding                      = 3;
	style.LogSliderDeadzone                 = 4;
	style.TabRounding                       = 4;

	// Load Fonts
	Console::Log(Console::Message, "Loading Editor Fonts");

	// Import Base Font
	ImGuiIO& io = ImGui::GetIO();
	static ImWchar ranges[] = { 0x1, static_cast<ImWchar>(0x1FFFF), 0 };
    static ImFontConfig cfg;
    io.Fonts->AddFontFromFileTTF("./Assets/Fonts/FiraMono-Regular.ttf", 20.0f, nullptr, ranges);

	// Import Icon Font
    cfg.MergeMode = true;
    cfg.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags_LoadColor;
	io.Fonts->AddFontFromFileTTF("./Assets/Fonts/remixicon.ttf", 18.0f, &cfg, ranges);

	// Set Docking Settings
	io.ConfigWindowsMoveFromTitleBarOnly = true;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	// Setup ImNodeGraph
	Console::Log(Console::Message, "Adding Fonts to ImNodeGraph");
    ImNodeGraph::AddFont("./Assets/Fonts/FiraMono-Regular.ttf", 20.0f, ranges);
    ImNodeGraph::AddFont("./Assets/Fonts/remixicon.ttf", 18.0f, ranges);

	Console::Log(Console::Message, "Creating ImNodeGraph Context");
    ImNodeGraph::CreateContext();

	// Initiialize Backend
	Console::Log(Console::Message, "Initializing ImGui Backend");
	ImGui_ImplSDL3_InitForOpenGL(Window.GetHandle(), Window.GetContext());
	ImGui_ImplOpenGL3_Init("#version 460 core");
	
	// Setup Framebuffer
	glm::ivec2 size = Engine::GetMainWindow().Size();
	DrawBuffer_ = new FrameBuffer(size);
	DrawBuffer_->build();

	glw::enum_t status = DrawBuffer_->get_status();
	Console::Log(Console::Message, "Framebuffer built, status: \"{}\"", glw::translate_framebuffer_status(status));

	// Setup color correction shader
	ColorCorrection_ = new glw::shader();
	if(not ColorCorrection_->attach_source(glw::compute, {
		"#version 460 core"															END_LINE
	,	glw::shader::built_ins_compute,												END_LINE
	,	"layout (local_size_x = 8, local_size_y = 8, local_size_z = 8) in;"			END_LINE
		"layout (rgba16f, binding = 0) coherent restrict uniform image2D Color;"	END_LINE
		"uniform float Whitepoint = 1.0, Gamma = 2.2;"								END_LINE
		"void main(void)"															END_LINE
		"{"																			END_LINE
		"	ivec2 pixel = ivec2(gl_GlobalInvocationID.xy);"							END_LINE
		"	uvec2 size  = glw_RequestedInvocations.xy;"								END_LINE
		"	if(pixel.x >= size.x || pixel.y >= size.y) return;"						END_LINE
		"	vec3 val = imageLoad(Color, pixel).rgb;"								END_LINE
		"	val = pow(val, vec3(Gamma)) * Whitepoint;"								END_LINE
		"	imageStore(Color, pixel, vec4(val, 1));"								END_LINE
		"}"																			END_LINE
	}))
	{
		Console::Log(
			Console::Severity::Fatal
		,	"Color Correction Compile Failed:" END_LINE "{}"
		,	ColorCorrection_->get_error_string()
		);
	}
	
	if(not ColorCorrection_->link())
	{
		Console::Log(
			Console::Severity::Fatal
		,	"Color Correction Link Failed:" END_LINE "{}"
		,	ColorCorrection_->get_error_string()
		);
	}

	auto& Settings = Engine::Settings();
	Whitepoint_ = std::stof(Settings["graphics"]["whitepoint"]);
	Gamma_      = std::stof(Settings["graphics"]["gamma"]);

	WhitepointDisplay_ = new HDRTexture({ 2, 1 });
	WhitepointDisplay_->set_mag_filter(glw::nearest);
	RebuildWhitepointDisplay_();
	
	// Alert we have initialized ImGui
	Console::Log(Console::Severity::Alert, "Initialized ImGui ({})", IMGUI_VERSION);
}

void EditorSystem::Draw()
{
	const auto& Window = Engine::GetMainWindow();
	
	if(DrawBuffer_)
	{
		DrawBuffer_->resize(Window.Size());
		DrawBuffer_->bind();
	}
	
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL3_NewFrame();

	ImGui::NewFrame();
	ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

	if(ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		ImGui::ResetMouseDragDelta();
	}

    if(MainMenuBar_)
    {
        MainMenuBar_->Draw();
    }

	ImGui::ShowDemoWindow();

	for (int i = 0; i < MAX_EDITORS; ++i)
	{
		EditorWindow* editor = Windows_[i];
		if(editor == nullptr || !editor->IsOpen()) continue;
		editor->Draw();
	}
	
	if(OpenCalibration_)
	{
		ImGui::OpenPopup("HDR Calibration");
	}
	
	if(ImGui::BeginPopupModal("HDR Calibration", &OpenCalibration_, ImGuiWindowFlags_None))
	{
		float Rescale = Whitepoint_ * 100.0f;
		if(ImGui::SliderFloat("White Point", &Rescale, 0.0f, 1000.0f, "%.0f"))
		{
			Whitepoint_ = Rescale / 100.0f;
			RebuildWhitepointDisplay_();
		}
		
		ImGui::SliderFloat("Gamma", &Gamma_, 0.0f, 10.0f);
		
		ImVec2 Avail = ImGui::GetContentRegionAvail();
		float  Width = Avail.x;
		if(Width / 2 > Avail.y) Width = Avail.y * 2;
		
		ImGui::Image(
			reinterpret_cast<ImTextureID>(static_cast<intptr_t>(WhitepointDisplay_->handle()))
		,	Avail
		);

		ImGui::EndPopup();
	}

	ImGui::Render();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if(DrawBuffer_)
	{
		if(Window.Config().Video.HDR && not OpenCalibration_)
		{
			ColorCorrection_->bind();
			ColorCorrection_->operator[]("Whitepoint") = Whitepoint_;
			ColorCorrection_->operator[]("Gamma") = Gamma_;
			DrawBuffer_->get<1>().bind_image(0, glw::access_read_write);
			ColorCorrection_->dispatch(Window.Size().x, Window.Size().y, 1);
		}
		
		DrawBuffer_->blit(
			glw::color_attachment0
		,	glw::default_framebuffer, glw::back_buffer
		,	{ 0, 0 }, Window.Size()
		,	{ 0, 0 }, Window.Size()
		,	glw::nearest
		);
	}
}

void EditorSystem::OpenHDRCalibration()
{
	OpenCalibration_ = true;
}

void EditorSystem::RebuildWhitepointDisplay_()
{
	glm::vec3 Pixels[] = { glm::vec3(Whitepoint_), glm::vec3(10.0f) };

	WhitepointDisplay_->upload(Pixels, { 2, 1 }, { 0, 0 }, 0, glw::rgb, glw::float32);
}

void EditorSystem::Shutdown()
{
    delete MainMenuBar_;
    for(auto it : Windows_) delete it;
    
	// Shutdown ImGui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL3_Shutdown();

    ImNodeGraph::DestroyContext();
	ImGui::DestroyContext();

	auto& Settings = Engine::Settings();
	Settings["graphics"]["whitepoint"] = std::to_string(Whitepoint_);
	Settings["graphics"]["gamma"]      = std::to_string(Gamma_);
}

void EditorSystem::HandleEvents(SDL_Event* event)
{
	ImGui_ImplSDL3_ProcessEvent(event);
}
