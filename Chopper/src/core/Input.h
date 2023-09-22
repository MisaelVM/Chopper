#pragma once

#include <common/definitions.h>
#include "InputCodes.h"

namespace Chopper {

	class CHOPPER_API Input {
	public:
		inline static bool IsKeyDown(Key keycode) { return s_Instance->IsKeyDownImpl(keycode); }
		inline static bool IsKeyUp(Key keycode) { return s_Instance->IsKeyUpImpl(keycode); }

		inline static bool IsMouseButtonDown(MouseButton button) { return s_Instance->IsMouseButtonDownImpl(button); }
		inline static bool IsMouseButtonUp(MouseButton button) { return s_Instance->IsMouseButtonUpImpl(button); }

		inline static float GetMouseX() { return s_Instance->GetMouseXImpl(); }
		inline static float GetMouseY() { return s_Instance->GetMouseYImpl(); }

	private:
		bool IsKeyDownImpl(Key keycode);
		bool IsKeyUpImpl(Key keycode);
		bool IsMouseButtonDownImpl(MouseButton button);
		bool IsMouseButtonUpImpl(MouseButton button);
		float GetMouseXImpl();
		float GetMouseYImpl();

		static Input* s_Instance;
	};

}
