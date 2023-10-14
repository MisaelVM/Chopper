#pragma once

#include <common/definitions.h>
#include <common/includes.h>

#include <core/Event.h>

#include <GLFW/glfw3.h>

namespace Chopper {

	class CHOPPER_API Window {
	public:
		using EventCallbackFn = std::function<void(Event&)>;
		struct WindowState {
			std::string Title = "Chopper Window";
			uint32_t Width = 1280u;
			uint32_t Height = 720u;
			bool VSyncEnabled = false;
			bool VulkanAsBackend = false;

			EventCallbackFn EventCallback;
		};

		Window(const WindowState& state);
		~Window();

		void OnUpdate();

		inline uint32_t GetWidth() const { return m_InternalState.Width; }
		inline uint32_t GetHeight() const { return m_InternalState.Height; }

		inline void SetEventCallback(const EventCallbackFn& callback) { m_InternalState.EventCallback = callback; }
		void SetVsync(bool enabled = true);

		inline bool IsVSyncEnabled() const { return m_InternalState.VSyncEnabled; }

		inline void* GetNativeWindow() const { return m_Window; }

	private:
		WindowState m_InternalState;
		GLFWwindow* m_Window;
	};

}
