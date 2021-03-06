#include "ButtonLayer.h"
#include "../Input.h"
#include "../GlobalState.h"
#include <iostream>
#include <vector>

#define BUTTON_QUAD_MARGIN 3.0f

namespace Glim {

	std::vector<ButtonLayer*> ButtonLayer::instances;
}

bool Glim::ButtonLayer::CollisionTest(int buttonIndex)
{
	return glm::distance(
		{ Glim::Input::mousePos[0], Glim::Input::mousePos[1] },
		m_buttons[buttonIndex].pos + glm::vec2(m_buttons[buttonIndex].size / 2.0f, m_buttons[buttonIndex].size / 2.0f))
		< m_buttons[buttonIndex].size / 2.0f - BUTTON_QUAD_MARGIN / 2.0f;
}

Glim::ButtonLayer::ButtonLayer(IconSource iconSource, const std::string& iconsPath)
{
	instances.push_back(this);

	if (iconSource == IconSource::CircleFont)
		m_circleFont.CreateFromFile(iconsPath);

	m_iconSource = iconSource;

	m_shader.CreateFromFiles("assets/shaders/vert.glsl", "assets/shaders/floatingButton.glsl");
	m_shader.Bind();
	m_shader.SetUniform1f("u_Margin", BUTTON_QUAD_MARGIN);
	m_quads.Init(&m_shader);
}

Glim::ButtonLayer::~ButtonLayer()
{
}

bool Glim::ButtonLayer::Evaluate(const glm::vec2& position, float size, int iconID, const glm::vec4& color, const glm::vec4& iconColor)
{
	bool returnValue = false;

	// add new button if necessary
	if (m_currentID == m_buttons.size())
	{
		m_buttons.emplace_back();
		m_buttons.back().geometryIndex = m_quads.CreateQuad();
		if (m_iconSource == IconSource::CircleFont)
			m_buttons.back().iconIndex = m_circleFont.Add(0, position, size, iconColor);
	}

	// update data
	m_buttons[m_currentID].pos = position;
	m_buttons[m_currentID].size = size;
	if (m_iconSource == IconSource::CircleFont)
	{
		m_circleFont.ChangeGlyph(m_buttons[m_currentID].iconIndex, iconID);
		m_circleFont.UpdatePosition(m_buttons[m_currentID].iconIndex, position);
		m_circleFont.UpdateSize(m_buttons[m_currentID].iconIndex, size);
		m_circleFont.SetGlyphColor(m_buttons[m_currentID].iconIndex, iconColor);
	}

	m_quads.UpdateQuadVertexCoords(m_buttons[m_currentID].geometryIndex, m_buttons[m_currentID].pos, { size, size });
	m_quads.UpdateQuadData(m_buttons[m_currentID].geometryIndex,
		{ m_buttons[m_currentID].pos.x, m_buttons[m_currentID].pos.y, m_buttons[m_currentID].size, 0.0f });

	// handle interaction
	bool cursorOver = false;
	bool needToHighlight = false;
	if (!GlobalState::cursorCollisionDetected)
	{
		cursorOver = CollisionTest(m_currentID);
		if (cursorOver)
		{
			if (GlobalState::currentlyHandling == nullptr)
			{
				GlobalState::cursorCollisionDetected = true;
				if (Input::MouseButtonDown(0))
				{
					GlobalState::currentlyHandling = this;
					m_currentlyInteracting = m_currentID;
				}
			}
		}
	}

	needToHighlight =
		cursorOver && GlobalState::currentlyHandling == nullptr ||
		GlobalState::currentlyHandling == this && m_currentlyInteracting == m_currentID;

	if (Input::MouseButtonUp(0) && GlobalState::currentlyHandling == this && m_currentlyInteracting == m_currentID)
	{
		GlobalState::currentlyHandling = nullptr;
		m_currentlyInteracting = -1;
		if (cursorOver)
			returnValue = true;
	}

	// update data
	if (needToHighlight)
	{
		glm::vec4 highlightedColor = color;
		highlightedColor += glm::vec4(0.1f, 0.1f, 0.1f, 0.0f);
		if (highlightedColor.r > 1.0f || highlightedColor.g > 1.0f || highlightedColor.b > 1.0f)
			highlightedColor -= glm::vec4(0.2f, 0.2f, 0.2f, 0.0f);
		m_quads.UpdateQuadColor(m_buttons[m_currentID].geometryIndex, highlightedColor);
	}
	else
	{
		m_quads.UpdateQuadColor(m_buttons[m_currentID].geometryIndex, color);
	}

	m_currentID++;
	return returnValue;
}

void Glim::ButtonLayer::FrameBegin()
{
}

void Glim::ButtonLayer::BeforeDraw()
{
	// hide all unused quads
	while (m_currentID < m_buttons.size())
	{
		m_quads.UpdateQuadColor(m_buttons[m_currentID].geometryIndex, { 0.0f, 0.0f, 0.0f, 0.0f });
		m_circleFont.SetGlyphColor(m_buttons[m_currentID].iconIndex, { 0.0f, 0.0f, 0.0f, 0.0f });
		m_currentID++;
	}
}

void Glim::ButtonLayer::FrameEnd()
{
	m_currentID = 0;
}