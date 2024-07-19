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
