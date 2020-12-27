#include "Checkbox.h"
#include "Geometry.h"
#include "Shader.h"
#include "Input.h"

#define MARGIN 1.0f
#define CORNER_RADIUS 3.0f
#define THICKNESS 2.0f
#define COLOR { 0.35f, 0.35f, 0.35f, 0.9f }
#define QUAD_SIZE 18.0f

namespace Glim::Checkbox {

	struct CheckboxInfo {

		uint32_t geometryIndex;
		glm::vec2 pos;
		float size;
		bool* value;
	};

	int currentCheckboxID = 0;
	std::vector<CheckboxInfo> checkboxes;

	const uint32_t* windowSize;
	Shader shader;
	Geometry quads;

	bool CollisionTest(int checkboxID);
}

bool Glim::Checkbox::CollisionTest(int checkboxID)
{
	return
		Input::mousePos[0] > checkboxes[checkboxID].pos.x &&
		Input::mousePos[0] < checkboxes[checkboxID].pos.x + QUAD_SIZE &&
		Input::mousePos[1] > checkboxes[checkboxID].pos.y &&
		Input::mousePos[1] < checkboxes[checkboxID].pos.y + QUAD_SIZE;

}

void Glim::Checkbox::Init(const uint32_t* windowSize)
{
	Checkbox::windowSize = windowSize;

	shader.CreateFromFiles("assets/shaders/vert.glsl", "assets/shaders/checkbox.glsl");
	shader.Bind();
	shader.SetUniform1f("u_Margin", MARGIN);
	shader.SetUniform1f("u_CornerRadius", CORNER_RADIUS);
	shader.SetUniform1f("u_Thickness", THICKNESS);

	quads.CreateFromShader(&shader);
}

void Glim::Checkbox::Evaluate(const glm::vec2& position, bool* value)
{
	if (currentCheckboxID == checkboxes.size())
	{
		checkboxes.emplace_back();
		checkboxes.back().geometryIndex = quads.CreateQuad();
	}

	checkboxes[currentCheckboxID].pos = position;
	checkboxes[currentCheckboxID].size = QUAD_SIZE;
	checkboxes[currentCheckboxID].value = value;

	quads.UpdateQuadVertexCoords(checkboxes[currentCheckboxID].geometryIndex, checkboxes[currentCheckboxID].pos, { QUAD_SIZE, QUAD_SIZE });
	quads.UpdateQuadColor(checkboxes[currentCheckboxID].geometryIndex, COLOR);

	if (CollisionTest(currentCheckboxID) && Input::MouseButtonDown(0) && !Input::cursorCollisionDetected)
	{
		*checkboxes[currentCheckboxID].value = !*checkboxes[currentCheckboxID].value;
		Glim::Input::cursorCollisionDetected = true;
	}

	quads.UpdateQuadData(checkboxes[currentCheckboxID].geometryIndex,
		{ checkboxes[currentCheckboxID].pos.x, checkboxes[currentCheckboxID].pos.y,
		  checkboxes[currentCheckboxID].pos.x + QUAD_SIZE, checkboxes[currentCheckboxID].pos.y + QUAD_SIZE },
		{ *checkboxes[currentCheckboxID].value ? 1.0f : 0.0f, 0.0f, 0.0f, 0.0f });

	currentCheckboxID++;
	return;
}

float Glim::Checkbox::GetSize()
{
	return QUAD_SIZE;
}

void Glim::Checkbox::BeforeDraw()
{
	while (currentCheckboxID < checkboxes.size())
	{
		quads.UpdateQuadColor(checkboxes[currentCheckboxID].geometryIndex, { 0.0f, 0.0f, 0.0f, 0.0f });
		currentCheckboxID++;
	}
}

void Glim::Checkbox::FrameEnd()
{
	currentCheckboxID = 0;
}
