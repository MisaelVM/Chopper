#pragma once

#include <common/definitions.h>

namespace Chopper {

	enum RendererBackendType {
		RENDERER_VULKAN_BACKEND,
		RENDERER_DIRECTX_BACKEND,
		RENDERER_OPENGL_BACKEND
	};

	class RendererBackend {
	public:
		RendererBackend() {}
		virtual ~RendererBackend() {}

		virtual bool BeginFrame(float deltaTime, void* pImGuiDrawData) = 0;
		virtual bool EndFrame(float deltaTime, void* pImGuiDrawData) = 0;

		virtual void OnResize(uint32_t width, uint32_t height) = 0;

	private:
	};
}
