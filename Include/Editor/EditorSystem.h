//
// Created by Maddie on 6/20/2024.
//

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
