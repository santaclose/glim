#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "Enums.h"
#include "Geometry.h"

namespace Glim {
	class SelectionBox {
	public:
		enum Selection { None = -2, Cancel = -1 };

	private:
		struct BoxInfo {

			glm::vec2 pos;
			const std::vector<std::string>* options;
			uint32_t geometryIndex;
			Corner cornerAtPos;
		};

		unsigned int m_fontID;
		bool m_evaluationHandled = false;
		int m_currentOnScreenBoxCount = 0;
		glm::vec2 m_lastSelectionPos = { -1, -1 };

		const uint32_t* m_windowSize;
		Geometry m_quads; // one quad for each selection box displayed at the same time
		Shader m_shader;

		glm::vec4 m_shaderUniformData = { -1.0f, 0.0f, 0.0f, 0.0f };

		std::vector<BoxInfo> m_selectionBoxes;

		int CollisionTest(int selectionBoxID);

	public:
		void Init(const uint32_t* windowSize);
		void OnResize();

		int Create(const std::vector<std::string>* options, const glm::vec2& position, Corner cornerAtPos = Corner::TopLeft);
		int Evaluate(int selectionBoxID);

		float GetWidth();
		const glm::vec2& GetLastSelectionPosition(Corner corner = Corner::TopLeft);
		void Delete(int selectionBoxID);

		void FrameEnd();
	};
}