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

#ifndef WINDOW_H
#define WINDOW_H

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <string>
#include <Core/EventSystem.h>

namespace OpenShaderDesigner
{
    BeginEvent(SDLEvent)
        const SDL_Event sdl_event;

        SDLEvent() : sdl_event() {}

        explicit SDLEvent(const SDL_Event &event) : sdl_event(event) {}
    EndEvent


    BeginEvent(BeginFrame)
    EndEvent;

    BeginEvent(SDLEventsDone)
    EndEvent;

    BeginEvent(EndFrame)
    EndEvent;


    class Window
    {
    public:
        enum class VSyncMode : int
        {
            DISABLED = 0,
            ENABLED = 1,
            ADAPTIVE = -1,
            DEFAULT = DISABLED,
        };

        enum class FullscreenMode : int
        {
            WINDOWED = 0,
            FULLSCREEN = SDL_WINDOW_FULLSCREEN,
            FULLSCREEN_WINDOW = SDL_WINDOW_FULLSCREEN_DESKTOP,
        };

        struct Configuration
        {
            struct
            {
                std::string Title;
            } Application;

            struct
            {
                FullscreenMode Fullscreen;
                glm::ivec2     Resolution;
                VSyncMode      VSync;
                bool           HDR;
            } Video;

            Configuration()
                : Application { "App" }
                , Video { FullscreenMode::WINDOWED, glm::ivec2(1280, 720), VSyncMode::DISABLED, false }
            { }
        };

        inline static const Configuration DefaultConfiguration;

        explicit Window(const Configuration& config);
        ~Window();

        void HandleEvents();
        void BeginFrame();
        void EndFrame();

        void Close() { Open = false; }
        [[nodiscard]] bool IsOpen() const { return Open; }

        SDL_Window* GetHandle() { return Handle; }
        [[nodiscard]] const SDL_Window* GetHandle() const { return Handle; }

        SDL_GLContext GetContext() { return Context; }
        [[nodiscard]] const SDL_GLContext GetContext() const { return Context; }

        [[nodiscard]] glm::ivec2 Size() const { return Config.Video.Resolution; }
    private:
        Configuration Config;
        SDL_Window*   Handle;
        SDL_GLContext Context;
        bool          Open;
    };
}




#endif //WINDOW_H
