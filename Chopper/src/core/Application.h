#pragma once

#include <common/definitions.h>

namespace Chopper {

	class CHOPPER_API Application {
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	// Externally defined
	Application* CreateApplication();
}
