#include "FloatingButton.h"
#include "Input.h"
#include <iostream>
#include <vector>

#define BUTTON_QUAD_MARGIN 3.0f
#define COLOR { 0.35f, 0.35f, 0.35f, 0.9f }
#define ICON_COLOR { 1.0f, 1.0f, 1.0f, 1.0f }

bool Glim::FloatingButton::CollisionTest(int buttonIndex)
{
	return glm::distance(
		{ Glim::Input::mousePos[0], Glim::Input::mousePos[1] },
		m_buttons[buttonIndex].pos + glm::vec2(m_buttons[buttonIndex].size / 2.0f, m_buttons[buttonIndex].size / 2.0f))
		< m_buttons[buttonIndex].size / 2.0f - BUTTON_QUAD_MARGIN / 2.0f;
}

void Glim::FloatingButton::Init(const uint32_t* windowSize, IconSource iconSource, const std::string& iconsPath)
{
	if (iconSource == IconSource::CircleFont)
		m_circleFont.CreateFromFile(iconsPath);

	m_iconSource = iconSource;
	m_windowSize = windowSize;

	m_shader.CreateFromFiles("assets/shaders/vert.glsl", "assets/shaders/floatingButton.glsl");
	m_shader.Bind();
	m_shader.SetUniform1f("u_Margin", BUTTON_QUAD_MARGIN);
	m_quads.CreateFromShader(&m_shader);
}

bool Glim::FloatingButton::Evaluate(const glm::vec2& position, float size, int iconID)
{
	// add new button if necessary
	if (m_currentID == m_buttons.size())
	{
		m_buttons.emplace_back();
		m_buttons.back().geometryIndex = m_quads.CreateQuad();
		if (m_iconSource == IconSource::CircleFont)
			m_buttons.back().iconIndex = m_circleFont.Add(0, position, size, ICON_COLOR);
	}

	// update data
	m_buttons[m_currentID].pos = position;
	m_buttons[m_currentID].size = size;
	if (m_iconSource == IconSource::CircleFont)
	{
		m_circleFont.ChangeGlyph(m_buttons[m_currentID].iconIndex, iconID);
		m_circleFont.UpdatePosition(m_buttons[m_currentID].iconIndex, position);
		m_circleFont.UpdateSize(m_buttons[m_currentID].iconIndex, size);
		m_circleFont.SetGlyphColor(m_buttons[m_currentID].iconIndex, ICON_COLOR);
	}

	m_quads.UpdateQuadVertexCoords(m_buttons[m_currentID].geometryIndex, m_buttons[m_currentID].pos, { size, size });

	bool cursorOver = CollisionTest(m_currentID);
	if (cursorOver)
	{
		glm::vec4 highlightedButtonColor = COLOR;
		highlightedButtonColor += glm::vec4(0.1f, 0.1f, 0.1f, 0.0f);
		m_quads.UpdateQuadColor(m_buttons[m_currentID].geometryIndex, highlightedButtonColor);
	}
	else
		m_quads.UpdateQuadColor(m_buttons[m_currentID].geometryIndex, COLOR);

	m_quads.UpdateQuadData(m_buttons[m_currentID].geometryIndex,
		{ m_buttons[m_currentID].pos.x, m_buttons[m_currentID].pos.y, m_buttons[m_currentID].size, 0.0f });

	m_currentID++;
	if (cursorOver && Input::MouseButtonDown(0) && !Input::cursorCollisionDetected)
	{
		Input::cursorCollisionDetected = true;
		return true;
	}
	return false;
}

void Glim::FloatingButton::FrameBegin()
{
}

void Glim::FloatingButton::BeforeDraw()
{
	// hide all unused quads
	while (m_currentID < m_buttons.size())
	{
		m_quads.UpdateQuadColor(m_buttons[m_currentID].geometryIndex, { 0.0f, 0.0f, 0.0f, 0.0f });
		m_circleFont.SetGlyphColor(m_buttons[m_currentID].iconIndex, { 0.0f, 0.0f, 0.0f, 0.0f });
		m_currentID++;
	}
}

void Glim::FloatingButton::FrameEnd()
{
	m_currentID = 0;
}