#pragma once

#include <renderer/RendererBackend.h>

//#include "VulkanContext.h"

namespace Chopper {

	class VulkanBackend : public RendererBackend {
	public:
		VulkanBackend();
		~VulkanBackend();

		bool BeginFrame(float deltaTime) override;
		bool EndFrame(float deltaTime) override;

	private:
		bool Init();
		void Shutdown();

		//VulkanContext m_VulkanContext;
	};

}
