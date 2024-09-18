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

#ifndef ENGINE_EVENTSYSTEM_H
#define ENGINE_EVENTSYSTEM_H

#include <open-cpp-utils/unique_id.h>

#include <cstdint>
#include <list>
#include <mutex>


#define MAX_EVENT_TYPES 256

namespace ocu = open_cpp_utils;

namespace OpenShaderDesigner
{
    
/**
 * \brief Base Event class for sending events to the Engine.
 */
struct Event
{
	template<typename T>
	static uint8_t TypeOf() { return static_cast<uint8_t>(ocu::unique_id<uint8_t, T>()); }

	/**
	 * \brief Get the Event's type ID.
	 * \return A pointer to the Event type ID.
	 */
	virtual inline uint8_t GetID() const = 0;
};


/**
 * \brief Base EventHandler for abstraction.
 */
class _ImplEventHandler
{
	virtual bool _HandleEvent(const Event* event) = 0;

	friend class EventSystem;
};

/**
 * \brief EventHandler interface for creating custom EventHandlers
 * \tparam EventType The ComponentType of Event handled by the EventHandler
 */
template<typename EventType>
class EventHandler : private _ImplEventHandler
{
public:
	using HandledType = EventType; //!< The type handled by the EventHandler

	/**
	 * \brief Virtual function for custom EventHandler implementations.
	 * \param event The Event being handled.
	 */
	virtual bool HandleEvent(const HandledType* event) = 0;
private:

	/**
	 * \brief Override for abstraction.
	 * \param event The Event being handled.
	 */
	bool _HandleEvent(const Event* event) override;
};

/**
 * \brief EventSystem for posting Events to be handled.
 */
class EventSystem
{
public:
	/**
	 * \brief Post an Event to be Handled.
	 */
	static void PostEvent(const Event*);

	/**
	 * \brief Register an EventHandler with the EventSystem.
	 * \tparam T ComponentType of Event handled by the EventHandler.
	 */
	template<typename T>
	static void RegisterHandler(EventHandler<T>*);

	/**
	 * \brief Unregister an EventHandler with the EventSystem.
	 * \tparam T ComponentType of Event handled by the EventHandler.
	 */
	template<typename T>
	static void UnregisterHandler(EventHandler<T>*);

private:
	inline static std::list<_ImplEventHandler*> HandlerMap_[MAX_EVENT_TYPES];
	inline static std::mutex                    Lock_;

	EventSystem(const EventSystem&) = delete;
	EventSystem(EventSystem&&) = delete;
};

template<typename T>
void EventSystem::UnregisterHandler(EventHandler<T>* handler)
{
	// Thread safe
	std::lock_guard guard(Lock_);
	const uint8_t index = T::ID;
	std::erase(HandlerMap_[index], reinterpret_cast<_ImplEventHandler*>(handler));
}

template<typename T>
void EventSystem::RegisterHandler(EventHandler<T>* handler)
{
	// Thread safe
	std::lock_guard guard(Lock_);
	const uint8_t index = T::ID;
	HandlerMap_[index].push_back(reinterpret_cast<_ImplEventHandler*>(handler));
}

template<typename EventType>
bool EventHandler<EventType>::_HandleEvent(const Event *event)
{
	if(EventType::ID != event->GetID()) return false;
	return HandleEvent(reinterpret_cast<const EventType*>(event));
}
    
}

#define BeginEvent(EVENT) struct EVENT : OpenShaderDesigner::Event \
    {                                           \
	static inline const uint8_t ID = Event::TypeOf<EVENT>(); \
	inline uint8_t GetID() const override { return ID; }

#define EndEvent };

#endif //ENGINE_EVENTSYSTEM_H
