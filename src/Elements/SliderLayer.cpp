#include "SliderLayer.h"
#include "../Input.h"
#include <iostream>

#define SIDE_MARGIN 1.0f
#define RAIL_THICKNESS 3.0f
#define HANDLE_RADIUS 8.0f
#define QUAD_WIDTH 25.0f

bool Glim::SliderLayer::CollisionTest(int sliderID)
{
	glm::vec2 circleCenter;
	if (m_sliders[sliderID].orientation == Orientation::Horizontal)
	{
		circleCenter.x = glm::mix(
			m_sliders[sliderID].pos.x + HANDLE_RADIUS + SIDE_MARGIN,
			m_sliders[sliderID].pos.x + m_sliders[sliderID].size - HANDLE_RADIUS - SIDE_MARGIN,
			*m_sliders[sliderID].value
		);
		circleCenter.y = m_sliders[sliderID].pos.y + QUAD_WIDTH / 2.0f;
		return glm::distance(circleCenter, *((glm::vec2*)&(Glim::Input::mousePos[0]))) < HANDLE_RADIUS;
	}
	else
	{
		circleCenter.x = m_sliders[sliderID].pos.x + QUAD_WIDTH / 2.0f;
		circleCenter.y = glm::mix(
			m_sliders[sliderID].pos.y + HANDLE_RADIUS + SIDE_MARGIN,
			m_sliders[sliderID].pos.y + m_sliders[sliderID].size - HANDLE_RADIUS - SIDE_MARGIN,
			*m_sliders[sliderID].value
		);
		return glm::distance(circleCenter, *((glm::vec2*)&(Glim::Input::mousePos[0]))) < HANDLE_RADIUS;
	}
}

void Glim::SliderLayer::Init(const uint32_t* windowSize)
{
	m_windowSize = windowSize;

	m_shader.CreateFromFiles("assets/shaders/vert.glsl", "assets/shaders/slider.glsl");
	m_shader.Bind();
	m_shader.SetUniform1f("u_HandleRadius", HANDLE_RADIUS);
	m_shader.SetUniform1f("u_RailThickness", RAIL_THICKNESS);
	m_shader.SetUniform1f("u_SideMargin", SIDE_MARGIN);

	m_quads.Init(&m_shader);
}

void Glim::SliderLayer::Evaluate(const glm::vec2& position, float size, float* value, Orientation orientation, const glm::vec4& color)
{
	// update data
	if (m_currentID == m_sliders.size())
	{
		m_sliders.emplace_back();
		m_sliders.back().geometryIndex = m_quads.CreateQuad();
	}

	m_sliders[m_currentID].pos = position;
	m_sliders[m_currentID].value = value;
	m_sliders[m_currentID].size = size;
	m_sliders[m_currentID].orientation = orientation;

	if (orientation == Orientation::Horizontal)
		m_quads.UpdateQuadVertexCoords(m_sliders[m_currentID].geometryIndex, m_sliders[m_currentID].pos, { size, QUAD_WIDTH });
	else
		m_quads.UpdateQuadVertexCoords(m_sliders[m_currentID].geometryIndex, m_sliders[m_currentID].pos, { QUAD_WIDTH, size });

	float minPos;
	float maxPos;
	if (m_sliders[m_currentID].orientation == Orientation::Horizontal)
	{
		minPos = m_sliders[m_currentID].pos.x + HANDLE_RADIUS;
		maxPos = m_sliders[m_currentID].pos.x + m_sliders[m_currentID].size - HANDLE_RADIUS;
	}
	else
	{
		minPos = m_sliders[m_currentID].pos.y + HANDLE_RADIUS;
		maxPos = m_sliders[m_currentID].pos.y + m_sliders[m_currentID].size - HANDLE_RADIUS;
	}

	// handle interaction
	bool needToHighlight = false;
	bool needToUpdateDraggingOffset = false;
	bool cursorOver = false;
	if (!Input::cursorCollisionDetected)
	{
		cursorOver = CollisionTest(m_currentID);
		if (cursorOver)
		{
			if (Input::currentlyHandling == nullptr)
			{
				Input::cursorCollisionDetected = true;
				if (Input::MouseButtonDown(0))
				{
					Input::currentlyHandling = this;
					m_currentlyDraggingSlider = m_currentID;
					needToUpdateDraggingOffset = true;
				}
			}
		}
	}
	needToHighlight =
		cursorOver && Input::currentlyHandling == nullptr ||
		Input::currentlyHandling == this && m_currentlyDraggingSlider == m_currentID;
	if (Input::MouseButtonUp(0) && Input::currentlyHandling == this && m_currentlyDraggingSlider == m_currentID)
	{
		Input::currentlyHandling = nullptr;
		m_currentlyDraggingSlider = -1;
	}

	// update data
	if (needToHighlight)
	{
		glm::vec4 highlightedColor = color;
		highlightedColor += glm::vec4(0.1f, 0.1f, 0.1f, 0.0f);
		if (highlightedColor.r > 1.0f || highlightedColor.g > 1.0f || highlightedColor.b > 1.0f)
			highlightedColor -= glm::vec4(0.2f, 0.2f, 0.2f, 0.0f);
		m_quads.UpdateQuadColor(m_sliders[m_currentID].geometryIndex, highlightedColor);
	}
	else
		m_quads.UpdateQuadColor(m_sliders[m_currentID].geometryIndex, color);
	if (needToUpdateDraggingOffset)
	{
		float valueFromMousePos = m_sliders[m_currentID].orientation == Orientation::Horizontal ?
			(Input::mousePos[0] - minPos) / (maxPos - minPos) :
			(Input::mousePos[1] - minPos) / (maxPos - minPos);
		m_draggingOffset = *m_sliders[m_currentID].value - valueFromMousePos;
	}
	if (m_currentlyDraggingSlider == m_currentID)
	{
		float valueFromMousePos = m_sliders[m_currentID].orientation == Orientation::Horizontal ?
			(Input::mousePos[0] - minPos) / (maxPos - minPos) :
			(Input::mousePos[1] - minPos) / (maxPos - minPos);

		*m_sliders[m_currentID].value = glm::clamp(valueFromMousePos + m_draggingOffset, 0.0f, 1.0f);
	}

	if (orientation == Orientation::Horizontal)
		m_quads.UpdateQuadData(m_sliders[m_currentID].geometryIndex,
			{ m_sliders[m_currentID].pos.x, m_sliders[m_currentID].pos.y,
			  m_sliders[m_currentID].pos.x + size, m_sliders[m_currentID].pos.y + QUAD_WIDTH },
			{ *m_sliders[m_currentID].value, 0.0f, 0.0f, 0.0f });
	else
		m_quads.UpdateQuadData(m_sliders[m_currentID].geometryIndex,
			{ m_sliders[m_currentID].pos.x, m_sliders[m_currentID].pos.y,
			  m_sliders[m_currentID].pos.x + QUAD_WIDTH, m_sliders[m_currentID].pos.y + size },
			{ *m_sliders[m_currentID].value, 1.0f, 0.0f, 0.0f });

	m_currentID++;
}

float Glim::SliderLayer::GetWidth()
{
	return QUAD_WIDTH;
}

void Glim::SliderLayer::BeforeDraw()
{
	// hide all the unused m_sliders
	while (m_currentID < m_sliders.size())
	{
		m_quads.UpdateQuadColor(m_sliders[m_currentID].geometryIndex, { 0.0f, 0.0f, 0.0f, 0.0f });
		m_currentID++;
	}
}

void Glim::SliderLayer::FrameEnd()
{
	m_currentID = 0;
}
