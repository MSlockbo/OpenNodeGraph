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

#ifndef TIMER_H
#define TIMER_H

#include <chrono>

namespace OpenShaderDesigner
{
    
class Timer
{
public:
    Timer() : Start(std::chrono::high_resolution_clock::now()) { }

    void Reset() { Start = std::chrono::high_resolution_clock::now(); }

    [[nodiscard]] double Poll() const
    { return std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - Start).count(); }
private:
    std::chrono::high_resolution_clock::time_point Start;
};
    
}

#endif //TIMER_H
