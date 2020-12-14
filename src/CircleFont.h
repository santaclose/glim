#pragma once
#include "Geometry.h"
#include <vector>
#include <string>

namespace Glim {

	class CircleFont {

		Shader m_shader;
		Geometry m_quads;
		unsigned int m_groupCount = 3;
		unsigned int m_circlesPerGroup = 3;
		unsigned int m_glyphCount = 0;
		std::vector<float> m_GroupData;
		std::vector<float> m_CircleData;

		unsigned int onScreenGlyphs = 0;

	public:
		void CreateFromFile(const std::string& filePath);

		void SetGlyphDataRandomly(unsigned int glyphID);
		void RandomGlyphChange(unsigned int glyphID);
		void DiscardLastChange();

		unsigned int Add(unsigned int glyphID, const glm::vec2& position, float size, const glm::vec4& color);
		void UpdatePosition(unsigned int glyphInstanceID, const glm::vec2& position);
		void UpdateSize(unsigned int glyphInstanceID, float newSize);
		void ChangeGlyph(unsigned int glyphInstanceID, unsigned int newGlyphID);
		unsigned int GetGlyphCount();
	};
}