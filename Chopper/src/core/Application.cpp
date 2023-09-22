#include "Application.h"

#include <core/Logger.h>
#include <core/Asserts.h>

#include <core/Input.h>

namespace Chopper {

	Application* Application::s_Instance = nullptr;

	Application::Application() {
		CHOPPER_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		Window::WindowState state{};
		m_Window = std::make_unique<Window>(state);
		m_Window->SetEventCallback([&](Event& e) { OnEvent(e); });
	}

	Application::~Application() {}

	void Application::Run() {
		while (m_Running) {
			m_Window->OnUpdate();
		}
	}

	void Application::OnEvent(Event& e) {
		CHOPPER_LOG_TRACE("Captured event {0}", e.GetStateLog());

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>([&](WindowCloseEvent& e) { return OnWindowClose(e); });
	}

	bool Application::OnWindowClose(WindowCloseEvent& e) {
		m_Running = false;
		return true;
	}

}
