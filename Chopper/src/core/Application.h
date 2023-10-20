#pragma once

#include <common/definitions.h>
#include <common/includes.h>

#include "Event.h"
#include <platform/Window.h>

#include "events/WindowEvent.h"

namespace Chopper {

	class CHOPPER_API Application {
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		inline Window& GetWindow() { return *m_Window; }

		inline static Application& Get() { return *s_Instance; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
		bool m_Suspended = false;

		static Application* s_Instance;
	};

	// Externally defined
	Application* CreateApplication();
}
