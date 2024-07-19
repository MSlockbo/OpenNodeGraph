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
#include <Utility/UniqueID.h>
#include <unordered_map>

#include <Editor/EditorWindow.h>

#define MAX_EDITORS 256

namespace OpenShaderDesigner
{
    class EditorSystem
    {
    public:
        using WindowID = uint64_t;

        template<typename T>
        static WindowID ID() { return OpenShaderDesigner::UniqueID<WindowID, T>(); }

        template<typename T>
        static T* Open() { T* window; (window = Get<T>())->Open(); return window; }

        template<typename T>
        static T* Close() { T* window; (window = Get<T>())->Close(); return window; }

        template<typename T>
        static T* Get()
        {
            T* window = reinterpret_cast<T*>(Windows[ID<T>()]);
            if(window == nullptr) Windows[ID<T>()] = window = new T();
            return window;
        }

        static void Initialize();
        static void Draw();
        static void Shutdown();
        static void HandleEvents(SDL_Event* event);

    private:
        inline static EditorWindow* Windows[MAX_EDITORS] { nullptr };
    };
}



#endif //EDITORSYSTEM_H
