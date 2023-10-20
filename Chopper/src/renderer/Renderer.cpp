#pragma once

#include "Renderer.h"

#include <core/Logger.h>
#include <core/Asserts.h>

#include "vulkan/VulkanBackend.h"

namespace Chopper {

	RendererBackend* Renderer::s_RenderBackend = nullptr;

	bool Renderer::Init(RendererBackendType backendType) {
		CHOPPER_ASSERT(!s_RenderBackend, "Renderer Backend already initialized!");

		if (backendType == RENDERER_VULKAN_BACKEND) {
			// Creare VULKAN Backend for Renderer
			s_RenderBackend = new VulkanBackend();
			return true;
		}
		else if (backendType == RENDERER_DIRECTX_BACKEND) {
			// Create DIRECTX Backend for Renderer
			CHOPPER_LOG_CRIT("DirectX Backend not implemented at the moment!");
			return false;
		}
		else if (backendType == RENDERER_OPENGL_BACKEND) {
			// Create OPENGL Backend for Renderer
			CHOPPER_LOG_CRIT("OpenGL Backend not implemented at the moment!");
			return false;
		}

		CHOPPER_LOG_CRIT("Unknown Backend");
		return false;
	}

	void Renderer::Shutdown() {
		delete s_RenderBackend;
	}

	bool Renderer::DrawFrame() {
		if (s_RenderBackend->BeginFrame(0.0f)) {
			bool result = s_RenderBackend->EndFrame(0.0f);

			if (!result) {
				CHOPPER_LOG_ERROR("Failed to end drawing frame!");
				return false;
			}
		}

		return true;
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height) {
		s_RenderBackend->OnResize(width, height);
	}

}
