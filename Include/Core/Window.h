//
// Created by Maddie on 6/18/2024.
//

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
