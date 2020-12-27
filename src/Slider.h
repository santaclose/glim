#pragma once
#include <cstdint>
#include <glm/glm.hpp>
#include "Enums.h"

namespace Glim::Slider {

	void Init(const uint32_t* windowSize);

	void Evaluate(const glm::vec2& position, float sizeInPixels, float* value, Orientation orientation = Orientation::Horizontal);

	float GetWidth();

	void BeforeDraw();
	void FrameEnd();
}