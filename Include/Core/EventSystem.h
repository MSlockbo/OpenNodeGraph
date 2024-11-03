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
