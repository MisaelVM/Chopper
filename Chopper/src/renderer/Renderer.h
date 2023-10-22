#pragma once

#include <common/definitions.h>

#include "RendererBackend.h"

namespace Chopper {

	struct RenderData {
		float DeltaTime = 0.0f;
		void* ImGuiDrawData = nullptr;
	};

	class Renderer {
	public:
		static bool Init(RendererBackendType backendType);
		static void Shutdown();

		static bool DrawFrame();
		static bool BeginFrame(RenderData* renderData);
		static bool EndFrame(RenderData* renderData);

		static void OnWindowResize(uint32_t width, uint32_t height);

	private:
		Renderer();

		static RendererBackend* s_RenderBackend;
	};

}
