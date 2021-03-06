#include "CheckboxLayer.h"
#include "../GlobalState.h"
#include "../Input.h"

#define CORNER_RADIUS 3.0f
#define QUAD_SIZE 18.0f

namespace Glim {

	std::vector<CheckboxLayer*> CheckboxLayer::instances;
}

bool Glim::CheckboxLayer::CollisionTest(int checkboxID)
{
	return
		Input::mousePos[0] > m_checkboxes[checkboxID].pos.x &&
		Input::mousePos[0] < m_checkboxes[checkboxID].pos.x + QUAD_SIZE &&
		Input::mousePos[1] > m_checkboxes[checkboxID].pos.y &&
		Input::mousePos[1] < m_checkboxes[checkboxID].pos.y + QUAD_SIZE;

}

Glim::CheckboxLayer::CheckboxLayer()
{
	instances.push_back(this);

	m_shader.CreateFromFiles("assets/shaders/vert.glsl", "assets/shaders/checkbox.glsl");
	m_shader.Bind();
	m_shader.SetUniform1f("u_CornerRadius", CORNER_RADIUS);

	m_quads.Init(&m_shader);
}

Glim::CheckboxLayer::~CheckboxLayer()
{
}

void Glim::CheckboxLayer::Evaluate(const glm::vec2& position, bool* value, const glm::vec4& color)
{
	// add a new one if necessary
	if (m_currentID == m_checkboxes.size())
	{
		m_checkboxes.emplace_back();
		m_checkboxes.back().geometryIndex = m_quads.CreateQuad();
	}

	// update data
	m_checkboxes[m_currentID].pos = position;
	m_checkboxes[m_currentID].size = QUAD_SIZE;
	m_checkboxes[m_currentID].value = value;

	m_quads.UpdateQuadVertexCoords(m_checkboxes[m_currentID].geometryIndex, m_checkboxes[m_currentID].pos, { QUAD_SIZE, QUAD_SIZE });

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
			*m_checkboxes[m_currentID].value = !*m_checkboxes[m_currentID].value;
	}

	// update data
	if (needToHighlight)
	{
		glm::vec4 highlightedColor = color;
		highlightedColor += glm::vec4(0.1f, 0.1f, 0.1f, 0.0f);
		if (highlightedColor.r > 1.0f || highlightedColor.g > 1.0f || highlightedColor.b > 1.0f)
			highlightedColor -= glm::vec4(0.2f, 0.2f, 0.2f, 0.0f);
		m_quads.UpdateQuadColor(m_checkboxes[m_currentID].geometryIndex, highlightedColor);
	}
	else
		m_quads.UpdateQuadColor(m_checkboxes[m_currentID].geometryIndex, color);
	m_quads.UpdateQuadData(m_checkboxes[m_currentID].geometryIndex,
		{ m_checkboxes[m_currentID].pos.x, m_checkboxes[m_currentID].pos.y,
		  m_checkboxes[m_currentID].pos.x + QUAD_SIZE, m_checkboxes[m_currentID].pos.y + QUAD_SIZE },
		{ *m_checkboxes[m_currentID].value ? 1.0f : 0.0f, 0.0f, 0.0f, 0.0f });

	m_currentID++;
}

float Glim::CheckboxLayer::GetSize()
{
	return QUAD_SIZE;
}

void Glim::CheckboxLayer::BeforeDraw()
{
	while (m_currentID < m_checkboxes.size())
	{
		m_quads.UpdateQuadColor(m_checkboxes[m_currentID].geometryIndex, { 0.0f, 0.0f, 0.0f, 0.0f });
		m_currentID++;
	}
}

void Glim::CheckboxLayer::FrameEnd()
{
	m_currentID = 0;
}
