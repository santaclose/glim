#include "LayerRenderer.h"
#include <vector>

namespace Glim::LayerRenderer {

	std::vector<Drawable*> layers;
}

void Glim::LayerRenderer::PushLayer(Drawable* layer)
{
	layers.push_back(layer);
}

void Glim::LayerRenderer::Render()
{
	for (int i = layers.size() - 1; i > -1; i--)
		layers[i]->Draw();
}
