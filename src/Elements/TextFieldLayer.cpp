#include "TextFieldLayer.h"
#include "../Input.h"
#include <iostream>

#define TEXT_COLOR 0xffffffff
#define BOX_COLOR { 0.35f, 0.35f, 0.35f, 0.9f }
#define BOX_CORNER_RADIUS 5.0f
#define CURSOR_THICKNESS 2.0f
#define MARGIN 8.0f

bool Glim::TextFieldLayer::CollisionTest(int textFieldID)
{
	TextFieldInfo& item = m_textFields[textFieldID];
	float xSize = m_textLayer->Measure(item.buffer, item.size, m_textLayerFontID);
	return Input::mousePos[0] > item.pos.x && Input::mousePos[0] < item.pos.x + xSize + 2 * MARGIN &&
		Input::mousePos[1] > item.pos.y && Input::mousePos[1] < item.pos.y + item.size + 2 * MARGIN;
}

void Glim::TextFieldLayer::Init(const uint32_t* windowSize, const std::string& fontFilePath)
{
	m_windowSize = windowSize;
	m_textLayer = new TextLayer();
	m_textLayer->Init(windowSize);
	m_textLayerFontID = m_textLayer->CreateFontFromFile(fontFilePath);

	m_shader.CreateFromFiles("assets/shaders/vert.glsl", "assets/shaders/textField.glsl");
	m_shader.Bind();
	m_shader.SetUniform1f("u_Margin", MARGIN);
	m_shader.SetUniform1f("u_CornerRadius", BOX_CORNER_RADIUS);
	m_shader.SetUniform1f("u_CursorThickness", CURSOR_THICKNESS);
	m_quads.Init(&m_shader);
}

void Glim::TextFieldLayer::Evaluate(const glm::vec2& pos, char* buffer, unsigned int bufferSize, float fontSize)
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
		glm::vec4 highlightedButtonColor = BOX_COLOR;
		highlightedButtonColor += glm::vec4(0.1f, 0.1f, 0.1f, 0.0f);
		m_quads.UpdateQuadColor(m_textFields[m_currentID].geometryIndex, highlightedButtonColor);
	}
	else
		m_quads.UpdateQuadColor(m_textFields[m_currentID].geometryIndex, BOX_COLOR);

	bool interacting = m_currentlyInteracting == m_currentID;
	float xSize = m_textLayer->Measure(buffer, fontSize, m_textLayerFontID);
	float xCursorPos = 0.0;
	if (interacting)
		xCursorPos = m_textLayer->Measure(buffer, fontSize, m_textLayerFontID, m_cursorIndex);
	m_quads.UpdateQuadVertexCoords(m_textFields[m_currentID].geometryIndex, m_textFields[m_currentID].pos, { xSize + 2 * MARGIN, fontSize + 2 * MARGIN });
	m_quads.UpdateQuadData(m_textFields[m_currentID].geometryIndex,
		{ m_textFields[m_currentID].pos.x, m_textFields[m_currentID].pos.y,
		  m_textFields[m_currentID].pos.x + xSize + 2 * MARGIN, m_textFields[m_currentID].pos.y + fontSize + 2 * MARGIN },
		{ interacting ? 1.0f : 0.0f, xCursorPos, 0.0f, 0.0f });
	m_textLayer->Element(buffer, pos + glm::vec2(MARGIN, MARGIN - fontSize * 0.15), fontSize, m_textLayerFontID, TEXT_COLOR);//, Glim::Alignment::Center);

	m_currentID++;
}
void Glim::TextFieldLayer::FrameEnd()
{
	m_currentID = 0;
	m_textLayer->FrameEnd();
}

void Glim::TextFieldLayer::Destroy()
{
	m_textLayer->Destroy();
	delete m_textLayer;
}


