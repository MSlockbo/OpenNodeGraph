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

#include <Core/Window.h>
#include <Core/Console.h>
#include <Editor/EditorSystem.h>

#include <gl/glew.h>

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_version.h>

#include "glw/debug.h"

using namespace OpenShaderDesigner;

static Console::Severity GLTranslateSeverity(glw::enum_t v)
{
	switch(v)
	{
	case glw::debug::severity_low:          return Console::Severity::Warning;
	case glw::debug::severity_medium:       return Console::Severity::Error;
	case glw::debug::severity_high:         return Console::Severity::Fatal;
	case glw::debug::severity_notification: return Console::Severity::Alert;
	}
}

static void GLMessageCallback(
	glw::enum_t source
,	glw::enum_t type
,	glw::handle_t id
,	glw::enum_t severity
,	glw::size_t length, const char* msg
,	const void* user_data)
{
	Console::Log(GLTranslateSeverity(severity), "{}", msg);
}


Window::Window(const Configuration& config)
    : Config_(config)
{
	int flags = static_cast<int>(Config_.Video.Fullscreen) | SDL_WINDOW_OPENGL;
	flags |= Config_.Video.Fullscreen == FullscreenMode::WINDOWED ? SDL_WINDOW_RESIZABLE : 0;
	flags |= Config_.Video.Maximized ? SDL_WINDOW_MAXIMIZED : 0; 
	
	SDL_Init(
		SDL_INIT_VIDEO
	|	SDL_INIT_JOYSTICK
	|   SDL_INIT_GAMEPAD
	|   SDL_INIT_HAPTIC
	);
    
#ifdef NDEBUG
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG | SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
#endif

    // Set OpenGL APIVersion
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	int num_displays = 0;
	SDL_DisplayID* displays = SDL_GetDisplays(&num_displays);
	const SDL_DisplayMode* mode = SDL_GetDesktopDisplayMode(displays[0]);
	SDL_free(displays);
	if(mode == nullptr)
	{
		Console::Log(Console::Severity::Fatal, "Failed to get desktop settings: {}", SDL_GetError());
		assert(false);
		return;
	}

	if(Config_.Video.Resolution.x <= 0)
	{
		Config_.Video.Resolution.x = mode->w;
		if(Config_.Video.Fullscreen == FullscreenMode::WINDOWED) Config_.Video.Resolution.x /= 2;
	}

	if(Config_.Video.Resolution.y <= 0)
	{
		Config_.Video.Resolution.y = mode->h;
		if(Config_.Video.Fullscreen == FullscreenMode::WINDOWED) Config_.Video.Resolution.y /= 2;
	}
	
    if(Config_.Video.HDR)
    {
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 16);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 16);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 16);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 16);
        SDL_GL_SetAttribute(SDL_GL_FLOATBUFFERS, SDL_TRUE);
    }

    if(Config_.Video.Multisamples() && Config_.Video.Multisamples > 1)
    {
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, SDL_TRUE);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, Config_.Video.Multisamples);
    }

    if((Handle_ = SDL_CreateWindow(
        Config_.Application.Title.c_str(),
        Config_.Video.Resolution.x, Config_.Video.Resolution.y,
        flags)) == nullptr)
    {
        Console::Log(Console::Severity::Fatal, "Failed to create SDL Window: {}", SDL_GetError());
        assert(false);
        return;
    }

    Context_ = SDL_GL_CreateContext(Handle_);

    if(Context_ == nullptr)
    {
        Console::Log(Console::Severity::Fatal, "Failed to create OpenGL Context: {}", SDL_GetError());
        SDL_DestroyWindow(Handle_);
        assert(false);
        return;
    }

	// Check if HDR was setup correctly
	if(Config_.Video.HDR)
	{
		auto id  = SDL_GetWindowProperties(Handle_);
		bool hdr = SDL_GetBooleanProperty(id, SDL_PROP_WINDOW_HDR_ENABLED_BOOLEAN, false);
        Console::Log(Console::Severity::Message, "HDR Enabled: {}", hdr);
		Config_.Video.HDR = hdr;
	}

    if(not SDL_GL_MakeCurrent(Handle_, Context_))
    {
        Console::Log(Console::Severity::Fatal, "Failed to set OpenGL Context: {}", SDL_GetError());
        SDL_GL_DeleteContext(Context_);
        SDL_DestroyWindow(Handle_);
        assert(false);
        return;
    }

    // Set VSync Mode
    SDL_GL_SetSwapInterval(static_cast<int>(Config_.Video.VSync));

    // Enable experimental features
    glewExperimental = GL_TRUE;

    // Initialize GLEW
    GLenum error;
    if((error = glewInit()) != GLEW_OK)
    {
        Console::Log(Console::Severity::Fatal, "Failed to Initialize GLEW: {}", reinterpret_cast<const char*>(glewGetErrorString(error)));
        SDL_GL_DeleteContext(Context_);
        SDL_DestroyWindow(Handle_);
        assert(false);
        return;
    }

	// Setup GL Debugger
	glw::debug::set_debug_callback(GLMessageCallback, nullptr);

    // Fill in black screen
    glEnable(GL_MULTISAMPLE);
    glDisable(GL_DEPTH_TEST);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    SDL_GL_SwapWindow(Handle_);

    // Set running
    Open_ = true;

    const char* glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    const char* glewVersion = reinterpret_cast<const char*>(glewGetString(GLEW_VERSION));
    int sdlVersion = SDL_GetVersion();
	
	Console::Log(Console::Severity::Alert, "Initialized SDL ({}.{}.{})",
				 SDL_VERSIONNUM_MAJOR(sdlVersion), SDL_VERSIONNUM_MAJOR(sdlVersion), SDL_VERSIONNUM_MAJOR(sdlVersion)
	);
	
    Console::Log(
    	Console::Severity::Alert, "Running OpenGL ({} - {}), GLEW ({})"
    ,	glVersion
    ,	SDL_GetCurrentVideoDriver()
    ,	glewVersion
    );
}

Window::~Window()
{	
    SDL_GL_DeleteContext(Context_);
    SDL_DestroyWindow(Handle_);
    SDL_Quit();
}

void Window::HandleEvents()
{
    SDL_Event event;

    // Poll for an event from SDL and check if it was received
    while (SDL_PollEvent(&event))
    {
        // if event was a quit event, stop the system
        if (event.type == SDL_QUIT)
        {
            Close();
        	SDL_RestoreWindow(Handle_);
        }


    	switch(event.type)
    	{
    		case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
    			Config_.Video.Resolution.x = event.window.data1;
    			Config_.Video.Resolution.y = event.window.data2;
    			Config_.Video.Maximized    = Open_ ? SDL_GetWindowFlags(Handle_) & SDL_WINDOW_MAXIMIZED : Config_.Video.Maximized;
    		break;
    		default:
    			break;
    	}

        EditorSystem::HandleEvents(&event);

        SDLEvent sdlEvent(event);
        EventSystem::PostEvent(&sdlEvent);
    }
}

void Window::BeginFrame()
{
    OpenShaderDesigner::BeginFrame event;
    EventSystem::PostEvent(&event);

    HandleEvents();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, Config_.Video.Resolution.x, Config_.Video.Resolution.y);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Window::EndFrame()
{
    SDL_GL_SwapWindow(Handle_);

    OpenShaderDesigner::EndFrame event;
    EventSystem::PostEvent(&event);
}

void Window::Restore()
{
	SDL_RestoreWindow(Handle_);
}
