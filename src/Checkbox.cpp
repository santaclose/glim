#include "Checkbox.h"
#include "Input.h"

#define MARGIN 1.0f
#define CORNER_RADIUS 3.0f
#define THICKNESS 2.0f
#define COLOR { 0.35f, 0.35f, 0.35f, 0.9f }
#define QUAD_SIZE 18.0f


bool Glim::Checkbox::CollisionTest(int checkboxID)
{
	return
		Input::mousePos[0] > m_checkboxes[checkboxID].pos.x &&
		Input::mousePos[0] < m_checkboxes[checkboxID].pos.x + QUAD_SIZE &&
		Input::mousePos[1] > m_checkboxes[checkboxID].pos.y &&
		Input::mousePos[1] < m_checkboxes[checkboxID].pos.y + QUAD_SIZE;

}

void Glim::Checkbox::Init(const uint32_t* windowSize)
{
	m_windowSize = windowSize;

	m_shader.CreateFromFiles("assets/shaders/vert.glsl", "assets/shaders/checkbox.glsl");
	m_shader.Bind();
	m_shader.SetUniform1f("u_Margin", MARGIN);
	m_shader.SetUniform1f("u_CornerRadius", CORNER_RADIUS);
	m_shader.SetUniform1f("u_Thickness", THICKNESS);

	m_quads.CreateFromShader(&m_shader);
}

void Glim::Checkbox::Evaluate(const glm::vec2& position, bool* value)
{
	if (m_currentID == m_checkboxes.size())
	{
		m_checkboxes.emplace_back();
		m_checkboxes.back().geometryIndex = m_quads.CreateQuad();
	}

	m_checkboxes[m_currentID].pos = position;
	m_checkboxes[m_currentID].size = QUAD_SIZE;
	m_checkboxes[m_currentID].value = value;

	m_quads.UpdateQuadVertexCoords(m_checkboxes[m_currentID].geometryIndex, m_checkboxes[m_currentID].pos, { QUAD_SIZE, QUAD_SIZE });
	m_quads.UpdateQuadColor(m_checkboxes[m_currentID].geometryIndex, COLOR);

	if (CollisionTest(m_currentID) && Input::MouseButtonDown(0) && !Input::cursorCollisionDetected)
	{
		*m_checkboxes[m_currentID].value = !*m_checkboxes[m_currentID].value;
		Glim::Input::cursorCollisionDetected = true;
	}

	m_quads.UpdateQuadData(m_checkboxes[m_currentID].geometryIndex,
		{ m_checkboxes[m_currentID].pos.x, m_checkboxes[m_currentID].pos.y,
		  m_checkboxes[m_currentID].pos.x + QUAD_SIZE, m_checkboxes[m_currentID].pos.y + QUAD_SIZE },
		{ *m_checkboxes[m_currentID].value ? 1.0f : 0.0f, 0.0f, 0.0f, 0.0f });

	m_currentID++;
	return;
}

float Glim::Checkbox::GetSize()
{
	return QUAD_SIZE;
}

void Glim::Checkbox::BeforeDraw()
{
	while (m_currentID < m_checkboxes.size())
	{
		m_quads.UpdateQuadColor(m_checkboxes[m_currentID].geometryIndex, { 0.0f, 0.0f, 0.0f, 0.0f });
		m_currentID++;
	}
}

void Glim::Checkbox::FrameEnd()
{
	m_currentID = 0;
}
