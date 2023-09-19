#pragma once

#include <common/definitions.h>
#include <common/includes.h>

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
		virtual std::string GetStateLog() const { return GetName(); }

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

	inline std::ostream& operator<<(std::ostream& os, const Event& e) {
		return os << e.GetStateLog();
	}
}
