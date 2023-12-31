#pragma once

#include <core/Event.h>
#include <core/InputCodes.h>

#include <sstream>

namespace Chopper {

	class CHOPPER_API MouseButtonEvent : public Event {
	public:
		inline MouseButtonCode GetMouseButton() const { return m_MouseButton; }

		std::string GetStateLog() const override {
			std::stringstream ss;
			ss << m_Name << ": " << static_cast<int>(m_MouseButton);
			return ss.str();
		}

	protected:
		MouseButtonEvent(MouseButtonCode button, std::string name)
			: m_MouseButton(button), Event(name) {}

		MouseButtonCode m_MouseButton;
	};

	class CHOPPER_API MouseButtonPressedEvent : public MouseButtonEvent {
	public:
		MouseButtonPressedEvent(MouseButtonCode button)
			: MouseButtonEvent(button, "MouseButtonPressed") {}

		static EventType GetStaticType() { return EventType::MouseButtonPressed; }
		EventType GetEventType() const override { return GetStaticType(); }
	};

	class CHOPPER_API MouseButtonReleasedEvent : public MouseButtonEvent {
	public:
		MouseButtonReleasedEvent(MouseButtonCode button)
			: MouseButtonEvent(button, "MouseButtonReleased") {}

		static EventType GetStaticType() { return EventType::MouseButtonReleased; }
		EventType GetEventType() const override { return GetStaticType(); }
	};



	class CHOPPER_API MouseMovedEvent : public Event {
	public:
		MouseMovedEvent(float x, float y)
			: m_MouseX(x), m_MouseY(y), Event("MouseMoved") {}

		inline float GetX() const { return m_MouseX; }
		inline float GetY() const { return m_MouseY; }

		static EventType GetStaticType() { return EventType::MouseMoved; }
		EventType GetEventType() const override { return GetStaticType(); }

		std::string GetStateLog() const override {
			std::stringstream ss;
			ss << m_Name << ": (" << m_MouseX << ", " << m_MouseY << ")";
			return ss.str();
		}

	private:
		float m_MouseX, m_MouseY;
	};

	class CHOPPER_API MouseWheelEvent : public Event {
	public:
		MouseWheelEvent(float xOffset, float yOffset)
			: m_XOffset(xOffset), m_YOffset(yOffset), Event("MouseWheel") {}

		inline float GetXOffset() const { return m_XOffset; }
		inline float GetYOffset() const { return m_YOffset; }

		static EventType GetStaticType() { return EventType::MouseWheel; }
		EventType GetEventType() const override { return GetStaticType(); }

		std::string GetStateLog() const override {
			std::stringstream ss;
			ss << m_Name << ": (" << m_XOffset << ", " << m_YOffset << ")";
			return ss.str();
		}

	private:
		float m_XOffset, m_YOffset;
	};

}
