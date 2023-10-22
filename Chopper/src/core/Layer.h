#pragma once

#include <common/definitions.h>

#include "Event.h"

namespace Chopper {

	class CHOPPER_API Layer {
	public:
		Layer() {}
		virtual ~Layer() {}

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(float deltaTime) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& e) {}
	};

}
