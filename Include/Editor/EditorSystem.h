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

#ifndef EDITORSYSTEM_H
#define EDITORSYSTEM_H

#include <SDL3/SDL_events.h>

#include <open-cpp-utils/unique_id.h>
#include <unordered_map>

#include <glw/framebuffer.h>

#include <Editor/EditorWindow.h>
#include <Editor/MainMenuBar.h>

#include "glw/shader.h"

#define MAX_EDITORS 256

namespace OpenShaderDesigner
{

class EditorSystem
{
private:
	using FrameBuffer = glw::framebuffer<glw::texture2D, glw::depth16, glw::rgba16f>;
	using HDRTexture  = glw::texture<glw::texture2D, glw::rgba16f>;
	
public:
    using WindowID = uint64_t;

    template<typename T>
    static WindowID ID() { return open_cpp_utils::unique_id<WindowID, T>(); }

    template<typename T>
    static T* Open() { T* window; (window = Get<T>())->Open(); return window; }

    template<typename T>
    static T* Close() { T* window; (window = Get<T>())->Close(); return window; }

    template<typename T>
    static T* Get()
    {
        T* window = reinterpret_cast<T*>(Windows_[ID<T>()]);
        if(window == nullptr) Windows_[ID<T>()] = window = new T();
        return window;
    }

    template<typename T>
    static T* SetMainMenuBar() { delete MainMenuBar_; T* bar = new T(); MainMenuBar_ = bar; return bar; }

    template<typename T>
    static T* GetMainMenuBar() { return static_cast<T*>(MainMenuBar_); }

    static void Initialize();
    static void Draw();
    static void Shutdown();
	static void HandleEvents(SDL_Event* event);
	static void OpenHDRCalibration();
	

private:
	static void RebuildWhitepointDisplay_();
	
    inline static EditorWindow* Windows_[MAX_EDITORS] { nullptr };
    inline static MainMenuBar*  MainMenuBar_ = nullptr;
	inline static FrameBuffer*  DrawBuffer_ = nullptr;
	inline static glw::shader*  ColorCorrection_ = nullptr;
	inline static float         Whitepoint = 5.5f;
	inline static float         Gamma = 2.2f;
	inline static HDRTexture*   WhitepointDisplay_ = nullptr;
	inline static bool          OpenCalibration_ = false;
};

}



#endif //EDITORSYSTEM_H
