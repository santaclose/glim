#pragma once
#include <string>
#include <glm/glm.hpp>
#include <vector>
#include "Enums.h"
#include "msdfgl.h"
#include "Drawable.h"

namespace Glim {

	class TextLayer : public Drawable {
	public:
		static std::vector<TextLayer*> instances;

	private:

		struct TextElement {

			const char* text;
			glm::vec2 pos;
			float size;
			unsigned int color;
			unsigned int fontID;
			HAlignment alignment;
			bool enabled;
		};

		int m_currentID = 0;

		float m_projection[4][4];

		std::vector<TextElement> m_textElements;

		void ComputeMatrix(const uint32_t* windowSize);

	public:
		TextLayer::TextLayer();
		TextLayer::~TextLayer();

		void Element(
			const char* text,
			const glm::vec2& pos,
			float size,
			unsigned int fontID,
			unsigned int color = 0xffffffff,
			HAlignment alignment = HAlignment::Left);

		// returns the length of the text in pixels
		float Measure(
			const char* text,
			float size,
			unsigned int fontID,
			unsigned int limit = 0xffffffff);
		
		void Draw();
		void FrameEnd();

		static void OnResize(const uint32_t* windowSize);
	};
}