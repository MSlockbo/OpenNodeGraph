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

#include <Core/EventSystem.h>
#include <Core/Console.h>

#include <cstring>

using namespace OpenShaderDesigner;

void EventSystem::PostEvent(const Event* event)
{
    // Thread safe
    std::lock_guard guard(Lock_);

    // Alert Handlers
    const int index = event->GetID();
    for(_ImplEventHandler* handler : HandlerMap_[index])
    {
        if(handler->_HandleEvent(event)) break;
    }
}