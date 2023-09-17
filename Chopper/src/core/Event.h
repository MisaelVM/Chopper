#pragma once

#include <common/definitions.h>

#include <functional>
#include <map>
#include <string>
#include <vector>

namespace Chopper {

	enum class EventType {
		NoEvent = 0,

		WindowClose,
		WindowResize,

		KeyPressed,
		KeyReleased,

		MouseButtonPressed,
		MouseButtonReleased,
		MouseMoved,
		MouseWheel
	};

	class CHOPPER_API Event {
		friend class EventDispatcher;
	public:
		Event() = default;
		Event(std::string name) : m_Name(name) {}

		virtual EventType GetEventType() const = 0;
		std::string GetName() const { return m_Name; }

		friend std::ostream& operator<<(std::ostream& os, const Event& e);

	protected:
		std::string m_Name;
		bool m_Handled = false;
	};

	class EventDispatcher {
		template<typename T>
		using EventCallback = std::function<bool(T&)>;
	public:
		EventDispatcher(Event& e) : m_Event(e) {}

		template <typename T>
		bool Dispatch(EventCallback<T> func) {
			if (m_Event.GetEventType() == T::GetStaticType())
				m_Event.m_Handled = func(*(T*)&m_Event);
			return m_Event.m_Handled;
		}

	private:
		Event& m_Event;
	};

	std::ostream& operator<<(std::ostream& os, const Event& e) {
		return os << e.m_Name;
	}
}
