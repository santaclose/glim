#pragma once
#include "../Geometry.h"
#include "TextLayer.h"
#include <vector>

namespace Glim {

	class TextFieldLayer {

	private:
		static std::vector<TextFieldLayer*> instances;

		struct TextFieldInfo {

			uint32_t geometryIndex;
			glm::vec2 pos;
			float size;
			const char* buffer;
			HAlignment hAlignment;
			VAlignment vAlignment;
			unsigned int fontID;
		};

		int m_currentID = 0;
		std::vector<TextFieldInfo> m_textFields;

		const uint32_t* m_windowSize;

		Shader m_shader;
		Geometry m_quads;
		TextLayer* m_textLayer;

		int m_clickPending = -1;
		int m_currentlyInteracting = -1;
		unsigned int m_cursorIndex;

		bool CollisionTest(int textFieldID);

	public:
		void Init(const uint32_t* windowSize);

		void Evaluate(
			const glm::vec2& pos,
			char* buffer,
			unsigned int bufferSize,
			unsigned int fontID,
			float fontSize = 11.0f,
			HAlignment hAlignment = HAlignment::Left,
			VAlignment vAlignment = VAlignment::Top,
			const glm::vec4& color = { 0.35f, 0.35f, 0.35f, 0.9f },
			unsigned int textColor = 0xffffffff);

		void FrameEnd();
		void Destroy();
	};
}