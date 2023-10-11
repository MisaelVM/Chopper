#pragma once

#include <common/definitions.h>

#include "RendererBackend.h"

namespace Chopper {

	class Renderer {
	public:
		static bool Init(RendererBackendType backendType);
		static void Shutdown();

		static bool DrawFrame();

	private:
		Renderer();

		static RendererBackend* s_RenderBackend;
	};

}
