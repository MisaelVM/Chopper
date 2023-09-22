#include <core/Input.h>

#ifdef CHOPPER_WINDOWS_PLATFORM

#include <core/Application.h>
#include <GLFW/glfw3.h>

namespace Chopper {

	Input* Input::s_Instance = new Input{};

	bool Input::IsKeyDownImpl(Key keycode) {
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		int action = glfwGetKey(window, static_cast<int>(keycode));
		return action == GLFW_PRESS || action == GLFW_REPEAT;
	}

	bool Input::IsKeyUpImpl(Key keycode) {
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		int action = glfwGetKey(window, static_cast<int>(keycode));
		return action == GLFW_RELEASE;
	}

	bool Input::IsMouseButtonDownImpl(MouseButton button) {
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		int action = glfwGetMouseButton(window, static_cast<int>(button));
		return action == GLFW_PRESS;
	}

	bool Input::IsMouseButtonUpImpl(MouseButton button) {
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		int action = glfwGetMouseButton(window, static_cast<int>(button));
		return action == GLFW_RELEASE;
	}

	float Input::GetMouseXImpl() {
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		return static_cast<float>(xpos);
	}

	float Input::GetMouseYImpl() {
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		return static_cast<float>(ypos);
	}

}

#endif
