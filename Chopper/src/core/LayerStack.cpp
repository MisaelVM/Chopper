#include "LayerStack.h"

namespace Chopper {

	LayerStack::LayerStack()
		: m_Layers{}, m_LayerInsertIndex{0} {}

	LayerStack::~LayerStack() {
		for (Layer* layer : m_Layers) {
			layer->OnDetach();
			delete layer;
		}
	}

	void LayerStack::PushLayer(Layer* layer) {
		m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
		++m_LayerInsertIndex;
	}

	void LayerStack::PopLayer(Layer* layer) {
		auto beginIt = m_Layers.begin();
		auto endIt = beginIt + m_LayerInsertIndex;
		auto it = std::find(beginIt, endIt, layer);
		if (it != endIt) {
			layer->OnDetach();
			m_Layers.erase(it);
			--m_LayerInsertIndex;
		}
	}

	void LayerStack::PushOverlay(Layer* overlay) {
		m_Layers.emplace_back(overlay);
	}

	void LayerStack::PopOverlay(Layer* overlay) {
		auto beginIt = m_Layers.begin() + m_LayerInsertIndex;
		auto endIt = m_Layers.end();
		auto it = std::find(beginIt, endIt, overlay);
		if (it != endIt) {
			overlay->OnDetach();
			m_Layers.erase(it);
		}
	}

}
