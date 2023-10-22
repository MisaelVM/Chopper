#pragma once

#include <core/Layer.h>

namespace Chopper {
	
	class CHOPPER_API ImGuiLayer : public Layer {
	public:
		ImGuiLayer() = default;
		~ImGuiLayer() = default;

		void OnAttach() override;
		void OnDetach() override;
		void OnImGuiRender() override;

		void Begin();
		void End();

	private:

	};

}
