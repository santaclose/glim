#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "../Enums.h"
#include "../Geometry.h"
#include "TextLayer.h"

namespace Glim {
	class ListViewLayer {
	private:
		struct ListViewInfo {

			glm::vec2 pos;
			glm::vec2 size;
			const std::vector<std::string>* options;
			uint32_t geometryIndex;
			float scrollOffset = 0.0f;
			unsigned int fontID;
		};

		int m_currentID = 0;

		const uint32_t* m_windowSize;
		Geometry m_quads; // one quad for each selection box displayed at the same time
		Shader m_shader;
		TextLayer m_textLayer;

		int m_currentlyInteracting = -1;

		glm::vec4 m_shaderUniformData = { -1.0f, -1.0, -1.0, -1.0 };

		std::vector<ListViewInfo> m_listViews;

		bool CursorOver(int listViewID);
		int CollisionTest(int listViewID);

	public:
		void Init(const uint32_t* windowSize);

		int Evaluate(
			const glm::vec2& position,
			const glm::vec2& size, const std::vector<std::string>* options,
			unsigned int fontID,
			const glm::vec4& color = { 0.35f, 0.35f, 0.35f, 0.9f },
			unsigned int textColor = 0xffffffff);

		void FrameBegin();
		void BeforeDraw();
		void FrameEnd();
	};
}