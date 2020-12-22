#pragma once
#include <cstdint>
#include <glm/glm.hpp>

namespace Glim::Slider {

	void Init(const uint32_t* windowSize);

	void Evaluate(const glm::vec2& position, float sizeInPixels, float* value);

	void BeforeDraw();
	void FrameEnd();
}