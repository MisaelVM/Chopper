#pragma once

#include <common/definitions.h>

#ifdef CHOPPER_WINDOWS_PLATFORM

#include "Window.h"

#include <core/Logger.h>
#include <core/Asserts.h>

#include <core/events/WindowEvent.h>
#include <core/events/MouseEvent.h>
#include <core/events/KeyEvent.h>

namespace Chopper {

	static bool s_GLFWInitialized = false;

	Window::Window(const WindowState& state) : m_InternalState(state) {
		if (!s_GLFWInitialized) {
			int result = glfwInit();
			CHOPPER_ASSERT(result, "GLFW could not be initialized!");
			glfwSetErrorCallback([](int error_code, const char* description) {
				CHOPPER_LOG_ERROR("GLFW encountered error {0}: {1}.", error_code, description);
			});

			s_GLFWInitialized = true;
			CHOPPER_LOG_INFO("GLFW successfully initialized.");
		}

		m_Window = glfwCreateWindow((int)m_InternalState.Width, (int)m_InternalState.Height, m_InternalState.Title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(m_Window);
		glfwSetWindowUserPointer(m_Window, &m_InternalState);
		SetVsync();

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
			auto state = reinterpret_cast<WindowState*>(glfwGetWindowUserPointer(window));

			WindowCloseEvent e{};
			state->EventCallback(e);
		});

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
			auto state = reinterpret_cast<WindowState*>(glfwGetWindowUserPointer(window));
			state->Width = static_cast<uint32_t>(width);
			state->Height = static_cast<uint32_t>(height);

			WindowResizeEvent e{ state->Width, state->Height };
			state->EventCallback(e);
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {
			auto state = reinterpret_cast<WindowState*>(glfwGetWindowUserPointer(window));

			switch (action) {
				case GLFW_PRESS: {
					MouseButtonPressedEvent e{ button };
					state->EventCallback(e);
					break;
				}
				case GLFW_RELEASE: {
					MouseButtonReleasedEvent e{ button };
					state->EventCallback(e);
					break;
				}
				default:
					break;
			}
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos) {
			auto state = reinterpret_cast<WindowState*>(glfwGetWindowUserPointer(window));

			MouseMovedEvent e{ static_cast<float>(xpos), static_cast<float>(ypos) };
			state->EventCallback(e);
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoffset, double yoffset) {
			auto state = reinterpret_cast<WindowState*>(glfwGetWindowUserPointer(window));

			MouseWheelEvent e{ static_cast<float>(xoffset), static_cast<float>(yoffset) };
			state->EventCallback(e);
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
			auto state = reinterpret_cast<WindowState*>(glfwGetWindowUserPointer(window));

			switch (action) {
				case GLFW_PRESS: {
					KeyPressedEvent e{ key, 0 };
					state->EventCallback(e);
					break;
				}
				case GLFW_RELEASE: {
					KeyReleasedEvent e{ key };
					state->EventCallback(e);
					break;
				}
				case GLFW_REPEAT: {
					KeyPressedEvent e{ key, 1 };
					state->EventCallback(e);
					break;
				}
				default:
					break;
			}
		});

		CHOPPER_LOG_INFO("Window Created.");
	}

	Window::~Window() {
		glfwDestroyWindow(m_Window);
	}

	void Window::OnUpdate() {
		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}

	void Window::SetVsync(bool enabled) {
		glfwSwapInterval((int)(enabled));
		m_InternalState.VSyncEnabled = enabled;
	}

}

#endif
