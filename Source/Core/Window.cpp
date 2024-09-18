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

#include <Core/Window.h>
#include <Core/Console.h>
#include <Editor/EditorSystem.h>

#include <gl/glew.h>

using namespace OpenShaderDesigner;

Window::Window(const Configuration& config)
    : Config_(config)
{
    int flags = static_cast<int>(Config_.Video.Fullscreen) | SDL_WINDOW_OPENGL;
    flags |= Config_.Video.Fullscreen == FullscreenMode::WINDOWED ? SDL_WINDOW_RESIZABLE : 0;

    SDL_Init(SDL_INIT_EVERYTHING & ~SDL_INIT_AUDIO);

#ifdef NDEBUG
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG | SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
#endif

    // Set OpenGL APIVersion
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    if(Config_.Video.HDR)
    {
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 10);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 10);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 10);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 2);
        SDL_GL_SetAttribute(SDL_GL_FLOATBUFFERS, SDL_TRUE);
    }

    if(Config_.Video.Multisamples())
    {
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, SDL_TRUE);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, Config_.Video.Multisamples);
    }


    if((Handle_ = SDL_CreateWindow(
        Config_.Application.Title.c_str(),
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
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

    if(SDL_GL_MakeCurrent(Handle_, Context_))
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
    SDL_version sdlVersion; SDL_GetVersion(&sdlVersion);
	Console::Log(Console::Severity::Alert, "Initialized SDL ({}.{}.{})", sdlVersion.major, sdlVersion.minor, sdlVersion.patch);
    Console::Log(Console::Severity::Alert, "Running OpenGL ({}), GLEW ({})", glVersion, glewVersion);
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
            return;
        }


        if (event.type == SDL_WINDOWEVENT)
        {
            switch(event.window.event)
            {
            case SDL_WINDOWEVENT_SIZE_CHANGED:
                Config_.Video.Resolution.x = event.window.data1;
                Config_.Video.Resolution.y = event.window.data2;
                break;
            default:
                break;
            }
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
