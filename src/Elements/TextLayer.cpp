#include "TextLayer.h"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include "LayerRenderer.h"
#include "../FontManager.h"

namespace Glim {

	std::vector<TextLayer*> TextLayer::instances;
}

void Glim::TextLayer::ComputeMatrix(const uint32_t* windowSize)
{
	_msdfgl_ortho(0.0, (float)windowSize[0], (float)windowSize[1], 0.0, -1.0, 1.0, m_projection);
}

Glim::TextLayer::TextLayer()
{
	instances.push_back(this);
	LayerRenderer::PushLayer(this);
}

Glim::TextLayer::~TextLayer()
{
}

void Glim::TextLayer::OnResize(const uint32_t* windowSize)
{
	for (TextLayer* t : instances)
		t->ComputeMatrix(windowSize);
}

void Glim::TextLayer::Element(const char* text, const glm::vec2& pos, float size, unsigned int fontID, unsigned int color, HAlignment alignment)
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

float Glim::TextLayer::Measure(const char* text, float size, unsigned int fontID, unsigned int limit)
{
	msdfgl_font_t* font = (msdfgl_font_t*) FontManager::GetFontData(fontID);
	return msdfgl_measure(*font, size, limit, (GLfloat*)m_projection, msdfgl_printf_flags::MSDFGL_KERNING, text);
}


void Glim::TextLayer::Draw()
{
	for (int i = m_currentID; i < m_textElements.size(); i++)
		m_textElements[i].enabled = false;

	for (const TextElement& element : m_textElements)
	{
		if (!element.enabled)
			continue;
		msdfgl_font_t* font = (msdfgl_font_t*)FontManager::GetFontData(element.fontID);
		msdfgl_printf(
			element.pos.x, element.pos.y + element.size,
			(int)element.alignment,
			*font, element.size,
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
