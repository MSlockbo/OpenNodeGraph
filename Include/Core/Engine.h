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


#ifndef ENGINE_H
#define ENGINE_H

#include <Core/Window.h>
#include <Utility/Timer.h>
#include <mini/src/mini/ini.h>

using namespace mINI;

namespace OpenShaderDesigner
{
    
class Engine
{
public:	
    static void Start();
    static void Stop();

	static inline const char* ApplicationName() { return PROJECT_NAME; }
    static inline const char* VersionString()   { return PROJECT_VERSION; }
    static inline int         VersionMajor()    { return PROJECT_VERSION_MAJOR; }
    static inline int         VersionMinor()    { return PROJECT_VERSION_MINOR; }
    static inline int         VersionPatch()    { return PROJECT_VERSION_PATCH; }

    static Window& GetMainWindow() { return *MainWindow_; }

	static INIStructure& Settings() { return Settings_; }
    
private:
    static void Initialize();
    static void Shutdown();
    static void Update();

	inline static INIStructure Settings_;
    inline static Timer        Time_;
    inline static Timer        Frame_;
    inline static double       Delta_;
    inline static double       Runtime_;
    inline static Window*      MainWindow_;

public:
    inline static const double& Delta   = Delta_;
    inline static const double& Runtime = Runtime_;
};
    
}



#endif //ENGINE_H
