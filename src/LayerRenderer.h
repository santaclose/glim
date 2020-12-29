#pragma once
#include "Drawable.h"

namespace Glim::LayerRenderer {

	void PushLayer(Drawable* layer);
	void Render();
}
