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

#ifndef WINDOW_H
#define WINDOW_H

#include <SDL3/SDL_video.h>
#include <SDL3/SDL_events.h>

#include <glm/glm.hpp>
#include <string>
#include <Core/EventSystem.h>

#include "open-cpp-utils/map.h"
#include "open-cpp-utils/optional.h"

namespace ocu = open_cpp_utils;

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

	inline static const ocu::map<std::string, VSyncMode> VSyncTranslateString = {
		{ "disabled", VSyncMode::DISABLED }
	,	{ "enabled",  VSyncMode::ENABLED  }
	,	{ "adaptive", VSyncMode::ADAPTIVE }
	};

	inline static const ocu::map<VSyncMode, std::string> VSyncTranslateEnum = {
		{ VSyncMode::DISABLED, "disabled" }
	,	{ VSyncMode::ENABLED,  "enabled"  }
	,	{ VSyncMode::ADAPTIVE, "adaptive" }
	};

    enum class FullscreenMode : int
    {
        WINDOWED = 0,
        BORDERLESS = SDL_WINDOW_BORDERLESS,
        FULLSCREEN = SDL_WINDOW_FULLSCREEN,
    };

	inline static const ocu::map<std::string, FullscreenMode> FullScreenTranslateString = {
		{ "windowed",   FullscreenMode::WINDOWED   }
	,	{ "borderless", FullscreenMode::BORDERLESS }
	,	{ "fullscreen", FullscreenMode::FULLSCREEN }
	};

	inline static const ocu::map<FullscreenMode, std::string> FullScreenTranslateEnum = {
		{ FullscreenMode::WINDOWED,   "windowed"   }
	,	{ FullscreenMode::BORDERLESS, "borderless" }
	,	{ FullscreenMode::FULLSCREEN, "fullscreen" }
	};

	inline static const ocu::map<std::string, int> MSAATranslateString = {
		{ "off", 1 }
	,	{ "2x",  2 }
	,	{ "4x",  4 }
	,	{ "8x",  8 }
	,	{ "16x", 16 }
	};

	inline static const ocu::map<int, std::string> MSAATranslateInt = {
		{ 1,  "off" }
	,	{ 2,  "2x"  }
	,	{ 4,  "4x"  }
	,	{ 8,  "8x"  }
	,	{ 16, "16x" }
	};

    struct Configuration
    {
        struct
        {
            std::string Title;
        } Application;

        struct
        {
            FullscreenMode     Fullscreen;
            glm::ivec2         Resolution;
            VSyncMode          VSync;
            bool               HDR, Maximized;
            ocu::optional<int> Multisamples;
        } Video;

        Configuration()
            : Application { "App" }
            , Video { FullscreenMode::WINDOWED, glm::ivec2(0, 0), VSyncMode::DISABLED, false, false }
        { }
    };

    inline static const Configuration DefaultConfiguration;

    explicit Window(const Configuration& config);
    ~Window();

    void HandleEvents();
    void BeginFrame();
    void EndFrame();

	void Restore();

    void Close() { Open_ = false; }
    [[nodiscard]] bool IsOpen() const { return Open_; }

    SDL_Window* GetHandle() { return Handle_; }
    [[nodiscard]] const SDL_Window* GetHandle() const { return Handle_; }

    SDL_GLContext GetContext() { return Context_; }
    [[nodiscard]] const SDL_GLContext GetContext() const { return Context_; }

    [[nodiscard]] glm::ivec2 Size() const { return Config_.Video.Resolution; }

	[[nodiscard]] const Configuration& Config() const { return Config_; }
private:
    Configuration Config_;
    SDL_Window*   Handle_;
    SDL_GLContext Context_;
    bool          Open_;
};

}




#endif //WINDOW_H
