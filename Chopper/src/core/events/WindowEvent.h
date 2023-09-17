#pragma once

#include <core/Event.h>

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

	private:
		uint32_t m_Width, m_Height;
	};

}
