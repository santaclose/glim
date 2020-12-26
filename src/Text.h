#pragma once
#include <string>
#include <glm/glm.hpp>
#include "Enums.h"

namespace Glim::Text {

	void Init(const uint32_t* windowSize);
	unsigned int CreateFontFromFile(const std::string& filePath);

	void OnResize();
	void Element(
		const std::string& text,
		const glm::vec2& pos,
		float size,
		unsigned int fontID,
		unsigned int color = 0xffffffff,
		Alignment alignment = Alignment::Left);
	void Destroy();

	void DrawAll();

	void FrameEnd();
}