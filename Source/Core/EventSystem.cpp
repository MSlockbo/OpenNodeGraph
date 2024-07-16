//
// Created by Maddie on 6/18/2024.
//

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