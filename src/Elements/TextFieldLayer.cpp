#include "TextFieldLayer.h"
#include "../Input.h"
#include <iostream>

#define BOX_CORNER_RADIUS 5.0f
#define CURSOR_THICKNESS 2.0f
#define MARGIN 8.0f

bool Glim::TextFieldLayer::CollisionTest(int textFieldID)
{
	TextFieldInfo& item = m_textFields[textFieldID];
	float xSize = m_textLayer->Measure(item.buffer, item.size, item.fontID) + 2 * MARGIN;
	float ySize = item.size + 2 * MARGIN;

	float spawnPosX = m_textFields[textFieldID].pos.x, spawnPosY = m_textFields[textFieldID].pos.y;
	if (m_textFields[textFieldID].hAlignment == HAlignment::Right)
		spawnPosX -= xSize;
	else if (m_textFields[textFieldID].hAlignment == HAlignment::Center)
		spawnPosX -= xSize / 2.0f;
	if (m_textFields[textFieldID].vAlignment == VAlignment::Bottom)
		spawnPosY -= ySize;
	else if (m_textFields[textFieldID].vAlignment == VAlignment::Center)
		spawnPosY -= ySize / 2.0f;

	return Input::mousePos[0] > spawnPosX && Input::mousePos[0] < spawnPosX + xSize &&
		Input::mousePos[1] > spawnPosY && Input::mousePos[1] < spawnPosY + ySize;
}

void Glim::TextFieldLayer::Init(const uint32_t* windowSize)
{
	m_windowSize = windowSize;
	m_textLayer = new TextLayer();
	m_textLayer->Init(windowSize);

	m_shader.CreateFromFiles("assets/shaders/vert.glsl", "assets/shaders/textField.glsl");
	m_shader.Bind();
	m_shader.SetUniform1f("u_Margin", MARGIN);
	m_shader.SetUniform1f("u_CornerRadius", BOX_CORNER_RADIUS);
	m_shader.SetUniform1f("u_CursorThickness", CURSOR_THICKNESS);
	m_quads.Init(&m_shader);
}

void Glim::TextFieldLayer::Evaluate(
	const glm::vec2& pos,
	char* buffer,
	unsigned int bufferSize,
	unsigned int fontID,
	float fontSize,
	HAlignment hAlignment,
	VAlignment vAlignment,
	const glm::vec4& color,
	unsigned int textColor)
{
	// update data
	if (m_currentID == m_textFields.size())
	{
		m_textFields.emplace_back();
		m_textFields.back().geometryIndex = m_quads.CreateQuad();
	}

	m_textFields[m_currentID].pos = pos;
	m_textFields[m_currentID].size = fontSize;
	m_textFields[m_currentID].buffer = buffer;
	m_textFields[m_currentID].hAlignment = hAlignment;
	m_textFields[m_currentID].vAlignment = vAlignment;
	m_textFields[m_currentID].fontID = fontID;

	// handle interaction
	bool cursorOver = false;
	bool needToHighlight = false;
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
					m_clickPending = m_currentID;
				}
			}
		}
		else if (Input::MouseButtonDown(0) && Input::currentlyHandling == this)
		{
			Input::currentlyHandling = nullptr;
			m_currentlyInteracting = -1;
			m_clickPending = -1;
		}
	}

	needToHighlight =
		cursorOver && Input::currentlyHandling == nullptr ||
		Input::currentlyHandling == this && m_clickPending == m_currentID;

	if (Input::MouseButtonUp(0) && Input::currentlyHandling == this && m_clickPending == m_currentID)
	{
		m_clickPending = -1;
		if (cursorOver)
		{
			m_currentlyInteracting = m_currentID;
			for (m_cursorIndex = 0; m_cursorIndex < bufferSize - 1 && buffer[m_cursorIndex] != '\0'; m_cursorIndex++);
		}
		else
			Input::currentlyHandling = nullptr;
	}

	if (m_currentlyInteracting == m_currentID)
	{
		unsigned int charInput;
		if (Input::CharacterInput(charInput))
		{
			if (buffer[bufferSize - 2] == '\0') // there is space for one more char
			{
				char charHere = buffer[m_cursorIndex];
				char nextChar = buffer[m_cursorIndex + 1];
				buffer[m_cursorIndex] = (char)charInput;

				for (int i = m_cursorIndex + 1; i < bufferSize && charHere != '\0'; i++)
				{
					buffer[i] = charHere;
					charHere = nextChar;
					nextChar = buffer[i + 1];
				}
				m_cursorIndex++;
			}
		}
		else if (Input::KeyRepeat(Input::KeyCode::Backspace) || Input::KeyDown(Input::KeyCode::Backspace))
		{
			if (m_cursorIndex > 0)
			{
				for (int i = m_cursorIndex - 1; buffer[i] != '\0'; i++)
					buffer[i] = buffer[i + 1];
				m_cursorIndex--;
			}
			/*int i = bufferSize - 1;
			for (; i > 0 && buffer[i] == '\0'; i--);
			buffer[i] = '\0';
			if (Input::Key(Input::KeyCode::LeftControl) || Input::Key(Input::KeyCode::RightControl))
			{
				if (i > 0)
					i--;
				for (; i > -1 && (buffer[i] >= 'A' && buffer[i] <= 'Z' || buffer[i] >= 'a' && buffer[i] <= 'z' || buffer[i] >= '0' && buffer[i] <= '9'); i--)
					buffer[i] = '\0';
			}*/
		}
		else if (Input::KeyRepeat(Input::KeyCode::Delete) || Input::KeyDown(Input::KeyCode::Delete))
		{
			for (int i = m_cursorIndex; buffer[i] != '\0'; i++)
				buffer[i] = buffer[i + 1];
		}
		else if (Input::KeyRepeat(Input::KeyCode::Left) || Input::KeyDown(Input::KeyCode::Left))
		{
			if (m_cursorIndex > 0)
				m_cursorIndex--;
		}
		else if (Input::KeyRepeat(Input::KeyCode::Right) || Input::KeyDown(Input::KeyCode::Right))
		{
			if (buffer[m_cursorIndex] != '\0')
				m_cursorIndex++;
		}
		else if (Input::KeyRepeat(Input::KeyCode::Left) || Input::KeyDown(Input::KeyCode::Left))
		{
			m_cursorIndex--;
		}
		else if (Input::KeyDown(Input::KeyCode::Escape) || Input::KeyDown(Input::KeyCode::Enter) || Input::KeyDown(Input::KeyCode::KPEnter))
		{
			Input::currentlyHandling = nullptr;
			m_currentlyInteracting = -1;
		}
	}

	// update data
	if (needToHighlight)
	{
		glm::vec4 highlightedColor = color;
		highlightedColor += glm::vec4(0.1f, 0.1f, 0.1f, 0.0f);
		if (highlightedColor.r > 1.0f || highlightedColor.g > 1.0f || highlightedColor.b > 1.0f)
			highlightedColor -= glm::vec4(0.2f, 0.2f, 0.2f, 0.0f);
		m_quads.UpdateQuadColor(m_textFields[m_currentID].geometryIndex, highlightedColor);
	}
	else
		m_quads.UpdateQuadColor(m_textFields[m_currentID].geometryIndex, color);

	bool interacting = m_currentlyInteracting == m_currentID;
	float xSize = m_textLayer->Measure(buffer, fontSize, m_textFields[m_currentID].fontID) + 2 * MARGIN;
	float ySize = fontSize + 2 * MARGIN;
	float xCursorPos = 0.0;
	if (interacting)
		xCursorPos = m_textLayer->Measure(buffer, fontSize, m_textFields[m_currentID].fontID, m_cursorIndex);

	float spawnPosX = m_textFields[m_currentID].pos.x, spawnPosY = m_textFields[m_currentID].pos.y;
	if (hAlignment == HAlignment::Right)
		spawnPosX -= xSize;
	else if (hAlignment == HAlignment::Center)
		spawnPosX -= xSize / 2.0f;
	if (vAlignment == VAlignment::Bottom)
		spawnPosY -= ySize;
	else if (vAlignment == VAlignment::Center)
		spawnPosY -= ySize / 2.0f;

	m_quads.UpdateQuadVertexCoords(m_textFields[m_currentID].geometryIndex, {spawnPosX, spawnPosY}, { xSize, ySize });
	m_quads.UpdateQuadData(m_textFields[m_currentID].geometryIndex,
		{ spawnPosX, spawnPosY,
		  spawnPosX + xSize, spawnPosY + ySize },
		{ interacting ? 1.0f : 0.0f, xCursorPos, 0.0f, 0.0f });
	m_textLayer->Element(buffer, glm::vec2(spawnPosX, spawnPosY) + glm::vec2(MARGIN, MARGIN - fontSize * 0.15), fontSize, m_textFields[m_currentID].fontID, textColor);

	m_currentID++;
}
void Glim::TextFieldLayer::FrameEnd()
{
	m_currentID = 0;
	m_textLayer->FrameEnd();
}

void Glim::TextFieldLayer::Destroy()
{
	delete m_textLayer;
}


