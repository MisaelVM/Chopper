#pragma once

#include <core/Event.h>

#include <sstream>

namespace Chopper {

	class CHOPPER_API WindowCloseEvent : public Event {
	public:
		WindowCloseEvent()
			: Event("WindowClose") {}

		static EventType GetStaticType() { return EventType::WindowClose; }
		EventType GetEventType() const override { return GetStaticType(); }
	};

	class CHOPPER_API WindowResizeEvent : public Event {
	public:
		WindowResizeEvent(uint32_t width, uint32_t height)
			: m_Width(width), m_Height(height), Event("WindowResize") {}

		inline uint32_t GetWidth() const { return m_Width; }
		inline uint32_t GetHeight() const { return m_Height; }

		static EventType GetStaticType() { return EventType::WindowResize; }
		EventType GetEventType() const override { return GetStaticType(); }

		std::string GetStateLog() const override {
			std::stringstream ss;
			ss << m_Name << ": (" << m_Width << ", " << m_Height << ")";
			return ss.str();
		}

	private:
		uint32_t m_Width, m_Height;
	};

}
