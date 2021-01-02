#include "SelectionBox.h"
#include "../Input.h"

#include <iostream>

#define CORNER_RADIUS 5.0f
#define OPTION_HEIGHT 32.0f
#define BOX_WIDTH 130.0f
#define TEXT_SIZE 12
#define TEXT_Y_OFFSET 9.0f
#define COLOR { 0.3f, 0.3f, 0.3f, 0.8f }

int Glim::SelectionBox::CollisionTest(int selectionBoxID)
{
	unsigned int optionCount = m_selectionBoxes[selectionBoxID].options->size();

	const glm::vec2& upperLeftCornerPos = m_quads.GetVertexPos(selectionBoxID * 4);
	if (Glim::Input::mousePos[0] < upperLeftCornerPos.x ||
		Glim::Input::mousePos[0] > upperLeftCornerPos.x + BOX_WIDTH)
		return -1;
	if (Glim::Input::mousePos[1] < upperLeftCornerPos.y + CORNER_RADIUS ||
		Glim::Input::mousePos[1] > upperLeftCornerPos.y + CORNER_RADIUS + OPTION_HEIGHT * optionCount)
		return -1;
	for (int i = 0; i < optionCount - 1; i++)
	{
		if (Glim::Input::mousePos[1] < upperLeftCornerPos.y + CORNER_RADIUS + OPTION_HEIGHT * (i + 1))
			return i;
	}
	return optionCount - 1;
}

void Glim::SelectionBox::Init(const uint32_t* windowSize)
{
	m_windowSize = windowSize;

	m_textLayer.Init(windowSize);
	m_fontID = m_textLayer.CreateFontFromFile("assets/fonts/Open_Sans/OpenSans-SemiBold.ttf");

	m_shader.CreateFromFiles("assets/shaders/vert.glsl", "assets/shaders/selectionBox.glsl");
	m_shader.Bind();
	m_shader.SetUniform1f("u_Radius", CORNER_RADIUS);
	m_shader.SetUniform1f("u_OptionHeight", OPTION_HEIGHT);
	m_shader.SetUniform4fv("u_SelectionData", &m_shaderUniformData.x);
	m_quads.Init(&m_shader);
}

void Glim::SelectionBox::OnResize()
{
	for (int i = 0; i < m_currentOnScreenBoxCount; i++)
	{
		glm::vec2 pos = m_selectionBoxes[i].pos;
		glm::vec2 boxSize = { BOX_WIDTH, OPTION_HEIGHT * m_selectionBoxes[i].options->size() + CORNER_RADIUS * 2.0f };
		if (m_selectionBoxes[i].cornerAtPos == Corner::BottomRight ||
			m_selectionBoxes[i].cornerAtPos == Corner::TopRight)
			pos.x -= boxSize.x;
		if (m_selectionBoxes[i].cornerAtPos == Corner::BottomLeft ||
			m_selectionBoxes[i].cornerAtPos == Corner::BottomRight)
			pos.y -= boxSize.y;
		m_quads.UpdateQuadVertexCoords(m_selectionBoxes[i].geometryIndex, pos, boxSize);
		m_quads.UpdateQuadData(m_selectionBoxes[i].geometryIndex,
			{
				pos.x, pos.y,
				pos.x + boxSize.x, pos.y + boxSize.y
			});
	}
}

int Glim::SelectionBox::Create(const std::vector<std::string>* options, const glm::vec2& position, Corner cornerAtPos)
{
	glm::vec2 pos = position;
	glm::vec2 boxSize = { BOX_WIDTH, OPTION_HEIGHT * options->size() + CORNER_RADIUS * 2.0f };
	if (cornerAtPos == Corner::BottomRight ||
		cornerAtPos == Corner::TopRight)
		pos.x -= boxSize.x;
	if (cornerAtPos == Corner::BottomLeft ||
		cornerAtPos == Corner::BottomRight)
		pos.y -= boxSize.y;

	if (m_currentOnScreenBoxCount == m_selectionBoxes.size()) // need to create a quad
	{
		unsigned int quadID = m_quads.CreateQuad(pos, boxSize, COLOR,
		{
			pos.x, pos.y,
			pos.x + boxSize.x, pos.y + boxSize.y
		});
		m_selectionBoxes.push_back({ position, options, quadID, cornerAtPos });
	}
	else
	{
		m_quads.UpdateQuadVertexCoords(m_selectionBoxes[m_currentOnScreenBoxCount].geometryIndex, pos, boxSize);
		m_quads.UpdateQuadColor(m_selectionBoxes[m_currentOnScreenBoxCount].geometryIndex, COLOR);
		m_quads.UpdateQuadData(m_selectionBoxes[m_currentOnScreenBoxCount].geometryIndex,
			{
				pos.x, pos.y,
				pos.x + boxSize.x, pos.y + boxSize.y
			});
		m_selectionBoxes[m_currentOnScreenBoxCount].options = options;
		m_selectionBoxes[m_currentOnScreenBoxCount].pos = position;
		m_selectionBoxes[m_currentOnScreenBoxCount].cornerAtPos = cornerAtPos;
	}

	m_currentOnScreenBoxCount++;
	return m_currentOnScreenBoxCount - 1;
}

int Glim::SelectionBox::Evaluate(int selectionBoxID)
{
	int returnValue = (int)Selection::None;

	// update data
	glm::vec2 pos = m_selectionBoxes[selectionBoxID].pos;
	glm::vec2 quadSize = { BOX_WIDTH, OPTION_HEIGHT * m_selectionBoxes[selectionBoxID].options->size() + CORNER_RADIUS * 2.0f };
	if (m_selectionBoxes[selectionBoxID].cornerAtPos == Corner::BottomRight ||
		m_selectionBoxes[selectionBoxID].cornerAtPos == Corner::TopRight)
		pos.x -= quadSize.x;
	if (m_selectionBoxes[selectionBoxID].cornerAtPos == Corner::BottomLeft ||
		m_selectionBoxes[selectionBoxID].cornerAtPos == Corner::BottomRight)
		pos.y -= quadSize.y;

	for (int i = 0; i < m_selectionBoxes[selectionBoxID].options->size(); i++)
	{
		m_textLayer.Element(
			(*(m_selectionBoxes[selectionBoxID].options))[i].c_str(),
			{ pos.x + BOX_WIDTH / 2.0f, pos.y + CORNER_RADIUS + OPTION_HEIGHT * i + TEXT_Y_OFFSET },
			TEXT_SIZE, m_fontID, 0xffffffff, Glim::Alignment::Center);
	}

	// handle interaction
	int collisionResult;
	if (!Input::cursorCollisionDetected)
	{
		collisionResult = CollisionTest(selectionBoxID);
		Input::cursorCollisionDetected = true;
		if (Input::MouseButtonDown(0) && Input::currentlyHandling == nullptr)
			Input::currentlyHandling = this;
		if (Input::MouseButtonUp(0) && Input::currentlyHandling == this)
		{
			returnValue = collisionResult;
			Input::currentlyHandling = nullptr;
		}
	}

	// update data
	if (collisionResult > -1)
		m_lastSelectionPos = { pos.x, pos.y + CORNER_RADIUS * 2 + OPTION_HEIGHT * (collisionResult) };
	else
		m_lastSelectionPos = { -1, -1 };


	m_shaderUniformData = {
		(float) collisionResult,
		pos.x,
		m_quads.GetVertexPos(m_selectionBoxes[selectionBoxID].geometryIndex).y,
		pos.x + BOX_WIDTH };
	m_shader.Bind();
	m_shader.SetUniform4fv("u_SelectionData", &m_shaderUniformData.x);

	return returnValue;
}

float GetWidth()
{
	return BOX_WIDTH;
}

const glm::vec2& Glim::SelectionBox::GetLastSelectionPosition(Corner corner)
{
	if (corner == Corner::BottomRight || corner == Corner::TopRight)
		m_lastSelectionPos.x += BOX_WIDTH;
	if (corner == Corner::BottomRight || corner == Corner::BottomLeft)
		m_lastSelectionPos.y += OPTION_HEIGHT;
	return m_lastSelectionPos;
}

void Glim::SelectionBox::Delete(int selectionBoxID)
{
	// hide box and decrement counter
	m_quads.UpdateQuadColor(m_selectionBoxes[selectionBoxID].geometryIndex, { 0.0f, 0.0f, 0.0f, 0.0f });
	m_currentOnScreenBoxCount--;
}

void Glim::SelectionBox::FrameEnd()
{
	m_textLayer.FrameEnd();
}
