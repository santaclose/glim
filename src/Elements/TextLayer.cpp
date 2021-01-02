#include "TextLayer.h"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include "LayerRenderer.h"

namespace Glim {

	std::vector<TextLayer*> TextLayer::instances;
}

void Glim::TextLayer::ComputeMatrix()
{
	_msdfgl_ortho(0.0, (float)m_windowSize[0], (float)m_windowSize[1], 0.0, -1.0, 1.0, m_projection);
}

void Glim::TextLayer::Init(const uint32_t* windowSize)
{
	m_windowSize = windowSize;
	m_context = msdfgl_create_context("460 core");
	if (!m_context)
		std::cout << "[Text] Failed to create msdfgl context\n";

	/* Enable auto-generating undefined glyphs as they are encountered for the first time. */
	msdfgl_set_missing_glyph_callback(m_context, msdfgl_generate_glyph, NULL);

	_msdfgl_ortho(0.0, (float)windowSize[0], (float)windowSize[1], 0.0, -1.0, 1.0, m_projection);

	LayerRenderer::PushLayer(this);
	instances.push_back(this);
}

unsigned int Glim::TextLayer::CreateFontFromFile(const std::string& filePath)
{
	m_fonts.emplace_back();
	m_fonts.back() = msdfgl_load_font(m_context, filePath.c_str(),
		1.7, 2.0, NULL);
	/* range, scale, atlas (NULL creates a new one) */

	/* Loads characters 0-128 onto the textures. This is where all the GPU cycles went. */
	msdfgl_generate_ascii(m_fonts.back());

	return m_fonts.size() - 1;
}

void Glim::TextLayer::OnResize()
{
	for (TextLayer* t : instances)
		t->ComputeMatrix();
}

void Glim::TextLayer::Element(const std::string& text, const glm::vec2& pos, float size, unsigned int fontID, unsigned int color, Alignment alignment)
{
	if (m_currentID == m_textElements.size())
		m_textElements.emplace_back();

	m_textElements[m_currentID].text = text;
	m_textElements[m_currentID].pos = pos;
	m_textElements[m_currentID].size = size;
	m_textElements[m_currentID].color = color;
	m_textElements[m_currentID].fontID = fontID;
	m_textElements[m_currentID].alignment = alignment;
	m_textElements[m_currentID].enabled = true;

	m_currentID++;
}

void Glim::TextLayer::Destroy()
{
	/* Cleanup */
	for (msdfgl_font_t& font : m_fonts)
		msdfgl_destroy_font(font);
	msdfgl_destroy_context(m_context);
}

void Glim::TextLayer::Draw()
{
	for (int i = m_currentID; i < m_textElements.size(); i++)
		m_textElements[i].enabled = false;

	for (const TextElement& element : m_textElements)
	{
		if (!element.enabled)
			continue;
		msdfgl_printf(
			element.pos.x, element.pos.y + element.size,
			(int)element.alignment,
			m_fonts[element.fontID], element.size,
			element.color,
			(GLfloat*)m_projection,
			msdfgl_printf_flags::MSDFGL_KERNING,
			element.text);
	}
}

void Glim::TextLayer::FrameEnd()
{
	m_currentID = 0;
}
