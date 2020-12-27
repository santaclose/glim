#include "Text.h"
#include "msdfgl.h"

#include <iostream>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include "Enums.h"

namespace Glim::Text {

	struct TextElement {

		std::string text;
		glm::vec2 pos;
		float size;
		unsigned int color;
		unsigned int fontID;
		Alignment alignment;
		bool enabled;
	};

	int currentTextElementID = 0;

	msdfgl_context_t context;
	std::vector<msdfgl_font_t> fonts;

	float projection[4][4];
	const uint32_t* windowSize;

	std::vector<TextElement> textElements;
}

void Glim::Text::Init(const uint32_t* windowSize)
{
	Text::windowSize = windowSize;
	context = msdfgl_create_context("460 core");
	if (!context)
		std::cout << "[Text] Failed to create msdfgl context\n";

	/* Enable auto-generating undefined glyphs as they are encountered for the first time. */
	msdfgl_set_missing_glyph_callback(context, msdfgl_generate_glyph, NULL);

	_msdfgl_ortho(0.0, (float)windowSize[0], (float)windowSize[1], 0.0, -1.0, 1.0, projection);
}

unsigned int Glim::Text::CreateFontFromFile(const std::string& filePath)
{
	fonts.emplace_back();
	fonts.back() = msdfgl_load_font(context, filePath.c_str(),
		1.7, 2.0, NULL);
	/* range, scale, atlas (NULL creates a new one) */

	/* Loads characters 0-128 onto the textures. This is where all the GPU cycles went. */
	msdfgl_generate_ascii(fonts.back());

	return fonts.size() - 1;
}

void Glim::Text::OnResize()
{
	_msdfgl_ortho(0.0, (float)windowSize[0], (float)windowSize[1], 0.0, -1.0, 1.0, projection);
}

void Glim::Text::Element(const std::string& text, const glm::vec2& pos, float size, unsigned int fontID, unsigned int color, Alignment alignment)
{
	if (currentTextElementID == textElements.size())
		textElements.emplace_back();

	textElements[currentTextElementID].text = text;
	textElements[currentTextElementID].pos = pos;
	textElements[currentTextElementID].size = size;
	textElements[currentTextElementID].color = color;
	textElements[currentTextElementID].fontID = fontID;
	textElements[currentTextElementID].alignment = alignment;
	textElements[currentTextElementID].enabled = true;

	currentTextElementID++;
}

void Glim::Text::Destroy()
{
	/* Cleanup */
	for (msdfgl_font_t& font : fonts)
		msdfgl_destroy_font(font);
	msdfgl_destroy_context(context);
}

void Glim::Text::DrawAll()
{
	for (int i = currentTextElementID; i < textElements.size(); i++)
		textElements[i].enabled = false;

	for (const TextElement& element : textElements)
	{
		if (!element.enabled)
			continue;
		msdfgl_printf(
			element.pos.x, element.pos.y,
			(int)element.alignment,
			fonts[element.fontID], element.size,
			element.color,
			(GLfloat*)projection,
			msdfgl_printf_flags::MSDFGL_KERNING,
			element.text.c_str());
	}
}

void Glim::Text::FrameEnd()
{
	currentTextElementID = 0;
}