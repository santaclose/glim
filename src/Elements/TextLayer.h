#pragma once
#include <string>
#include <glm/glm.hpp>
#include <vector>
#include "Enums.h"
#include "msdfgl.h"
#include "Drawable.h"

namespace Glim {

	class TextLayer : public Drawable {

	private:
		static std::vector<TextLayer*> instances;

		struct TextElement {

			const char* text;
			glm::vec2 pos;
			float size;
			unsigned int color;
			unsigned int fontID;
			Alignment alignment;
			bool enabled;
		};

		int m_currentID = 0;

		msdfgl_context_t m_context;
		std::vector<msdfgl_font_t> m_fonts;

		float m_projection[4][4];
		const uint32_t* m_windowSize;

		std::vector<TextElement> m_textElements;

		void ComputeMatrix();

	public:
		void Init(const uint32_t* windowSize);
		unsigned int CreateFontFromFile(const std::string& filePath);

		void Element(
			const char* text,
			const glm::vec2& pos,
			float size,
			unsigned int fontID,
			unsigned int color = 0xffffffff,
			Alignment alignment = Alignment::Left);

		// returns the length of the text in pixels
		float Measure(
			const char* text,
			float size,
			unsigned int fontID,
			unsigned int limit = 0xffffffff);

		void Destroy();
		
		void Draw();
		void FrameEnd();

		static void OnResize();
	};
}