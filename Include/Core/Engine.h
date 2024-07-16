//
// Created by Maddie on 6/20/2024.
//

#ifndef ENGINE_H
#define ENGINE_H

#include <Core/Window.h>
#include <Utility/Timer.h>

namespace OpenShaderDesigner
{
    class Engine
    {
    public:
        static void Start(const Window::Configuration& config);
        static void Stop();

        static Window& GetMainWindow() { return *MainWindow; }
    private:
        static void Initialize();
        static void Shutdown();
        static void Update();

        inline static Timer   Frame;
        inline static double  _Delta;
        inline static Window* MainWindow;

    public:
        inline static const double& Delta = _Delta;
    };
}



#endif //ENGINE_H
