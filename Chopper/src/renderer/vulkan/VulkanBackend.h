#pragma once

#include <renderer/RendererBackend.h>

namespace Chopper {

	class VulkanBackend : public RendererBackend {
	public:
		VulkanBackend();
		~VulkanBackend();

		bool BeginFrame(float deltaTime) override;
		bool EndFrame(float deltaTime) override;

		void OnResize(uint32_t width, uint32_t height) override;

	private:
		bool Init();
		void Shutdown();
	};

}
