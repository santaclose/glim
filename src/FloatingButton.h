#pragma once
#include <cstdint>
#include <string>
#include <glm/glm.hpp>
#include "CircleFont.h"

namespace Glim::FloatingButton {

	enum class IconSource { None = 0, CircleFont = 1 };

	void Init(const uint32_t* windowSize, IconSource iconSource = IconSource::None, const std::string& iconsPath = "");

	bool Evaluate(const glm::vec2& position, float size, int iconID = -1);

	void FrameBegin();
	void BeforeDraw();
	void FrameEnd();
}