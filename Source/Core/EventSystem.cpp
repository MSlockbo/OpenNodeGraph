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

#include <Core/EventSystem.h>
#include <Core/Console.h>

#include <cstring>

using namespace OpenShaderDesigner;

void EventSystem::PostEvent(const Event* event)
{
    // Thread safe
    std::lock_guard guard(Lock);

    // Alert Handlers
    const int index = event->GetID();
    for(_ImplEventHandler* handler : HandlerMap[index])
    {
        if(handler->_HandleEvent(event)) break;
    }
}