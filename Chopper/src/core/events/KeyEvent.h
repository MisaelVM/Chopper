#pragma once

#include <core/Event.h>

#include <sstream>

namespace Chopper {

	class CHOPPER_API KeyEvent : public Event {
	public:
		inline int GetKeyCode() const { return m_KeyCode; }

	protected:
		KeyEvent(int keycode, std::string name)
			: m_KeyCode(keycode), Event(name) {}

		int m_KeyCode;
	};

	class CHOPPER_API KeyPressedEvent : public KeyEvent {
	public:
		KeyPressedEvent(int keycode, int repeatcount)
			: m_RepeatCount(repeatcount), KeyEvent(keycode, "KeyPressed") {}

		inline int GetRepeatCount() const { return m_RepeatCount; }

		static EventType GetStaticType() { return EventType::KeyPressed; }
		EventType GetEventType() const override { return GetStaticType(); }

		std::string GetStateLog() const override {
			std::stringstream ss;
			ss << m_Name << ": " << m_KeyCode << " (" << m_RepeatCount << ")";
			return ss.str();
		}

	private:
		int m_RepeatCount;
	};

	class CHOPPER_API KeyReleasedEvent : public KeyEvent {
	public:
		KeyReleasedEvent(int keycode)
			: KeyEvent(keycode, "KeyReleased") {}

		static EventType GetStaticType() { return EventType::KeyReleased; }
		EventType GetEventType() const override { return GetStaticType(); }

		std::string GetStateLog() const override {
			std::stringstream ss;
			ss << m_Name << ": " << m_KeyCode;
			return ss.str();
		}
	};
}
