#pragma once
#include <cstdint>
#include <string>
#include <glm/glm.hpp>
#include "CornerEnum.h"
#include "CircleFont.h"

namespace Glim::FloatingButton {

	enum class IconSource { None = 0, CircleFont = 1 };

	void Init(const uint32_t* windowSize, float margin, IconSource iconSource = IconSource::None, const std::string& iconsPath = "");
	void OnResize();

	void FrameBegin();
	void FrameEnd();

	bool Evaluate(Corner corner, float size, int iconID = -1);
	glm::vec2 GetLastButtonPosition();
}