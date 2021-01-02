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
		};

		int m_currentID = 0;
		std::vector<TextFieldInfo> m_textFields;

		const uint32_t* m_windowSize;

		Shader m_shader;
		Geometry m_quads;
		TextLayer* m_textLayer;
		unsigned int m_textLayerFontID;

		int m_clickPending = -1;
		int m_currentlyInteracting = -1;
		unsigned int m_cursorIndex;

		bool CollisionTest(int textFieldID);

	public:
		void Init(const uint32_t* windowSize, const std::string& fontFilePath);

		void Evaluate(
			const glm::vec2& pos,
			char* buffer,
			unsigned int bufferSize,
			float fontSize = 11.0f);

		void FrameEnd();
		void Destroy();
	};
}