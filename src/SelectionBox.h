#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "Enums.h"

namespace Glim::SelectionBox {

	enum Selection { None = -2, Cancel = -1 };

	void Init(const uint32_t* windowSize);
	void OnResize();

	int Create(const std::vector<std::string>* options, const glm::vec2& position, Corner cornerAtPos = Corner::TopLeft);
	int Evaluate(int selectionBoxID);
	const glm::vec2& GetLastSelectionPosition(bool leftSide = false);
	void Delete(int selectionBoxID);


	void FrameEnd();
}