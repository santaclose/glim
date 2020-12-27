#pragma once
#include <cstdint>
#include <glm/glm.hpp>

namespace Glim::Checkbox {

	void Init(const uint32_t* windowSize);

	void Evaluate(const glm::vec2& position, bool* value);

	float GetSize();

	void BeforeDraw();
	void FrameEnd();
}