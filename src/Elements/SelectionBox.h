#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "../Enums.h"
#include "../Geometry.h"
#include "TextLayer.h"

namespace Glim {
	class SelectionBox {
	public:
		static std::vector<SelectionBox*> instances;
		enum Selection { None = -2, Cancel = -1 };

	private:
		struct BoxInfo {

			glm::vec2 pos;
			const std::vector<std::string>* options;
			uint32_t geometryIndex;
			Corner cornerAtPos;
			unsigned int fontID;
			unsigned int textColor;
		};

		int m_currentOnScreenBoxCount = 0;
		glm::vec2 m_lastSelectionPos = { -1, -1 };

		Geometry m_quads; // one quad for each selection box displayed at the same time
		Shader m_shader;
		TextLayer* m_textLayer;

		glm::vec4 m_shaderUniformData = { -1.0f, 0.0f, 0.0f, 0.0f };

		std::vector<BoxInfo> m_selectionBoxes;

		int CollisionTest(int selectionBoxID);

	public:
		SelectionBox::SelectionBox();
		SelectionBox::~SelectionBox();

		void OnResize();

		int Create(
			const std::vector<std::string>* options,
			const glm::vec2& position, unsigned int fontID,
			Corner cornerAtPos = Corner::TopLeft,
			const glm::vec4& color = { 0.35f, 0.35f, 0.35f, 0.9f },
			unsigned int textColor = 0xffffffff);
		int Evaluate(int selectionBoxID);

		float GetWidth();
		const glm::vec2& GetLastSelectionPosition(Corner corner = Corner::TopLeft);
		void Delete(int selectionBoxID);
	};
}