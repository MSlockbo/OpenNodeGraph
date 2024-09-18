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

#ifndef EDITORSYSTEM_H
#define EDITORSYSTEM_H

#include <SDL_events.h>
#include <open-cpp-utils/unique_id.h>
#include <unordered_map>

#include <Editor/EditorWindow.h>
#include <Editor/MainMenuBar.h>

#define MAX_EDITORS 256

namespace OpenShaderDesigner
{

class EditorSystem
{
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

private:
    inline static EditorWindow* Windows_[MAX_EDITORS] { nullptr };
    inline static MainMenuBar*  MainMenuBar_ = nullptr;
};

}



#endif //EDITORSYSTEM_H
