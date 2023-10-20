#pragma once

#include <common/definitions.h>

#include "RendererBackend.h"

namespace Chopper {

	class Renderer {
	public:
		static bool Init(RendererBackendType backendType);
		static void Shutdown();

		static bool DrawFrame();

		static void OnWindowResize(uint32_t width, uint32_t height);

	private:
		Renderer();

		static RendererBackend* s_RenderBackend;
	};

}
