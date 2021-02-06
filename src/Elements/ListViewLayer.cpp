#include "ListViewLayer.h"
#include "Input.h"
#include <iostream>

#define TEXT_COLOR 0xffffffff
#define BOX_COLOR { 0.5f, 0.5f, 0.5f, 0.9f }
#define H_MARGIN 48.0f
#define V_MARGIN 16.0f
#define BAR_HEIGHT 32
#define TEXT_SIZE 14
#define TEXT_OFFSET_X 16.0f
#define TEXT_OFFSET_Y 8.0f
#define SCROLL_SENSITIVITY 12.0f
#define CORNER_RADIUS 5.0f

bool Glim::ListViewLayer::CursorOver(int listViewID)
{
	return
		m_listViews[listViewID].pos.x < Input::mousePos[0] &&
		m_listViews[listViewID].pos.y < Input::mousePos[1] &&
		m_listViews[listViewID].pos.x + m_listViews[listViewID].size.x > Input::mousePos[0] &&
		m_listViews[listViewID].pos.y + m_listViews[listViewID].size.y > Input::mousePos[1];
}

int Glim::ListViewLayer::CollisionTest(int listViewID)
{
	if (Input::mousePos[0] < m_listViews[listViewID].pos.x + H_MARGIN ||
		Input::mousePos[0] > m_listViews[listViewID].pos.x + m_listViews[listViewID].size.x - H_MARGIN)
		return -1;

	float yOffset = Input::mousePos[1] - (m_listViews[listViewID].pos.y + V_MARGIN + m_listViews[listViewID].scrollOffset);
	int index = (int)glm::floor(yOffset / BAR_HEIGHT);
	if (index < m_listViews[listViewID].options->size() && index > -1)
		return index;
	return -1;
}

void Glim::ListViewLayer::Init(const uint32_t* windowSize)
{
	m_windowSize = windowSize;
	m_textLayer.Init(windowSize);

	m_shader.CreateFromFiles("assets/shaders/vert.glsl", "assets/shaders/listView.glsl");
	m_shader.Bind();
	m_shader.SetUniform1f("u_Radius", CORNER_RADIUS);
	m_shader.SetUniform2fv("u_SelectionTopLeft", &m_shaderUniformData.x);
	m_shader.SetUniform2fv("u_SelectionBottomRight", &m_shaderUniformData.z);
	m_quads.Init(&m_shader);
}

int Glim::ListViewLayer::Evaluate(const glm::vec2& position, const glm::vec2& size, const std::vector<std::string>* options, unsigned int fontID)
{
	// update data
	if (m_currentID == m_listViews.size())
	{
		m_listViews.emplace_back();
		m_listViews.back().geometryIndex = m_quads.CreateQuad();
	}

	m_listViews[m_currentID].pos = position;
	m_listViews[m_currentID].size = size;
	m_listViews[m_currentID].options = options;
	m_listViews[m_currentID].fontID = fontID;

	// handle interaction
	int itemIndex = -2;
	bool cursorOver = CursorOver(m_currentID);
	if (cursorOver && !Input::cursorCollisionDetected)
	{
		Input::cursorCollisionDetected = true;
		if (Input::MouseScrollUp())
			m_listViews[m_currentID].scrollOffset += SCROLL_SENSITIVITY;
		else if (Input::MouseScrollDown())
			m_listViews[m_currentID].scrollOffset -= SCROLL_SENSITIVITY;
		itemIndex = CollisionTest(m_currentID);
		if (Input::MouseButtonDown(0))
			std::cout << itemIndex << std::endl;
	}

	// update data
	glm::vec2 currentPos = position + glm::vec2(H_MARGIN, V_MARGIN + m_listViews[m_currentID].scrollOffset);
	for (const std::string& option : *options)
	{
		m_textLayer.Element(option.c_str(), currentPos + glm::vec2(TEXT_OFFSET_X, TEXT_OFFSET_Y), TEXT_SIZE, fontID, TEXT_COLOR);
		currentPos.y += BAR_HEIGHT;
	}
	if (itemIndex > -2)
	{
		if (itemIndex == -1)
			m_shaderUniformData.x =
				m_shaderUniformData.z =
				m_shaderUniformData.y =
				m_shaderUniformData.w = -1.0f;
		else
		{
			m_shaderUniformData.x = position.x + H_MARGIN;
			m_shaderUniformData.z = position.x + size.x - H_MARGIN;
			m_shaderUniformData.y = position.y + V_MARGIN + m_listViews[m_currentID].scrollOffset + BAR_HEIGHT * itemIndex;
			m_shaderUniformData.w = position.y + V_MARGIN + m_listViews[m_currentID].scrollOffset + BAR_HEIGHT * (itemIndex + 1);
		}
		m_shader.Bind();
		m_shader.SetUniform2fv("u_SelectionTopLeft", &m_shaderUniformData.x);
		m_shader.SetUniform2fv("u_SelectionBottomRight", &m_shaderUniformData.z);
	}

	m_quads.UpdateQuadVertexCoords(m_listViews[m_currentID].geometryIndex, position, size);
	m_quads.UpdateQuadColor(m_listViews[m_currentID].geometryIndex, BOX_COLOR);

	m_currentID++;
	return -1;
}

void Glim::ListViewLayer::FrameBegin()
{
}

void Glim::ListViewLayer::BeforeDraw()
{
}

void Glim::ListViewLayer::FrameEnd()
{
	m_currentID = 0;
	m_textLayer.FrameEnd();
}
