#pragma once

#include <core/Event.h>
#include <core/InputCodes.h>

#include <sstream>

namespace Chopper {

	class CHOPPER_API KeyEvent : public Event {
	public:
		inline KeyCode GetKeyCode() const { return m_KeyCode; }

	protected:
		KeyEvent(KeyCode keycode, std::string name)
			: m_KeyCode(keycode), Event(name) {}

		KeyCode m_KeyCode;
	};

	class CHOPPER_API KeyPressedEvent : public KeyEvent {
	public:
		KeyPressedEvent(KeyCode keycode, int repeatcount)
			: m_RepeatCount(repeatcount), KeyEvent(keycode, "KeyPressed") {}

		inline int GetRepeatCount() const { return m_RepeatCount; }

		static EventType GetStaticType() { return EventType::KeyPressed; }
		EventType GetEventType() const override { return GetStaticType(); }

		std::string GetStateLog() const override {
			std::stringstream ss;
			ss << m_Name << ": " << static_cast<int>(m_KeyCode) << " (" << m_RepeatCount << ")";
			return ss.str();
		}

	private:
		int m_RepeatCount;
	};

	class CHOPPER_API KeyReleasedEvent : public KeyEvent {
	public:
		KeyReleasedEvent(KeyCode keycode)
			: KeyEvent(keycode, "KeyReleased") {}

		static EventType GetStaticType() { return EventType::KeyReleased; }
		EventType GetEventType() const override { return GetStaticType(); }

		std::string GetStateLog() const override {
			std::stringstream ss;
			ss << m_Name << ": " << static_cast<int>(m_KeyCode);
			return ss.str();
		}
	};
}
