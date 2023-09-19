#pragma once

#include <common/definitions.h>
#include <common/includes.h>

#include <core/Event.h>
#include <platform/Window.h>

#include <core/events/WindowEvent.h>

namespace Chopper {

	class CHOPPER_API Application {
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
	};

	// Externally defined
	Application* CreateApplication();
}
