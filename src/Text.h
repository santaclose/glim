#pragma once
#include <string>
#include <glm/glm.hpp>

namespace Glim::Text {

	void Init(const std::string& fontFilePath, const uint32_t* windowSize);
	void OnResize();
	void Element(
		const std::string& text,
		const glm::vec2& pos,
		float size,
		unsigned int color = 0xffffffff,
		Alignment alignment = Alignment::Left);
	void Destroy();

	void DrawAll();

	void FrameEnd();
}