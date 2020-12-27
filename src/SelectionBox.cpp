#include "SelectionBox.h"
#include "Geometry.h"
#include "Input.h"
#include "Text.h"

#include <iostream>

#define CORNER_RADIUS 5.0f
#define OPTION_HEIGHT 32.0f
#define BOX_WIDTH 130.0f
#define TEXT_SIZE 12
#define TEXT_Y_OFFSET 9.0f
#define COLOR { 0.3f, 0.3f, 0.3f, 0.8f }

namespace Glim::SelectionBox {

	struct BoxInfo {

		glm::vec2 pos;
		const std::vector<std::string>* options;
		uint32_t geometryIndex;
		Corner cornerAtPos;
	};

	unsigned int fontID;
	bool evaluationHandled = false;
	int currentOnScreenBoxCount = 0;
	glm::vec2 lastSelectionPos = {-1, -1};

	const uint32_t* windowSize;
	Geometry quads; // one quad for each selection box displayed at the same time
	Shader shader;

	glm::vec4 shaderUniformData = { -1.0f, 0.0f, 0.0f, 0.0f };

	std::vector<BoxInfo> selectionBoxes;

	int CollisionTest(int selectionBoxID);
}

int Glim::SelectionBox::CollisionTest(int selectionBoxID)
{
	unsigned int optionCount = selectionBoxes[selectionBoxID].options->size();

	const glm::vec2& upperLeftCornerPos = quads.GetVertexPos(selectionBoxID * 4);
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
	SelectionBox::windowSize = windowSize;

	shader.CreateFromFiles("assets/shaders/vert.glsl", "assets/shaders/selectionBox.glsl");
	shader.Bind();
	shader.SetUniform1f("u_Radius", CORNER_RADIUS);
	shader.SetUniform1f("u_OptionHeight", OPTION_HEIGHT);
	shader.SetUniform4fv("u_SelectionData", &shaderUniformData.x);
	quads.CreateFromShader(&shader);

	fontID = Glim::Text::CreateFontFromFile("assets/fonts/Open_Sans/OpenSans-Regular.ttf");
}

void Glim::SelectionBox::OnResize()
{
	for (int i = 0; i < currentOnScreenBoxCount; i++)
	{
		glm::vec2 pos = selectionBoxes[i].pos;
		glm::vec2 boxSize = { BOX_WIDTH, OPTION_HEIGHT * selectionBoxes[i].options->size() + CORNER_RADIUS * 2.0f };
		if (selectionBoxes[i].cornerAtPos == Corner::BottomRight ||
			selectionBoxes[i].cornerAtPos == Corner::TopRight)
			pos.x -= boxSize.x;
		if (selectionBoxes[i].cornerAtPos == Corner::BottomLeft ||
			selectionBoxes[i].cornerAtPos == Corner::BottomRight)
			pos.y -= boxSize.y;
		quads.UpdateQuadVertexCoords(selectionBoxes[i].geometryIndex, pos, boxSize);
		quads.UpdateQuadData(selectionBoxes[i].geometryIndex,
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

	if (currentOnScreenBoxCount == selectionBoxes.size()) // need to create a quad
	{
		unsigned int quadID = quads.CreateQuad(pos, boxSize, COLOR,
		{
			pos.x, pos.y,
			pos.x + boxSize.x, pos.y + boxSize.y
		});
		selectionBoxes.push_back({ position, options, quadID, cornerAtPos });
	}
	else
	{
		quads.UpdateQuadVertexCoords(selectionBoxes[currentOnScreenBoxCount].geometryIndex, pos, boxSize);
		quads.UpdateQuadColor(selectionBoxes[currentOnScreenBoxCount].geometryIndex, COLOR);
		quads.UpdateQuadData(selectionBoxes[currentOnScreenBoxCount].geometryIndex,
			{
				pos.x, pos.y,
				pos.x + boxSize.x, pos.y + boxSize.y
			});
		selectionBoxes[currentOnScreenBoxCount].options = options;
		selectionBoxes[currentOnScreenBoxCount].pos = position;
		selectionBoxes[currentOnScreenBoxCount].cornerAtPos = cornerAtPos;
	}

	currentOnScreenBoxCount++;
	return currentOnScreenBoxCount - 1;
}

int Glim::SelectionBox::Evaluate(int selectionBoxID)
{
	glm::vec2 pos = selectionBoxes[selectionBoxID].pos;
	glm::vec2 quadSize = { BOX_WIDTH, OPTION_HEIGHT * selectionBoxes[selectionBoxID].options->size() + CORNER_RADIUS * 2.0f };
	if (selectionBoxes[selectionBoxID].cornerAtPos == Corner::BottomRight ||
		selectionBoxes[selectionBoxID].cornerAtPos == Corner::TopRight)
		pos.x -= quadSize.x;
	if (selectionBoxes[selectionBoxID].cornerAtPos == Corner::BottomLeft ||
		selectionBoxes[selectionBoxID].cornerAtPos == Corner::BottomRight)
		pos.y -= quadSize.y;

	for (int i = 0; i < selectionBoxes[selectionBoxID].options->size(); i++)
	{
		Text::Element(
			(*(selectionBoxes[selectionBoxID].options))[i],
			{ pos.x + BOX_WIDTH / 2.0f, pos.y + CORNER_RADIUS + OPTION_HEIGHT * (i + 1) - TEXT_Y_OFFSET },
			TEXT_SIZE, fontID, 0xffffffff, Glim::Alignment::Center);
	}

	if (evaluationHandled)
		return Selection::None;

	int collisionResult = CollisionTest(selectionBoxID);
	evaluationHandled = collisionResult > -1;

	if (evaluationHandled)
		lastSelectionPos = { pos.x, pos.y + CORNER_RADIUS * 2 + OPTION_HEIGHT * (collisionResult + 1) };
	else
		lastSelectionPos = { -1, -1 };

	shaderUniformData = {
		(float) collisionResult,
		pos.x,
		quads.GetVertexPos(selectionBoxes[selectionBoxID].geometryIndex).y,
		pos.x + BOX_WIDTH };
	shader.Bind();
	shader.SetUniform4fv("u_SelectionData", &shaderUniformData.x);

	if (Input::MouseButtonDown(0) && !Input::cursorCollisionDetected)
	{
		Input::cursorCollisionDetected = true;
		return collisionResult;
	}

	// user has not made a selection
	return Selection::None;
}

const glm::vec2& Glim::SelectionBox::GetLastSelectionPosition(bool leftSide)
{
	if (leftSide)
		return lastSelectionPos;
	lastSelectionPos.x += BOX_WIDTH;
	return lastSelectionPos;
}

void Glim::SelectionBox::Delete(int selectionBoxID)
{
	// hide box and decrement counter
	quads.UpdateQuadColor(selectionBoxes[selectionBoxID].geometryIndex, { 0.0f, 0.0f, 0.0f, 0.0f });
	currentOnScreenBoxCount--;
}

void Glim::SelectionBox::FrameEnd()
{
	evaluationHandled = false;
}
